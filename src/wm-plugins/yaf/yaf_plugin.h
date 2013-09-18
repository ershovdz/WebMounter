#ifndef YAF_PLUGIN_H
#define YAF_PLUGIN_H

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../wm-base/plugin_interface.h"
#include "./driver/yaf_driver.h"
#include "./view/yaf_view.h"

class YafPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	YafPlugin();
	~YafPlugin();
private:
	virtual QString name();

	virtual void* getRVFSDriver();
	virtual void* getView();
	virtual void* getSettings();
	virtual QIcon* getIcon();
	virtual QString getTranslationFile(const QString& locale);
private:
	RemoteDriver::YafRVFSDriver* _driver;
	Ui::YafView* _view;
	QIcon* _icon;
};


#endif // YAF_PLUGIN_H
