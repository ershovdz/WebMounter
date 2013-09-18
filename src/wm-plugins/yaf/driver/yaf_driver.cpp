#include <QRegExp>

#include "yaf_driver.h"
#include "webmounter.h"

namespace RemoteDriver
{
  using namespace Common;
  using namespace Connector;

  YafRVFSDriver::YafRVFSDriver(const QString& pluginName)
  {
    _state = RemoteDriver::eNotConnected;
    _httpConnector = new YafHTTPConnector;
    _pluginName = pluginName;

    RESULT res = WebMounter::getCache()->restoreCache();
    if(res == eNO_ERROR)
    {
      QString rootPath = WebMounter::getSettingStorage()->getAppStoragePath() + QString(QDir::separator()) + _pluginName;
      QFileInfo fInfo(rootPath);
      //syncCacheWithFileSystem(fInfo.absoluteFilePath());
    }
  }

  YafRVFSDriver::~YafRVFSDriver(void)
  {
    delete _httpConnector;
  }

  RESULT YafRVFSDriver::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
  {
    VFSCache* vfsCache = WebMounter::getCache();
    int initialListSize = urlList.size();

    VFSCache::iterator iter = vfsCache->end();
    for(int i = 0; i < pathList.size(); ++i)
    {
      iter = vfsCache->find(pathList.at(i));
      if(iter != vfsCache->end()
        && !(iter->getFlags() & VFSElement::eFl_Downloading)
        && !(iter->getFlags() & VFSElement::eFl_Downloaded))
      {	
        vfsCache->setFlag(iter, VFSElement::eFl_Downloading, VFSElement::eFl_None);

        QFile::Permissions permissions = QFile::permissions(pathList.at(i));
        permissions |= (QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
        QFile::setPermissions(pathList.at(i), permissions);

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

    return res;
  }

  RESULT YafRVFSDriver::uploadFile(const QString& path, const QString& title, const QString& /*Id*/, const QString& parentId)
  {
    QString xmlResp;
    QString id = ROOT_ID;
    QFileInfo fInfo(path);

    QString suffix = fInfo.suffix();
    suffix = suffix.toLower();

		if(getState() != eConnected)
		{
			QDir dir;
			dir.remove(fInfo.absoluteFilePath());

			notifyUser(Ui::Notification::eINFO, QObject::tr("Info"),QObject::tr("Plugin is not connected !\nPlugin has to be in connected state.\n"));
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eNO_ERROR);
			return eNO_ERROR;
		}

    if(suffix != "jpg" && suffix != "jpeg" && suffix != "png" && suffix != "gif" && suffix != "bmp")
    {
      notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file extension is not supported !\n"));
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eERROR_GENERAL);
      return eERROR_GENERAL;
    }
    else if(id != ROOT_ID)
    {
      notifyUser(Ui::Notification::eINFO, tr("Info"), tr("This file will be changed locally!\n"));
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eERROR_GENERAL);
      return eERROR_GENERAL;
    }
		else if(parentId == ROOT_ID)
		{
			QDir dir;
			dir.remove(fInfo.absoluteFilePath());

			notifyUser(Ui::Notification::eINFO, tr("Info"), tr("You can not upload photos into the root directory!\n"));
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eERROR_NOT_SUPPORTED);
			return eERROR_GENERAL;
		}
    else if (fInfo.size() == 0)
    {
			WebMounter::getProxy()->fileUploaded(fInfo.absoluteFilePath(), eNO_ERROR);
      return eNO_ERROR;
    }

    RESULT err = _httpConnector->uploadFile(path, title, parentId, xmlResp);
    unsigned int retryUploadCounter = 0;

    while(err != eNO_ERROR && retryUploadCounter < MAX_UPLOAD_RETRY)
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

        elem.setFlags(VFSElement::eFl_SelfMade | VFSElement::eFl_Downloaded);

        vfsCache->insert(elem);

        QFile::Permissions permissions = QFile::permissions(elem.getPath());
        permissions &= ~(QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
        QFile::setPermissions(elem.getPath(), permissions);
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
    return eERROR_GENERAL;
  }

