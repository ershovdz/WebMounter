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

namespace Ui
{
	class GeneralView;
	
	class ControlPanel : public QDialog
	{
		Q_OBJECT

	public:
		ControlPanel();

		void setVisible(bool visible);
		NotificationDevice* operator()()
		{
			return _pNotificationDevice;
		}

	protected:
		void closeEvent(QCloseEvent *event);
		public slots:
			void showMsgBox(const QString&, const QString&);
			void showTrayMsg(int, const QString&, const QString&);
			void changeLanguage(const QString&);

		//public slots:
			virtual void showNotification(const Notification& msg);

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
	private:

		//QGroupBox *iconGroupBox;
		// QLabel *iconLabel;
		//QComboBox *iconComboBox;
		//QCheckBox *showIconCheckBox;

		GeneralView*  _generalView;
		QList<PluginView*> _pluginViewList;

		QVBoxLayout *_mainLayout;
		QHBoxLayout *_buttonsLayout;
		QHBoxLayout *_horizontalLayout;
		QPushButton *_closeButton;
		QTranslator _translator;
		QList<QTranslator*> _pluginTransList;
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

		QList<QListWidgetItem*> _pluginButtonsList;
		QListWidgetItem *_configButton;

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
