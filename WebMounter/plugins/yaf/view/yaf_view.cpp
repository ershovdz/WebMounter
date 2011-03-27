#include "yaf_oauth.h"
#include "yaf_view.h"
#include "../driver/yaf_driver.h"
#include "WebMounter.h"

namespace Ui
{
	using namespace RemoteDriver;

	YafView::YafView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("fotki.yandex.ru");
		_urlEdit->setEnabled(false);

#ifdef WM_VERSION_FULL
		_keyLabel->setVisible(false);
		_keyEdit->setVisible(false);
		_keyUrl->setVisible(false);
#endif
		_oauthCheckBox->setChecked(true);
		_oauthCheckBox->setVisible(true);

		_oauthObj = new YafOAuth();
		connect(_oauthObj, SIGNAL(authFinished(RESULT, const QString&, const QString&)), this, SLOT(oAuthFinished(RESULT, const QString&, const QString&)));
	}

	bool YafView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void YafView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);

		PluginSettings pluginSettings; 
		WebMounter::getSettingStorage()->getData(pluginSettings, "Yandex.Fotki");
		if(pluginSettings.isOAuthUsing && (state == RemoteDriver::eAuthInProgress) && progress == 0) // Started to authenticate
		{
			_oauthObj->authenticate();
		}
	}

	void YafView::oAuthFinished(RESULT error, const QString& login, const QString& token)
	{
		PluginSettings pluginSettings; 
		WebMounter::getSettingStorage()->getData(pluginSettings, "Yandex.Fotki");
		pluginSettings.userName = login;

		static_cast<YafRVFSDriver*>(_driver)->connectHandlerStage2(error, pluginSettings, token);
	}
}
