#ifndef PLUGIN_VIEW_H
#define PLUGIN_VIEW_H

#include <QWidget>
#include <QtGui>

#include "data.h"

#include "rvfs_driver.h"

#include "vfs_cache.h"
#include "view.h"

namespace Ui
{
	class PluginView : public View
	{
		Q_OBJECT
	public:
		PluginView(const Data::PluginSettings* settings, const QString& title);
		virtual void changeLang();
	protected:
		void changeEvent ( QEvent * event );
		int getSyncPeriod();
		virtual bool isKeyValueValid(const Data::PluginSettings& settings);

	private slots:
		void autoSyncClicked(int state);
		void oauthClicked(int state);
		void startSyncClicked(bool);
		void stopSyncClicked(bool);
		void startPluginClicked(bool);
		void stopPluginClicked(bool);
	
	public slots:
		virtual void updateView(int progress, int state);
	
	protected:
		QCheckBox* _oauthCheckBox;
		QLabel* _dummyLabel;
		QLabel *_urlLabel;
		QLineEdit *_urlEdit;
		QLabel *_nameLabel;
		QLineEdit *_nameEdit;
		QLabel *_passwordLabel;
		QLineEdit *_passwordEdit;
#ifdef WM_FULL_VERSION
		QLabel *_keyLabel;
		QLineEdit *_keyEdit;
		QLabel* _keyUrl;
#endif

		QPushButton *_startPluginButton;
		QPushButton *_stopPluginButton;
		QPushButton *_startSyncButton;
		QPushButton *_stopSyncButton;
		
		QRadioButton *_fullSyncRadioButton;
		QRadioButton *_partSyncRadioButton;
		QCheckBox* _autoSyncCheckBox; 
		QLabel *_syncPeriodLabel; 
		QComboBox *_syncPeriodBox;
		QGroupBox *_authGroup;
		QGridLayout *_authLayout;
		QGroupBox *_syncGroup;
		QGridLayout *_syncLayout;
		QGroupBox *_buttonGroup; 
		QGridLayout *_buttonLayout;
		QGroupBox *_progressGroup;
		QGridLayout *_progressLayout;
		QLabel *_statusLabel;
		QLabel *_statusValue;
		QLabel *_progressBarLabel; 
		QProgressBar *_progressBar;
		QVBoxLayout *_mainLayout;

		RemoteDriver::RVFSDriver* _driver;
		QString _pluginName;
		RemoteDriver::DriverState _driverState;
		
	};
}
#endif