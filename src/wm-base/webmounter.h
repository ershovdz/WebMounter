#ifndef WEBMOUNTER_H
#define WEBMOUNTER_H

#include <vector>
#include <QString>

#include "vfs_cache.h"
#include "plugin_interface.h"
#include "rvfs_driver.h"
#include "lvfs_driver.h"

#include "data.h"

#include "file_proxy.h"

#include "notification_device.h"
#include "IGui.h"

//#include "control_panel.h"

using std::vector;
using namespace Data;
using namespace LocalDriver;

namespace Common
{
	typedef boost::shared_ptr<PluginInterface> PluginPtr;
	typedef std::map<QString, PluginPtr> PluginList;
	typedef std::pair <QString, PluginPtr> PluginList_Pair;

	class WEBMOUNTER_EXPORT WebMounter : public QObject
	{
		Q_OBJECT

	public:
		WebMounter(void);
		~WebMounter(void);

		void startApp(Ui::IGui* pGui);

		static Ui::NotificationDevice* getNotificationDevice() { return _pGui->getNotificationDevice(); };
		static RemoteDriver::RVFSDriver* getPlugin(const QString& pluginName);
		static FileProxy* getProxy();

		static VFSCache* getCache() {return _vfsCache;};
		static LocalDriver::ILVFSDriver* getLocalDriver();
		static SettingStorage* getSettingStorage() {return _globalSettings;};

		static PluginList& plugins() { return _pluginList; };

		public slots:
			void mount();
			void unmount();

			private slots:
				void mounted();
				void unmounted();

	private:
		bool loadPlugins();
		void initGlobalSettings();
#ifdef Q_OS_WIN
		QString findFreeDriveLetter();
#endif

		//static QString _rootPath;

		static LocalDriver::ILVFSDriver* _lvfsDriver;
		static PluginList _pluginList;
		static VFSCache* _vfsCache;
		static SettingStorage* _globalSettings;
		static FileProxy* _fileProxy;
		static Ui::IGui* _pGui;
	};
}

#endif
