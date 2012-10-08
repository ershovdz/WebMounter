#include "webmounter.h"
#include "../driver/vk_driver.h"
#include "reg_exp.h"

namespace RemoteDriver
{
	using namespace Common;
	using namespace Connector;

	VkRVFSDriver::VkRVFSDriver(const QString& pluginName)
	{
		_state = RemoteDriver::eNotConnected;
		_httpConnector = new VkHTTPConnector();
		_pluginName = pluginName;

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	}

	VkRVFSDriver::~VkRVFSDriver(void)
	{
		delete _httpConnector;
	}

	RESULT VkRVFSDriver::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
	{
		VFSCache* vfsCache = WebMounter::getCache();
		int initialListSize = urlList.size();
		
		{ 	LOCK(_driverMutex)
		
			VFSCache::iterator iter = vfsCache->end();
			for(int i = 0; i < pathList.size(); ++i)
			{
				iter = vfsCache->find(pathList.at(i));
				if(iter != vfsCache->end()
					&& !(iter->getFlags() & VFSElement::eFl_Downloading)
					&& !(iter->getFlags() & VFSElement::eFl_Downloaded))
				{	
					vfsCache->setFlag(iter, VFSElement::eFl_Downloading, VFSElement::eFl_None);
					QFile::remove(pathList.at(i));
				}
				else
				{
					urlList.removeAt(i);
					pathList.removeAt(i);
					i = 0;
				}
			}
			
			if(urlList.size() == 0) // there is nothing to download
			{
				if(initialListSize == 1
					&& iter != vfsCache->end() 
					&& iter->getFlags() & VFSElement::eFl_Downloaded) // download request has been recieved for one file, but file is already downloaded
				{	
					return eNO_ERROR;
				}
				else
				{
					return eERROR;
				}
			}
		}
		
		RESULT res = _httpConnector->downloadFiles(urlList, pathList);
		if(res == eERROR)
		{
			unsigned int retryDownloadCounter = 0;
			while(res == eERROR && retryDownloadCounter < MAX_DOWNLOAD_RETRY)
			{
				res = _httpConnector->downloadFiles(urlList, pathList);
				retryDownloadCounter++;
			}
		}
		
		{ 	LOCK(_driverMutex)
		
			for(int i = 0; i < pathList.size(); i++)
			{
				QFileInfo fInfo(pathList.at(i));
				VFSCache::iterator iter = vfsCache->find(fInfo.absoluteFilePath());

				if(iter != vfsCache->end())
				{
					if(res == eERROR)
					{
						vfsCache->setFlag(iter, VFSElement::eFl_None, VFSElement::eFl_Downloading);
					}
					else
					{
						QFile::Permissions permissions = QFile::permissions(pathList.at(i));
						permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
						QFile::setPermissions(pathList.at(i), permissions);
						vfsCache->setFlag(iter, VFSElement::eFl_Downloaded, VFSElement::eFl_Downloading);
					}
				}
			}
		}
		return res;
	}

