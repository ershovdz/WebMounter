#include "common_stuff.h"

#include "jmm_driver.h"
#include "WebMounter.h"

#include "plugins/jmm/connector/jmm_connector.h"
#include "plugins/jmm/xml/jmm_xml.h"

#include <QFile>
#include <QTime>
#include <QDir>

namespace RemoteDriver
{
	using namespace Common;

	//JmmRVFSDriver* JmmRVFSDriver::_driverInstance = 0;
	//QMutex JmmRVFSDriver::_driverMutex;

	JmmRVFSDriver::JmmRVFSDriver(const QString& pluginName) : _pluginName(pluginName)
	{
		_state = RemoteDriver::eNotConnected;

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	}

	JmmRVFSDriver::JmmRVFSDriver(const QString& pluginName
								, const QString& rootPath
								, const QString& url
								, const QString& login
								, const QString& password
								, const QString& proxy//"proxy.te.mera.ru:8080"
								, const QString& proxyLoginPwd) : _pluginName(pluginName)//:  _rootPath(rootPath)
	{
		_state = RemoteDriver::eNotConnected;
		
		RESULT res = WebMounter::getCache()->restoreCache();
		/*if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}*/
	}

	JmmRVFSDriver::~JmmRVFSDriver(void)
	{
		//Connector::JmmHTTPConnector::deleteConnector();
	}

	int JmmRVFSDriver::removeFolder(QDir& dir)
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
	
