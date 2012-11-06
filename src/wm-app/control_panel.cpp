
#include <QtGui>
#include <QTranslator>

#include "control_panel.h"

#include "webmounter.h"

#include "general_view.h"

#include "plugin_interface.h"
#include "fvupdater.h"
#include "fvupdatewindow.h"

namespace Ui
{
  ControlPanel::ControlPanel()
  {
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );

    _contentsWidget = 0;
    _pagesWidget = 0;
    _horizontalLayout = 0;
    _buttonsLayout = 0;
    _mainLayout = 0;
    _checkUpdateButton = 0;

    _bShowOnCloseMessage = true;

    recreateAllWidgets();
    initUpdater();

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
    _contentsWidget->setMaximumWidth(100);
    _contentsWidget->setMinimumWidth(100);
    _contentsWidget->setMinimumHeight(380);
    _contentsWidget->setMaximumHeight(380);
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

    _closeButton = new QPushButton(tr("Minimize"));
    connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));

    createIcons();

    _contentsWidget->setCurrentRow(0);

    _horizontalLayout = new QHBoxLayout;
    _horizontalLayout->addWidget(_contentsWidget);
    _horizontalLayout->addWidget(_pagesWidget);
    
    _version = new QLabel(tr("version ") + VERSION);
    _version->setDisabled(true);

    _checkUpdateResultLabel = new QLabel(tr("Checking..."));
    _checkUpdateResultLabel->setEnabled(false);
    _checkUpdateResultLabel->hide();

    _checkUpdateButton = new QPushButton;
    _checkUpdateButton->setText(tr("Check for updates..."));
    connect(_checkUpdateButton, SIGNAL(clicked()), this, SLOT(onUpdateClicked()));

    _buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    _buttonsLayout->addSpacing(135);
    _buttonsLayout->addWidget(_version, 1, Qt::AlignLeft);
    _buttonsLayout->addWidget(_checkUpdateButton, 1, Qt::AlignLeft);
    _buttonsLayout->addWidget(_checkUpdateResultLabel, 1, Qt::AlignLeft);
    _buttonsLayout->addStretch(5);

    _mainLayout = new QVBoxLayout;
    _mainLayout->addLayout(_horizontalLayout);
    _mainLayout->addStretch(1);
    _mainLayout->addSpacing(12);
    _mainLayout->addLayout(_buttonsLayout);
    setLayout(_mainLayout);

    setWindowTitle(tr("IPP_Webmounter"));

    this->setMinimumWidth(520);
    this->setMinimumHeight(400);

    this->setMaximumWidth(520);
    this->setMaximumHeight(400);

    resize(520, 400);
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
    _checkUpdateButton->setText(tr("Check for updates..."));
    _restoreAction->setText(tr("&Restore"));

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

    _configButton = new QListWidgetItem(_contentsWidget);
    _configButton->setIcon(QIcon(":/resources/config.png"));
    _configButton->setText(tr("Configuration"));
    _configButton->setTextAlignment(Qt::AlignHCenter);
    _configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

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
        QTranslator* tr = new QTranslator;
        printf("ControlPanel::initializeTranslators, tr = %p\n", tr);
        result = tr->load(filename, translationDir());
        if(result)
        {
          QApplication::installTranslator(tr);
          _transList.append(tr);
        }
      }
    }
    printf("ControlPanel::initializeTranslators, after addition\n");
  }

  void ControlPanel::initUpdater()
  {
    createUpdateWindow();

    FvUpdater::sharedUpdater()->SetFeedURL("http://lab3d.ru/ipp-webmounter/ipp_webmounter.xml");

    // signals from WINDOW
    connect(_updateWindow, SIGNAL(installRequested()), FvUpdater::sharedUpdater(), SLOT(InstallUpdate()));
    connect(_updateWindow, SIGNAL(skipInstallRequested()), FvUpdater::sharedUpdater(), SLOT(SkipUpdate()));
    connect(_updateWindow, SIGNAL(remindLaterRequested()), FvUpdater::sharedUpdater(), SLOT(RemindMeLater()));
    connect(_updateWindow, SIGNAL(cancelRequested()), FvUpdater::sharedUpdater(), SLOT(CancelUpdate()));

    // signals from UPDATER
    connect(FvUpdater::sharedUpdater(), SIGNAL(finished()), _updateWindow, SLOT(onFinished()));
    connect(FvUpdater::sharedUpdater(), SIGNAL(failed(QString)), _updateWindow, SLOT(onFailed(QString)));
    connect(FvUpdater::sharedUpdater(), SIGNAL(progress(uint)), _updateWindow, SLOT(onProgress(uint)));
    connect(FvUpdater::sharedUpdater(), SIGNAL(updateAvailable(FvAvailableUpdate*)), this, SLOT(onUpdates(FvAvailableUpdate*)));
    connect(FvUpdater::sharedUpdater(), SIGNAL(noUpdates()), this, SLOT(onNoUpdates()));
    connect(FvUpdater::sharedUpdater(), SIGNAL(closeAppToRunInstaller(QString)), this, SLOT(onCloseApp(QString)));
  }

  void ControlPanel::createUpdateWindow()
  {
    _updateWindow = new Ui::FvUpdateWindow();
  }

  void ControlPanel::onUpdates(FvAvailableUpdate* update)
  {
    _checkUpdateButton->setEnabled(true);
    onHideCheckingResult();
    _updateWindow->onShowWindow(update);
  }

  void ControlPanel::onUpdateClicked()
  {
    _checkUpdateResultLabel->show();
    _checkUpdateButton->setEnabled(false);
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
  }

  void ControlPanel::onNoUpdates()
  {
    _checkUpdateButton->setEnabled(true);
    _checkUpdateResultLabel->setText(tr("No updates"));
    QTimer::singleShot(7000, this, SLOT(onHideCheckingResult()));
  }

  void ControlPanel::onHideCheckingResult()
  {
    _checkUpdateResultLabel->hide();
    _checkUpdateResultLabel->setText(tr("Checking..."));
  }

  void ControlPanel::onCloseApp(QString pathToInstaller)
  {
    if(QFile::exists(pathToInstaller))
    {
      QMessageBox* closeAppRequest = new QMessageBox(
        QMessageBox::Question
        , tr("Close app")
        , tr("WebMounter has to be closed to install updates. Click OK button to close it now.")
        , QMessageBox::Ok | QMessageBox::Cancel);
      int res = closeAppRequest->exec();

      if(res == QMessageBox::Ok)
      {
        QProcess installerProcess;
        installerProcess.startDetached(pathToInstaller);
        Sleep(1000);
        QApplication::instance()->quit();
      }
    }
    else
    {
      QMessageBox* errorDialog = new QMessageBox(
        QMessageBox::Information
        , tr("Error")
        , tr("Installer file is corrupted")
        , QMessageBox::Ok);
      errorDialog->exec();
    }
  }
};
