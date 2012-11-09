
#include "vk_plugin.h"
#include "./driver/vk_driver.h"
#include "./view/vk_view.h"
#include "../../wm-base/data.h"
#include "webmounter.h"

VkPlugin::VkPlugin() : _driver(NULL), _view(NULL), _icon(NULL)
{
}

VkPlugin::~VkPlugin()
{
}

QString VkPlugin::name()
{
	QString name("Vkontakte");
	return name;
}

void* VkPlugin::getRVFSDriver()
{
	if(!_driver)
		_driver = new RemoteDriver::VkRVFSDriver(name());
	return (void*)(_driver);
}

void* VkPlugin::getView()
{
	if(!_view)
	{
		Data::PluginSettings settings;
		settings.pluginName = name();
		Common::WebMounter::getSettingStorage()->getData(settings, name());

		_view = new Ui::VkView(&settings, name());
	}

	return (void*)(_view);
}

void* VkPlugin::getSettings()
{
	return NULL;
}

QIcon* VkPlugin::getIcon()
{
	if(!_icon)
	{
		_icon = new QIcon(":/icons/vk.png");
	}

	return _icon;
}

QString VkPlugin::getTranslationFile(const QString& locale)
{
	return QString("vk_wm_pl_" + locale);
}

Q_EXPORT_PLUGIN2(wm-vk-plugin, VkPlugin);
