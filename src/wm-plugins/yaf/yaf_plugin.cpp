
#include "yaf_plugin.h"
#include "./driver/yaf_driver.h"
#include "./view/yaf_view.h"
#include "data.h"
#include "webmounter.h"

YafPlugin::YafPlugin() : _driver(NULL), _view(NULL), _icon(NULL)
{
}

YafPlugin::~YafPlugin()
{
	delete _driver;
}

QString YafPlugin::name()
{
	QString name("Yandex.Fotki");
	return name;
}

void* YafPlugin::getRVFSDriver()
{
	if(!_driver)
		_driver = new RemoteDriver::YafRVFSDriver(name());
	return (void*)(_driver);
}

void* YafPlugin::getView()
{
	if(!_view)
	{
		Data::PluginSettings settings;
		settings.pluginName = name();
		Common::WebMounter::getSettingStorage()->getData(settings, name());

		_view = new Ui::YafView(&settings, name());
	}

	return (void*)(_view);
}

void* YafPlugin::getSettings()
{
	return NULL;
}

QIcon* YafPlugin::getIcon()
{
	if(!_icon)
	{
		_icon = new QIcon(":/icons/yaf.png");
	}

	return _icon;
}

QString YafPlugin::getTranslationFile(const QString& locale)
{
	QString file("yaf_wm_pl_" + locale);
	return file;
}

Q_EXPORT_PLUGIN2(wm-yandex-plugin, YafPlugin);
