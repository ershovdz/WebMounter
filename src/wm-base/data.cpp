#include "data.h"
#include <QMutexLocker>
#include <QDesktopServices>
#include <QDir>

namespace Data
{
  SettingStorage* SettingStorage::_storageInstance = 0;
  QMutex SettingStorage::_storageMutex;

  SettingStorage::SettingStorage() 
    : _settings(QSettings::IniFormat, QSettings::UserScope, QString("JSoft"), QString("WebMounter"))
  {	
    QString storagePath = _settings.value("general/appStoragePath").toString();
    QString settingStoragePath = _settings.value("general/appSettingStoragePath").toString();
    QDir dir;

    QString fn = _settings.fileName();
    if(storagePath == "")
    {
      storagePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
      storagePath += QString(QDir::separator()) + "storage";

      _settings.setValue("general/appStoragePath", storagePath);
    }

    if(settingStoragePath == "")
    {
      settingStoragePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
      settingStoragePath += QString(QDir::separator()) + "settings";

      _settings.setValue("general/appSettingStoragePath", settingStoragePath);
    }

    bool res = false;
    if(!dir.exists(storagePath))
    {
      res = dir.mkpath(storagePath);
    }

    if(!dir.exists(settingStoragePath))
    {
      res = dir.mkpath(settingStoragePath);
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
    _settings.setValue("appVersion", settings.appVersion);
    _settings.endGroup();
  }

  void SettingStorage::addSettings(const PluginSettings& settings)
  {
    QMutexLocker locker(&_storageMutex);

    _settings.beginGroup(settings.pluginName);
    _settings.setValue("serverUrl", settings.serverUrl);
    _settings.setValue("userName", settings.userName);
    _settings.setValue("prevUserName", settings.prevUserName);
    _settings.setValue("fullSync", settings.bFullSync);
    _settings.setValue("autoSync", settings.bAutoSync);
    _settings.setValue("syncPeriod", settings.syncPeriod);
    _settings.setValue("lastSync", settings.lastSync);
    _settings.setValue("isOAuthUsing", settings.isOAuthUsing);
    _settings.setValue("oAuthToken", settings.oAuthToken);
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
    settings.appVersion	= _settings.value("general/appVersion", "").toString();
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
    settings.prevUserName = _settings.value("prevUserName", "").toString();
    settings.bFullSync  = _settings.value("fullSync", true).toBool();
    settings.bAutoSync  = _settings.value("autoSync", true).toBool();
    settings.syncPeriod = _settings.value("syncPeriod", "300").toString();
    settings.isOAuthUsing = _settings.value("isOAuthUsing", false).toBool();
    settings.oAuthToken = _settings.value("oAuthToken", "").toString();

    if(!settings.bAutoSync)
    {
      settings.syncPeriod = QString("300");
    }
    settings.lastSync   = _settings.value("lastSync", "").toString();

    _settings.endGroup();
  }
};

