#ifndef WEBMOUNTER_H
#define WEBMOUNTER_H

#include <vector>
#include <QString>

#include "vfs_cache.h"
#include "rvfs_driver.h"
#include "lvfs_driver.h"

#include "data.h"

#include "file_proxy.h"

#include "tray_notification_device.h"
#include "control_panel.h"

using std::vector;
using namespace Data;
using namespace LocalDriver;

namespace Common
{
	class WebMounter
	{
	public:
		WebMounter(void) {};
		~WebMounter(void);

		void startApp();

		static Ui::ControlPanel* getControlPanel() {return _panel;};
		static RemoteDriver::RVFSDriver* getPlugin(const QString& pluginName);
		static FileProxy* getProxy();
		
		static VFSCache* getCache() {return _vfsCache;};
		static LocalDriver::ILVFSDriver* getLocalDriver();
		static SettingStorage* getSettingStorage() {return _globalSettings;};

	private:
		typedef boost::shared_ptr<RemoteDriver::RVFSDriver> PluginPtr;
		typedef std::map<QString, PluginPtr> PluginList;
		typedef std::pair <QString, PluginPtr> PluginList_Pair;

		//static QString _rootPath;

		static LocalDriver::ILVFSDriver* _lvfsDriver;
		static PluginList _pluginList;
		static VFSCache* _vfsCache;
		static SettingStorage* _globalSettings;
		static FileProxy* _fileProxy;
		static Ui::ControlPanel* _panel;
	};
}

#endif