	RESULT VkRVFSDriver::uploadFile(const QString& path, const QString& title, const QString& id, const QString& parentId)
	{
		RESULT err = eERROR;
		int count = 0;

		QFileInfo fInfo(path);
		QString suffix = fInfo.suffix();
		suffix = suffix.toLower();

		if(suffix != "jpg" && suffix != "jpeg" && suffix != "png" && suffix != "gif")
		{
			notifyUser(Ui::Notification::eINFO, RemoteDriver::VkRVFSDriver::tr("Info"), RemoteDriver::VkRVFSDriver::tr("This file extension is not supported !\n"));
			return eERROR;
		}
		else if(id != ROOT_ID)
		{
			notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file will be changed locally!\n"));
			return eERROR;
		}
		else if (fInfo.size() == 0)
		{
			return eNO_ERROR;
		}

		VFSCache::iterator iter = WebMounter::getCache()->begin();
		for(iter; iter != WebMounter::getCache()->end(); ++iter)
		{
			if((iter->getPluginName() == _pluginName)
				&&(iter->getFlags()&VFSElement::eFl_SelfMade)
				&&(iter->getType() == VFSElement::FILE)
				&&(iter->getParentId() == parentId)
				)
			{
				count++;
			}
		}

		if(count < DEMO_PHOTOS_PER_ALBUM_LIMIT)
		{
			QString xmlResp;
                        unsigned int retryUploadCounter = 0;
			
			err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
			while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
			{
				err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
				retryUploadCounter++;
			}

			VFSElement elem;
			if(!err && xmlResp != "")
			{
				VFSCache* vfsCache = WebMounter::getCache();
				parsePhotoEntry(xmlResp, elem);

				fInfo.setFile(fInfo.absolutePath() + QString(QDir::separator()) + elem.getName());

				elem.setPath(fInfo.absoluteFilePath());

				elem.setFlags(VFSElement::eFl_SelfMade);

				vfsCache->insert(elem);

				QDir dir;
				dir.rename(path, elem.getPath());

				QFile::Permissions permissions = QFile::permissions(elem.getPath());
				permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
				bool errPerm = QFile::setPermissions(elem.getPath(), permissions);

			}
			else
			{
				notifyUser(Ui::Notification::eCRITICAL
							, RemoteDriver::VkRVFSDriver::tr("Error")
							, RemoteDriver::VkRVFSDriver::tr("File upload failed (") + elem.getName() + QString(")"));

			}

			fInfo.setFile(path);
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), err);
		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL, RemoteDriver::VkRVFSDriver::tr("Error"),RemoteDriver::VkRVFSDriver::tr("An upload limit is reached! \nActivation key required.\n"));
		}

		if(err)
		{
			QDir dir;
			dir.remove(path);
		}

		return err;
	}

	RESULT VkRVFSDriver::modifyFile(const QString&)
	{
		return eERROR;
	}

	RESULT VkRVFSDriver::renameElement(const QString& path, const QString& newTitle)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			if(elem->getType() == VFSElement::FILE)
			{
				return res; // Renaming of photos is not supported by Vkontakte
			}

			res = _httpConnector->renameAlbum(elem->getId(), newTitle);
			if(res == eNO_ERROR)
			{
				qInfo = qInfo.dir().absolutePath() + QString(QDir::separator()) + newTitle;
				VFSElement newElem(elem->getType()
					, qInfo.absoluteFilePath()
					, newTitle
					, elem->getEditMetaUrl()
					, elem->getEditMediaUrl()
					, elem->getSrcUrl()
					, elem->getId()
					, elem->getParentId()
					, elem->getModified()
					, elem->getPluginName()
					, elem->getFlags());

				cache->erase(elem);
				cache->insert(newElem);

				if(newElem.getType() == VFSElement::DIRECTORY)
				{
					updateChildrenPath(newElem);
				}
			}
		}

		return res;
	}

	RESULT VkRVFSDriver::deleteDirectory(const QString& path)
	{
		RESULT res = eERROR;
		QDir qDirFrom(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qDirFrom.absolutePath());

		if(elem != cache->end() && elem->getType() == VFSElement::DIRECTORY)
		{
			res = _httpConnector->deleteAlbum(elem->getId());
			if(res != eERROR)
			{
				cache->erase(elem);
			}
		}
		return res;
	}

	RESULT VkRVFSDriver::deleteFile(const QString& path)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = res = _httpConnector->deleteFile(elem->getId());
			if(res == eNO_ERROR)
			{
				WebMounter::getProxy()->fileDeleted(elem->getPath(), res);
				cache->erase(elem);
			}
		}

		return res;
	}

	RESULT VkRVFSDriver::moveElement(const QString& path, const QString& newParentId)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			if(elem->getType() == VFSElement::DIRECTORY)
			{
				return res; // Moving of albums is not supported
			}

			res = _httpConnector->moveFile(elem->getId(), elem->getParentId(), newParentId);
			if(res == eNO_ERROR)
			{
				VFSElement newElem(elem->getType()
					, elem->getPath()
					, elem->getName()
					, elem->getEditMetaUrl()
					, elem->getEditMediaUrl()
					, elem->getSrcUrl()
					, elem->getId()
					, newParentId
					, elem->getModified()
					, elem->getPluginName());

				VFSCache::iterator iter = cache->begin();
				for(iter; iter != cache->end(); ++iter)
				{
					if(iter->getId() == newParentId)
						break;
				}

				QString path = iter->getPath() + QString(QDir::separator()) + newElem.getName();
				QFileInfo fInfo(path);
				newElem.setPath(fInfo.absoluteFilePath());

				cache->erase(elem);
				cache->insert(newElem);
			}
		}

		return res;
	}

	RESULT VkRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		RESULT res = eERROR;
		int count = 0;
		VFSCache::iterator iter = WebMounter::getCache()->begin();
		for(iter; iter != WebMounter::getCache()->end(); ++iter)
		{
			if((iter->getPluginName() == _pluginName)
				&&(iter->getFlags()&VFSElement::eFl_SelfMade)
				&&(iter->getType() == VFSElement::DIRECTORY))
			{
				count++;
			}
		}

		if(parentId != ROOT_ID)
		{
			return eERROR; // Creation of sub-albums is not supported
		}

		QString xmlResp;
		res = _httpConnector->createDirectory(title, xmlResp);
		if(!res)
		{
			VFSCache* vfsCache = WebMounter::getCache();
			VFSElement elem;
			parseAlbumEntry(xmlResp, elem);
			elem.setParentId(ROOT_ID);

			QString pluginStoragePath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QString path = pluginStoragePath + QString(QDir::separator()) + elem.getName();
			QFileInfo fInfo(path);
			elem.setPath(fInfo.absoluteFilePath());

			elem.setFlags(VFSElement::eFl_SelfMade);

			vfsCache->insert(elem);
		}

		return res;
	}

	RESULT VkRVFSDriver::getAlbums(QList<VFSElement>& elements)
	{
		QString xmlResp;
		RESULT err = _httpConnector->getAlbums(xmlResp);
		if(!err)
		{
			QString pattern = QString::fromAscii("<album>(.*)</album>");
			QRegExp rx(pattern);
			rx.setCaseSensitivity(Qt::CaseSensitive);
			rx.setMinimal(true);
			rx.setPatternSyntax(QRegExp::RegExp);

			int pos = 0;
			while((pos = rx.indexIn(xmlResp, pos)) != -1)
			{
				VFSElement elem;
				QString cap(rx.cap(1));

				parseAlbumEntry(cap, elem);

				elem.setDownloaded(false);
				elem.setParentId(ROOT_ID);

				QString pluginStoragePath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
				QString path = pluginStoragePath + QString(QDir::separator()) + elem.getName();
				QFileInfo fInfo(path);
				elem.setPath(fInfo.absoluteFilePath());

				elements.append(elem);

				pos += rx.matchedLength();
			}
		}

		if(err != eERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	int VkRVFSDriver::findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem)
	{
		int res = -1;
		for(int i=0; i<elemList.count(); i++)
		{
			if(elemList[i].getId() == elem.getParentId())
			{
				res = i;
				break;
			}
		}

		return res;
	}

	RESULT VkRVFSDriver::getPhotos(QList<VFSElement>& elements)
	{
		RESULT err = eERROR;
		int offset = 0;
		forever
		{
			int count = 0; 
			QString xmlResp = "";
			err = _httpConnector->getPhotos(offset, xmlResp);
			if(!err)
			{
				count = RegExp::getByPattern("<count>(.*)</count>", xmlResp).toInt();

				QString pattern = QString::fromAscii("<photo>(.*)</photo>");
				QRegExp rx(pattern);
				rx.setCaseSensitivity(Qt::CaseSensitive);
				rx.setMinimal(true);
				rx.setPatternSyntax(QRegExp::RegExp);

				int pos = 0;
				while((pos = rx.indexIn(xmlResp, pos)) != -1)
				{
					VFSElement elem;
					QString cap(rx.cap(1));

					elem.setDownloaded(false);
					parsePhotoEntry(cap, elem);

					QString path;
					int i = findParentIndex(elements, elem);
					if(i < 0)
					{
						elem.setParentId(ROOT_ID);
						QString rootPath = path = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator());
						path = rootPath + QString(QDir::separator()) + elem.getName();
					}
					else
					{
						path = elements[i].getPath() + QString(QDir::separator()) + elem.getName();
					}

					QFileInfo fInfo(path);
					elem.setPath(fInfo.absoluteFilePath());

					elements.append(elem);

					pos += rx.matchedLength();
				}
			}

			offset +=100;
			if(offset >= count)
				break;
		}

		if(err != eERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	RESULT VkRVFSDriver::getElements()
	{
		QList<VFSElement> elements;

		VFSElement elem = VFSElement(VFSElement::DIRECTORY
									, _pluginName
									, "ROOT"
									, ""
									, ""
									, ""
									, ROOT_ID
									, ROOT_ID
									, ""
									, _pluginName);

		elements.append(elem);

		RESULT res = getAlbums(elements);
		if(!res)
		{
			res = getPhotos(elements);
		}

		VFSCache* vfsCache = WebMounter::getCache();
		vfsCache->flush();

		for(int i=0; i<elements.count(); i++)
		{
			vfsCache->insert(elements[i]);
		}

		return res;
	}

	void VkRVFSDriver::parseAlbumEntry(QString& xmlEntry, VFSElement& elem)
	{
		elem.setType(VFSElement::DIRECTORY);
		elem.setPluginName(_pluginName);

		QString title = QString::fromAscii("<title>(.*)</title>");
		QRegExp rx(title);
		rx.setCaseSensitivity(Qt::CaseSensitive);
		rx.setMinimal(true);
		rx.setPatternSyntax(QRegExp::RegExp);

		rx.indexIn(xmlEntry);
		elem.setName(rx.cap(1));

		QString url = QString::fromAscii("http://vkontakte.ru/album");

		rx.setPattern("<owner_id>(.*)</owner_id>");
		rx.indexIn(xmlEntry);
		url += rx.cap(1);

		rx.setPattern("<aid>(.*)</aid>");
		rx.indexIn(xmlEntry);
		elem.setId(rx.cap(1));
		url += QString::fromAscii("_") + rx.cap(1);

		elem.setSrcUrl(url);
	}

	void VkRVFSDriver::parsePhotoEntry(QString& xmlEntry, VFSElement& elem)
	{
		elem.setType(VFSElement::FILE);
		elem.setPluginName(_pluginName);

		QString pid = QString::fromAscii("<pid>(.*)</pid>");
		QRegExp rx(pid);
		rx.setCaseSensitivity(Qt::CaseSensitive);
		rx.setMinimal(true);
		rx.setPatternSyntax(QRegExp::RegExp);

		rx.indexIn(xmlEntry);
		elem.setId(rx.cap(1));

		rx.setPattern("<aid>(.*)</aid>");
		rx.indexIn(xmlEntry);
		elem.setParentId(rx.cap(1));

		rx.setPattern("<src_big>(.*)</src_big>");
		rx.indexIn(xmlEntry);
		elem.setSrcUrl(rx.cap(1));

		QString url = elem.getSrcUrl();
		int pos = url.lastIndexOf("/");
		elem.setName(url.right(url.length() - pos - 1));
	}

	void VkRVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
	{
		Ui::NotificationDevice* notifDevice = WebMounter::getNotificationDevice();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT VkRVFSDriver::sync()
	{
		RESULT res = eERROR;
		SettingStorage* settings = WebMounter::getSettingStorage();
		QString pluginStoragePath = settings->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
		QFileInfo fInfo(pluginStoragePath);
                unsigned int uNotDownloaded = 0;
		PluginSettings plSettings;
		settings->getData(plSettings, _pluginName);

		_driverMutex.lock();
		if(_state != eSyncStopping)
		{
			updateState(0, RemoteDriver::eSync);
		}
		_driverMutex.unlock();

		QList<VFSElement> elements;

		VFSElement elem = VFSElement(VFSElement::DIRECTORY
										, fInfo.absoluteFilePath()
										, "ROOT"
										, ""
										, ""
										, ""
										, ROOT_ID
										, ROOT_ID
										, ""
										, _pluginName);

		elements.append(elem);

		// Handle albums
		res = getAlbums(elements);
		if(!res)
		{
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState(10, RemoteDriver::eSync);
			}
			_driverMutex.unlock();

			res = getPhotos(elements);
			if(!res)
			{
				QDir qDir;
				QFile qFile;

				uNotDownloaded = countNotDownloaded();
				VFSCache* vfsCache = WebMounter::getCache();
				VFSCache::iterator iter = vfsCache->begin();
				
				_driverMutex.lock();
				if(_state != eSyncStopping)
				{
					updateState(50, RemoteDriver::eSync);
				}
				_driverMutex.unlock();
				
				for(iter; iter != vfsCache->end(); ++iter)
				{
					if(iter->getPluginName() == _pluginName)
					{
						bool found = false;
						int count = elements.count();
						// Try to find element in new data
						for(int i=0; i<count; i++)
						{
							if(iter->getId() == elements[i].getId())
							{
								found = true;
								if(iter != elements[i])
								{
									if(iter->getType() == VFSElement::DIRECTORY)
									{
										bool err = qDir.rename(iter->getPath(), elements[i].getPath());
									}
									break;
								}

								elements.removeAt(i);
								break;
							}
						}

						// Element has been deleted
						if(!found)
						{
							vfsCache->erase(iter);
							iter = vfsCache->begin();
						}
					}
				}

				// Add newly created elements
				for(int i=0; i<elements.count(); i++)
				{
					vfsCache->insert(elements[i]);
				}

				QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
				QFileInfo fInfo(rootPath);
				syncCacheWithFileSystem(fInfo.absoluteFilePath());

				PluginSettings plSettings;
				settings->getData(plSettings, _pluginName);

				if(plSettings.bFullSync)
				{
					res = downloadFiles();
				}

				_driverMutex.lock();
				if(_state != eSyncStopping)
				{
					updateState(100, eConnected);
				}
				_driverMutex.unlock();
				vfsCache->flush();
			}
		}
		if(res == eERROR)
		{
			stopPlugin();

			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Sync failed !\n"));
			//updateState(100, RemoteDriver::eNotConnected);
		}

		return res;
	}

	void VkRVFSDriver::run()
	{
		PluginSettings pluginSettings;

		WebMounter::getSettingStorage()->getData(pluginSettings, _pluginName);

		int sync_period = pluginSettings.syncPeriod.toInt();

		forever
		{
			if(_state == eSyncStopping || _state == eNotConnected)
			{
				return;
			}

			if(_state == eAuthorized)
			{
				sync();
				if(_state == eSyncStopping)
				{
					return;
				}
			}

			_syncMutex.lock();

			bool result = _forceSync.wait(&_syncMutex, sync_period * 1000);

			_syncMutex.unlock();

			if(_state == eConnected)
			{
				sync();
			}

			if(_state == eSyncStopping)
			{
				return;
			}
		}
	}


        void VkRVFSDriver::updateDownloadStatus(RESULT downloadResult, const unsigned int uDownloaded, const unsigned int uNotDownloaded)
	{
		if(!downloadResult)
		{
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState((int)(((float)uDownloaded/uNotDownloaded)*50) + 50, RemoteDriver::eSync);
			}
			_driverMutex.unlock();
		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Downloading failed  !\n"));
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState(100, RemoteDriver::eNotConnected);
			}
			_driverMutex.unlock();
		}
	}

	void VkRVFSDriver::connectHandler(PluginSettings& pluginSettings)
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == RemoteDriver::eNotConnected)
		{
			GeneralSettings generalSettings; 

			WebMounter::getSettingStorage()->getData(generalSettings);
			WebMounter::getSettingStorage()->addSettings(pluginSettings);

			_httpConnector->setSettings(
				pluginSettings.userName
				, pluginSettings.userPassword
				, generalSettings.proxyAddress
				, generalSettings.proxyLogin + ":" + generalSettings.proxyPassword
 				, pluginSettings.isOAuthUsing
				, pluginSettings.oAuthToken
				);

			if(_state != eSyncStopping)
			{
				updateState(0, eAuthInProgress);
			}

			if(pluginSettings.isOAuthUsing && pluginSettings.oAuthToken != "")
			{
				updateState(100, eAuthorized);

				start(); // run sync thread				
			}
			else
			{
				updateState(0, eAuthInProgress);
			}
		}
	}

	void VkRVFSDriver::connectHandlerStage2(RESULT error, PluginSettings pluginSettings)
	{
		if(error == eNO_ERROR && _state == eAuthInProgress)
		{
			GeneralSettings generalSettings; 

			WebMounter::getSettingStorage()->getData(generalSettings);
			WebMounter::getSettingStorage()->addSettings(pluginSettings);

 			_httpConnector->setSettings(
 				pluginSettings.userName
 				, pluginSettings.userPassword
 				, generalSettings.proxyAddress
 				, generalSettings.proxyLogin + ":" + generalSettings.proxyPassword
 				, pluginSettings.isOAuthUsing
				, pluginSettings.oAuthToken
 				);
			if(_state != eSyncStopping)
			{
				updateState(100, eAuthorized);
			}

			start(); // run sync thread
		}
		else
		{
			updateState(100, eNotConnected);

			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Authorization failed !\n"
				"Please check proxy settings on Configuration tab and check settings on corresponding plugin tab...\n"));
		}
	}

	void VkRVFSDriver::disconnectHandler()
	{
		_driverMutex.lock();

		if(isRunning()) //if sync thread is running 
		{
			updateState(40, eSyncStopping);
			_driverMutex.unlock();

			_forceSync.wakeAll(); // we have to wake up the sync thread for safely termination

			while(isRunning()) // wait thread termination 
			{
				sleep(1);
			}

			//WebMounter::getCache()->restoreCache(); // restore cache from DB
		}
		else
		{
			_driverMutex.unlock();	
		}

		updateState(100, eNotConnected);
	}

	void VkRVFSDriver::syncHandler()
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	}

	void VkRVFSDriver::stopSyncHandler()
	{
		if(isRunning()) //if sync thread is running 
		{
			PluginSettings pluginSettings;
			WebMounter::getSettingStorage()->getData(pluginSettings, _pluginName);

			updateState(40, eSyncStopping);

			_forceSync.wakeAll();

			while(isRunning())
			{
				sleep(1);
			}

			updateState(80, eSyncStopping);

			//WebMounter::getCache()->restoreCache(); // restore cache from DB
		}
		updateState(100, eConnected);
		start(); // start sync thread again
	}

	QString VkRVFSDriver::addPathSuffix(ElementType type, const QString& path, const QString& suffix)
	{
		QString res;
		int ind = path.lastIndexOf(".");
		if(ind > 0 && type == VFSElement::FILE)
		{
			res = path.left(ind) + "_" + suffix + path.right(path.length() - ind);
		}
		else
		{
			res = path + "_" + suffix;
		}

		return res;
	}

	void VkRVFSDriver::markNameDuplicates(QList<VFSElement>& elemList)
	{
		for(int i=0; i<elemList.count(); i++)
		{
			for(int k=0; k<elemList.count(); k++)
			{
				if(!(elemList[i].getFlags()&VFSElement::eFl_NameDup) 
					&& (i!=k) && (elemList[i].getName() == elemList[k].getName()) 
					&& (elemList[i].getParentId() == elemList[k].getParentId()))
				{
					elemList[i].setFlags(VFSElement::eFl_NameDup);
				}
			}
		}
	}

	void VkRVFSDriver::handleNameDuplicates(QList<VFSElement>& elemList)
	{
		for(int i=0; i<elemList.count(); i++)
		{
			for(int k=0; k<elemList.count(); k++)
			{
				if(!(elemList[i].getFlags()&VFSElement::eFl_NameDup) 
					&& (i!=k) && (elemList[i].getName() == elemList[k].getName()) 
					&& (elemList[i].getParentId() == elemList[k].getParentId()))
				{
					elemList[i].setFlags(VFSElement::eFl_NameDup);
					elemList[i].setPath(addPathSuffix(elemList[i].getType(), elemList[i].getPath(), elemList[i].getId()));
				}
			}
		}

		for(int i=0; i<elemList.count(); i++)
		{
			for(int k=0; k<elemList.count(); k++)
			{
				if((i!=k) && (elemList[i].getPath() == elemList[k].getPath()))
				{
					elemList[i].setFlags(VFSElement::eFl_NameDup);
					elemList[i].setPath(addPathSuffix(elemList[i].getType(), elemList[i].getPath(), elemList[i].getId()));
					for(int j=0; j<elemList.count(); j++)
					{
						if(elemList[j].getParentId() == elemList[i].getId())
						{
							QString path = elemList[i].getPath() + QString(QDir::separator()) + elemList[j].getName();
							QFileInfo fInfo(path);
							elemList[j].setPath(fInfo.absoluteFilePath());
						}
					}
				}
			}
		}
	}

	bool VkRVFSDriver::areFileAttributesValid(const QString& path, unsigned long attributes)
	{
                return true;///////(attributes & FILE_ATTRIBUTE_READONLY);
	}
}
