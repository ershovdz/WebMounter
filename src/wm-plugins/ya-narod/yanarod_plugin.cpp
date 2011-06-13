
#include "yanarod_plugin.h"
#include "./driver/yandex_narod_driver.h"
#include "./view/yandex_narod_view.h"
#include "data.h"
#include "webmounter.h"
 
YaNarodPlugin::YaNarodPlugin() : _driver(NULL), _view(NULL), _icon(NULL)
{
}

YaNarodPlugin::~YaNarodPlugin()
{
	delete _driver;
}

QString YaNarodPlugin::name()
{
	QString name("Yandex.Narod");
	return name;
}

void* YaNarodPlugin::getRVFSDriver()
{
	if(!_driver)
		_driver = new RemoteDriver::YandexNarodRVFSDriver(name());
	return (void*)(_driver);
}

void* YaNarodPlugin::getView()
{
 	if(!_view)
 	{
 		Data::PluginSettings settings;
 		settings.pluginName = name();
 		Common::WebMounter::getSettingStorage()->getData(settings, name());
 
		_view = new Ui::YandexNarodView(&settings, name());
 	}

	return (void*)(_view);
}

void* YaNarodPlugin::getSettings()
{
	return NULL;
}

QIcon* YaNarodPlugin::getIcon()
{
	if(!_icon)
	{
		_icon = new QIcon(":/icons/yandexnarod.png");
	}

	return _icon;
}

QString YaNarodPlugin::getTranslationFile(const QString& locale)
{
	char* loc = locale.toUtf8().data();
	QString file("yanarod_wm_pl_" + locale);
	return file;
}

Q_EXPORT_PLUGIN2(wm-ya-narod-plugin, YaNarodPlugin);
