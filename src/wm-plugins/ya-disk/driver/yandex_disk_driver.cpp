#include <QRegExp>
#include <QtXml>

#include "yandex_disk_driver.h"
#include "webmounter.h"

namespace RemoteDriver
{
	using namespace Common;
	using namespace Connector;
	
	YaDiskRVFSDriver::YaDiskRVFSDriver(const QString& pluginName)
	{
		_state = RemoteDriver::eNotConnected;
		_httpConnector = new YaDiskHTTPConnector;
		_pluginName = pluginName;

		RESULT res = WebMounter::getCache()->restoreCache();
		if(res == eNO_ERROR)
		{
			QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
			QFileInfo fInfo(rootPath);
			//syncCacheWithFileSystem(fInfo.absoluteFilePath());
		}
	}

	YaDiskRVFSDriver::~YaDiskRVFSDriver(void)
	{
		delete _httpConnector;
	}

	RESULT YaDiskRVFSDriver::getElements() 
	{ 
		return eNO_ERROR;
	}

	RESULT YaDiskRVFSDriver::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
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

	RESULT YaDiskRVFSDriver::uploadFile(const QString& path, const QString& title,  const QString& Id, const QString& parentId)
	{
		QString xmlResp;
		QString id = ROOT_ID;
		QFileInfo fInfo(path);
		if (fInfo.size() == 0)
		{
			return eNO_ERROR;
		}
		
		RESULT err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
                unsigned int retryUploadCounter = 0;

		while(err == eERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
		{
			err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
			retryUploadCounter++;
		}
		
		VFSElement elem;
		if(!err)
		{
			QFileInfo fInfo(path);
			id = fInfo.fileName();

			VFSCache* vfsCache = WebMounter::getCache();
			elem = VFSElement(VFSElement::FILE
				, fInfo.absoluteFilePath()
				, title
				, ""
				, ""
				, id
				, id
				, parentId
				, ""
				, _pluginName);

			vfsCache->insert(elem);

			QFile::Permissions permissions = QFile::permissions(elem.getPath());
			permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
			bool err = QFile::setPermissions(elem.getPath(), permissions);
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

	RESULT YaDiskRVFSDriver::modifyFile(const QString&)
	{
		return eERROR;
	}

	RESULT YaDiskRVFSDriver::renameElement(const QString& path, const QString& newTitle)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = _httpConnector->renameElement(elem->getId(), elem->getType(), newTitle, response);
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
					, elem->getPluginName());

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

	RESULT YaDiskRVFSDriver::deleteDirectory(const QString& path)
	{
		/*RESULT res = eERROR;
		QDir qDirFrom(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qDirFrom.absolutePath());

		if(elem != cache->end() && elem->getType() == VFSElement::DIRECTORY)
		{
			QString response;
			res = _httpConnector->deleteFile("album/" + elem->getId() + "/", response);
			if(res != eERROR)
			{
				cache->erase(elem);
			}
		}*/


		return deleteFile(path);
	}

	RESULT YaDiskRVFSDriver::deleteFile(const QString& path)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = _httpConnector->deleteFile(elem->getId(), response);
			if(res == eNO_ERROR)
			{
				WebMounter::getProxy()->fileDeleted(elem->getPath(), res);
				cache->erase(elem);
			}
		}
	
		return res;
	}

