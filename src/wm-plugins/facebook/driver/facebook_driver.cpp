#include "webmounter.h"
#include "../driver/facebook_driver.h"
#include "reg_exp.h"

namespace RemoteDriver
{
	using namespace Common;
	using namespace Connector;

	FacebookRVFSDriver::FacebookRVFSDriver(const QString& pluginName)
	{
		_state = RemoteDriver::eNotConnected;
		_httpConnector = new FacebookHTTPConnector();
		_pluginName = pluginName;

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	}

	FacebookRVFSDriver::~FacebookRVFSDriver(void)
	{
		delete _httpConnector;
	}

	RESULT FacebookRVFSDriver::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
	{
		VFSCache* vfsCache = WebMounter::getCache();
		int initialListSize = urlList.size();

		{ LOCK(_driverMutex)

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
				return eERROR_GENERAL;
			}
		}
		}

		RESULT res = _httpConnector->downloadFiles(urlList, pathList);
		if(res != eNO_ERROR)
		{
			unsigned int retryDownloadCounter = 0;
			while(res != eNO_ERROR && retryDownloadCounter < MAX_DOWNLOAD_RETRY)
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
					if(res != eNO_ERROR)
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

	RESULT FacebookRVFSDriver::uploadFile(const QString& path, const QString& title, const QString& id, const QString& parentId)
	{
		QVariant resp;
		QFileInfo fInfo(path);

		QString suffix = fInfo.suffix();
		suffix = suffix.toLower();

		if(suffix != "jpg" && suffix != "jpeg" && suffix != "png" && suffix != "gif" && suffix != "bmp")
		{
			notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
			return eERROR_GENERAL;
		}
		else if(id != ROOT_ID)
		{
			notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file will be changed locally!\n"));
			return eERROR_GENERAL;
		}
		else if (fInfo.size() == 0)
		{
			return eNO_ERROR;
		}

		RESULT err = _httpConnector->uploadFile(path, title, parentId, resp);
		unsigned int retryUploadCounter = 0;

		while(err != eNO_ERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
		{
			err = _httpConnector->uploadFile(path, title, parentId, resp);
			retryUploadCounter++;
		}

		VFSElement elem;
		if(!err && resp != "")
		{
			QVariantMap map = resp.toMap();
			VFSCache* vfsCache = WebMounter::getCache();
			elem = VFSElement(VFSElement::FILE
				, fInfo.absoluteFilePath()
				, ""
				, ""
				, ""
				, map["source"].toString()
				, id
				, parentId
				, ""
				, _pluginName);

			QString url = map["source"].toString();
			int pos = url.lastIndexOf("/");
			elem.setName(url.right(url.length() - pos - 1));

			fInfo.setFile(fInfo.absolutePath() + QString(QDir::separator()) + elem.getName());

			elem.setPath(fInfo.absoluteFilePath());

			elem.setFlags(VFSElement::eFl_SelfMade);

			vfsCache->insert(elem);

			QDir dir;
			dir.rename(path, elem.getPath());

			QFile::Permissions permissions = QFile::permissions(elem.getPath());
			permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
			QFile::setPermissions(elem.getPath(), permissions);

		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL
				, tr("Error")
				, tr("File upload failed (") + elem.getName() + QString(")"));
		}

		fInfo.setFile(path);
		WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), err);
		return err;
	}

	RESULT FacebookRVFSDriver::modifyFile(const QString&)
	{
		return eERROR_GENERAL;
	}

	RESULT FacebookRVFSDriver::renameElement(const QString& path, const QString& newTitle)
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		return res;
	}

	RESULT FacebookRVFSDriver::deleteDirectory(const QString& path)
	{
		RESULT res = eERROR_GENERAL;
		QDir qDirFrom(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qDirFrom.absolutePath());

		if(elem != cache->end() && elem->getType() == VFSElement::DIRECTORY)
		{
			res = _httpConnector->deleteObject(elem->getId());
			if(res == eNO_ERROR)
			{
				cache->erase(elem);
			}
		}
		return res;
	}

	RESULT FacebookRVFSDriver::deleteFile(const QString& path)
	{
		RESULT res = eERROR_GENERAL;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = res = _httpConnector->deleteObject(elem->getId());
			if(res == eNO_ERROR)
			{
				WebMounter::getProxy()->fileDeleted(elem->getPath(), res);
				cache->erase(elem);
			}
		}

		return res;
	}

	RESULT FacebookRVFSDriver::moveElement(const QString& path, const QString& newParentId)
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		return res;
	}

	RESULT FacebookRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		RESULT res = eERROR_GENERAL;
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
			return eERROR_GENERAL; // Creation of sub-albums is not supported
		}

		QVariant entry;
		res = _httpConnector->createDirectory(title, entry);
		if(!res)
		{
			VFSCache* vfsCache = WebMounter::getCache();
			VFSElement elem;
			parseAlbumEntry(entry, elem);

			elem.setParentId(ROOT_ID);

			QFileInfo fInfo(path);
			elem.setPath(fInfo.absoluteFilePath());

			elem.setFlags(VFSElement::eFl_SelfMade);

			vfsCache->insert(elem);
		}

		return res;
	}

	RESULT FacebookRVFSDriver::getAlbums(QList<VFSElement>& elements, int& errorCode)
	{
		QVariant resp;
		RESULT err = _httpConnector->getAlbums(resp, errorCode);
		if(!err)
		{
			QVariantMap map = resp.toMap();
			QVariantList list = map["data"].toList();
			for(int i = 0; i<list.count(); i++)
			{
				VFSElement elem;

				parseAlbumEntry(list[i], elem);

				elem.setDownloaded(false);
				elem.setParentId(ROOT_ID);

				QString pluginStoragePath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
				QString path = pluginStoragePath + QString(QDir::separator()) + elem.getName();
				QFileInfo fInfo(path);
				elem.setPath(fInfo.absoluteFilePath());

				elements.append(elem);
			}
		}

		if(err == eNO_ERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	int FacebookRVFSDriver::findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem)
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

	RESULT FacebookRVFSDriver::getPhotos(QList<VFSElement>& elements)
	{
		RESULT err = eERROR_GENERAL;
		int count = 0; 

		for(int i=0; i<elements.count(); i++)
		{
			if(elements[i].getPath().contains("Facebook/"))
			{
				err = getAlbumPhotos(elements[i], elements);
			}
		}

		if(err == eNO_ERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	RESULT FacebookRVFSDriver::getAlbumPhotos(const VFSElement& album, QList<VFSElement>& elements)
	{
		RESULT res = eERROR_GENERAL;
		QVariant resp;
		res = _httpConnector->getPhotos(album.getId(), resp);
		if(!res)
		{
			QVariantMap map = resp.toMap();
			QVariantList list = map["data"].toList();
			for(int k = 0; k<list.count(); k++)
			{
				VFSElement elem;

				parsePhotoEntry(list[k], elem);

				elem.setDownloaded(false);
				elem.setParentId(album.getId());

				QString path = album.getPath() + QString(QDir::separator()) + elem.getName();
				QFileInfo fInfo(path);
				elem.setPath(fInfo.absoluteFilePath());

				elements.append(elem);
			}
		}

		return res;
	}

	RESULT FacebookRVFSDriver::getElements()
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		return res;
	}

	void FacebookRVFSDriver::parseAlbumEntry(QVariant& entry, VFSElement& elem)
	{
		elem.setType(VFSElement::DIRECTORY);
		elem.setPluginName(_pluginName);

		QVariantMap map = entry.toMap();
		elem.setName(map["name"].toString());
		elem.setSrcUrl(map["link"].toString());
		elem.setId(map["id"].toString());
	}

	void FacebookRVFSDriver::parsePhotoEntry(QVariant& entry, VFSElement& elem)
	{
		elem.setType(VFSElement::FILE);
		elem.setPluginName(_pluginName);

		QVariantMap map = entry.toMap();
		elem.setId(map["id"].toString());
		QVariantList imagesList = map["images"].toList();
		QVariantMap largeImg = imagesList[0].toMap();
		elem.setSrcUrl(largeImg["source"].toString());

		QString url = elem.getSrcUrl();
		int pos = url.lastIndexOf("/");
		elem.setName(url.right(url.length() - pos - 1));
	}

	void FacebookRVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
	{
		Ui::NotificationDevice* notifDevice = WebMounter::getNotificationDevice();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT FacebookRVFSDriver::sync()
	{
		RESULT res = eERROR_GENERAL;
		SettingStorage* settings = WebMounter::getSettingStorage();
		QString pluginStoragePath = settings->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
		QFileInfo fInfo(pluginStoragePath);
		unsigned int uNotDownloaded = 0;
		PluginSettings plSettings;
		settings->getData(plSettings, _pluginName);
		int percentage = 0;

		_driverMutex.lock();
		if(_state != eSyncStopping)
		{
			updateState(percentage, RemoteDriver::eSync);
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
		int errorCode = 0;
		res = getAlbums(elements, errorCode);
		if(!res)
		{
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				percentage += 10;
				updateState(percentage, RemoteDriver::eSync);
			}
			_driverMutex.unlock();

			percentage += 60/elements.count();
			for(int j=0; j<elements.count(); j++)
			{
				if(elements[j].getPath().contains("Facebook/"))
				{
					res = getAlbumPhotos(elements[j], elements);
					percentage += 60/elements.count();
					updateState(percentage, RemoteDriver::eSync);
				}
			}

			if(res == eNO_ERROR)
			{
				handleNameDuplicates(elements);
			}

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
					percentage+=10;
					updateState(percentage, RemoteDriver::eSync);
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
										qDir.rename(iter->getPath(), elements[i].getPath());
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
							VFSCache::iterator iter1 = iter;
							iter++;
							vfsCache->erase(iter1);
						}
					}
				}

				_driverMutex.lock();
				if(_state != eSyncStopping)
				{
					percentage+=10;
					updateState(percentage, RemoteDriver::eSync);
				}
				_driverMutex.unlock();

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

		if(res != eNO_ERROR)
		{
			/*if(errorCode == ERROR_AUTH_FAILED)
			{
			plSettings.oAuthToken="";
			settings->addSettings(plSettings);
			stopPlugin();
			startPlugin(plSettings);
			//updateState(0, RemoteDriver::eAuthInProgress);
			}
			else
			{*/
			stopPlugin();
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Sync failed !\n"));
			//}
		}

		return res;
	}

	void FacebookRVFSDriver::run()
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

			_forceSync.wait(&_syncMutex, sync_period * 1000);

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


	void FacebookRVFSDriver::updateDownloadStatus(RESULT downloadResult, const unsigned int uDownloaded, const unsigned int uNotDownloaded)
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

	void FacebookRVFSDriver::connectHandler(PluginSettings& pluginSettings)
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

			/*if(pluginSettings.isOAuthUsing && pluginSettings.oAuthToken != "")
			{
			updateState(100, eAuthorized);

			start(); // run sync thread				
			}
			else
			{
			updateState(0, eAuthInProgress);
			}*/
		}
	}

	void FacebookRVFSDriver::connectHandlerStage2(RESULT error, PluginSettings pluginSettings)
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

	void FacebookRVFSDriver::disconnectHandler()
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

	void FacebookRVFSDriver::syncHandler()
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	}

	void FacebookRVFSDriver::stopSyncHandler()
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

	QString FacebookRVFSDriver::addPathSuffix(ElementType type, const QString& path, const QString& suffix)
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

	void FacebookRVFSDriver::markNameDuplicates(QList<VFSElement>& elemList)
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

	void FacebookRVFSDriver::handleNameDuplicates(QList<VFSElement>& elemList)
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

	bool FacebookRVFSDriver::areFileAttributesValid(const QString& /*path*/, unsigned long /*attributes*/)
	{
		return true;///////(attributes & FILE_ATTRIBUTE_READONLY);
	}
}
