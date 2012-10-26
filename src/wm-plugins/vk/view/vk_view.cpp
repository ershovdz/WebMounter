#include "vk_oauth.h"
#include "common_stuff.h"
#include "vk_view.h"
#include "../driver/vk_driver.h"
#include "webmounter.h"

namespace Ui
{
	using namespace RemoteDriver;
	using namespace Common;

	VkView::VkView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("vkontakte.ru");
		_urlEdit->setEnabled(false);

		_keyLabel->setVisible(false);
		_keyEdit->setVisible(false);
		_keyUrl->setVisible(false);

		_oauthCheckBox->setChecked(true);
		_oauthCheckBox->setVisible(false);

		_nameLabel->setVisible(false);
		_nameEdit->setVisible(false);
		_passwordLabel->setVisible(false);
		_passwordEdit->setVisible(false);

		_oauthObj = new VkOAuth();
		connect(_oauthObj, SIGNAL(authFinished(RESULT, const QString&, const QString&)), this, SLOT(oAuthFinished(RESULT, const QString&, const QString&)));
	}

	bool VkView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void VkView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);

		_nameLabel->setVisible(false);
		_nameEdit->setVisible(false);
		_passwordLabel->setVisible(false);
		_passwordEdit->setVisible(false);

		PluginSettings pluginSettings; 
		Common::WebMounter::getSettingStorage()->getData(pluginSettings, "Vkontakte");
		if(pluginSettings.isOAuthUsing && (state == RemoteDriver::eAuthInProgress) && progress == 0) // Started to authenticate
		{
			_oauthObj->authenticate();
		}
	}

	void VkView::oAuthFinished(RESULT error, const QString& login, const QString& token)
	{
		if(error == eERROR_CANCEL)
		{
			emit disconnectPlugin();
		}
		else
		{
			PluginSettings pluginSettings; 
			Common::WebMounter::getSettingStorage()->getData(pluginSettings, "Vkontakte");
			pluginSettings.userName = login;
			pluginSettings.oAuthToken = token;

			static_cast<VkRVFSDriver*>(_driver)->connectHandlerStage2(error, pluginSettings);
		}
	}
}
