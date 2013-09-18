#ifndef YANDEX_DISK_VIEW_H
#define YANDEX_DISK_VIEW_H

#include "plugin_view.h"

namespace Ui
{
	class YaDiskOAuth;

	class YaDiskView : public PluginView
	{
		Q_OBJECT
	public:
		YaDiskView(const Data::PluginSettings* settings, const QString& title);
	
	public slots:
		virtual void updateView(int progress, int state);
		void oAuthFinished(RESULT error, const QString& login, const QString& token);
	private:
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);
	private:
 		YaDiskOAuth* _oauthObj;
	};
}

#endif // YANDEX_DISK_VIEW_H
