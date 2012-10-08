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


void WebMounter::startApp(Ui::NotificationDevice* notificationDevice)
{
	try
	{
		Q_ASSERT(notificationDevice);
		_notificationDevice = notificationDevice;

/*
		if(!dir.exists(_globalSettings->getAppStoragePath() + QDir::separator() + "Joomla.Article"))
		{
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
