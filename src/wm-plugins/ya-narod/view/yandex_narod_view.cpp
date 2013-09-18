#include "yaf_oauth.h"
#include "yandex_narod_view.h"
#include "../driver/yandex_narod_driver.h"
#include "webmounter.h"

namespace Ui
{
	using namespace RemoteDriver;

	YandexNarodView::YandexNarodView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("narod.yandex.ru");
		_urlEdit->setEnabled(false);

//		_oauthCheckBox->setChecked(true);
//		_oauthCheckBox->setVisible(false);

		_authGroup = new QGroupBox(tr("Authorization"));
		_authLayout = new QGridLayout;
		//_authLayout->addWidget(_urlLabel, 0, 0);
		//_authLayout->addWidget(_urlEdit, 0, 1);
		_authLayout->addWidget(_nameLabel, 1, 0);
		_authLayout->addWidget(_nameEdit, 1, 1);
		_authLayout->addWidget(_passwordLabel, 2, 0);
		_authLayout->addWidget(_passwordEdit, 2, 1);
		//_authLayout->addWidget(_oauthCheckBox, 3, 0);
		_authGroup->setLayout(_authLayout);

		_mainLayout->insertWidget(0, _authGroup);

//		_oauthObj = new YafOAuth();
//		connect(_oauthObj, SIGNAL(authFinished(RESULT, const QString&, const QString&)), this, SLOT(oAuthFinished(RESULT, const QString&, const QString&)));
	}

	bool YandexNarodView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void YandexNarodView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);
	}

	void YandexNarodView::oAuthFinished(RESULT /*error*/, const QString& /*login*/, const QString& /*token*/)
	{
//		PluginSettings pluginSettings;
//		Common::WebMounter::getSettingStorage()->getData(pluginSettings, "Yandex.Fotki");
//		pluginSettings.userName = login;

//		static_cast<YandexNarodRVFSDriver*>(_driver)->connectHandlerStage2(error, pluginSettings, token);
	}
}
