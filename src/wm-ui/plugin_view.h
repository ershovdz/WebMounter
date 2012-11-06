#ifndef PLUGIN_VIEW_H
#define PLUGIN_VIEW_H

#include <QWidget>
#include <QtGui>

#include "data.h"

#include "rvfs_driver.h"

#include "vfs_cache.h"
#include "view.h"

#if defined(WEBMOUNTER_UI_LIBRARY)
#  define WEBMOUNTER_UI_EXPORT Q_DECL_EXPORT
#else
#  define WEBMOUNTER_UI_EXPORT Q_DECL_IMPORT
#endif

namespace Ui
{
	using namespace Common;

	class WEBMOUNTER_UI_EXPORT PluginView : public View
	{
		Q_OBJECT

	public:
		PluginView(const Data::PluginSettings* settings, const QString& title);
		virtual void changeLang();

	protected:
		void changeEvent ( QEvent * event );
		//int getSyncPeriod();
		
	private slots:
		void oauthClicked(int state);
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

		QPushButton *_startPluginButton;
		QPushButton *_stopPluginButton;
		
		QGroupBox *_authGroup;
		QGridLayout *_authLayout;
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