  RESULT YafRVFSDriver::renameElement(const QString& path, const QString& newTitle)
  {
    RESULT res = eERROR_GENERAL;
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

  RESULT YafRVFSDriver::deleteDirectory(const QString& path)
  {
    RESULT res = eNO_ERROR;
    QDir qDirFrom(path);
    VFSCache* cache = WebMounter::getCache();
    VFSCache::iterator elem = cache->find(qDirFrom.absolutePath());

    if(elem != cache->end() && elem->getType() == VFSElement::DIRECTORY)
    {
      QString response;
      res = _httpConnector->deleteFile("album/" + elem->getId() + "/", response);
      if(res == eNO_ERROR)
      {
        cache->erase(elem);
      }
    }

    return res;
  }

  RESULT YafRVFSDriver::deleteFile(const QString& path)
  {
    RESULT res = eERROR_GENERAL;
    QFileInfo qInfo(path);
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

    return res;
  }

  RESULT YafRVFSDriver::moveElement(const QString& path, const QString& newParentId)
  {
    RESULT res = eERROR_GENERAL;
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

  RESULT YafRVFSDriver::createDirectory(const QString& /*path*/, const QString& parentId, const QString& title)
  {
    QString xmlResp;
    RESULT err = _httpConnector->createDirectory(title, parentId, xmlResp);
    if(!err)
    {
      VFSElement elem;
      parseAlbumEntry(xmlResp, elem);

      QString path = WebMounter::getSettingStorage()->getAppStoragePath() + 
        QString(QDir::separator()) + 
        _pluginName + 
        QString(QDir::separator()) + 
        elem.getName();
      QFileInfo fInfo(path);
      elem.setPath(fInfo.absoluteFilePath());

      WebMounter::getCache()->insert(elem);

      if(parentId != ROOT_ID)
      {
        err = moveElement(elem.getPath(), parentId);
      }
    }

    return err;
  }

  QString YafRVFSDriver::getElementPath(QList<VFSElement>& elements, VFSElement& element)
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
      for (int i = 0; i < elements.count(); ++i) 
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

  RESULT YafRVFSDriver::getAlbums(QList<VFSElement>& elements)
  {
    RESULT err = eERROR_GENERAL;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ssZ");
    
    forever
    {
      {
        QMutexLocker locker(&_driverMutex);
        if(_state == eSyncStopping)
        {
          notifyUser(Ui::Notification::eINFO, tr("Info"), tr("Syncing is stopped !\n"));
          return eERROR_CANCEL;
        }
      }
      
      bool entriesFound = false;
      QString xmlResp = "";
      QString path = QString("albums/updated;%1/").arg(dateTime);
      err = _httpConnector->getTreeElements(path, xmlResp);
      if(!err)
      {
        _progressData._currProgress += 5;
        updateSyncStatus(_progressData._currProgress);

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
          QString cap(rx.cap(1));

          parseAlbumEntry(cap, elem);

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

        _progressData._currProgress += 1;
        updateSyncStatus(_progressData._currProgress);
      }

      if(err != eNO_ERROR || !entriesFound)
        break;

      QDateTime dt = QDateTime::fromString(elements[elements.count()-1].getModified(), "yyyy-MM-ddThh:mm:ssZ");
      dt = dt.addSecs(-1); 
      dateTime = dt.toString("yyyy-MM-ddThh:mm:ssZ");
    }

    if(err == eNO_ERROR)
    {
      //Populate path for elemenets
      for(int k=0, count = elements.count(); k < count; k++)
      {
        elements[k].setPath(QFileInfo(getElementPath(elements, elements[k])).absoluteFilePath());
      }

      handleNameDuplicates(elements);

      double albumsMaxProgress = _progressData._currProgress + (_progressData._maxValue - _progressData._currProgress)/3;
      _progressData._currProgress = albumsMaxProgress;
      updateSyncStatus(albumsMaxProgress);
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
    RESULT err = eERROR_GENERAL;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ssZ");

    forever
    {
      {
        QMutexLocker locker(&_driverMutex);
        if(_state == eSyncStopping)
        {
          notifyUser(Ui::Notification::eINFO, tr("Info"), tr("Syncing is stopped !\n"));
          return eERROR_CANCEL;
        }
      }
    
      bool entriesFound = false;
      QString xmlResp = "";
      QString path = QString("photos/updated;%1/").arg(dateTime);

      err = _httpConnector->getTreeElements(path, xmlResp);

      if(!err)
      {
        int count = 0;
        QString pattern = QString::fromAscii("<entry>(.*)</entry>");
        QRegExp rx(pattern);
        rx.setCaseSensitivity(Qt::CaseSensitive);
        rx.setMinimal(true);
        rx.setPatternSyntax(QRegExp::RegExp);

        int pos = rx.indexIn(xmlResp, 0);
        while(pos != -1)
        {
          pos += rx.matchedLength();
          pos = rx.indexIn(xmlResp, pos);
          count++;
        }

        double progressPortion = (count == 0) ? 0 : max((_progressData._maxValue - _progressData._currProgress)/count, double(0));
        pos = rx.indexIn(xmlResp, 0);
        entriesFound = (pos != -1);
        while(pos != -1)
        {
          VFSElement elem;
          QString cap(rx.cap(1));
          parsePhotoEntry(cap, elem);

          if(elem.getId() != elements[elements.count() - 1].getId())
          {
            elem.setDownloaded(false);
            elements.append(elem);
          }

          pos += rx.matchedLength();
          pos = rx.indexIn(xmlResp, pos);

          _progressData._currProgress += progressPortion;
          updateSyncStatus(_progressData._currProgress);
        }
      }

      if(err != eNO_ERROR || !entriesFound)
        break;

      QDateTime dt = QDateTime::fromString(elements[elements.count()-1].getModified(), "yyyy-MM-ddThh:mm:ssZ");
      dt = dt.addSecs(-2); 
      /*QTime time(dt.time().hour(), dt.time().minute(), dt.time().second());
      dt.setTime(time);*/
      dateTime = dt.toString("yyyy-MM-ddThh:mm:ssZ");
    }

    if(err == eNO_ERROR)
    {
      for(int k=0, count = elements.count(); k < count; k++)
      {
        if(elements[k].getType() == VFSElement::FILE)
        {
          QString path;
          int i = findParentIndex(elements, elements[k]);
          if(i < 0)
          {
            return eERROR_GENERAL;
          }
          else
          {
            path = elements[i].getPath() + QString(QDir::separator()) + elements[k].getName();
          }

          QFileInfo fInfo(path);
          elements[k].setPath(fInfo.absoluteFilePath());
        }
      }

      handleNameDuplicates(elements);

      _progressData._currProgress = _progressData._maxValue;
      updateSyncStatus(_progressData._currProgress);
    }

    return err;
  }

  RESULT YafRVFSDriver::getElements(QList<VFSElement>& elements)
  {
    RESULT res = getAlbums(elements);
    if(!res)
    {
      res = getPhotos(elements);
    }

    return res;
  }

  void YafRVFSDriver::parseAlbumEntry(QString& xmlEntry, VFSElement& elem)
  {
    QString temp;

    elem.setType(VFSElement::DIRECTORY);
    elem.setPluginName(_pluginName);

    QString title = QString::fromAscii("<title>(.*)</title>");
    QRegExp rx(title);
    rx.setCaseSensitivity(Qt::CaseSensitive);
    rx.setMinimal(true);
    rx.setPatternSyntax(QRegExp::RegExp);

    rx.indexIn(xmlEntry);
    temp = rx.cap(1);
    if(temp.endsWith("."))
    {
      temp = temp.mid(0, temp.length() - 1);  
    }
    temp = temp.replace("?", "");
    temp = temp.replace(":", " ");
    temp = temp.replace("\"", "'");
    temp = temp.replace("|", " ");
    temp = temp.replace("*", " ");
    temp = temp.replace("\\", " ");
    temp = temp.replace("/", " ");
    temp = temp.replace("<", " ");
    temp = temp.replace(">", " ");
    temp = temp.replace("©", " ");

    elem.setName(temp);

    rx.setPattern("album:(.*)</id>");
    rx.indexIn(xmlEntry);
    elem.setId(rx.cap(1));

    rx.setPattern("<updated>(.*)</updated>");
    rx.indexIn(xmlEntry);
    elem.setModified(rx.cap(1));

    rx.setPattern("<link href=\"(.*)\" rel=\"self\" />");
    rx.indexIn(xmlEntry);
    elem.setSrcUrl(rx.cap(1));

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
    QString temp;
    elem.setType(VFSElement::FILE);
    elem.setPluginName(_pluginName);

    QString title = QString::fromAscii("<title>(.*)</title>");
    QRegExp rx(title);
    rx.setCaseSensitivity(Qt::CaseSensitive);
    rx.setMinimal(true);
    rx.setPatternSyntax(QRegExp::RegExp);

    rx.indexIn(xmlEntry);

    temp = rx.cap(1);
    if(temp.endsWith("."))
    {
      temp = temp.mid(0, temp.length() - 1);  
    }
    temp = temp.replace("?", "");
    temp = temp.replace(":", " ");
    temp = temp.replace("\"", "'");
    temp = temp.replace("|", " ");
    temp = temp.replace("*", " ");
    temp = temp.replace("\\", " ");
    temp = temp.replace("/", " ");
    temp = temp.replace("<", " ");
    temp = temp.replace(">", " ");
    temp = temp.replace("©", " ");
    elem.setName(temp);

    int offset = elem.getName().lastIndexOf(".");
    QString ext = elem.getName().mid(offset + 1).toLower();

    if(offset == -1 
      || (ext != "bmp" 
      && ext != "jpg"
      && ext != "jpeg"
      && ext != "png"
      && ext != "gif"))
    {
      elem.setName(elem.getName() + ".jpg");
    }

    rx.setPattern("photo:(.*)</id>");
    rx.indexIn(xmlEntry);
    elem.setId(rx.cap(1));

    rx.setPattern("<updated>(.*)</updated>");
    rx.indexIn(xmlEntry);
    elem.setModified(rx.cap(1));

    rx.setPattern("api-fotki.yandex.ru/api/users/(.*)/album/(.*)/\" rel=\"album\" />");
    rx.indexIn(xmlEntry);
    elem.setParentId(rx.cap(2));

    rx.setPattern("<content src=\"(.*)\" type=");
    rx.indexIn(xmlEntry);
    elem.setSrcUrl(rx.cap(1));
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

  void YafRVFSDriver::connectHandlerStage2(RESULT error, PluginSettings pluginSettings)
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

      notifyUser(Ui::Notification::eCRITICAL, tr("Error"), tr("Authorization failed !\n"));
    }
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

      updateState(_progressData._currProgress, eSyncStopping);

      _forceSync.wakeAll();

      while(isRunning())
      {
        sleep(1);
      }
    }
    updateState(100, eConnected);
    start(); // start sync thread again
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

  bool YafRVFSDriver::areFileAttributesValid(const QString& /*path*/, unsigned long /*attributes*/)
  {
    return true;///////////(attributes & FILE_ATTRIBUTE_READONLY);
  }
}
