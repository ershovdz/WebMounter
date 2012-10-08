#include "yandex_disk_oauth.h"
#include "yandex_disk_view.h"
#include "../driver/yandex_disk_driver.h"
#include "webmounter.h"

namespace Ui
{
	using namespace RemoteDriver;

	YaDiskView::YaDiskView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("webdav.yandex.ru");
		_urlEdit->setEnabled(false);

#ifdef WM_FULL_VERSION
		_keyLabel->setVisible(false);
		_keyEdit->setVisible(false);
		_keyUrl->setVisible(false);
#endif
		_oauthCheckBox->setChecked(true);
		_oauthCheckBox->setVisible(false);

		_nameLabel->setVisible(false);
		_nameEdit->setVisible(false);
		_passwordLabel->setVisible(false);
		_passwordEdit->setVisible(false);

		_oauthObj = new YaDiskOAuth();
		connect(_oauthObj, SIGNAL(authFinished(RESULT, const QString&, const QString&)), this, SLOT(oAuthFinished(RESULT, const QString&, const QString&)));
	}

	bool YaDiskView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void YaDiskView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);

		_nameLabel->setVisible(false);
		_nameEdit->setVisible(false);
		_passwordLabel->setVisible(false);
		_passwordEdit->setVisible(false);

		PluginSettings pluginSettings; 
		Common::WebMounter::getSettingStorage()->getData(pluginSettings, "Yandex.Disk");
		if(pluginSettings.isOAuthUsing && pluginSettings.oAuthToken == "" && (state == RemoteDriver::eAuthInProgress) && progress == 0) // Started to authenticate
		{
			_oauthObj->authenticate();
		}
	}

	void YaDiskView::oAuthFinished(RESULT error, const QString& login, const QString& token)
	{
		PluginSettings pluginSettings; 
		Common::WebMounter::getSettingStorage()->getData(pluginSettings, "Yandex.Disk");
		pluginSettings.userName = login;
		pluginSettings.oAuthToken = token;

		static_cast<YaDiskRVFSDriver*>(_driver)->connectHandlerStage2(error, pluginSettings);
	}
}
