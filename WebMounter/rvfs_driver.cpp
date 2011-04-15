#include "rvfs_driver.h"
#include <QtCore>
#include <boost/bind.hpp>
#include "WebMounter.h"

using namespace RemoteDriver;
using namespace Common;

RESULT RVFSDriver::createFile(const QString& path, const QString& title,  const QString& id, const QString& parentId)
{
	return eNO_ERROR;
}
/***********************************************************************
***************************** SLOTS ************************************							  
***********************************************************************/
void RVFSDriver::startPlugin(PluginSettings& pluginSettings)
{
	QtConcurrent::run(
		boost::bind(&RemoteDriver::RVFSDriver::connectHandler, _1, _2)
		, this
		, pluginSettings
		);
}
void RVFSDriver::stopPlugin()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::disconnectHandler, _1), this);
}
void RVFSDriver::startSync()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::syncHandler, _1), this);
}
void RVFSDriver::stopSync()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::stopSyncHandler, _1), this);
}

void RVFSDriver::updateState(int progress, DriverState newState)
{
	_state = newState;
	emit updateView(progress, (int)newState);
}

RESULT RVFSDriver::checkKey(const PluginSettings&)
{
	return eNO_ERROR;
}

void RVFSDriver::updateDownloadStatus(RESULT downloadResult, const UINT uDownloaded, const UINT uNotDownloaded)
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

