#include <data.h>
#include <QMutexLocker>
#include <QDesktopServices>
#include <QDir>

namespace Data
{
	SettingStorage* SettingStorage::_storageInstance = 0;
	QMutex SettingStorage::_storageMutex;
	
	SettingStorage::SettingStorage() : _settings(QSettings::Format::NativeFormat,
											QSettings::Scope::UserScope,
											QString("JSoft"),
											QString("WebMounter"))
	{	
		QString storagePath = _settings.value("general/appStoragePath").toString();
		QString settingStoragePath = _settings.value("general/appSettingStoragePath").toString();
		QDir dir;

		if(storagePath == "")
		{
			storagePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
			storagePath += QDir::separator() + _settings.applicationName() + QDir::separator() + "storage";
			
			_settings.setValue("general/appStoragePath", storagePath);
		}
		
		if(settingStoragePath == "")
		{
			settingStoragePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
			settingStoragePath += QDir::separator() + _settings.applicationName() + QDir::separator() + "settings";
			
			_settings.setValue("general/appSettingStoragePath", settingStoragePath);
		}

		if(!dir.exists(storagePath))
		{
			dir.mkdir(storagePath);
		}

		if(!dir.exists(settingStoragePath))
		{
			dir.mkdir(storagePath);
		}
	}

	SettingStorage::~SettingStorage()
	{
	}

	SettingStorage* SettingStorage::getStorage()
	{
		QMutexLocker locker(&_storageMutex);

		if(!_storageInstance)
		{
			_storageInstance = new SettingStorage();
		}
		return _storageInstance;
	}

	void SettingStorage::addSettings(const GeneralSettings& settings)
	{
		QMutexLocker locker(&_storageMutex);

		_settings.beginGroup("general");
		_settings.setValue("proxyAddress", settings.proxyAddress);
		_settings.setValue("proxyLogin", settings.proxyLogin);
		_settings.setValue("proxyPassword", settings.proxyPassword);
		_settings.setValue("appStoragePath", settings.appStoragePath);
		_settings.setValue("appLang", settings.appLang);
		_settings.setValue("driveLetter", settings.driveLetter);
		_settings.endGroup();
	}

	void SettingStorage::addSettings(const PluginSettings& settings)
	{
		QMutexLocker locker(&_storageMutex);

		_settings.beginGroup(settings.pluginName);
		_settings.setValue("serverUrl", settings.serverUrl);
		_settings.setValue("userName", settings.userName);
		_settings.setValue("fullSync", settings.bFullSync);
		_settings.setValue("autoSync", settings.bAutoSync);
		_settings.setValue("syncPeriod", settings.syncPeriod);
		_settings.setValue("lastSync", settings.lastSync);
		_settings.setValue("key", settings.key);
		_settings.setValue("isOAuthUsing", settings.isOAuthUsing);
		_settings.endGroup();
	}

	void SettingStorage::getData(GeneralSettings& settings)
	{
		QMutexLocker locker(&_storageMutex);

		settings.proxyAddress   = _settings.value("general/proxyAddress", "").toString();
		settings.proxyLogin     = _settings.value("general/proxyLogin", "").toString();
		settings.proxyPassword  = _settings.value("general/proxyPassword", "").toString();
		settings.appStoragePath = _settings.value("general/appStoragePath", "").toString();
		settings.appLang		= _settings.value("general/appLang", "").toString();
		settings.driveLetter	= _settings.value("general/driveLetter", "").toString();
	}

	QString SettingStorage::getAppStoragePath()
	{
		QMutexLocker locker(&_storageMutex);

		return  _settings.value("general/appStoragePath", "").toString();
	}

	QString SettingStorage::getAppSettingStoragePath()
	{
		QMutexLocker locker(&_storageMutex);

		return  _settings.value("general/appSettingStoragePath", "").toString();
	}

	void SettingStorage::getData(PluginSettings& settings, QString pluginName)
	{
		QMutexLocker locker(&_storageMutex);

		settings.pluginName   = pluginName;
		_settings.beginGroup(pluginName);

		settings.serverUrl  = _settings.value("serverUrl", "").toString();
		settings.userName   = _settings.value("userName", "").toString();
		settings.bFullSync  = _settings.value("fullSync", true).toBool();
		settings.bAutoSync  = _settings.value("autoSync", false).toBool();
		settings.syncPeriod = _settings.value("syncPeriod", "86400").toString();
		settings.isOAuthUsing = _settings.value("isOAuthUsing", false).toBool();

		if(!settings.bAutoSync)
		{
			settings.syncPeriod = QString("86400");
		}
		settings.lastSync   = _settings.value("lastSync", "").toString();
		settings.key		= _settings.value("key", "").toString();

		_settings.endGroup();
	}
};

