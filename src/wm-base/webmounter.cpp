#include "common_stuff.h"

#include "webmounter.h"
#include "filesystemtools.h"
#include "plugin_interface.h"

#include <QDir>
#include <QFileInfo>
#include <QtNetwork/QNetworkProxyFactory>

#include LVFS_DRIVER_H

using namespace Common;
using namespace Ui;

VFSCache* WebMounter::_vfsCache = 0;
SettingStorage* WebMounter::_globalSettings = 0;
Ui::IGui* WebMounter::_pGui = 0;
Common::PluginList WebMounter::_pluginList;
FileProxy* WebMounter::_fileProxy = 0;
ILVFSDriver* WebMounter::_lvfsDriver = 0;

WebMounter::WebMounter(void)
{
  _globalSettings = SettingStorage::getStorage();
  
  _vfsCache = VFSCache::getCache( _globalSettings->getAppSettingStoragePath() );
  cleanCacheIfNeeded();
  
  initGlobalSettings();
  
  _fileProxy = FileProxy::CreateFileProxy();

  _lvfsDriver = LVFSDriver::createDriver( _fileProxy );

  connect(_lvfsDriver, SIGNAL(mounted()), this, SLOT(mounted()));
  connect(_lvfsDriver, SIGNAL(unmounted()), this, SLOT(unmounted())); 

  loadPlugins();
}

void WebMounter::initGlobalSettings()
{
  QNetworkProxyFactory::setUseSystemConfiguration( true );
  QList<QNetworkProxy> proxyList = QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(QUrl("http://www.yandex.ru")));

  Data::GeneralSettings settings;
  _globalSettings->getData( settings );

  if( !proxyList.empty() )
  {
    if( !proxyList[0].hostName().isEmpty() )
    {
      settings.proxyAddress = proxyList[0].hostName() + ":" + QString::number(proxyList[0].port());
    }
  }

#ifdef Q_OS_WIN
  settings.driveLetter = findFreeDriveLetter();
#endif

  _globalSettings->addSettings(settings);
}

#ifdef Q_OS_WIN
QString WebMounter::findFreeDriveLetter()
{
  QFileInfoList driveList = QFSFileEngine::drives();

  for(char a = 67; a < 91; a++)
  {
    bool insert = true;
    QString letter = (QString::fromAscii(&a, 1) + ":");
    QString letterWithSlash = (QString::fromAscii(&a, 1) + ":/");

    for( QFileInfoList::Iterator iter = driveList.begin(); 
      iter != driveList.end();
      ++iter )
    {
      if(iter->canonicalPath() == letter || iter->canonicalPath() == letterWithSlash)
      {
        insert = false;
        break;
      }
    }
    if(insert)
    {
      return letter;
    }
  }

  return "";
}
#endif

WebMounter::~WebMounter(void)
{
  delete _lvfsDriver;
  delete _fileProxy;
  if(_vfsCache) delete _vfsCache; // to release DB connection
  delete _globalSettings;
}

RemoteDriver::RVFSDriver* WebMounter::getPlugin(const QString& pluginName)
{
  PluginList::iterator iter;
  iter = _pluginList.find(pluginName);
  if(iter != _pluginList.end())
  {
    return (RemoteDriver::RVFSDriver*)iter->second.get()->getRVFSDriver();
  }
  return 0;
}

FileProxy* WebMounter::getProxy()
{
  return _fileProxy;
}

void WebMounter::startApp(Ui::IGui* pGui)
{
  Q_ASSERT(pGui);
  _pGui = pGui;

  mount();
}

LocalDriver::ILVFSDriver* WebMounter::getLocalDriver()
{
  return _lvfsDriver;
}

bool WebMounter::loadPlugins()
{
  QDir pluginsDir(qApp->applicationDirPath());
  pluginsDir.cd("..");
  pluginsDir.cd("lib");
  
  if(pluginsDir.exists(pluginsDir.absolutePath() + QDir::separator() + "webmounter"))
  {
    pluginsDir.cd("webmounter/plugins");
  }
  else
  {
    pluginsDir.cd("plugins");
  }

  foreach (QString fileName, pluginsDir.entryList(QDir::Files))
  {
    QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
    QObject *plugin = pluginLoader.instance();
    if (plugin)
    {
      PluginInterface* pluginInterface = qobject_cast<PluginInterface*>(plugin);
      if(pluginInterface)
      {
        QString name = pluginInterface->name();
        _pluginList.insert(PluginList_Pair(pluginInterface->name(), PluginPtr(pluginInterface)));

        QDir dir;
        if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + name))
        {
          dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + name);
        }

		QFileInfo fInfo(_globalSettings->getAppStoragePath() + QDir::separator() + name);
		VFSElement elem = VFSElement(VFSElement::DIRECTORY
			, fInfo.absoluteFilePath()
			, "ROOT"
			, ""
			, ""
			, ""
			, ROOT_ID
			, ROOT_ID
			, ""
			, name);

		VFSCache::iterator parentDir = WebMounter::getCache()->find(_globalSettings->getAppStoragePath() + QDir::separator() + name);
		if(parentDir == WebMounter::getCache()->end())
		{
			WebMounter::getCache()->insert(elem);
		}
      }
    }
  }

  return true;
}

void WebMounter::mount()
{
  Q_ASSERT(_lvfsDriver);

  Data::GeneralSettings settings;
  _globalSettings->getData( settings );

  _lvfsDriver->mount( settings );
}

void WebMounter::unmount()
{
  Q_ASSERT(_lvfsDriver);
  _lvfsDriver->unmount();
}

void WebMounter::mounted()
{
  _pGui->mounted();
}

void WebMounter::unmounted()
{
  _pGui->unmounted();

}

void WebMounter::cleanCacheIfNeeded() 
{
  Data::GeneralSettings settings;
  _globalSettings->getData( settings );
  QString currentVersion(VERSION);
  if( currentVersion != settings.appVersion)
  {
    if( largeDifference(currentVersion, settings.appVersion) )
    {
      cleanCaches();
    }
    settings.appVersion = currentVersion;
    _globalSettings->addSettings(settings);
  }
}

bool WebMounter::largeDifference(QString version1, QString version2)
{
  if( "" == version1 || "" == version2 || !version1.startsWith(version2.mid(0,4)) ) // e.g. 1.3.0 and 1.2.0
    return true;
  else
    return false;
}

void WebMounter::cleanCaches()
{
  if( NULL == _vfsCache )
    throw std::string("vfsCache has not been loaded");

  Data::GeneralSettings settings;
  _globalSettings->getData( settings );

  QDir dir( settings.appStoragePath );

  FileSystemTools::removeFolderContent( dir );
  _vfsCache->clean();
}