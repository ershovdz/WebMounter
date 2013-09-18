
#include <QtGui>
#include <QTranslator>

#include "control_panel.h"

#include "webmounter.h"

#include "general_view.h"

#include "plugin_interface.h"
//#include "fvupdater.h"
//#include "fvupdatewindow.h"

namespace Ui
{
  ControlPanel::ControlPanel()
  {
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint );

    _contentsWidget = 0;
    _pagesWidget = 0;
    _horizontalLayout = 0;
    _buttonsLayout = 0;
    _mainLayout = 0;
    _helpButton = 0;

    _bShowOnCloseMessage = true;

    recreateAllWidgets();
    
    //QObject::connect(_generalView, SIGNAL(changeLanguage(const QString&)), this, SLOT(changeLanguage(const QString&)));
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

    QIcon icon = QIcon(":resources/drive.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->show();

    _contentsWidget = new QListWidget;
    _contentsWidget->setViewMode(QListView::IconMode);
    _contentsWidget->setIconSize(QSize(75, 55));
    _contentsWidget->setMovement(QListView::Static);
		_contentsWidget->setMaximumWidth(110);
		_contentsWidget->setMinimumWidth(110);
		_contentsWidget->setMinimumHeight(390);
		_contentsWidget->setMaximumHeight(390);
    _contentsWidget->setSpacing(10);

    _pagesWidget = new QStackedWidget;

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

    _generalView = new GeneralView(generalSettings, this);
    connect(_generalView, SIGNAL(mount()), this, SIGNAL(mount()));
    connect(_generalView, SIGNAL(unmount()), this, SIGNAL(unmount())); 

    _pagesWidget->addWidget(_generalView);

    createIcons();

    _contentsWidget->setCurrentRow(0);

    _horizontalLayout = new QHBoxLayout;
    _horizontalLayout->addWidget(_contentsWidget);
    _horizontalLayout->addWidget(_pagesWidget);

    _version = new QLabel(tr("version ") + VERSION);
    _version->setDisabled(true);

    _helpButton = new QPushButton;
    _helpButton->setText(tr("Help..."));
    connect(_helpButton, SIGNAL(clicked()), this, SLOT(onHelpClicked()));

    _buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    _buttonsLayout->addSpacing(135);
    _buttonsLayout->addWidget(_version, 1, Qt::AlignLeft);
    _buttonsLayout->addWidget(_helpButton, 1, Qt::AlignLeft);
    _buttonsLayout->addStretch(5);

    _mainLayout = new QVBoxLayout;
    _mainLayout->addLayout(_horizontalLayout);
    _mainLayout->addStretch(1);
    _mainLayout->addSpacing(12);
    _mainLayout->addLayout(_buttonsLayout);
    setLayout(_mainLayout);

    setWindowTitle(tr("WebMounter"));

		this->setMinimumWidth(620);
		this->setMinimumHeight(420);

		this->setMaximumWidth(620);
		this->setMaximumHeight(420);

		resize(620, 420);
  }

  void ControlPanel::onHelpClicked()
  {
    QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/Help.chm");
    QDesktopServices::openUrl(QUrl("file:///" + path));
  }

  void ControlPanel::mounted()
  {
    _generalView->mounted();
  }

  void ControlPanel::unmounted()
  {
    _generalView->unmounted();
  }

  void ControlPanel::changeLanguage(const QString& locale)
  {
    initializeTranslators(locale);

    _version->setText(tr("version ") + VERSION);
    _helpButton->setText(tr("Help..."));
    _restoreAction->setText(tr("&Restore"));
	_aboutAction->setText(tr("&About"));
	_quitAction->setText(tr("&Quit"));

    _configButton->setText(tr("Configuration"));
  }

  //void ControlPanel::showNotification(const Notification& msg)
  //{
  //  _pNotificationDevice->showNotification(msg);
  //}

  void ControlPanel::createIcons()
  {
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

    /*_configButton = new QListWidgetItem(_contentsWidget);
    _configButton->setIcon(QIcon(":/resources/config.png"));
    _configButton->setText(tr("Configuration"));
    _configButton->setTextAlignment(Qt::AlignHCenter);
    _configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/

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

  void ControlPanel::closeEvent(QCloseEvent * /*event*/)
  {
	  delete _pNotificationDevice;

	  qDeleteAll(_transList.begin(), _transList.end());
	  _transList.clear();

    QApplication::instance()->quit();
  }

  void ControlPanel::setIcon(int /*index*/)
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
      showNormal();
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

	_aboutAction = new QAction(tr("&About"), this);
	connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	
    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
  }

  void ControlPanel::createTrayIcon()
  {
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(_restoreAction);
    trayIconMenu->addSeparator();
	trayIconMenu->addAction(_aboutAction);
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
    for(int i=_transList.count()-1; i>=0; i--)
    {
      QTranslator* tr = _transList[i];
      QApplication::removeTranslator(tr);
      _transList.removeLast();
      delete tr;
    }

    printf("ControlPanel::initializeTranslators, after removing\n");
    QTranslator* tr = new QTranslator;
    bool result = tr->load(QString("wmbase_" + locale), translationDir());
    QApplication::installTranslator(tr);
    _transList.append(tr);

    tr = new QTranslator;
    result = tr->load(QString("webmounter_" + locale), translationDir());
    QApplication::installTranslator(tr);
    _transList.append(tr);

    Common::PluginList& plugins = Common::WebMounter::plugins();
    Common::PluginList::iterator iter;
    for(iter=plugins.begin(); iter!=plugins.end(); iter++)
    {
      //printf("ControlPanel::initializeTranslators, plugin = %p\n", iter->second);
      QString filename(iter->second->getTranslationFile(locale));
      if(filename.length())
      {
        QTranslator* tr1 = new QTranslator;
        printf("ControlPanel::initializeTranslators, tr = %p\n", tr1);
        result = tr1->load(filename, translationDir());
        if(result)
        {
          QApplication::installTranslator(tr1);
          _transList.append(tr1);
        }
		else
		{
			delete tr1;
		}
      }
    }
    printf("ControlPanel::initializeTranslators, after addition\n");
  }

  void ControlPanel::changeEvent(QEvent* e)
  {
    switch (e->type())
    {
    case QEvent::WindowStateChange:
      {
        if (this->windowState() & Qt::WindowMinimized)
        {
          QTimer::singleShot(250, this, SLOT(hide()));
        }
        break;
      }
    default:
      break;
    }

    QDialog::changeEvent(e);
  }

  void ControlPanel::about()
  {
        QString text1 = tr("<h4>Application allows to map http storages as local</h4>\n\n");
	QString author = tr("<h5>Author: Alex Ershov</h5>\n");
        QString text2 = tr("<h5>For technical support</h5>");
        QString text3 = "<a href=\"ershav@yandex.ru\">mailto:ershav@yandex.ru</a>";
        QString text4 = tr("<h5>or see</h5>");
        QMessageBox::about(this, tr("About"), text1+author+text2+text3+text4);
  }
};
