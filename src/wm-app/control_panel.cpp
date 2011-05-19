
#include <QtGui>
#include <QTranslator>

#include "control_panel.h"

#include "webmounter.h"

#include "general_view.h"

#include "plugin_interface.h"

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

		QObject::connect(_generalView, SIGNAL(changeLanguage(const QString&)), this, SLOT(changeLanguage(const QString&)));
		QObject::connect(_pNotificationDevice, SIGNAL(showMsgBox(const QString&, const QString&)), this, SLOT(showMsgBox(const QString&, const QString&)));
		QObject::connect(_pNotificationDevice, SIGNAL(showTrayMsg(int, const QString&, const QString&)), this, SLOT(showTrayMsg(int, const QString&, const QString&)));
	}

	void ControlPanel::showMsgBox(const QString& title, const QString& description)
	{
		QMessageBox::critical(0, title, description, QMessageBox::Cancel);
	}

	void ControlPanel::showTrayMsg(int type, const QString& title, const QString& description)
	{
		QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(type);
		if(trayIcon)
		{
			trayIcon->showMessage(title, description, icon, 3 * 1000);
		}
	}

	void ControlPanel::recreateAllWidgets()
	{
		GeneralSettings generalSettings;

		Common::WebMounter::getSettingStorage()->getData(generalSettings);
		
		if(generalSettings.appLang == "")
		{
			generalSettings.appLang = QLocale::system().name();
		}
		
		initializeTranslators(generalSettings.appLang);

		createActions();
		createTrayIcon();

		_pNotificationDevice = new TrayNotificationDevice(trayIcon);

		connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
		
		QIcon icon = QIcon(":resources/drive.png");//iconComboBox->itemIcon(index);
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
		_pagesWidget->addWidget(_generalView);

		Common::PluginList& plugins = Common::WebMounter::plugins();
		Common::PluginList::iterator iter;
		for(iter=plugins.begin(); iter!=plugins.end(); iter++)
		{
			PluginSettings settings;
			Common::WebMounter::getSettingStorage()->getData(settings, iter->first);

			PluginView* view;
			view = (PluginView*)iter->second->getView();
			_pagesWidget->addWidget(view);
		}

		_closeButton = new QPushButton(tr("Minimize"));

		createIcons();

		_contentsWidget->setCurrentRow(0);

		connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));

		_horizontalLayout = new QHBoxLayout;
		_horizontalLayout->addWidget(_contentsWidget);
		_horizontalLayout->addWidget(_pagesWidget);

		QLabel* version = new QLabel(VERSION);
		version->setDisabled(true);

		_buttonsLayout = new QHBoxLayout;
		//_buttonsLayout->addStretch(1);
		_buttonsLayout->addSpacing(15);
		_buttonsLayout->addWidget(version, 1, Qt::AlignHCenter);
		_buttonsLayout->addWidget(_closeButton, 1, Qt::AlignRight);

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
		initializeTranslators(locale);

		_closeButton->setText(tr("Minimize"));

		//_minimizeAction->setText(tr("Mi&nimize"));
		
		//_maximizeAction->setText(tr("Ma&ximize"));
		
		_restoreAction->setText(tr("&Restore"));
		
		_quitAction->setText(tr("&Quit"));

		_configButton->setText(tr("Configuration"));
	}

	void ControlPanel::showNotification(const Notification& msg)
	{
		_pNotificationDevice->showNotification(msg);
	}

	void ControlPanel::createIcons()
	{
		_configButton = new QListWidgetItem(_contentsWidget);
		_configButton->setIcon(QIcon(":/resources/config.png"));
		_configButton->setText(tr("Configuration"));
		_configButton->setTextAlignment(Qt::AlignHCenter);
		_configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		Common::PluginList& plugins = Common::WebMounter::plugins();
		Common::PluginList::iterator iter;
		for(iter=plugins.begin(); iter!=plugins.end(); iter++)
		{
			QListWidgetItem* button = new QListWidgetItem(_contentsWidget);
			button->setIcon(*(iter->second->getIcon()));
			button->setText(iter->first);
			button->setTextAlignment(Qt::AlignHCenter);
			button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}

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

	QString ControlPanel::translationDir()
	{
		QDir appDir(qApp->applicationDirPath());
		printf("ControlPanel::translationDir, appDir = %s\n", qApp->applicationDirPath().toAscii().data());
		appDir.cd("..");
		printf("appDir = %s\n", appDir.absolutePath().toAscii().data());
		appDir.cd("share/webmounter");
		printf("ControlPanel::translationDir, pluginsdir = %s\n", appDir.absolutePath().toAscii().data());
		return appDir.absolutePath();
	}

	void ControlPanel::initializeTranslators(const QString& locale)
	{
		QApplication::removeTranslator(&_translator);
		bool result = _translator.load(QString("wmbase_" + locale), translationDir());
		QApplication::installTranslator(&_translator);

		for(int i=_pluginTransList.count()-1; i>=0; i--)
		{
			QTranslator* tr = _pluginTransList[i];
			QApplication::removeTranslator(tr);
			_pluginTransList.removeLast();
			delete tr;
		}

		printf("ControlPanel::initializeTranslators, after removing\n");
		Common::PluginList& plugins = Common::WebMounter::plugins();
		Common::PluginList::iterator iter;
		for(iter=plugins.begin(); iter!=plugins.end(); iter++)
		{
			//printf("ControlPanel::initializeTranslators, plugin = %p\n", iter->second);
			QString filename(iter->second->getTranslationFile(locale));
			char* name = filename.toUtf8().data();
			if(filename.length())
			{
				QTranslator* tr = new QTranslator;
				printf("ControlPanel::initializeTranslators, tr = %p\n", tr);
				result = tr->load(filename, translationDir());
				QApplication::installTranslator(tr);
				_pluginTransList.append(tr);
			}
		}
		printf("ControlPanel::initializeTranslators, after addition\n");
	}
};
