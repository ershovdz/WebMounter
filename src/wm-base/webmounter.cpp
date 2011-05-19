#include "common_stuff.h"

#include "webmounter.h"

#include "plugin_interface.h"

#include <QDir>
#include <QFileInfo>

#include LVFS_DRIVER_H

using namespace Common;
using namespace Ui;

VFSCache* WebMounter::_vfsCache = 0;
SettingStorage* WebMounter::_globalSettings = 0;
Ui::NotificationDevice* WebMounter::_notificationDevice = 0;
Common::PluginList WebMounter::_pluginList;
FileProxy* WebMounter::_fileProxy = 0;
ILVFSDriver* WebMounter::_lvfsDriver = 0;

WebMounter::WebMounter(void)
{
	_globalSettings = SettingStorage::getStorage();

	_vfsCache = VFSCache::getCache(_globalSettings->getAppSettingStoragePath());

	_fileProxy = FileProxy::CreateFileProxy();

	_lvfsDriver = LVFSDriver::createDriver(_fileProxy);

	loadPlugins();
}

WebMounter::~WebMounter(void)
{
	if(_vfsCache) delete _vfsCache; // to release DB connection
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


void WebMounter::startApp(Ui::NotificationDevice* notificationDevice)
{
	try
	{
		Q_ASSERT(notificationDevice);
		_notificationDevice = notificationDevice;

/*
		if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Joomla.Article"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + "Joomla.Article");
		}

		if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Joomla.Gallery"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + "Joomla.Gallery");
		}

	#ifdef WM_FULL_VERSION
		if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Vkontakte"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + "Vkontakte");
		}

				//if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Google.Docs"))
                //{
				//	dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + "Google.Docs");
                //}

		if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Yandex.Fotki"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + QDir::separator() + "Yandex.Fotki");
		}
	#endif
*/

//		_vfsCache = VFSCache::getCache(_globalSettings->getAppSettingStoragePath());
		
				/*_pluginList.insert(PluginList_Pair("Joomla.Article", PluginPtr(new RemoteDriver::JmmRVFSDriver("Joomla.Article"))));
				_pluginList.insert(PluginList_Pair("Joomla.Gallery", PluginPtr(new RemoteDriver::JmmRVFSDriver("Joomla.Gallery"))));
	#ifdef WM_FULL_VERSION
				_pluginList.insert(PluginList_Pair(QString("Yandex.Fotki"), PluginPtr(new RemoteDriver::YafRVFSDriver("Yandex.Fotki"))));
                _pluginList.insert(PluginList_Pair(QString("Vkontakte"), PluginPtr(new RemoteDriver::VkRVFSDriver("Vkontakte"))));
	#endif
                //_pluginList.insert(PluginList_Pair(QString("Google.Docs"), new RemoteDriver::GoogleRVFSDriver("Google.Docs")));
		*/


//		_fileProxy = FileProxy::CreateFileProxy();

//		_lvfsDriver = LVFSDriver::createDriver(_fileProxy);
	}
	catch(Notification msg)
	{
		_notificationDevice->showNotification(msg);
	}

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