	RESULT YaDiskRVFSDriver::moveElement(const QString& path, const QString& newParentId)
	{
		RESULT res = eERROR;
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			QString response;
			res = _httpConnector->moveElement(elem->getId(), elem->getParentId(), newParentId, elem->getType(), response);
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

	RESULT YaDiskRVFSDriver::createDirectory(const QString& path, const QString& parentId, const QString& title)
	{
		QString xmlResp;
		RESULT err = _httpConnector->createDirectory(title, parentId, xmlResp);
		if(!err)
		{
			VFSElement elem;
			elem.setType(VFSElement::DIRECTORY);
			elem.setPluginName(_pluginName);

			elem.setName(title);

			elem.setId(title);

			elem.setModified(QDateTime::currentDateTime().toString());

			elem.setSrcUrl(title);
			
			elem.setPath(path);
			
			WebMounter::getCache()->insert(elem);
		}

		return err;
	}

	QString YaDiskRVFSDriver::getElementPath(QList<VFSElement>& elements, VFSElement& element)
	{
		QString parentId = element.getParentId();
		QString rootPath = QFileInfo(Common::WebMounter::getSettingStorage()->getAppStoragePath() 
							+ QDir::separator() 
							+ _pluginName).absoluteFilePath(); 
		
		if(ROOT_ID == element.getId())
		{
			return element.getPath();
		}
		if(ROOT_ID == parentId)
		{
			return QFileInfo(rootPath + QDir::separator() + element.getName())
				.absoluteFilePath();	
		}
		else
		{
			for (unsigned int i = 0; i < elements.count(); ++i) 
			{
				if(elements[i].getId() == parentId)
				{
					return QFileInfo(getElementPath(elements, elements[i]) + QDir::separator() + element.getName())
						.absoluteFilePath();		
				}
			}
			
		}
		return "";
	}

	RESULT YaDiskRVFSDriver::getElements(QList<VFSElement>& elements)
	{
		RESULT err = eERROR;
		bool entriesFound = false;
		for(int i=0; i<elements.count()&&elements[i].getType()==VFSElement::DIRECTORY; i++)
		{
			QString xmlResp = "";
			QString path = elements[i].getSrcUrl();
			err = _httpConnector->getTreeElements(path, xmlResp);
			if(err || xmlResp.length() == 0)
			{
				continue;
			}

			QDomDocument xml;
			xml.setContent(xmlResp);
			QDomElement  root         = xml.firstChildElement(); // <multistatus> root element
			QDomNode  responseElement = root.firstChildElement(); // <response>

			while(!responseElement.isNull())
			{
				VFSElement elem;

				parseEntry(responseElement, elem);
				if(elem.getSrcUrl() != elements[i].getSrcUrl()) // skip duplicates
				{
					elem.setParentId(elements[i].getId());
					QString path = elements[i].getPath() + QString(QDir::separator()) + elem.getName();
					QFileInfo fInfo(path);
					elem.setPath(fInfo.absoluteFilePath());

					elem.setDownloaded(false);
					elements.append(elem);
				}
				
				responseElement = responseElement.nextSibling(); //next element
			}
		}

		if(err != eERROR)
		{
			//Populate path for elemenets
			for(int k=0, count = elements.count(); k < count; k++)
			{
				elements[k].setPath(QFileInfo(getElementPath(elements, elements[k])).absoluteFilePath());
			}

			handleNameDuplicates(elements);
		}

		return err;
	}

	int YaDiskRVFSDriver::findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem)
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

	void YaDiskRVFSDriver::parseEntry(QDomNode& xmlEntry, VFSElement& elem)
	{
		QString temp;

		int a = xmlEntry.toElement().elementsByTagName("d:resourcetype").item(0).childNodes().count();
		if(a > 0)
		{
			elem.setType(VFSElement::DIRECTORY);
		}
		else
		{
			elem.setType(VFSElement::FILE);
		}

		elem.setPluginName(_pluginName);

		elem.setName(xmlEntry.toElement().elementsByTagName("d:displayname").item(0).toElement().text());

		QString tmp = xmlEntry.toElement().elementsByTagName("d:href").item(0).toElement().text();
		if(tmp.left(1) == "/" || tmp.left(1) == "\\")
		{
			tmp = tmp.mid(1);
		}

		if(tmp.right(1) == "/" || tmp.right(1) == "\\")
		{
			tmp = tmp.mid(0, tmp.length() - 1);
		}

		elem.setId(tmp.lastIndexOf("/") != -1 ? tmp.mid(tmp.lastIndexOf("/") + 1) : tmp);

		elem.setModified(xmlEntry.toElement().elementsByTagName("d:getlastmodified").item(0).toElement().text());

		elem.setSrcUrl(tmp);
	}

	RESULT YaDiskRVFSDriver::sync()
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
		res = getElements(elements);
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

		return res;
	}

	void YaDiskRVFSDriver::run()
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

	void YaDiskRVFSDriver::connectHandler(PluginSettings& pluginSettings)
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

	void YaDiskRVFSDriver::connectHandlerStage2(RESULT error, PluginSettings pluginSettings)
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

	void YaDiskRVFSDriver::disconnectHandler()
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

	void YaDiskRVFSDriver::syncHandler()
	{
		QMutexLocker locker(&_driverMutex);

		if(_state == eConnected)
		{
			_forceSync.wakeAll();
		}
	}

	void YaDiskRVFSDriver::stopSyncHandler()
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

	QString YaDiskRVFSDriver::addPathSuffix(ElementType type, const QString& path, const QString& suffix)
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

	void YaDiskRVFSDriver::markNameDuplicates(QList<VFSElement>& elemList)
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

	void YaDiskRVFSDriver::handleNameDuplicates(QList<VFSElement>& elemList)
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

	bool YaDiskRVFSDriver::areFileAttributesValid(const QString& path, unsigned long attributes)
	{
                return true;///////////(attributes & FILE_ATTRIBUTE_READONLY);
	}
}
