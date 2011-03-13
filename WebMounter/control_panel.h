#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QtGui/QMainWindow>
//#include "ui_webdisk.h"

#include "tray_notification_device.h"

#include <QSystemTrayIcon>
#include <QDialog>
#include <QTranslator>

#include <QtGui>


#include "plugin_view.h"

//#include "plugins/jmm/view/jmm_view.h"
//#include "plugins/yaf/view/yaf_view.h"
//#include "plugins/vk/view/vk_view.h"
//#include "plugins/odnoklassniki/view/odn_view.h"


//QT_BEGIN_NAMESPACE
//class QAction;
//class QRadioButton;
//class QCheckBox;
//class QComboBox;
//class QGroupBox;
//class QLabel;
//class QLineEdit;
//class QMenu;
//class QPushButton;
//class QSpinBox;
//class QTextEdit;
//class QListWidget;
//class QListWidgetItem;
//class QStackedWidget;
//class QProgressBar;
//QT_END_NAMESPACE

namespace Ui
{
	class GeneralView;
	
	class ControlPanel : public QDialog
	{
		Q_OBJECT

	public:
		ControlPanel();

		void setVisible(bool visible);
		operator NotificationDevice*() 
		{
			return _pNotificationDevice;
		}

	protected:
		void closeEvent(QCloseEvent *event);
		public slots:
			void showMsgBox(const QString&, const QString&);
		
		//public slots:
			virtual void showNotification(const Notification& msg);

		public:
			void changeLanguage(const QString& locale);

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

		//QGroupBox *iconGroupBox;
		// QLabel *iconLabel;
		//QComboBox *iconComboBox;
		//QCheckBox *showIconCheckBox;

		GeneralView*  _generalView;
		PluginView*	  _vkView;
		PluginView*	  _yafView;
		PluginView*   _jmmgView;
		PluginView*   _jmmaView;
		PluginView*   _googleView;
		//PluginView*   _odnView;


		QVBoxLayout *_mainLayout;
		QHBoxLayout *_buttonsLayout;
		QHBoxLayout *_horizontalLayout;
		QPushButton *_closeButton;
		QTranslator _translator;
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

		//QAction *_minimizeAction;
		//QAction *_maximizeAction;
		QAction *_restoreAction;
		QAction *_quitAction;

		QSystemTrayIcon *trayIcon;

		QMenu *trayIconMenu;

		QListWidgetItem *_configButton;
		QListWidgetItem *_vkButton;
		QListWidgetItem *_yafButton;
		QListWidgetItem *_jmmgButton;
		QListWidgetItem *_jmmaButton;
		//QListWidgetItem *_odnButton;
		QListWidgetItem *_googleButton;

		bool _bShowOnCloseMessage;


		TrayNotificationDevice* _pNotificationDevice;

		public slots:
			void changePage(QListWidgetItem *current, QListWidgetItem *previous);

	private:
		void createIcons();

		QListWidget *_contentsWidget;
		QStackedWidget *_pagesWidget;

	};
}

#endif // CONTROLPANEL_H