UINT RVFSDriver::countNotDownloaded()
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
int RVFSDriver::removeFolder(QDir& dir)
{
	int res = 0;

	QStringList lstDirs  = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
	QStringList lstFiles = dir.entryList(QDir::Files);

	foreach (QString entry, lstFiles)
	{
		QString entryAbsPath = dir.absolutePath() + "/" + entry;
	
		QFile::Permissions permissions = QFile::permissions(entryAbsPath);
		permissions |= (QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
		bool err = QFile::setPermissions(entryAbsPath, permissions);

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

void RVFSDriver::syncCacheWithFileSystem(const QString& path)
{
	VFSCache* vfsCache = WebMounter::getCache();
	QDir dir(path);
	VFSCache::iterator parent = vfsCache->find(path);
	if(parent == vfsCache->end()) 
		return;

	for(VFSCache::iterator iter = vfsCache->begin(); iter != vfsCache->end(); ++iter)
	{
		if(iter->getParentId() != parent->getId()) continue;
		
		if(iter->getType() == VFSElement::DIRECTORY)
		{
			if(!dir.exists(iter->getPath())) 
				dir.mkpath(iter->getPath());
		}
		else if(iter->getType() == VFSElement::FILE)
		{
			QFile file(iter->getPath());
			if(!file.exists())
			{
				bool res = file.open(QIODevice::WriteOnly);
				file.close();
				vfsCache->setFlag(iter, VFSElement::eFl_None, VFSElement::eFl_Downloaded);
			}
			else if(file.size() == 0 
				&& iter->getFlags() != VFSElement::eFl_None 
				&& iter->getFlags() != VFSElement::eFl_NameDup)
			{
				vfsCache->setFlag(iter, VFSElement::eFl_None, VFSElement::eFl_Downloaded);
			}
		}
	}

	QStringList lstDirs = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
	QStringList lstFiles = dir.entryList(QDir::Files);

	foreach (QString entry, lstFiles)
	{
		QString entryAbsPath = QFileInfo(dir.absolutePath() + "/" + entry).absoluteFilePath();

		if(vfsCache->find(entryAbsPath) == vfsCache->end())
		{
			QFile::Permissions permissions = QFile::permissions(entryAbsPath);
			permissions |= (QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
			bool err = QFile::setPermissions(entryAbsPath, permissions);

			QFile::remove(entryAbsPath);
		}
	}

	foreach (QString entry, lstDirs)
	{
		QString entryAbsPath = QFileInfo(dir.absolutePath() + "/" + entry).absoluteFilePath();

		if(vfsCache->find(entryAbsPath) == vfsCache->end())
		{
			removeFolder(QDir(entryAbsPath));
		}
		else
		{
			syncCacheWithFileSystem(entryAbsPath);
		}
	}
}

RESULT RVFSDriver::downloadFiles()
{
	QList <QString> urlToDownload;
	QList <QString> pathToDownload;
	VFSCache* vfsCache = WebMounter::getCache();
	VFSCache::iterator iter = vfsCache->begin();
	UINT uDownloaded = 0;
	RESULT err = eNO_ERROR;
	for(iter = vfsCache->begin(); iter != vfsCache->end(); ++iter)
	{
		{ 	LOCK(_driverMutex)

			if(_state == eSyncStopping)
			{
				notifyUser(Ui::Notification::eINFO, tr("Info"), tr("Downloading is stopped !\n"));
				//updateState(100, RemoteDriver::eConnected);
				return eNO_ERROR;
			}
		}

		if(iter->getPluginName() == _pluginName)
		{
			if(iter->getType() == VFSElement::FILE 
				&& !(iter->getFlags() & VFSElement::eFl_Downloading)
				&& !(iter->getFlags() & VFSElement::eFl_Downloaded))
			{
				urlToDownload.append(iter->getSrcUrl());
				pathToDownload.append(iter->getPath());

				if(urlToDownload.size() == DOWNLOAD_CHUNK_SIZE)
				{
					err = downloadFiles(urlToDownload, pathToDownload);

					if(err)
					{
						updateDownloadStatus(err, uDownloaded, countNotDownloaded());
						return err;
					}
					else
					{
						uDownloaded += urlToDownload.size();
						updateDownloadStatus(err, uDownloaded, countNotDownloaded());
						urlToDownload.clear();
						pathToDownload.clear();
					}
				}
			}
		}
	}

	if(urlToDownload.size() > 0)
	{
		return downloadFiles(urlToDownload, pathToDownload);
	}

	return err;
}

void RVFSDriver::notifyUser(Ui::Notification::_Types type, QString title, QString description) const
{
	Ui::ControlPanel* notifDevice = WebMounter::getControlPanel();

	Ui::Notification msg(type, title, description);

	notifDevice->showNotification(msg);
}


/***********************************************************************
*********************** Handlers for slots *****************************							  
***********************************************************************/

void RVFSDriver::connectHandler(PluginSettings& pluginSettings)
{
	updateState(100, eAuthInProgress);

	updateState(100, eAuthorized);

	updateState(100, eSync);

	updateState(100, eConnected);
}

void RVFSDriver::disconnectHandler()
{
	updateState(100, eNotConnected);
}

void RVFSDriver::syncHandler()
{
	updateState(100, eSync);

	updateState(100, eConnected);
}

void RVFSDriver::stopSyncHandler()
{
	updateState(100, eSyncStopping);

	updateState(100, eConnected);
}

bool RVFSDriver::areFileAttributesValid(const QString&, unsigned long)
{
	return true;
}

void RVFSDriver::updateChildrenPath(const VFSElement& elem)
{
	VFSCache* vfsCache = WebMounter::getCache();
	VFSCache::iterator iter = vfsCache->begin();
	for(iter; iter != vfsCache->end(); ++iter)
	{
		if(iter->getParentId() == elem.getId())
		{
			QString path = elem.getPath() + QString(QDir::separator()) + iter->getName();
			QFileInfo fInfo(path);

			if(iter->getPath() != fInfo.absoluteFilePath())
			{
				VFSElement newElem(iter->getType()
					, fInfo.absoluteFilePath()
					, iter->getName()
					, iter->getEditMetaUrl()
					, iter->getEditMediaUrl()
					, iter->getSrcUrl()
					, iter->getId()
					, iter->getParentId()
					, iter->getModified()
					, iter->getPluginName()
					, iter->getFlags());

				vfsCache->erase(iter);
				vfsCache->insert(newElem);
				iter = vfsCache->begin();
			}
		}
	}
}
