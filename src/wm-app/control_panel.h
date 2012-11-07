#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include "tray_notification_device.h"
#include "plugin_view.h"
#include "IGui.h"
#include <QDialog>

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QtGui>

class FvAvailableUpdate;

namespace Ui
{
  class GeneralView;
  class FvUpdateWindow;

  class ControlPanel : public QDialog
                     , public IGui
  {
    Q_OBJECT

  public:
    ControlPanel();

    void setVisible(bool visible);
  
  // IGui implementation
  public:  
    virtual NotificationDevice* getNotificationDevice()
    {
      return _pNotificationDevice;
    }

    virtual void mounted();
    virtual void unmounted();

  protected:
    void changeEvent(QEvent* e);
    void closeEvent(QCloseEvent *event);
    public slots:
      void showMsgBox(const QString&, const QString&);
      void showTrayMsg(int, const QString&, const QString&);
      void changeLanguage(const QString&);

signals:
      void mount();
      void unmount();

      //public slots:
      //virtual void showNotification(const Notification& msg);

      private slots:
        void setIcon(int index);
        void iconActivated(QSystemTrayIcon::ActivationReason reason);
        void messageClicked();

  private:
    void recreateAllWidgets();

    void createIconGroupBox();
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();

    static QString translationDir();
    void initializeTranslators(const QString& locale);
    void createIcons();
    void initUpdater();
    void createUpdateWindow();

  private:
    GeneralView*  _generalView;
    QList<PluginView*> _pluginViewList;
    QVBoxLayout *_mainLayout;
    QBoxLayout *_buttonsLayout;
    QHBoxLayout *_horizontalLayout;
    QList<QTranslator*> _transList;
    QGroupBox *messageGroupBox;
    QLabel *typeLabel;
    QLabel *durationLabel;
    QLabel *durationWarningLabel;
    QLabel *titleLabel;
    QLabel *bodyLabel;
    QComboBox *typeComboBox;
    QSpinBox *durationSpinBox;
    QLineEdit *titleEdit;
    QTextEdit *bodyEdit;
    QPushButton *showMessageButton;
    QPushButton *showMessageButton2;
    QListWidget *_contentsWidget;
    QStackedWidget *_pagesWidget;
    QAction *_restoreAction;
    QAction *_quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QPushButton *_closeButton;

    QList<QListWidgetItem*> _pluginButtonsList;
    QListWidgetItem *_configButton;
    bool _bShowOnCloseMessage;
    TrayNotificationDevice* _pNotificationDevice;
    QLabel* _version;
    QPushButton* _checkUpdateButton;
    FvUpdateWindow* _updateWindow;
    QLabel* _checkUpdateResultLabel;

    public slots:
      void changePage(QListWidgetItem *current, QListWidgetItem *previous);
      
      private slots:
        void onUpdateClicked();
        void onUpdates(FvAvailableUpdate* update);
        void onNoUpdates();
        void onHideCheckingResult();
        void onCloseApp(QString);
  };
}

#endif // CONTROLPANEL_H
