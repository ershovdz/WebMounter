#ifndef VK_PLUGIN_H
#define VK_PLUGIN_H

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../wm-base/plugin_interface.h"
#include "./driver/vk_driver.h"
#include "./view/vk_view.h"

class VkPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	VkPlugin();
	~VkPlugin();
private:
	virtual QString name();

	virtual void* getRVFSDriver();
	virtual void* getView();
	virtual void* getSettings();
	virtual QIcon* getIcon();
	virtual QString getTranslationFile(const QString& locale);
private:
	RemoteDriver::VkRVFSDriver* _driver;
	Ui::VkView* _view;
	QIcon* _icon;
};


#endif // VK_PLUGIN_H
