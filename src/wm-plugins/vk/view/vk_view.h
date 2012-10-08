#ifndef VK_VIEW_H
#define VK_VIEW_H

#include "plugin_view.h"

namespace Ui
{
	class VkOAuth;

	class VkView : public PluginView
	{
		Q_OBJECT
	public:
		VkView(const Data::PluginSettings* settings, const QString& title);

	public slots:
		virtual void updateView(int progress, int state);
		void oAuthFinished(RESULT error, const QString& login, const QString& token);
	private:
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);
	private:
 		VkOAuth* _oauthObj;
	};
}

#endif
