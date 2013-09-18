#ifndef YANDEX_NAROD_PLUGIN_H
#define YANDEX_NAROD_PLUGIN_H

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../wm-base/plugin_interface.h"
#include "./driver/yandex_narod_driver.h"
#include "./view/yandex_narod_view.h"

class YaNarodPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	YaNarodPlugin();
	~YaNarodPlugin();
private:
	virtual QString name();

	virtual void* getRVFSDriver();
	virtual void* getView();
	virtual void* getSettings();
	virtual QIcon* getIcon();
	virtual QString getTranslationFile(const QString& locale);
private:
	RemoteDriver::YandexNarodRVFSDriver* _driver;
	Ui::YandexNarodView* _view;
	QIcon* _icon;
};


#endif // YANDEX_NAROD_PLUGIN_H
