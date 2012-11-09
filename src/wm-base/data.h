#ifndef DATA_H
#define DATA_H

#include <QSettings>
#include <QMutex>
#include "common_stuff.h"

namespace Data
{
	struct GeneralSettings
	{
		QString proxyAddress;
		QString proxyLogin;
		QString proxyPassword;

		QString appStoragePath;
		QString appSettingStoragePath;

		QString appLang;
		QString driveLetter;
	};

	struct PluginSettings
	{
		QString pluginName;
		QString serverUrl;
		QString userName;
		QString prevUserName;

		QString userPassword; // password is not kept in setting storage !!!

		bool bFullSync;
		bool bAutoSync;
		QString syncPeriod;
		QString lastSync;
		bool isOAuthUsing;
		QString oAuthToken; 
	};

	class WEBMOUNTER_EXPORT SettingStorage
	{
	public:
		static SettingStorage* getStorage();
		bool restoreStorage();

		void addSettings(/*in*/ const GeneralSettings& settings);
		void addSettings(/*in*/ const PluginSettings& settings);

		void getData(/*out*/ GeneralSettings& settings);
		void getData(/*out*/ PluginSettings& settings, /*in*/ QString pluginName);

		QString getAppStoragePath();
		QString getAppSettingStoragePath();

		~SettingStorage();

	private:
		SettingStorage();

	private:
		static SettingStorage* _storageInstance;
		static QMutex _storageMutex;
		QSettings _settings;
	};
}

#endif