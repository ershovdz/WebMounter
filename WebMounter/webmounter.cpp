#include "common_stuff.h"


#include "plugins.h"

#include "WebMounter.h"

#include <QDir>
#include <QFileInfo>

#include LVFS_DRIVER_H

using namespace Common;
using namespace Ui;

VFSCache* WebMounter::_vfsCache = 0;
SettingStorage* WebMounter::_globalSettings = 0;
Ui::ControlPanel* WebMounter::_panel = 0;
Common::WebMounter::PluginList WebMounter::_pluginList;
FileProxy* WebMounter::_fileProxy = 0;
ILVFSDriver* WebMounter::_lvfsDriver = 0;
		
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
		return iter->second.get();
	}
	return 0;
}

FileProxy* WebMounter::getProxy()
{
	return _fileProxy;
}


void WebMounter::startApp()
{
	try
	{
		QDir dir;
		
		_globalSettings = SettingStorage::getStorage();

		if(!dir.exists(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "\\WebMounter"))
		{
			dir.mkdir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "\\WebMounter");
		}

		if(!dir.exists(_globalSettings->getAppStoragePath()))
		{
			dir.mkdir(_globalSettings->getAppStoragePath());
		}

		if(!dir.exists(_globalSettings->getAppSettingStoragePath()))
		{
			dir.mkdir(_globalSettings->getAppSettingStoragePath());
		}

		if(!dir.exists(_globalSettings->getAppStoragePath() + "\\Joomla.Article"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + "\\Joomla.Article");
		}

		if(!dir.exists(_globalSettings->getAppStoragePath() + "\\Joomla.Gallery"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + "\\Joomla.Gallery");
		}

		if(!dir.exists(_globalSettings->getAppStoragePath() + "\\Vkontakte"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + "\\Vkontakte");
		}

		if(!dir.exists(_globalSettings->getAppStoragePath() + "\\Yandex.Fotki"))
		{
			dir.mkdir(_globalSettings->getAppStoragePath() + "\\Yandex.Fotki");
		}

		_vfsCache = VFSCache::getCache(_globalSettings->getAppSettingStoragePath());
		
		_pluginList.insert(PluginList_Pair("Joomla.Article", new RemoteDriver::JmmRVFSDriver("Joomla.Article")));
		_pluginList.insert(PluginList_Pair("Joomla.Gallery", new RemoteDriver::JmmRVFSDriver("Joomla.Gallery")));
		_pluginList.insert(PluginList_Pair(QString("Yandex.Fotki"), new RemoteDriver::YafRVFSDriver("Yandex.Fotki")));
		_pluginList.insert(PluginList_Pair(QString("Vkontakte"), new RemoteDriver::VkRVFSDriver("Vkontakte")));
		
		_fileProxy = FileProxy::CreateFileProxy();

		_lvfsDriver = LVFSDriver::createDriver(_fileProxy);

		_panel = new Ui::ControlPanel;
		_panel->show();
		
	}
	catch(Notification msg)
	{
		//_pNotificationDevice->showNotification(msg);
	}

}

LocalDriver::ILVFSDriver* WebMounter::getLocalDriver()
{
	return _lvfsDriver;
}
