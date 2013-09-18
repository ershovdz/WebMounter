#ifndef YANDEX_NAROD_VIEW_H
#define YANDEX_NAROD_VIEW_H

#include "plugin_view.h"

namespace Ui
{
	class YafOAuth;

	class YandexNarodView : public PluginView
	{
		Q_OBJECT
	public:
		YandexNarodView(const Data::PluginSettings* settings, const QString& title);
	
	public slots:
		virtual void updateView(int progress, int state);
		void oAuthFinished(RESULT error, const QString& login, const QString& token);
	private:
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);
	private:
 		YafOAuth* _oauthObj;
	};
}

#endif // YANDEX_NAROD_VIEW_H
