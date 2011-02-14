#include "file_proxy.h"
#include "vfs_cache.h"
#include "WebMounter.h"

#include <QDir>
#include <QFileInfo>
#include <QObject>

#include <boost/bind.hpp>

namespace Common
{

	//RVFSDriver* FileProxy::_pRVFSDriver = 0;
	FileProxy* FileProxy::_pFileProxyInstance = 0;
	QMutex FileProxy::_FileProxyMutex;
	set<QString> FileProxy::_currentFiles;

	UINT FileProxy::uUploaded = 0;
	UINT FileProxy::uNotUploaded = 0;
	QList<QString> FileProxy::_uploadQueue;
	//JmmRVFSDriver rvfsDriver;

	FileProxy::FileProxy(void)
	{
	}


	FileProxy::~FileProxy(void)
	{
	}

	/*FileProxy::FileProxy(RVFSDriver* pDriver)
	{
		if(pDriver && pDriver != _pRVFSDriver)
		{
			if(_pRVFSDriver != 0) delete _pRVFSDriver;
			_pRVFSDriver = pDriver;
		}
	}*/

	FileProxy* FileProxy::CreateFileProxy()
	{
		//QMutexLocker locker(&_FileProxyMutex);
		if(_pFileProxyInstance)
		{
			delete _pFileProxyInstance;
		}
		_pFileProxyInstance = new FileProxy();
		return _pFileProxyInstance;
	}

