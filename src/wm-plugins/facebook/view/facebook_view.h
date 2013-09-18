#ifndef FACEBOOK_VIEW_H
#define FACEBOOK_VIEW_H

#include "plugin_view.h"

namespace Ui
{
	class FacebookOAuth;

	class FacebookView : public PluginView
	{
		Q_OBJECT
	public:
		FacebookView(const Data::PluginSettings* settings, const QString& title);

	public slots:
		virtual void updateView(int progress, int state);
		void oAuthFinished(RESULT error, const QString& login, const QString& token);
	private:
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);
	private:
		FacebookOAuth* _oauthObj;
	};
}

#endif // FACEBOOK_VIEW_H
