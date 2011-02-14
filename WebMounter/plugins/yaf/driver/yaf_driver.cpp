#include <QRegExp>
#include "WebMounter.h"
#include "yaf_driver.h"

namespace RemoteDriver
{
	using namespace Common;
	using namespace Connector;
	
	YafRVFSDriver::YafRVFSDriver(const QString& pluginName) : _pluginName(pluginName)
	{
		_state = RemoteDriver::eNotConnected;
		_httpConnector = new YafHTTPConnector;

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	}

	void YafRVFSDriver::syncCacheWithFileSystem(const QString& path)
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

	YafRVFSDriver::~YafRVFSDriver(void)
	{
		delete _httpConnector;
	}

	RESULT YafRVFSDriver::downloadFile(const QString& url, const QString& path)
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

	RESULT YafRVFSDriver::downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList, UINT& uDownloaded, UINT& uNotDownloaded)
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
		/*else
		{
			notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Downloading failed  !\n"));
			_driverMutex.lock();
			if(_state != eSyncStopping)
			{
				updateState(100, RemoteDriver::eConnected);
			}
			_driverMutex.unlock();
			return eERROR;
		}*/
		return res;
	}

	RESULT YafRVFSDriver::uploadFile(const QString& path, const QString& title,  const QString& Id, const QString& parentId)
	{
		QString xmlResp;
		QString id = ROOT_ID;
		RESULT err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
		UINT retryUploadCounter = 0;

		while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
		{
			err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
			retryUploadCounter++;
		}

		QFileInfo fInfo(path);

		QString suffix = fInfo.suffix();
		suffix = suffix.toLower();

		if(suffix != "jpg" && suffix != "jpeg" && suffix != "png" && suffix != "gif")
		{
			notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
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
				elem = VFSElement(VFSElement::DIRECTORY
					, fInfo.absoluteFilePath()
					, title
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
		return err;
	}

	RESULT YafRVFSDriver::modifyFile(const QString&)
	{
		return eERROR;
	}

	RESULT YafRVFSDriver::renameElement(const QString& id, ElementType type, const QString& newTitle)
	{
		QString response;
		return _httpConnector->renameElement(id, type, newTitle, response);
	}

	RESULT YafRVFSDriver::deleteDirectory(const QString& id)
	{
		QString response;
		return _httpConnector->deleteFile("album/" + id + "/", response);
	}

	RESULT YafRVFSDriver::deleteFile(const QString& id)
	{
		QString response;
		RESULT res = _httpConnector->deleteFile("photo/" + id + "/", response);
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

	RESULT YafRVFSDriver::moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type)
	{
		RESULT res = eERROR;

		QString response;
		res = _httpConnector->moveElement(id, oldParentId, newParentId, type, response);
		return res;
	}

	RESULT YafRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		QString xmlResp;
		RESULT err = _httpConnector->createDirectory(title, parentId, xmlResp);
		if(!err)
		{
			VFSElement elem;
			parseAlbumEntry(xmlResp, elem);
			
			if(parentId != ROOT_ID)
			{
				err = moveElement(elem.getId(), elem.getParentId(), parentId, elem.getType());
				if(err == eNO_ERROR)
				{
					elem.setParentId(parentId);
				}
			}

			VFSCache* vfsCache = WebMounter::getCache();
			VFSCache::iterator iter = vfsCache->begin();
			for(iter; iter != vfsCache->end(); ++iter)
			{
				if(iter->getId() == elem.getParentId())
					break;
			}

			QString path = iter->getPath() + QString(QDir::separator()) + elem.getName();
			QFileInfo fInfo(path);
			elem.setPath(fInfo.absoluteFilePath());

			WebMounter::getCache()->insert(elem);
		}

		return err;
	}

	RESULT YafRVFSDriver::getAlbums(QList<VFSElement>& elements)
	{
		RESULT err = eERROR;
		QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ssZ");

		forever
		{
			bool entriesFound = false;
			QString xmlResp = "";
			QString path = QString("albums/updated;%1/").arg(dateTime);
			err = _httpConnector->getTreeElements(path, xmlResp);
			if(!err)
			{
				QString pattern = QString::fromAscii("<entry>(.*)</entry>");
				QRegExp rx(pattern);
				rx.setCaseSensitivity(Qt::CaseSensitive);
				rx.setMinimal(true);
				rx.setPatternSyntax(QRegExp::RegExp);

				int pos = rx.indexIn(xmlResp, 0);
				entriesFound = (pos != -1);
				while(pos != -1)
				{
					VFSElement elem;
					parseAlbumEntry(rx.cap(1), elem);

					if(elem.getParentId() == ROOT_ID)
					{
						QString pluginStoragePath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
						QString path = pluginStoragePath + QString(QDir::separator()) + elem.getName();
						QFileInfo fInfo(path);
						elem.setPath(fInfo.absoluteFilePath());
					}

					elem.setDownloaded(false);
					elements.append(elem);

					pos += rx.matchedLength();
					pos = rx.indexIn(xmlResp, pos);
				}

				for(int k=0; k<elements.count(); k++)
				{
					if(elements[k].getParentId() != ROOT_ID)
					{
						int index = findParentIndex(elements, elements[k]);
						QString parentPath = elements[index].getPath();

						while(!parentPath.length())
						{
							index = findParentIndex(elements, elements[index]);
							if(index < 0)
								return eERROR;

							parentPath = elements[index].getPath();
						}

						QString path = elements[index].getPath() + QString(QDir::separator()) + elements[k].getName();
						QFileInfo fInfo(path);
						elements[k].setPath(fInfo.absoluteFilePath());
					}
				}
			}

			if(err != eNO_ERROR || !entriesFound)
				break;

			QDateTime dt = QDateTime::fromString(elements[elements.count()-1].getModified(), "yyyy-MM-ddThh:mm:ssZ");
			QTime time(dt.time().hour(), dt.time().minute(), dt.time().second() - 1);
			dt.setTime(time);
			dateTime = dt.toString("yyyy-MM-ddThh:mm:ssZ");
		}

		if(err != eERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	int YafRVFSDriver::findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem)
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

	RESULT YafRVFSDriver::getPhotos(QList<VFSElement>& elements)
	{
		RESULT err = eERROR;
		QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ssZ");

		forever
		{
			bool entriesFound = false;
			QString xmlResp = "";
			QString path = QString("photos/updated;%1/").arg(dateTime);
			err = _httpConnector->getTreeElements(path, xmlResp);
			if(!err)
			{
				QString pattern = QString::fromAscii("<entry>(.*)</entry>");
				QRegExp rx(pattern);
				rx.setCaseSensitivity(Qt::CaseSensitive);
				rx.setMinimal(true);
				rx.setPatternSyntax(QRegExp::RegExp);

				int pos = rx.indexIn(xmlResp, 0);
				entriesFound = (pos != -1);
				while(pos != -1)
				{
					VFSElement elem;
					parsePhotoEntry(rx.cap(1), elem);

					if(elem.getId() != elements[elements.count() - 1].getId())
					{
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
						elem.setDownloaded(false);
						elements.append(elem);
					}

					pos += rx.matchedLength();
					pos = rx.indexIn(xmlResp, pos);
				}
			}

			if(err != eNO_ERROR || !entriesFound)
				break;

			QDateTime dt = QDateTime::fromString(elements[elements.count()-1].getModified(), "yyyy-MM-ddThh:mm:ssZ");
			QTime time(dt.time().hour(), dt.time().minute(), dt.time().second() - 1);
			dt.setTime(time);
			dateTime = dt.toString("yyyy-MM-ddThh:mm:ssZ");
		}

		if(err != eERROR)
		{
			handleNameDuplicates(elements);
		}

		return err;
	}

	RESULT YafRVFSDriver::getElements()
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

	void YafRVFSDriver::parseAlbumEntry(QString& xmlEntry, VFSElement& elem)
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

		rx.setPattern("album:(.*)</id>");
		rx.indexIn(xmlEntry);
		elem.setId(rx.cap(1));

		rx.setPattern("<updated>(.*)</updated>");
		rx.indexIn(xmlEntry);
		elem.setModified(rx.cap(1));

		rx.setPattern("<link href=\"(.*)\" rel=\"self\" />");
		rx.indexIn(xmlEntry);
		elem.setOrigUrl(rx.cap(1));

		rx.setPattern("<link href=\"(.*)\" rel=\"album\" />");
		rx.indexIn(xmlEntry);
		QString rootId = ROOT_ID;
		if(rx.cap(1).length() > 0)
		{
			QString parentRef = rx.cap(1).left(rx.cap(1).length() - 1);
			int index = parentRef.lastIndexOf("/");
			if(index > 0)
			{
				rootId = parentRef.right(parentRef.length()-index-1);
			}
		}

		elem.setParentId(rootId);
	}

	void YafRVFSDriver::parsePhotoEntry(QString& xmlEntry, VFSElement& elem)
	{
		elem.setType(VFSElement::FILE);
		elem.setPluginName(_pluginName);

		QString title = QString::fromAscii("<title>(.*)</title>");
		QRegExp rx(title);
		rx.setCaseSensitivity(Qt::CaseSensitive);
		rx.setMinimal(true);
		rx.setPatternSyntax(QRegExp::RegExp);

		rx.indexIn(xmlEntry);
		elem.setName(rx.cap(1));

		rx.setPattern("photo:(.*)</id>");
		rx.indexIn(xmlEntry);
		elem.setId(rx.cap(1));

		rx.setPattern("<updated>(.*)</updated>");
		rx.indexIn(xmlEntry);
		elem.setModified(rx.cap(1));

		rx.setPattern("/album/(.*)/\" rel=\"album\" />");
		rx.indexIn(xmlEntry);
		elem.setParentId(rx.cap(1));

		rx.setPattern("<content src=\"(.*)\" type=");
		rx.indexIn(xmlEntry);
		elem.setOrigUrl(rx.cap(1));
	}

	void YafRVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
	{
		Ui::ControlPanel* notifDevice = WebMounter::getControlPanel();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT YafRVFSDriver::sync()
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
			res = getPhotos(elements);
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
							vfsCache->erase(iter--);
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
					


					///////////////////////////////////
					////////////////////////////////////////

					res = downloadFiles();

					///////////////////////////////////////
					////////////////////////////////////////
				}
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
		
		return res;
	}

	void YafRVFSDriver::run()
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

	void YafRVFSDriver::connectHandler(PluginSettings& pluginSettings)
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
				updateState(100, eNotConnected);

				notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Authorization failed !\n"
					"Please check proxy settings on Configuration tab and check settings on corresponding plugin tab...\n"));
			}
		}
	}

	RESULT YafRVFSDriver::downloadFiles()
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

	void YafRVFSDriver::updateDownloadStatus(RESULT downloadResult, const UINT uDownloaded, const UINT uNotDownloaded)
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

	UINT YafRVFSDriver::countNotDownloaded()
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

	void YafRVFSDriver::disconnectHandler()
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

	void YafRVFSDriver::syncHandler()
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	}

	void YafRVFSDriver::stopSyncHandler()
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

	int YafRVFSDriver::removeFolder(QDir& dir)
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

	QString YafRVFSDriver::addPathSuffix(ElementType type, const QString& path, const QString& suffix)
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

	void YafRVFSDriver::markNameDuplicates(QList<VFSElement>& elemList)
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

	void YafRVFSDriver::handleNameDuplicates(QList<VFSElement>& elemList)
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

	bool YafRVFSDriver::areFileAttributesValid(const QString& path, unsigned long attributes)
	{
		return (attributes & FILE_ATTRIBUTE_READONLY);
	}
}
