
#include "facebook_plugin.h"
#include "./driver/facebook_driver.h"
#include "./view/facebook_view.h"
#include "../../wm-base/data.h"
#include "webmounter.h"

FacebookPlugin::FacebookPlugin() : _driver(NULL), _view(NULL), _icon(NULL)
{
}

FacebookPlugin::~FacebookPlugin()
{
}

QString FacebookPlugin::name()
{
	QString name("Facebook");
	return name;
}

void* FacebookPlugin::getRVFSDriver()
{
	if(!_driver)
		_driver = new RemoteDriver::FacebookRVFSDriver(name());
	return (void*)(_driver);
}

void* FacebookPlugin::getView()
{
	if(!_view)
	{
		Data::PluginSettings settings;
		settings.pluginName = name();
		Common::WebMounter::getSettingStorage()->getData(settings, name());

		_view = new Ui::FacebookView(&settings, name());
	}

	return (void*)(_view);
}

void* FacebookPlugin::getSettings()
{
	return NULL;
}

QIcon* FacebookPlugin::getIcon()
{
	if(!_icon)
	{
		_icon = new QIcon(":/icons/facebook.png");
	}

	return _icon;
}

QString FacebookPlugin::getTranslationFile(const QString& locale)
{
	return QString("facebook_wm_pl_" + locale);
}

Q_EXPORT_PLUGIN2(wm-facebook-plugin, FacebookPlugin);
