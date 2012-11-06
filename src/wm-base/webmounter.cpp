#include "common_stuff.h"

#include "webmounter.h"

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
  initGlobalSettings();

  _vfsCache = VFSCache::getCache( _globalSettings->getAppSettingStoragePath() );

  _fileProxy = FileProxy::CreateFileProxy();

  _lvfsDriver = LVFSDriver::createDriver( _fileProxy );


  connect(_lvfsDriver, SIGNAL(mounted()), this, SLOT(mounted()));
  connect(_lvfsDriver, SIGNAL(unmounted()), this, SLOT(unmounted())); 

  loadPlugins();
}

void WebMounter::initGlobalSettings()
{
  QNetworkProxyFactory::setUseSystemConfiguration( true );
  QList<QNetworkProxy> proxyList = QNetworkProxyFactory::systemProxyForQuery();

  _globalSettings = SettingStorage::getStorage();
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

  Common::WebMounter::getSettingStorage()->addSettings(settings);
  _globalSettings = SettingStorage::getStorage();
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
  printf("pluginsdir = %s\n", qApp->applicationDirPath().toAscii().data());
  pluginsDir.cd("..");
  printf("pluginsdir = %s\n", pluginsDir.absolutePath().toAscii().data());
  pluginsDir.cd("lib");
  printf("pluginsdir = %s\n", pluginsDir.absolutePath().toAscii().data());

  if(pluginsDir.exists(pluginsDir.absolutePath() + QDir::separator() + "webmounter"))
  {
    pluginsDir.cd("webmounter/plugins");
    printf("pluginsdir = %s\n", pluginsDir.absolutePath().toAscii().data());
  }
  else
  {
    pluginsDir.cd("plugins");
    printf("pluginsdir = %s\n", pluginsDir.absolutePath().toAscii().data());
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
        printf("pluginname = %s\n", name.toAscii().data());
        _pluginList.insert(PluginList_Pair(pluginInterface->name(), PluginPtr(pluginInterface)));

        QDir dir;
        if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + name))
        {
          dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + name);
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