//#include <QRegExp>

#include "yanarod_driver.h"
//#include <webmounter.h>

//namespace RemoteDriver
//{
	using namespace Common;
//	using namespace Connector;
	
	YaNarodRVFSDriver::YaNarodRVFSDriver(const QString& pluginName)
	{
//		_state = RemoteDriver::eNotConnected;
//		_httpConnector = new YaNarodHTTPConnector;
//		_pluginName = pluginName;

	/*	RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	*/
	}

	YaNarodRVFSDriver::~YaNarodRVFSDriver(void)
	{
//		delete _httpConnector;
	}

	RESULT YaNarodRVFSDriver::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
	{
/*		VFSCache* vfsCache = WebMounter::getCache();
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
		
		{
			LOCK(_driverMutex)
		
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
	*/
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodRVFSDriver::uploadFile(const QString& path, const QString& title,  const QString& Id, const QString& parentId)
	{
/*		QString xmlResp;
		QString id = ROOT_ID;
		QFileInfo fInfo(path);

		RESULT err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
		unsigned int retryUploadCounter = 0;

		while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
		{
			err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
			retryUploadCounter++;
		}
		
		VFSElement elem;
		if(!err && xmlResp != "")
		{
			QString pattern = QString::fromAscii("image_id=(.*)&");
			QRegExp rx(pattern);
			rx.setCaseSensitivity(Qt::CaseSensitive);
			rx.setMinimal(true);
			rx.setPatternSyntax(QRegExp::RegExp);

			rx.indexIn(xmlResp);
			id = rx.cap(1);
			if(id != "")
			{
				VFSCache* vfsCache = WebMounter::getCache();
				elem = VFSElement(VFSElement::FILE
									, fInfo.absoluteFilePath()
									, title
									, ""
									, ""
									, ""
									, id
									, parentId
									, ""
									, _pluginName);

				vfsCache->insert(elem);

				QFile::Permissions permissions = QFile::permissions(elem.getPath());
				permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
				bool err = QFile::setPermissions(elem.getPath(), permissions);
			}
		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL
				, tr("Error")
				, tr("File upload failed (") + elem.getName() + QString(")"));
		}

		WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), err);
*/
		RESULT err = eERROR;
		return err;
	}

	RESULT YaNarodRVFSDriver::modifyFile(const QString&)
	{
		return eERROR;
	}

	RESULT YaNarodRVFSDriver::renameElement(const QString& path, const QString& newTitle)
	{
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodRVFSDriver::deleteDirectory(const QString& path)
	{
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodRVFSDriver::deleteFile(const QString& path)
	{
		RESULT res = eERROR;
/*		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = _httpConnector->deleteFile("photo/" + elem->getId() + "/", response);
			if(res == eNO_ERROR)
			{
				WebMounter::getProxy()->fileDeleted(elem->getPath(), res);
				cache->erase(elem);
			}
		}
*/
		return res;
	}

	RESULT YaNarodRVFSDriver::moveElement(const QString& path, const QString& newParentId)
	{
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		RESULT err = eERROR;
		return err;
	}

	RESULT YaNarodRVFSDriver::getFiles(QList<RemoteDriver::VFSElement>& elements)
	{
		RESULT err = eERROR;
		return err;
	}

	void YaNarodRVFSDriver::parseEntry(QString& xmlEntry, RemoteDriver::VFSElement& elem)
	{
	}

	RESULT YaNarodRVFSDriver::sync()
	{
		RESULT res = eERROR;
/*		SettingStorage* settings = WebMounter::getSettingStorage();
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
		res = getFiles(elements);
		if(!res)
		{
			QDir qDir;
			QFile qFile;

			uNotDownloaded = elements.count();
			VFSCache* vfsCache = WebMounter::getCache();
			VFSCache::iterator iter = vfsCache->begin();
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
						VFSCache::iterator iter1 = iter--;
						vfsCache->erase(iter1);
					}
				}
			}

			{ 	LOCK(_driverMutex);
				if(_state != eSyncStopping)
				{
					updateState(30, RemoteDriver::eSync);
				}
			}

			// Add newly created elements
			for(int i=0; i<elements.count(); i++)
			{
				vfsCache->insert(elements[i]);
			}

			{ 	LOCK(_driverMutex);
				if(_state != eSyncStopping)
				{
					updateState(40, RemoteDriver::eSync);
				}
			}

			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			syncCacheWithFileSystem(fInfo.absoluteFilePath());

			{ 	LOCK(_driverMutex);
				if(_state != eSyncStopping)
				{
					updateState(50, RemoteDriver::eSync);
				}
			}

			if(plSettings.bFullSync)
			{
				res = downloadFiles();
			}
		}

		if(res == eERROR)
		{
			stopPlugin();
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Sync failed !\n"));
			//updateState(100, RemoteDriver::eNotConnected);
		}
		else
		{
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState(100, eConnected);
			}
			_driverMutex.unlock();
		}
	*/
		return res;
	}

	void YaNarodRVFSDriver::run()
	{
/*		PluginSettings pluginSettings;

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
*/
	}

	void YaNarodRVFSDriver::connectHandler(RemoteDriver::PluginSettings& pluginSettings)
	{
//		QMutexLocker locker(&_driverMutex);

/*		if(_state == RemoteDriver::eNotConnected)
		{
			GeneralSettings generalSettings; 

			WebMounter::getSettingStorage()->getData(generalSettings);
			WebMounter::getSettingStorage()->addSettings(pluginSettings);

 			_httpConnector->setSettings(
 				pluginSettings.userName
 				, pluginSettings.userPassword
 				, generalSettings.proxyAddress
				, generalSettings.proxyLogin + ":" + generalSettings.proxyPassword);

			if(_state != eSyncStopping)
			{
				updateState(0, eAuthInProgress);
			}

			if(!pluginSettings.isOAuthUsing)
			{
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
 					updateState(100, eNotConnected);
 
 					notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Authorization failed !\n"
 						"Please check proxy settings on Configuration tab and check settings on corresponding plugin tab...\n"));
 				}
			}		
		}
		*/
	}

	void YaNarodRVFSDriver::disconnectHandler()
	{
	/*	_driverMutex.lock();

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
		*/
	}

	void YaNarodRVFSDriver::syncHandler()
	{
	/*	QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	*/
	}

	void YaNarodRVFSDriver::stopSyncHandler()
	{
	/*	if(isRunning()) //if sync thread is running
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
		*/
	}

	bool YaNarodRVFSDriver::areFileAttributesValid(const QString& path, unsigned long attributes)
	{
                return true;///////////(attributes & FILE_ATTRIBUTE_READONLY);
	}
//}
