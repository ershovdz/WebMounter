#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QtCore/qplugin.h>
#include <QString>
#include <QIcon>

 class PluginInterface
 {
 public:
	 virtual ~PluginInterface() {}

	 virtual QString name() = 0;
	 virtual void* getRVFSDriver() = 0;
	 virtual void* getView() = 0;
	 virtual void* getSettings() = 0;
	 virtual QIcon* getIcon() = 0;
	 virtual QString getTranslationFile(const QString& locale) = 0;
 };

 Q_DECLARE_INTERFACE(PluginInterface, "com.jsoft.webmounter.plugin/1.0");

#endif // PLUGIN_INTERFACE_H
