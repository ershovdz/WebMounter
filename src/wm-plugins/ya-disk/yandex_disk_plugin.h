#ifndef YANDEX_DISK_PLUGIN_H
#define YANDEX_DISK_PLUGIN_H

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../wm-base/plugin_interface.h"
#include "./driver/yandex_disk_driver.h"
#include "./view/yandex_disk_view.h"

class YaDiskPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	YaDiskPlugin();
	~YaDiskPlugin();
private:
	virtual QString name();

	virtual void* getRVFSDriver();
	virtual void* getView();
	virtual void* getSettings();
	virtual QIcon* getIcon();
	virtual QString getTranslationFile(const QString& locale);
private:
	RemoteDriver::YaDiskRVFSDriver* _driver;
	Ui::YaDiskView* _view;
	QIcon* _icon;
};


#endif // YAF_PLUGIN_H
