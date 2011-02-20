#ifndef VK_VIEW_H
#define VK_VIEW_H

#include "plugin_view.h"

namespace Ui
{
	class VkView : public PluginView
	{
		Q_OBJECT
	public:
		VkView(const Data::PluginSettings* settings, const QString& title);
		virtual void changeLang();

	public slots:
		virtual void updateView(int progress, int state);

	private:
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);
	};
}

#endif