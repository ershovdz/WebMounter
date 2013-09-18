#include "rvfs_driver.h"
#include <QtCore>
#include <boost/bind.hpp>
#include "webmounter.h"
#include "filesystemtools.h"

using namespace RemoteDriver;
using namespace Common;

RESULT RVFSDriver::createFile(const QString& /*path*/, const QString& /*title*/,  const QString& /*id*/, const QString& /*parentId*/)
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

void RVFSDriver::updateDownloadStatus(RESULT downloadResult, const unsigned int uDownloaded, const unsigned int uNotDownloaded)
{
  if(!downloadResult)
  {
    updateSyncStatus((int)(((float)uDownloaded/uNotDownloaded)*50) + 50);
  }
  else
  {
    notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Downloading failed  !\n"));
    updateSyncStatus(100);
  }
}

unsigned int RVFSDriver::countNotDownloaded()
{
  VFSCache* cache = WebMounter::getCache();
  unsigned int counter = 0;

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

unsigned int RVFSDriver::countObjects(VFSElement::VFSElementType type)
{
  VFSCache* cache = WebMounter::getCache();
  unsigned int counter = 0;

  for(VFSCache::iterator iter = cache->begin(); 
    iter != cache->end(); 
    ++iter)
  {
    if( iter->getPluginName() == _pluginName && iter->getType() == type)
    {
      counter++;
    }
  }

  return counter;
}

void RVFSDriver::updateSyncStatus(double currentStatus)
{
  QMutexLocker locker(&_driverMutex);
  if(_state != eSyncStopping)
  {
    DriverState state = (currentStatus != 100) ? RemoteDriver::eSync : RemoteDriver::eConnected;
    updateState(currentStatus, state);
  }
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
        file.open(QIODevice::WriteOnly);
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
      QFile::setPermissions(entryAbsPath, permissions);

      QFile::remove(entryAbsPath);
    }
  }

  foreach (QString entry, lstDirs)
  {
    QString entryAbsPath = QFileInfo(dir.absolutePath() + "/" + entry).absoluteFilePath();

    if(vfsCache->find(entryAbsPath) == vfsCache->end())
    {
      QDir dir(entryAbsPath);
      FileSystemTools::removeFolder(dir);
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
  
  unsigned int uDownloaded = 0;
  RESULT err = eNO_ERROR;

  int filesCount = countObjects(VFSElement::FILE);
  double progressPortion = (filesCount == 0) ? 0 : (_progressData._maxValue - _progressData._currProgress)/filesCount;

  for(iter = vfsCache->begin(); iter != vfsCache->end(); ++iter)
  {
    {
      QMutexLocker locker(&_driverMutex);
      if(_state == eSyncStopping)
      {
        notifyUser(Ui::Notification::eINFO, tr("Info"), tr("Downloading is stopped !\n"));
        return eERROR_CANCEL;
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

        //if(urlToDownload.size() == DOWNLOAD_CHUNK_SIZE)
        //{
        err = downloadFiles(urlToDownload, pathToDownload);

        if(err)
        {
          notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Downloading failed  !\n"));
          updateSyncStatus(_progressData._maxValue);
          return err;
        }
        else
        {
          uDownloaded += urlToDownload.size();
          _progressData._currProgress += urlToDownload.size()*progressPortion;
          updateSyncStatus(_progressData._currProgress);
          urlToDownload.clear();
          pathToDownload.clear();
        }
        //}
      }
      else if(iter->getFlags() & VFSElement::eFl_Downloaded)
      {
        uDownloaded++;
        _progressData._currProgress += progressPortion;
        updateSyncStatus(_progressData._currProgress);
        double duration = filesCount > 0 ? min(1000,	10000/filesCount) : 1000;
        msleep(duration);
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
  Ui::NotificationDevice* notifDevice = WebMounter::getNotificationDevice();

  Ui::Notification msg(type, title, description);

  notifDevice->showNotification(msg);
}


/***********************************************************************
*********************** Handlers for slots *****************************							  
***********************************************************************/

void RVFSDriver::connectHandler(PluginSettings& /*pluginSettings*/)
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

void RVFSDriver::mergeToCache(QList<VFSElement>& elements)
{
  QDir qDir;
  QFile qFile;
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
              qDir.rename(iter->getPath(), elements[i].getPath());
            }
            break;
          }

          elements.removeAt(i);
          break;
        }
      }

      // Element has been deleted
      if(!found && iter->getId() != ROOT_ID)
      {
        VFSCache::iterator iter1 = iter--;
        vfsCache->erase(iter1);
      }
    }
  }

  _progressData._currProgress += 5;
  updateSyncStatus(_progressData._currProgress);

  // Add newly created elements
  for(int i=0; i<elements.count(); i++)
  {
    vfsCache->insert(elements[i]);
  }
}

RESULT RVFSDriver::sync()
{
  RESULT res = eERROR_GENERAL;
  SettingStorage* settings = WebMounter::getSettingStorage();
  QString pluginStoragePath = settings->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
  QFileInfo fInfo(pluginStoragePath);
  PluginSettings plSettings;
  settings->getData(plSettings, _pluginName);
  _progressData._currProgress = 0;

  updateSyncStatus(_progressData._currProgress);
  
  VFSCache* vfsCache = WebMounter::getCache();

  if( plSettings.userName != plSettings.prevUserName )
  {
      QDir dir(pluginStoragePath);
    FileSystemTools::removeFolderContent( dir );
    vfsCache->erasePlugin( _pluginName );
  }

  _progressData._currProgress += 5;
  updateSyncStatus(_progressData._currProgress);

  QList<VFSElement> elements;
  _progressData._maxValue = plSettings.bFullSync ? 40 : 90;

  res = getElements(elements);
  if(!res)
  {
    mergeToCache(elements);
    
    _progressData._currProgress += 4;
    updateSyncStatus(_progressData._currProgress);

    QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
    QFileInfo fInfo(rootPath);
    syncCacheWithFileSystem(fInfo.absoluteFilePath());

    _progressData._currProgress += 4;
    _progressData._maxValue = 100;
    updateSyncStatus(_progressData._currProgress);
    
    if(plSettings.bFullSync)
    {
      res = downloadFiles();
    }
  }

  if(res != eNO_ERROR && res != eERROR_CANCEL)
  {
    stopPlugin();
    notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Sync failed !\n"));
    //updateState(100, RemoteDriver::eNotConnected);
  }
  else if(res != eERROR_CANCEL)
  {
    updateSyncStatus(_progressData._maxValue);
  }

  return res;
}

void RVFSDriver::run()
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

    {
      QMutexLocker locker(&_syncMutex);
      _forceSync.wait(&_syncMutex, sync_period * 1000);
    }
   
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
