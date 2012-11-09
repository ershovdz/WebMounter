#include "plugin_view.h"

#include "webmounter.h"

namespace Ui
{
	PluginView::PluginView(const Data::PluginSettings* settings, const QString& /*title*/)
	{
		_driver = 0;

		if(!settings)
		{

			_dummyLabel = new QLabel(tr("<font size=\"5\" color=\"red\" align=\"right\">Not implemented yet</font>"));

			_mainLayout = new QVBoxLayout;
			_mainLayout->setAlignment(Qt::AlignHCenter);
			_mainLayout->addWidget(_dummyLabel);
			setLayout(_mainLayout);
			return;
		}

		_oauthCheckBox = new QCheckBox(tr("Use OAuth"));
		_oauthCheckBox->setChecked(settings->isOAuthUsing);
		_oauthCheckBox->setVisible(false);

		_urlLabel = new QLabel(tr("Server address:"));
		_urlEdit = new QLineEdit;
		_urlEdit->setText(settings->serverUrl);

		_nameLabel = new QLabel(tr("Login:"));
		_nameLabel->setTextFormat(Qt::RichText);
		_nameEdit = new QLineEdit;
		_nameEdit->setText(!settings->isOAuthUsing ? settings->userName : "");
		_nameEdit->setEnabled(!settings->isOAuthUsing);

		_passwordLabel = new QLabel(tr("Password:"));
		_passwordEdit = new QLineEdit;

		_passwordEdit->setEchoMode(QLineEdit::Password);
		_passwordEdit->setEnabled(!settings->isOAuthUsing);

		_startPluginButton = new QPushButton(tr("Connect"));
		_stopPluginButton = new QPushButton(tr("Disconnect"));

		_stopPluginButton->setEnabled(false);

		_authGroup = new QGroupBox(tr("Authorization"));
		_authLayout = new QGridLayout;
		_authLayout->addWidget(_urlLabel, 0, 0);
		_authLayout->addWidget(_urlEdit, 0, 1);
		_authLayout->addWidget(_nameLabel, 1, 0);
		_authLayout->addWidget(_nameEdit, 1, 1);
		_authLayout->addWidget(_passwordLabel, 2, 0);
		_authLayout->addWidget(_passwordEdit, 2, 1);
		_authLayout->addWidget(_oauthCheckBox, 3, 0);

		_authGroup->setLayout(_authLayout);

		_buttonGroup = new QGroupBox();
		_buttonLayout = new QGridLayout;
		_buttonLayout->addWidget(_startPluginButton, 0, 0);
		_buttonLayout->addWidget(_stopPluginButton, 0, 1);
		_buttonGroup->setLayout(_buttonLayout);

		_progressGroup = new QGroupBox(tr("Status"));
		_progressLayout = new QGridLayout;
		_progressLayout->setAlignment(Qt::AlignRight);
		_statusLabel = new QLabel(tr("Plugin status:"));
		_statusValue = new QLabel(tr("<font color=\"red\">Not connected</font>"));
		_statusValue->setTextFormat(Qt::RichText);

		_progressBarLabel = new QLabel(tr("Progress:"));
		_progressBar = new QProgressBar();
		_progressBar->setValue(0);

		_progressLayout->addWidget(_statusLabel, 0,0);
		_progressLayout->addWidget(_statusValue, 0,1);
		_progressLayout->addWidget(_progressBarLabel, 1,0);
		_progressLayout->addWidget(_progressBar, 1,1);
		_progressGroup->setLayout(_progressLayout);

		_mainLayout = new QVBoxLayout;
		_mainLayout->addWidget(_authGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addWidget(_progressGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addWidget(_buttonGroup);
		_mainLayout->addStretch(1);
		setLayout(_mainLayout);

		_pluginName = settings->pluginName;
		_driver = Common::WebMounter::getPlugin(_pluginName);

		connect(_oauthCheckBox, SIGNAL(stateChanged(int)), this, SLOT(oauthClicked(int)));

		if(_driver)
		{
			_driverState = _driver->getState();

			/*connect(_startSyncButton, SIGNAL(clicked (bool)), this, SLOT(startSyncClicked(bool)));
			connect(_stopSyncButton, SIGNAL(clicked (bool)), this, SLOT(stopSyncClicked(bool)));
			*/connect(_startPluginButton, SIGNAL(clicked (bool)), this, SLOT(startPluginClicked(bool)));
			connect(_stopPluginButton, SIGNAL(clicked (bool)), this, SLOT(stopPluginClicked(bool)));

			connect(this, SIGNAL(connectPlugin(Data::PluginSettings&)), _driver, SLOT(startPlugin(Data::PluginSettings&)));
			connect(this, SIGNAL(disconnectPlugin()), _driver, SLOT(stopPlugin()));
			/*connect(this, SIGNAL(startSync()), _driver, SLOT(startSync()));
			connect(this, SIGNAL(stopSync()), _driver, SLOT(stopSync()));*/

			connect(_driver, SIGNAL(updateView(int, int)), this, SLOT(updateView(int, int)));

		}
		else
		{
			_driverState = RemoteDriver::eNotConnected;

			_urlEdit->setEnabled(false);
			_nameEdit->setEnabled(false);
			_passwordEdit->setEnabled(false);

			_startPluginButton->setEnabled(false);
			_stopPluginButton->setEnabled(false);

			_statusValue->setText(tr("<font color=\"red\">Plugin not available...</font>"));
		}
	}

	void PluginView::changeLang()
	{
		if(!_driver)
		{
			_dummyLabel->setText(tr("<font size=\"5\" color=\"red\" align=\"right\">Not implemented yet</font>"));
			return;
		}
		_oauthCheckBox->setText(tr("Use OAuth"));
		_urlLabel->setText(tr("Server address:"));
		_nameLabel->setText(tr("Login:"));
		_passwordLabel->setText(tr("Password:"));

		_startPluginButton->setText(tr("Connect"));
		_stopPluginButton->setText(tr("Disconnect"));

		_authGroup->setTitle(tr("Authorization"));

		_progressGroup->setTitle(tr("Status:"));
		_statusLabel->setText(tr("Plugin status:"));

		if(!_driver)
		{
			_statusValue->setText(tr("<font color=\"red\">Not available</font>"));
		}

		switch(_driver->getState())
		{
		case RemoteDriver::eAuthInProgress:
			{
				_statusValue->setText(tr("<font color=\"green\">Authorization...</font>"));
				break;
			}
		case RemoteDriver::eAuthorized:
			{
				_statusValue->setText(tr("<font color=\"green\">Authorized</font>"));
				break;
			}
		case RemoteDriver::eConnected:
			{
				_statusValue->setText(tr("<font color=\"green\">Connected</font>"));
				break;
			}
		case RemoteDriver::eNotConnected:
			{
				_statusValue->setText(tr("<font color=\"red\">Not Connected</font>"));
				break;
			}
		case RemoteDriver::eSyncStopping:
			{
				_statusValue->setText(tr("<font color=\"green\">Sync stopping...</font>"));
				break;
			}
		case RemoteDriver::eSync:
			{
				_statusValue->setText(tr("<font color=\"green\">Synchronization...</font>"));
				break;
			}
		};

		_progressBarLabel->setText(tr("Progress:"));
	}

	void PluginView::changeEvent ( QEvent * event )
	{
		if(event->type() == QEvent::LanguageChange)
		{
			changeLang();
		}
		QWidget::changeEvent(event);
	}

	void PluginView::oauthClicked(int state)
	{
		if(state)
		{
			_nameEdit->setText("");
			_passwordEdit->setText("");
		}

		_nameEdit->setEnabled(!state);
		_passwordEdit->setEnabled(!state);
	}

	void PluginView::startPluginClicked(bool)
	{
		Data::PluginSettings pluginSettings;
		WebMounter::getSettingStorage()->getData(pluginSettings, _pluginName);

		pluginSettings.bAutoSync = true;
		pluginSettings.bFullSync = true;
		pluginSettings.pluginName = _pluginName;
		pluginSettings.serverUrl = _urlEdit->text();
		pluginSettings.syncPeriod.setNum( 300 ); // 5 min

		if(_nameEdit->isEnabled())
		{
			pluginSettings.userName = _nameEdit->text();
			pluginSettings.userPassword = _passwordEdit->text();
		}

		pluginSettings.isOAuthUsing = _oauthCheckBox->isChecked();

		if(pluginSettings.serverUrl == "")
		{
			QMessageBox::critical(0, tr("Error"),
				tr("Enter server address"), QMessageBox::Ok);
			return;
		}
		else if(pluginSettings.userName == "" && !pluginSettings.isOAuthUsing)
		{
			QMessageBox::critical(0, tr("Error"),
				tr("Enter login"), QMessageBox::Ok);
			return;
		}
		else if(_passwordEdit->text() == "" && !pluginSettings.isOAuthUsing)
		{
			QMessageBox::critical(0, tr("Error"),
				tr("Enter password"), QMessageBox::Ok);
			return;
		}

		_urlEdit->setEnabled(false);
		_nameEdit->setEnabled(false);
		_passwordEdit->setEnabled(false);

		_startPluginButton->setEnabled(false);
		_stopPluginButton->setEnabled(true);

		_statusValue->setText(tr("<font color=\"green\">Connecting...</font>"));

		emit connectPlugin(pluginSettings);
	}

	void PluginView::stopPluginClicked(bool)
	{
		_stopPluginButton->setEnabled(false);
		//_statusValue->setText(tr("<font color=\"red\">Not Connected</font>"));
		emit disconnectPlugin();
	}

	void PluginView::updateView(int progress, int state)
	{
		_progressBar->setValue(progress);

		switch(state)
		{
		case RemoteDriver::eAuthInProgress:
			{
				if(_driverState == RemoteDriver::eNotConnected)
				{
					_driverState = RemoteDriver::eAuthInProgress;
					_statusValue->setText(tr("<font color=\"green\">Authorization...</font>"));
				}
				break;
			}
		case RemoteDriver::eAuthorized:
			{
				if(_driverState == RemoteDriver::eAuthInProgress)
				{
					_driverState = RemoteDriver::eAuthorized;
					_statusValue->setText(tr("<font color=\"green\">Authorized</font>"));
				}
				break;
			}
		case RemoteDriver::eConnected:
			{
				if(_driverState == RemoteDriver::eSync
					|| _driverState == RemoteDriver::eSyncStopping)
				{
					_driverState = RemoteDriver::eConnected;

					_statusValue->setText(tr("<font color=\"green\">Connected</font>"));

					_progressBar->setValue(0);
				}
				break;
			}
		case RemoteDriver::eNotConnected:
			{
				_driverState = RemoteDriver::eNotConnected;

				_urlEdit->setEnabled(true);
				_nameEdit->setEnabled(!_oauthCheckBox->isChecked());
				_passwordEdit->setEnabled(!_oauthCheckBox->isChecked());

				_startPluginButton->setEnabled(true);
				_stopPluginButton->setEnabled(false);

				_statusValue->setText(tr("<font color=\"red\">Not Connected</font>"));

				_progressBar->setValue(0);

				break;
			}
		case RemoteDriver::eSyncStopping:
			{
				if(_driverState == RemoteDriver::eConnected
					|| _driverState == RemoteDriver::eSync 
					|| _driverState == RemoteDriver::eSyncStopping)
				{
					_driverState = RemoteDriver::eSyncStopping;

					_statusValue->setText(tr("<font color=\"green\">Sync stopping...</font>"));
				}

				break;
			}
		case RemoteDriver::eSync:
			{
				if(_driverState == RemoteDriver::eAuthorized
					|| _driverState == RemoteDriver::eConnected)
				{
					_driverState = RemoteDriver::eSync;

					_statusValue->setText(tr("<font color=\"green\">Synchronization...</font>"));
				}
				break;
			}
		}
	}
};

