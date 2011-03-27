
#include <QtGui>

#include "control_panel.h"

#include "WebMounter.h"
//#include "LocalDriver.h"
//#include "FileProxy.h"
//#include "jmmrvfsdriver.h"

#include "general_view.h"
#include "plugins\yaf\view\yaf_view.h"
#include "plugins\vk\view\vk_view.h"
#include "plugins\google\view\google_view.h"

namespace Ui
{
	//using namespace Common;
	//using namespace LocalDriver;
	//using namespace RemoteDriver;

	ControlPanel::ControlPanel()
	{
		_contentsWidget = 0;
		_pagesWidget = 0;
		_closeButton = 0;
		_horizontalLayout = 0;
		_buttonsLayout = 0;
		_mainLayout = 0;

		_bShowOnCloseMessage = true;

		recreateAllWidgets();

		QObject::connect(_pNotificationDevice, SIGNAL(showMsgBox(const QString&, const QString&)), this, SLOT(showMsgBox(const QString&, const QString&)));

	}
	void ControlPanel::showMsgBox(const QString& title, const QString& description)
	{
		QMessageBox::critical(0, title, description, QMessageBox::Cancel);
	}

	void ControlPanel::recreateAllWidgets()
	{
		PluginSettings googleSettings;
		PluginSettings yafSettings;
		PluginSettings vkSettings;
		PluginSettings jmmgSettings;
		PluginSettings jmmaSettings;
		GeneralSettings generalSettings;

		Common::WebMounter::getSettingStorage()->getData(googleSettings, "Google.Docs");
	#ifdef WM_VERSION_FULL
		Common::WebMounter::getSettingStorage()->getData(yafSettings, "Yandex.Fotki");
		Common::WebMounter::getSettingStorage()->getData(vkSettings, "Vkontakte");
	#endif
		Common::WebMounter::getSettingStorage()->getData(jmmgSettings, "Joomla.Gallery");
		Common::WebMounter::getSettingStorage()->getData(jmmaSettings, "Joomla.Article");
		Common::WebMounter::getSettingStorage()->getData(generalSettings);
		
		
		if(generalSettings.appLang == "")
		{
			generalSettings.appLang = QLocale::system().name();
		}
		
		_translator.load(QString("webmounter_" + generalSettings.appLang));
		QApplication::installTranslator(&_translator);

		createActions();
		createTrayIcon();

		/*ptrMsgBox->critical(0, "sdsd",
					"sdsdsdsd", QMessageBox::Cancel);*/

		_pNotificationDevice = new TrayNotificationDevice(trayIcon);

		connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
		
		QIcon icon = QIcon(":/Resources/drive.png");//iconComboBox->itemIcon(index);
		trayIcon->setIcon(icon);
		setWindowIcon(icon);

		trayIcon->show();
		
		_contentsWidget = new QListWidget;
		_contentsWidget->setViewMode(QListView::IconMode);
		_contentsWidget->setIconSize(QSize(75, 55));
		_contentsWidget->setMovement(QListView::Static);
		_contentsWidget->setMaximumWidth(120);
		_contentsWidget->setMinimumWidth(120);
		_contentsWidget->setMinimumHeight(530);
		_contentsWidget->setMaximumHeight(530);
		_contentsWidget->setSpacing(10);

		_pagesWidget = new QStackedWidget;
		
		_generalView = new GeneralView(generalSettings, this);
	
	#ifdef WM_VERSION_FULL	
		_vkView		 = new VkView(&vkSettings,  QString(tr("Vkontakte")));
		_yafView	 = new YafView(&yafSettings,  QString(tr("Yandex.Fotki")));
	#endif
		_jmmgView	 = new PluginView(&jmmgSettings, QString(tr("Joomla (Gallery)")));
		_jmmaView	 = new PluginView(&jmmaSettings,  QString(tr("Joomla (Article)")));
		_googleView	 = new GoogleView(&googleSettings,  QString(tr("Google Docs")));
		//_odnView	 = new PluginView(0,  QString(tr("Odnoklassniki")));

		_pagesWidget->addWidget(_generalView);
	#ifdef WM_VERSION_FULL	
		_pagesWidget->addWidget(_vkView);
		_pagesWidget->addWidget(_yafView);
	#endif	
		_pagesWidget->addWidget(_googleView);
		//_pagesWidget->addWidget(_odnView);
		_pagesWidget->addWidget(_jmmgView);
		_pagesWidget->addWidget(_jmmaView);
		
		_closeButton = new QPushButton(tr("Minimize"));

		createIcons();

		_contentsWidget->setCurrentRow(0);

		connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));

		_horizontalLayout = new QHBoxLayout;
		_horizontalLayout->addWidget(_contentsWidget);
		_horizontalLayout->addWidget(_pagesWidget);

		_buttonsLayout = new QHBoxLayout;
		_buttonsLayout->addStretch(1);
		_buttonsLayout->addWidget(_closeButton);

		_mainLayout = new QVBoxLayout;
		_mainLayout->addLayout(_horizontalLayout);
		_mainLayout->addStretch(1);
		_mainLayout->addSpacing(12);
		_mainLayout->addLayout(_buttonsLayout);
		setLayout(_mainLayout);

		setWindowTitle(tr("WebMounter"));

		this->setMinimumWidth(620);
		this->setMinimumHeight(550);

		this->setMaximumWidth(620);
		this->setMaximumHeight(550);

		resize(620, 550);
	}
	
	void ControlPanel::changeLanguage(const QString& locale)
	{
		QApplication::removeTranslator(&_translator);
		bool result = _translator.load(QString("webmounter_" + locale));
		QApplication::installTranslator(&_translator);

		_closeButton->setText(tr("Minimize"));

		//_minimizeAction->setText(tr("Mi&nimize"));
		
		//_maximizeAction->setText(tr("Ma&ximize"));
		
		_restoreAction->setText(tr("&Restore"));
		
		_quitAction->setText(tr("&Quit"));

		_configButton->setText(tr("Configuration"));
		
	#ifdef WM_VERSION_FULL	
		_vkButton->setText(tr("Vkontakte"));
		_yafButton->setText(tr("Yandex Fotki"));
	#endif
	
		_jmmgButton->setText(tr("Joomla (Gallery)"));		
		_jmmaButton->setText(tr("Joomla (Article)"));

		_googleButton->setText(QString(tr("Google Docs")));
		//_odnButton->setText(QString(tr("Odnoklassniki")));
	}

	void ControlPanel::showNotification(const Notification& msg)
	{
		_pNotificationDevice->showNotification(msg);
	}

	void ControlPanel::createIcons()
	{
		_configButton = new QListWidgetItem(_contentsWidget);
		_configButton->setIcon(QIcon(":/Resources/config.png"));
		_configButton->setText(tr("Configuration"));
		_configButton->setTextAlignment(Qt::AlignHCenter);
		_configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	#ifdef WM_VERSION_FULL	
		_vkButton = new QListWidgetItem(_contentsWidget);
		_vkButton->setIcon(QIcon(":/Resources/vk.png"));
		_vkButton->setText(tr("Vkontakte"));
		_vkButton->setTextAlignment(Qt::AlignHCenter);
		_vkButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		_yafButton = new QListWidgetItem(_contentsWidget);
		_yafButton->setIcon(QIcon(":/Resources/yaf.png"));
		_yafButton->setText(tr("Yandex Fotki"));
		_yafButton->setTextAlignment(Qt::AlignHCenter);
		_yafButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	#endif
	
		_googleButton = new QListWidgetItem(_contentsWidget);
		_googleButton->setIcon(QIcon(":/Resources/docs_logo_ru.png"));
		_googleButton->setText(tr("Google Docs"));
		_googleButton->setTextAlignment(Qt::AlignHCenter);
		_googleButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		/*_odnButton = new QListWidgetItem(_contentsWidget);
		_odnButton->setIcon(QIcon(":/Resources/odnoklassniki.png"));
		_odnButton->setText(tr("Odnoklassniki"));
		_odnButton->setTextAlignment(Qt::AlignHCenter);
		_odnButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/

		_jmmgButton = new QListWidgetItem(_contentsWidget);
		_jmmgButton->setIcon(QIcon(":/Resources/Joomla.png"));
		_jmmgButton->setText(tr("Joomla (Gallery)"));
		_jmmgButton->setTextAlignment(Qt::AlignHCenter);
		_jmmgButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		_jmmaButton = new QListWidgetItem(_contentsWidget);
		_jmmaButton->setIcon(QIcon(":/Resources/Joomla.png"));
		_jmmaButton->setText(tr("Joomla (Article)"));
		_jmmaButton->setTextAlignment(Qt::AlignHCenter);
		_jmmaButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		connect(_contentsWidget,
			SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
	}

	void ControlPanel::changePage(QListWidgetItem *current, QListWidgetItem *previous)
	{
		if (!current)
			current = previous;

		_pagesWidget->setCurrentIndex(_contentsWidget->row(current));
	}

	void ControlPanel::setVisible(bool visible)
	{
		_restoreAction->setEnabled(isMaximized() || !visible);
		QDialog::setVisible(visible);
	}

	void ControlPanel::closeEvent(QCloseEvent *event)
	{
	}

	void ControlPanel::setIcon(int index)
	{
	}

	void ControlPanel::iconActivated(QSystemTrayIcon::ActivationReason reason)
	{
		switch (reason) 
		{
		case QSystemTrayIcon::MiddleClick:
			//showMessage();
			break;
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			this->setVisible(true);
			break;
		default:
			;
		}
	}

	

	void ControlPanel::messageClicked()
	{
		
	}
	
	void ControlPanel::createActions()
	{
		_restoreAction = new QAction(tr("&Restore"), this);
		connect(_restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

		_quitAction = new QAction(tr("&Quit"), this);
		connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	}

	void ControlPanel::createTrayIcon()
	{
		trayIconMenu = new QMenu(this);
		trayIconMenu->addAction(_restoreAction);
		trayIconMenu->addSeparator();
		trayIconMenu->addAction(_quitAction);

		trayIcon = new QSystemTrayIcon(this);
		trayIcon->setContextMenu(trayIconMenu);
	}
};