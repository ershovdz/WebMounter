
#include "yandex_disk_plugin.h"
#include "./driver/yandex_disk_driver.h"
#include "./view/yandex_disk_view.h"
#include "data.h"
#include "webmounter.h"
 
YaDiskPlugin::YaDiskPlugin() : _driver(NULL), _view(NULL), _icon(NULL)
{
}

YaDiskPlugin::~YaDiskPlugin()
{
	delete _driver;
}

QString YaDiskPlugin::name()
{
	QString name("Yandex.Disk");
	return name;
}

void* YaDiskPlugin::getRVFSDriver()
{
	if(!_driver)
		_driver = new RemoteDriver::YaDiskRVFSDriver(name());
	return (void*)(_driver);
}

void* YaDiskPlugin::getView()
{
 	if(!_view)
 	{
 		Data::PluginSettings settings;
 		settings.pluginName = name();
 		Common::WebMounter::getSettingStorage()->getData(settings, name());
 
 		_view = new Ui::YaDiskView(&settings, name());
 	}

	return (void*)(_view);
}

void* YaDiskPlugin::getSettings()
{
	return NULL;
}

QIcon* YaDiskPlugin::getIcon()
{
	if(!_icon)
	{
		_icon = new QIcon(":/icons/yandex_disk.png");
	}

	return _icon;
}

QString YaDiskPlugin::getTranslationFile(const QString& locale)
{
	char* loc = locale.toUtf8().data();
	QString file("yandex_disk_wm_pl_" + locale);
	return file;
}

Q_EXPORT_PLUGIN2(wm-ya-disk-plugin, YaDiskPlugin);