	RESULT FileProxy::CreateFileW(QString path)
	{
		QMutexLocker locker(&_FileProxyMutex);
		
		VFSCache* cache = WebMounter::getCache();
		QFileInfo qFileInfo(path);

		VFSCache::iterator file		 = cache->find(qFileInfo.absoluteFilePath());
		VFSCache::iterator parentDir = cache->find(qFileInfo.dir().absolutePath());

		RVFSDriver* driver = extractPlugin(path);

		if(!driver)
		{
			return eERROR;
		}

		if(driver->getState() != RemoteDriver::eConnected)
		{
			notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
			return eERROR;
		}

		QDir qDir = qFileInfo.dir();
		
		VFSCache::iterator it_dir = cache->find(qDir.absolutePath());

		if(it_dir == cache->end())
		{
			notifyUser(Ui::Notification::eCRITICAL, QString(QObject::tr("Error")), QString(QObject::tr("Select a specific category, at first\n")));
			return eERROR; // Looks like it was root path
		}

		QFileInfo fInfo(path);
		if(fInfo.size())
		{
			IncreaseNotUploadedCounter();
			_uploadQueue.append(fInfo.absoluteFilePath());
		
			if(file != cache->end())
			{
				QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::uploadFile, _1, _2, _3, _4, _5), driver, path, qFileInfo.fileName(), file->getId(), parentDir->getId());
				//return driver->uploadFile(path, qFileInfo.baseName(), file->getId(), parentDir->getId());
			}
			else
			{
				QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::uploadFile, _1, _2, _3, _4, _5), driver, path, qFileInfo.fileName(), QString(ROOT_ID), parentDir->getId());
				//return driver->uploadFile(path, qFileInfo.baseName(), ROOT_ID, parentDir->getId());
			}
		}
		else
		{
			return driver->createFile(path, qFileInfo.fileName(), QString(ROOT_ID), parentDir->getId());
		}
		
		
		return eNO_ERROR;
	}

	RESULT FileProxy::CheckFile(QString path)
	{
		QMutexLocker locker(&_FileProxyMutex);
		QFileInfo fInfo(path);

		if(_currentFiles.empty() || _currentFiles.find(fInfo.absoluteFilePath()) == _currentFiles.end()) 
		{
			_currentFiles.insert(fInfo.absoluteFilePath());
			return eERROR;
		}
		else
		{
			//Sleep(2000);
			return eNO_ERROR;
		}
	}

	RESULT FileProxy::UnCheckFile(QString path)
	{
		QMutexLocker locker(&_FileProxyMutex);
		QFileInfo fInfo(path);

		if(_currentFiles.find(fInfo.absoluteFilePath()) != _currentFiles.end()) 
		{
			//_currentFiles.erase(_currentFiles.find(fInfo.absoluteFilePath()));
			return eNO_ERROR;
		}
		else
		{
			//Sleep(2000);
			return eERROR;
		}
	}

	RESULT FileProxy::ReadFile(QString path)
	{
		QMutexLocker locker(&_FileProxyMutex);
		
		QFileInfo fInfo(path);
		VFSCache* cache = WebMounter::getCache();

		//if(_currentFiles.empty() || _currentFiles.find(fInfo.absoluteFilePath()) == _currentFiles.end()) 
		//{
		//	_currentFiles.insert(fInfo.absoluteFilePath());
		//}
		//else
		//{
		//	//Sleep(2000);
		//	return false;
		//}

		VFSCache::iterator iter = cache->find(fInfo.absoluteFilePath());
		if(iter != cache->end())
		{
			if(iter->isDownloaded() == false)
			{
				RVFSDriver* driver = extractPlugin(path);

				if(!driver)
				{
					return eERROR;
				}

				if(driver->getState() != RemoteDriver::eConnected)
				{
					notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QString(iter->getPluginName() + QObject::tr(" plugin is not connected !\nPlugin has to be in connected state.\n")));
					return eERROR;
				}

				if(driver->downloadFile(iter->getOrigUrl(), iter->getPath()) == eNO_ERROR)
				{
					return eNO_ERROR;
				}
				else
				{
					if(_currentFiles.empty() || _currentFiles.find(fInfo.absoluteFilePath()) != _currentFiles.end()) 
					{
						//_currentFiles.erase(_currentFiles.find(fInfo.absoluteFilePath()));
						//return eNO_ERROR;
					}
				}
			}
			else
			{
				return eNO_ERROR;
			}
		}
		return eERROR;
	}

	/*void FileProxy::Sync(QString path, bool bFullSync)
	{
		
	}*/

	RESULT FileProxy::CreateDirectoryW(QString path)
	{
		QDir dir(path);
		VFSCache* cache = WebMounter::getCache();
		
		if(cache->find(dir.absolutePath()) !=  cache->end())
		{
			return eNO_ERROR; //directory already exists. Let's return no error.
		}

		QString dirName = dir.dirName();
		QString dirPath = dir.absolutePath();
		QString parentId = ROOT_ID;
		
		if(dir.cdUp())
		{
			VFSCache::iterator iter = cache->find(dir.absolutePath());
			if(iter != cache->end())
			{
				parentId = iter->getId();
			}
		}

		RVFSDriver* driver = extractPlugin(path);

		if(!driver)
		{
			return eERROR;
		}

		if(driver->getState() != RemoteDriver::eConnected)
		{
			notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
			return eERROR;
		}
		
		return driver->createDirectory(dirPath, parentId, dirName);  
	}

	RESULT FileProxy::MoveElement(QString from, QString to)
	{
		QMutexLocker locker(&_FileProxyMutex);
		
		VFSCache* cache = WebMounter::getCache();

		QFile qFile(from);
		QFileInfo fInfoFrom(from);
		QFileInfo fInfoTo(to);

		QDir qDirFrom = fInfoFrom.dir();
		QDir qDirTo = fInfoTo.dir();

		VFSCache::iterator it_dir_from = cache->find(qDirFrom.absolutePath());
		VFSCache::iterator it_dir_to = cache->find(qDirTo.absolutePath());
		
		
		VFSCache::iterator it_element = cache->find(fInfoFrom.absoluteFilePath());
// 		if(it_dir_to != cache->end() &&
//			(((it_dir_to->getId() == "0")&&(it_dir_from->getId() != "0")) ||
//			((it_dir_to->getId() != "0")&&(it_dir_from->getId() == "0"))))
// 		{
// 			return eERROR;
// 		}

		if(from != to && it_dir_from == it_dir_to && it_element != cache->end()) //rename element
		{
			RVFSDriver* driver = extractPlugin(from);

			if(!driver)
			{
				return eERROR;
			}

			if(driver->getState() != RemoteDriver::eConnected)
			{
				notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
				return eERROR;
			}

			if(!driver->renameElement(it_element->getId()
				, it_element->getType()
				, fInfoTo.fileName()))
			{

				VFSElement elem(it_element->getType()
					, fInfoTo.absoluteFilePath()
					, fInfoTo.fileName()
					, it_element->getSmallUrl()
					, it_element->getOrigUrl()
					, it_element->getId()
					, it_element->getParentId()
					, it_element->getModified()
					, it_element->getPluginName());

				cache->erase(it_element);
				cache->insert(elem);
				return eNO_ERROR;
			}

		}

		//if(it_dir_from == _FileList.end() && it_dir_to != _FileList.end()) //From without, on the inside. 
		//Thus we have to upload a photo 
		//{
		//if()
		//	_pRVFSDriver->uploadFile(from, fInfoFrom.fileName(), it_dir_to->second->getId(), _FileList);
		//}
		else if(//it_dir_from != cache->end() 
			it_dir_to != cache->end() 
			&& it_element != cache->end()) //From the inside, on the inside.
		{
			RVFSDriver* plugin_from = extractPlugin(from);
			RVFSDriver* plugin_to = extractPlugin(to);

			if(!plugin_from || !plugin_to)
			{
				return eERROR;
			}

			if(plugin_to != plugin_from)
			{
				notifyUser(Ui::Notification::eCRITICAL, QString("Info"), QString("Moving is possible within a single plugin only\n"));
				return eERROR;
			}

			if(plugin_to->getState() != RemoteDriver::eConnected)
			{
				notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
				return eERROR;
			}

			if(!plugin_to->moveElement(it_element->getId()
				, it_element->getParentId()
				, it_dir_to->getId()
				, it_element->getType()))
			{
				QString str = fInfoTo.absoluteFilePath();
				VFSElement elem(it_element->getType()
					, fInfoTo.absoluteFilePath()
					, it_element->getName()
					, it_element->getSmallUrl()
					, it_element->getOrigUrl()
					, it_element->getId()
					, it_dir_to->getId()
					, it_dir_to->getModified()
					, it_dir_to->getPluginName());

				cache->erase(it_element);
				cache->insert(elem);

				return eNO_ERROR;
			}
		}
		//else if(it_dir_from != _FileList.end() //From the inside, to outside.
		//	&& it_dir_to == _FileList.end()	   //Thus we have to delete a photo
		//	&& it_element != _FileList.end()) 

		//{
		//	this->RemoveFile(from);
		//}
		return eERROR;
	}

	RESULT FileProxy::RemoveFile(QString path)
	{
		QFileInfo qInfo(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qInfo.absoluteFilePath());

		if(elem != cache->end())
		{
			RVFSDriver* driver = extractPlugin(path);

			if(!driver)
			{
				return eERROR;
			}

			if(driver->getState() != RemoteDriver::eConnected)
			{
				notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
				return eERROR;
			}

			if(driver->deleteFile(elem->getId()) == eNO_ERROR)
			{
				cache->erase(elem);
				return eNO_ERROR;
			}
		}
		return eERROR;
	}

	void FileProxy::notifyUser(Ui::Notification::_Types type, QString title, QString description)
	{
		Ui::ControlPanel* notifDevice = WebMounter::getControlPanel();

		Ui::Notification msg(type, title, description);

		notifDevice->showNotification(msg);
	}

	RESULT FileProxy::RemoveDir(QString& path)
	{
		QDir qDirFrom(path);
		VFSCache* cache = WebMounter::getCache();
		VFSCache::iterator elem = cache->find(qDirFrom.absolutePath());

		if(elem != cache->end() 
			&& elem->getType() == VFSElement::DIRECTORY)
		{
			RVFSDriver* driver = extractPlugin(path);

			if(!driver)
			{
				return eERROR;
			}

			if(driver->deleteDirectory(elem->getId()) == eNO_ERROR)
			{
				cache->erase(elem);
				return eNO_ERROR;
			}
		}
		return eERROR;
	}

	RVFSDriver* FileProxy::extractPlugin(const QString& path) const
	{
		QDir dirFrom(path);
		
		QDir rootDir(WebMounter::getSettingStorage()->getAppStoragePath());

		QString pluginName = "";
		
		while(dirFrom != rootDir)
		{
			pluginName = dirFrom.dirName();

			if(!dirFrom.cdUp())
			{
				 break;
			}
		}

		if(pluginName != "")
		{
			return WebMounter::getPlugin(pluginName);
		}
		else
		{
			notifyUser(Ui::Notification::eINTERACTIVE, QString("Info"), QString("Select a specific plugin, at first\n"));
			return 0; // Looks like it was root path
		}
	}

	bool FileProxy::CheckFileAttributes(const QString& path, unsigned long attributes)
	{
		RVFSDriver* driver = extractPlugin(path);
		return driver->areFileAttributesValid(path, attributes);
	}

	void FileProxy::fileUploaded(QString filePath, RESULT result)
	 {
		 QList<QString>::iterator iter;
		 for(iter = _uploadQueue.begin(); iter != _uploadQueue.end(); iter++)
		 {
			 if(*iter == filePath)
			 {
				 _uploadQueue.erase(iter);
				 break;
			 }
		 }

		 if(_uploadQueue.empty())
		 {
			uUploaded = 0;
			uNotUploaded = 0;

			if(result == eNO_ERROR)
			{
				notifyUser(Ui::Notification::eINFO
					, QObject::tr("Info")
					, QObject::tr("Upload completed !"));
			}
			else
			{
				notifyUser(Ui::Notification::eINFO
					, QObject::tr("Info")
					, QObject::tr("Upload completed, but some files has been not uploaded !"));
			}		
		 }
		 else
		 {
			IncreaseUploadedCounter();
			notifyUser(Ui::Notification::eINFO
						, QObject::tr("Info")
						, QObject::tr("Uploading progress (") 
							+ QString::number(GetUploadedCounter()) 
							+ QObject::tr(" of ") 
							+ QString::number(GetNotUploadedCounter())
							+ " ) !");
		 }
	 }
}
