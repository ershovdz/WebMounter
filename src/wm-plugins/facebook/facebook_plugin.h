#ifndef FACEBOOK_PLUGIN_H
#define FACEBOOK_PLUGIN_H

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../wm-base/plugin_interface.h"
#include "./driver/facebook_driver.h"
#include "./view/facebook_view.h"

class FacebookPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	FacebookPlugin();
	~FacebookPlugin();
private:
	virtual QString name();

	virtual void* getRVFSDriver();
	virtual void* getView();
	virtual void* getSettings();
	virtual QIcon* getIcon();
	virtual QString getTranslationFile(const QString& locale);
private:
	RemoteDriver::FacebookRVFSDriver* _driver;
	Ui::FacebookView* _view;
	QIcon* _icon;
};


#endif // FACEBOOK_PLUGIN_H
