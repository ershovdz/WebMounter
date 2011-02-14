#include "WebMounter.h"
#include "plugins/vk/driver/vk_driver.h"
#include "reg_exp.h"

namespace RemoteDriver
{
	using namespace Common;
	using namespace Connector;

	VkRVFSDriver::VkRVFSDriver(const QString& pluginName) : _pluginName(pluginName), _isDemo(true)
	{
		_state = RemoteDriver::eNotConnected;
		_httpConnector = new VkHTTPConnector();

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}

		//connect(this, SIGNAL(fileUploaded(/*QString, RESULT*/)), WebMounter::getProxy(), SLOT(FileProxy::fileUploaded1(/*QString, RESULT*/)));
	}

	void VkRVFSDriver::syncCacheWithFileSystem(const QString& path)
	{
		VFSCache* vfsCache = WebMounter::getCache();

		QDir dir(path);
		QStringList lstDirs = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
		QStringList lstFiles = dir.entryList(QDir::Files);

		foreach (QString entry, lstFiles)
		{
			QString entryAbsPath = dir.absolutePath() + "/" + entry;
			if(vfsCache->find(entryAbsPath) == vfsCache->end())
			{
				QFile::remove(entryAbsPath);
			}
		}

		foreach (QString entry, lstDirs)
		{
			QString entryAbsPath = dir.absolutePath() + "/" + entry;

			if(vfsCache->find(entryAbsPath) == vfsCache->end())
			{
				removeFolder(QDir(entryAbsPath));
			}
			else
			{
				syncCacheWithFileSystem(entryAbsPath);
			}
		}

		for(VFSCache::iterator iter = vfsCache->begin(); iter != vfsCache->end(); ++iter)
		{
			if(iter->getType() == VFSElement::DIRECTORY)
			{
				if(!dir.exists(iter->getPath())) 
					dir.mkdir(iter->getPath());
			}
			else if(iter->getType() == VFSElement::FILE)
			{
				if(!QFile::exists(iter->getPath()))
				{
					QFile file(iter->getPath());
					file.open(QIODevice::WriteOnly);
					file.close();
				}
			}
		}
	}

	VkRVFSDriver::~VkRVFSDriver(void)
	{
		delete _httpConnector;
	}

	RESULT VkRVFSDriver::downloadFile(const QString& url, const QString& path)
	{
		RESULT res = _httpConnector->downloadFile(url, path);
		if(res == eNO_ERROR)
		{
			QFile::Permissions permissions = QFile::permissions(path);
			permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
			QFile::setPermissions(path, permissions);
		}
		return res;
	}

	RESULT VkRVFSDriver::downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList, UINT& uDownloaded, UINT& uNotDownloaded)
	{
		RESULT res = _httpConnector->downloadFile(urlList, pathList);
		VFSCache* cache = WebMounter::getCache();
		if(res == eNO_ERROR)
		{
			for(int i = 0; i < pathList.size(); i++)
			{
				QFile::Permissions permissions = QFile::permissions(pathList.at(i));
				permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
				QFile::setPermissions(pathList.at(i), permissions);

				QFileInfo fInfo(pathList.at(i));
				VFSCache::iterator iter = cache->find(fInfo.absoluteFilePath());

				if(iter != cache->end())
				{
					if(!iter->isDownloaded())
					{
						cache->setDownloaded(iter, true);
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

		if(_isDemo)
		{
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
		}

		if(count < DEMO_PHOTOS_PER_ALBUM_LIMIT)
		{
			QString xmlResp;
			UINT retryUploadCounter = 0;
			
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

	RESULT VkRVFSDriver::renameElement(const QString& id, ElementType type, const QString& newTitle)
	{
		RESULT res = eERROR;
		if(type == VFSElement::FILE)
		{
			return res; // Renaming of photos is not supported by Vkontakte
		}

		return _httpConnector->renameAlbum(id, newTitle);
	}

	RESULT VkRVFSDriver::deleteDirectory(const QString& id)
	{
		return _httpConnector->deleteAlbum(id);
	}

	RESULT VkRVFSDriver::deleteFile(const QString& id)
	{
		RESULT res = eERROR;
		res = _httpConnector->deleteFile(id);
		if(res == eNO_ERROR)
		{
			VFSCache* vfsCache = WebMounter::getCache();
			VFSCache::iterator iter = vfsCache->begin();
			for(iter; iter != vfsCache->end(); ++iter)
			{
				if(iter->getId() == id)
				{
					QFile::Permissions permissions = QFile::permissions(iter->getPath());
					permissions |= (QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
					bool err = QFile::setPermissions(iter->getPath(), permissions);

					break;
				}
			}
		}
		return res;
	}

	RESULT VkRVFSDriver::moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type)
	{
		if(type == VFSElement::DIRECTORY)
		{
			return eERROR; // Moving of albums is not supported
		}

		return _httpConnector->moveFile(id, oldParentId, newParentId);
	}

	RESULT VkRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		RESULT res = eERROR;
		int count = 0;
		if(_isDemo)
		{
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
		}

		if(count < DEMO_ALBUMS_LIMIT)
		{
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
		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL, RemoteDriver::VkRVFSDriver::tr("Error"),RemoteDriver::VkRVFSDriver::tr("Creation limit is reached! \nActivation key required.\n"));
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

				parseAlbumEntry(rx.cap(1), elem);

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
					elem.setDownloaded(false);
					parsePhotoEntry(rx.cap(1), elem);

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

		elem.setOrigUrl(url);
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
		elem.setOrigUrl(rx.cap(1));

		QString url = elem.getOrigUrl();
		int pos = url.lastIndexOf("/");
		elem.setName(url.right(url.length() - pos - 1));
	}

	void VkRVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
	{
		Ui::ControlPanel* notifDevice = WebMounter::getControlPanel();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT VkRVFSDriver::sync()
	{
		RESULT res = eERROR;
		SettingStorage* settings = WebMounter::getSettingStorage();
		QString pluginStoragePath = settings->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
		QFileInfo fInfo(pluginStoragePath);
		UINT uNotDownloaded = 0;

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
							vfsCache->erase(iter--);
						}
					}
				}

				if(!_isDemo) 
				{ // In demo mode there is no way to create elements outside the WebMounter 
					// Add newly created elements
					for(int i=0; i<elements.count(); i++)
					{
						vfsCache->insert(elements[i]);
					}
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

	RESULT VkRVFSDriver::downloadFiles()
	{
		QList <QString> urlToDownload;
		QList <QString> pathToDownload;
		VFSCache* vfsCache = WebMounter::getCache();
		VFSCache::iterator iter = vfsCache->begin();
		UINT uNotDownloaded = countNotDownloaded();
		UINT uDownloaded = 0;
		RESULT err;
		for(iter = vfsCache->begin(); iter != vfsCache->end(); ++iter)
		{
			if(this->_state == eSyncStopping)
			{
				notifyUser(Ui::Notification::eINFO, tr("Info"), tr("Synchronization is stopped !\n"));
				//updateState(100, RemoteDriver::eConnected);
				return eNO_ERROR;
			}

			if(iter->getPluginName() == _pluginName)
			{
				if(iter->getType() == VFSElement::FILE && !iter->isDownloaded())
				{
					QFile::remove(iter->getPath());

					urlToDownload.append(iter->getOrigUrl());
					pathToDownload.append(iter->getPath());

					if(urlToDownload.size() == DOWNLOAD_CHUNK_SIZE)
					{
						err = downloadFiles(urlToDownload, pathToDownload, uDownloaded, uNotDownloaded);
						UINT retryDownloadCounter = 0;
						while(err == eERROR && retryDownloadCounter < MAX_DOWNLOAD_RETRY)
						{
							err = downloadFiles(urlToDownload, pathToDownload, uDownloaded, uNotDownloaded);
							retryDownloadCounter++;
						}

						if(err)
						{
							updateDownloadStatus(err, uDownloaded, uNotDownloaded);
							return err;
						}
						else
						{
							uDownloaded += urlToDownload.size();
							updateDownloadStatus(err, uDownloaded, uNotDownloaded);
							urlToDownload.clear();
							pathToDownload.clear();
						}
					}
				}
			}
		}

		if(urlToDownload.size() > 0)
		{
			return downloadFiles(urlToDownload, pathToDownload, uDownloaded, uNotDownloaded);
		}

		return err;
	}

	void VkRVFSDriver::updateDownloadStatus(RESULT downloadResult, const UINT uDownloaded, const UINT uNotDownloaded)
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

			_isDemo = (checkKey(pluginSettings) == eERROR);

			_httpConnector->setSettings(
				pluginSettings.userName
				, pluginSettings.userPassword
				, generalSettings.proxyAddress
				, generalSettings.proxyLogin + ":" + generalSettings.proxyPassword
				);

			if(_state != eSyncStopping)
			{
				updateState(0, eAuthInProgress);
			}

			if(_httpConnector->auth() == eNO_ERROR)
			{
				if(_state != eSyncStopping)
				{
					updateState(100, eAuthorized);
				}

				start(); // run sync thread
			}
			else
			{
				//disconnectHandler();
				updateState(100, eNotConnected);

				notifyUser(Ui::Notification::eCRITICAL,  RemoteDriver::VkRVFSDriver::tr("Error"),  RemoteDriver::VkRVFSDriver::tr("Authorization failed !\n"
					"Please check proxy settings on Configuration tab and check settings on corresponding plugin tab...\n"));
			}
		}
	}

	UINT VkRVFSDriver::countNotDownloaded()
	{
		VFSCache* cache = WebMounter::getCache();
		UINT counter = 0;

		for(VFSCache::iterator iter = cache->begin(); 
			iter != cache->end(); 
			++iter)
		{
			if( iter->getPluginName() == _pluginName && !iter->isDownloaded() && iter->getType() == VFSElement::FILE)
			{
				counter++;
			}
		}

		return counter;
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

	int VkRVFSDriver::removeFolder(QDir& dir)
	{
		int res = 0;

		QStringList lstDirs  = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
		QStringList lstFiles = dir.entryList(QDir::Files);

		foreach (QString entry, lstFiles)
		{
			QString entryAbsPath = dir.absolutePath() + "/" + entry;
			QFile::remove(entryAbsPath);
		}

		foreach (QString entry, lstDirs)
		{
			QString entryAbsPath = dir.absolutePath() + "/" + entry;
			removeFolder(QDir(entryAbsPath));
		}

		if (!QDir().rmdir(dir.absolutePath()))
		{
			res = 1;
		}
		return res;
	}

	RESULT VkRVFSDriver::checkKey(const PluginSettings& pluginSettings)
	{
		QCryptographicHash md5Hash(QCryptographicHash::Md5);
		QCryptographicHash sha1Hash(QCryptographicHash::Sha1);

		QString url = pluginSettings.serverUrl;

		// Let's clear an url
		url = url.simplified();
		url = url.toLower();
		url.remove(QRegExp("http://"));

		if(url.endsWith("/"))
		{
			url.chop(1);
		}

		md5Hash.addData(QByteArray(qPrintable(pluginSettings.userName + "Sasha198779_")));
		sha1Hash.addData(md5Hash.result().toHex().data());

		QString validKey(sha1Hash.result().toHex().data());
		validKey = validKey.mid(0, 7);

		if(validKey == pluginSettings.key)
		{
			return eNO_ERROR;
		}
		else
		{
			return eERROR;
		}
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
		return (attributes & FILE_ATTRIBUTE_READONLY);
	}
}