	void JmmRVFSDriver::syncCacheWithFileSystem(const QString& path)
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
				QFile file(iter->getPath());
				if(!file.exists() || file.size() == 0)
				{
					file.open(QIODevice::WriteOnly);
					file.close();
					vfsCache->setDownloaded(iter, false);
				}
			}
		}
	}

	RESULT JmmRVFSDriver::createFile(const QString& path, const QString& title,  const QString& id, const QString& parentId)
	{
		if(_pluginName == "Joomla.Article")
		{
			QString response;
			QString response_id;


			QFileInfo fInfo(path);
			QString suffix = fInfo.suffix();
			suffix = suffix.toLower();

			if(suffix != "txt" && suffix != "html" && suffix != "htm")
			{
				notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
				return eERROR;
			}
			
			if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
			{
				return eERROR;	
			}

			RESULT err = _httpConnector.uploadFile(path, title, id, parentId, response, _pluginName);

			UINT retryUploadCounter = 0;

			while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
			{
				err = _httpConnector.uploadFile(path, title, id, parentId, response, _pluginName);
				retryUploadCounter++;
			}

			if(!err && response != "")
			{
				response_id = Xml::JmmXmlParser::parseIdResponse(response);

				if(response_id != "" && response_id != ROOT_ID)
				{
					VFSCache* cache = WebMounter::getCache();
					QFileInfo fInfo(path);

					VFSElement elem (VFSElement::FILE
						, fInfo.absoluteFilePath()
						, title
						, ""
						, ""
						, response_id
						, parentId
						, "0000-00-00 00:00:00"
						, _pluginName
						, (VFSElement::eFl_Dirty | VFSElement::eFl_Downloaded) // we have uploaded the file, thus it's downloaded :)
						);

					cache->insert(elem);
				}
				else
				{
					err = eERROR;
				}
			}
			else
			{
				notifyUser(Ui::Notification::eCRITICAL
					, tr("Error")
					, tr("File creating failed (") + title + QString(")"));

				err = eERROR;
			}

			return err;
		}
		else
		{
			return eNO_ERROR;
		}
	}

	RESULT JmmRVFSDriver::downloadFile(const QString& url, const QString& path)
	{
		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Plugin is not connected !\nPlease connect plugin at first.\n"));
			return eERROR;
		}
		
		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}
		
		VFSCache* cache = WebMounter::getCache();
		QFileInfo fInfo(path);

		VFSCache::iterator iter = cache->find(fInfo.absoluteFilePath());

		if(iter != cache->end())
		{
			if(!iter->isDownloaded() && _httpConnector.downloadFile(url, path) == eNO_ERROR)
			{
				cache->setDownloaded(iter, true);
				return eNO_ERROR;
			}
		}
		return eERROR;
	};

	RESULT JmmRVFSDriver::uploadFile(const QString& path, const QString& title, const QString& id, const QString& parentid)
	{
		QString response;
		QString response_id;

		
		QFileInfo fInfo(path);
		QString suffix = fInfo.suffix();
		suffix = suffix.toLower();

		if(_pluginName == "Joomla.Article")
		{
			if(suffix != "txt" && suffix != "html" && suffix != "htm")
			{
				notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
				return eERROR;
			}
		}
		else
		{
			if(suffix != "jpg" && suffix != "jpeg" && suffix != "png" && suffix != "gif")
			{
				notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
				return eERROR;
			}
		}
		
		if (fInfo.size() == 0 && _pluginName != "Joomla.Article")
		{
			return eNO_ERROR;
		}

		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			notifyUser(Ui::Notification::eCRITICAL
				, tr("Error")
				, tr("File upload failed (") + title + QString(")"));
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eERROR);
			return eERROR;	
		}

		RESULT err = _httpConnector.uploadFile(path, title, id, parentid, response, _pluginName);

		UINT retryUploadCounter = 0;

		while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
		{
			err = _httpConnector.uploadFile(path, title, id, parentid, response, _pluginName);
			retryUploadCounter++;
		}

		if(!err && response != "")
		{
			response_id = Xml::JmmXmlParser::parseIdResponse(response);
			
			if(id == ROOT_ID) // upload new file
			{
				if(response_id != "" && response_id != ROOT_ID)
				{
					VFSCache* cache = WebMounter::getCache();
					
					VFSElement elem (VFSElement::FILE
									, fInfo.absoluteFilePath()
									, title
									, ""
									, ""
									, response_id
									, parentid
									, "0000-00-00 00:00:00"
									, _pluginName
									, (VFSElement::eFl_Dirty | VFSElement::eFl_Downloaded) // we have uploaded the file, thus it's downloaded :)
									);

					cache->insert(elem);
				}
				else
				{
					err = eERROR;
				}
			}
		}
		else
		{
			notifyUser(Ui::Notification::eCRITICAL
				, tr("Error")
				, tr("File upload failed (") + title + QString(")"));
			
			err = eERROR;
		}

		WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), err);
		return err;
	};

	RESULT JmmRVFSDriver::modifyFile(const QString&)
	{
		return eNO_ERROR;
	};

	RESULT JmmRVFSDriver::renameElement( const QString& id, ElementType type, const QString& newTitle)
	{
		QString response;

		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Plugin is not connected !\nPlease connect plugin at first.\n"));
			return eERROR;
		}
		
		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}

		QFileInfo fInfo(newTitle);

		_httpConnector.renameElement(id, type, fInfo.baseName(), response, _pluginName);
		return (RESULT)(Xml::JmmXmlParser::parseResponse(response).toInt() != -1);
	};

	RESULT JmmRVFSDriver::deleteDirectory( const QString& id)
	{
		QString response;

		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Plugin is not connected !\nPlease connect plugin at first.\n"));
			return eERROR;
		}
		
		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}

		_httpConnector.deleteDirectory(id, response, _pluginName);
		return (RESULT)(Xml::JmmXmlParser::parseResponse(response).toInt() != -1);
	};

	RESULT JmmRVFSDriver::deleteFile( const QString& id)
	{
		QString response;

		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Plugin is not connected !\nPlease connect plugin at first.\n")));
			return eERROR;
		}
		
		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}

		_httpConnector.deleteFile(id, response, _pluginName);
		return (RESULT)(Xml::JmmXmlParser::parseResponse(response).toInt() != -1);
	};

	RESULT JmmRVFSDriver::moveElement( const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type)
	{
		QString response;

		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Plugin is not connected !\nPlease connect plugin at first.\n")));
			return eERROR;
		}

		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}

		if(type == VFSElement::DIRECTORY)
		{
			notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Moving directory is not allowed !\n")));
			return eERROR;
		}

		_httpConnector.moveElement(id, oldParentId, newParentId, type, response, _pluginName);
		return (RESULT)(Xml::JmmXmlParser::parseResponse(response).toInt() != -1);
	};

	RESULT JmmRVFSDriver::createDirectory(const QString& path, const QString& parentid, const QString& title)
	{
		QString response;
		QString id;

		if(!isRunning())
		{
			notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Plugin is not connected !\nPlease connect plugin at first.\n")));
			return eERROR;
		}

		//====HACK START !!!
		SettingStorage* settings = WebMounter::getSettingStorage();
		QDir pluginRootDir(settings->getAppStoragePath() + QDir::separator() + _pluginName);
		QDir createDir(path);
		if(pluginRootDir == createDir)
		{
			return eERROR;
		}	
		//====HACK END !!!

		if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
		{
			//stopPlugin();
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;	
		}

		_httpConnector.createDirectory(parentid, title, response, _pluginName);

		if(Xml::JmmXmlParser::parseResponse(response).toInt() == -1)
		{
			id = Xml::JmmXmlParser::parseIdResponse(response);
			if(id != "" && id != "0")
			{
				VFSCache* cache = WebMounter::getCache();

				VFSElement elem(VFSElement::DIRECTORY
								, path
								, title
								, ""
								, ""
								, id
								, parentid
								, ""
								, _pluginName);

				cache->insert(elem);

				return eNO_ERROR;
			}
		}
		return eERROR;
	};

	RESULT JmmRVFSDriver::getElements()
	{
		try
		{
			SettingStorage* settings = WebMounter::getSettingStorage();
			QString pluginStoragePath = settings->getAppStoragePath() + QDir::separator() + _pluginName;
			QFileInfo treeFile = settings->getAppSettingStoragePath() + QDir::separator() + _pluginName + QDir::separator() + QString("tree.xml");
			QString treeFilePath = treeFile.absoluteFilePath();

			QDir dir;

			if(!dir.exists(settings->getAppSettingStoragePath() + QDir::separator() + _pluginName))
			{
				dir.mkdir(settings->getAppSettingStoragePath() + QDir::separator() + _pluginName);
			}

			if(!isRunning())
			{
				notifyUser(Ui::Notification::eCRITICAL
					, QString(tr("Error"))
					, QString(tr("Plugin is not connected !\nPlease connect plugin at first.\n")));
				return eERROR;
			}

			if(_httpConnector.auth() != eNO_ERROR) // to be sure that session has not been expired
			{
				return eERROR;	
			}

			QFile::remove(treeFilePath);

			VFSCache* vfsCache = WebMounter::getCache();

			vfsCache->flush();

			if(!_httpConnector.getTreeElements(treeFilePath, _pluginName))
			{
				if(!QFile::exists(treeFilePath))
				{
					return eERROR;
				}

				if((RESULT)Xml::JmmXmlParser::populateCache(treeFilePath, pluginStoragePath, _pluginName) == eNO_ERROR)
				{
					// All existing on the server elements are dirty in the cache now. 
					// If element doesn't exist on server, so it is not dirty and we may delete it. 
					return deleteNotDirty();
				}
			}
		}
		catch(...)
		{
		}
		return eERROR;
	}

	void JmmRVFSDriver::run()
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

	void JmmRVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
	{
		Ui::ControlPanel* notifDevice = WebMounter::getControlPanel();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT JmmRVFSDriver::checkKey(const PluginSettings& pluginSettings)
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
		
		md5Hash.addData(QByteArray(qPrintable(url + "Sasha198779_")));
		sha1Hash.addData(md5Hash.result().toHex().data());

		QString validKey(sha1Hash.result().toHex().data());
		
		if(validKey == pluginSettings.key)
		{
			return eNO_ERROR;
		}
		else
		{
			return eERROR;
		}
	}

	/***********************************************************************
	*********************** Handlers for slots *****************************							  
	***********************************************************************/

	void JmmRVFSDriver::connectHandler(PluginSettings& pluginSettings)
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == RemoteDriver::eNotConnected)
		{
			if(checkKey(pluginSettings) == eERROR)
			{
				updateState(100, eNotConnected);
				notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Activation key is not valid !\n")));
				return;
			}
			
			GeneralSettings generalSettings; 

			WebMounter::getSettingStorage()->getData(generalSettings);
			WebMounter::getSettingStorage()->addSettings(pluginSettings);

			_httpConnector.setSettings(
				pluginSettings.serverUrl
				, pluginSettings.userName
				, pluginSettings.userPassword
				, generalSettings.proxyAddress
				, generalSettings.proxyLogin + ":" + generalSettings.proxyPassword
				, _pluginName
				);

			if(_state != eSyncStopping)
			{
				updateState(0, eAuthInProgress);
			}
			
			if(_httpConnector.auth() == eNO_ERROR)
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

				notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Authorization failed !\n"
					"Please check proxy settings on Configuration tab and check settings on corresponding plugin tab...\n")));
			}
		}
	}

	void JmmRVFSDriver::disconnectHandler()
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

	void JmmRVFSDriver::syncHandler()
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	}

	RESULT JmmRVFSDriver::sync()
	{
		VFSCache* cache = WebMounter::getCache();
		PluginSettings settings;
		WebMounter::getSettingStorage()->getData(settings, _pluginName);
		UINT uNotDownloaded = 0;
		UINT uDownloaded = 0;

		_driverMutex.lock();
		if(_state != eSyncStopping)
		{
			updateState(0, RemoteDriver::eSync);
		}
		_driverMutex.unlock();

		//sleep(1000);

		if(getElements() == eNO_ERROR)
		{
			uNotDownloaded = countNotDownloaded();

			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState(50, RemoteDriver::eSync);
			}
			_driverMutex.unlock();
		}
		else
		{
			stopPlugin();

			notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Sync failed !\n")));
			//updateState(100, RemoteDriver::eNotConnected);
			return eERROR;
		}

		QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
		QFileInfo fRootInfo(rootPath);
		syncCacheWithFileSystem(fRootInfo.absoluteFilePath());

		QDir qDir;
		QFile qFile;

		//updateState(0, RemoteDriver::eDownloading);

		for(VFSCache::iterator iter = cache->begin(); 
			iter != cache->end(); 
			++iter)
		{
			if(this->_state == eSyncStopping)
			{
				notifyUser(Ui::Notification::eINFO, QString(tr("Info")), QString(tr("Synchronization is stopped !\n")));
				//updateState(100, RemoteDriver::eConnected);
				return eNO_ERROR;
			}

			if(iter->getPluginName() == _pluginName)
			{
				if(iter->getType() == VFSElement::DIRECTORY)
				{
					if(!qDir.exists(iter->getPath()))
					{
						qDir.mkdir(iter->getPath());
					}
				}
				else if(iter->getType() == VFSElement::FILE && !iter->isDownloaded())
				{
					QFileInfo fInfo(iter->getPath());

					if(fInfo.exists() && fInfo.size() != 0) 
					{	
						QFile::remove(iter->getPath());
					}

					if(settings.bFullSync)
					{	
						if(downloadFile(iter->getOrigUrl(), iter->getPath()) == eNO_ERROR)
						{
							QFile file(iter->getPath());
							if(!file.exists())
							{
								file.open(QIODevice::WriteOnly);
								file.close();
							}

							uDownloaded++;

							_driverMutex.lock();
							if(_state != eSyncStopping)
							{
								updateState((int)(((float)uDownloaded/uNotDownloaded)*50) + 50, RemoteDriver::eSync);
							}
							_driverMutex.unlock();
						}
						else
						{
							notifyUser(Ui::Notification::eCRITICAL, QString(tr("Error")), QString(tr("Downloading failed ") + "(" + iter->getName() + ") !\n"));
							updateState(100, RemoteDriver::eConnected);
							return eERROR;
						}
					}
					else
					{
						QFile file(iter->getPath());
						file.open(QIODevice::WriteOnly);
						file.close();

						uDownloaded++;

						_driverMutex.lock();
						if(_state != eSyncStopping)
						{
							updateState((int)((uDownloaded/uNotDownloaded)*50) + 50, RemoteDriver::eSync);
						}
						_driverMutex.unlock();
					}

				}
			}
		}

		updateState(100, RemoteDriver::eConnected); // Ready for use
		return eNO_ERROR;
	}

	void JmmRVFSDriver::stopSyncHandler()
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

	UINT JmmRVFSDriver::countNotDownloaded()
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

	RESULT JmmRVFSDriver::deleteNotDirty()
	{
		VFSCache* cache = WebMounter::getCache();

		for(VFSCache::iterator iter = cache->begin(); 
			iter != cache->end(); 
			++iter)
		{
			if( iter->getPluginName() == _pluginName && !iter->isDirty() )
			{
				if(iter->getType() == VFSElement::FILE)
				{
					if(QFile::exists(iter->getPath()))
					{
						QFile::remove(iter->getPath());
					}
				}
				cache->erase(iter);
				iter = cache->begin();
			}
		}

		for(VFSCache::iterator iter = cache->begin(); 
			iter != cache->end(); 
			++iter)
		{
			if( iter->getPluginName() == _pluginName && !iter->isDirty() )
			{
				if(iter->getType() == VFSElement::DIRECTORY)
				{
					QDir dir(iter->getPath());
					dir.rmdir(iter->getPath());
				}
				cache->erase(iter);
				iter = cache->begin();
			}
		}

		return cache->flush();
	}
};