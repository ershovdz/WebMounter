//#include <common_stuff.h>

#include "webmounter.h"
#include "control_panel.h"
#include "single_application.h"
#include "common_stuff.h"

#include <QtGui>
#include <QtGui/QApplication>
#include <QMutex>
#include <QCryptographicHash>

int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);

	QTranslator translator;
	translator.load(QString("webmounter_" + QLocale::system().name()));
	QApplication::installTranslator(&translator);

	if(!app.isSingle())
	{
		QMessageBox::information(0, QObject::tr("IPP-Webmounter"),
			QObject::tr("IPP-Webmounter is already running."));

		return 1;
	}

	if (!QSystemTrayIcon::isSystemTrayAvailable()) 
	{
		QMessageBox::critical(0, QObject::tr("Systray"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}

	QApplication::setQuitOnLastWindowClosed(false);
	app.setApplicationName(QObject::tr("IPP-Webmounter"));
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("iPhotoPrint");
	app.setOrganizationDomain("iphotoprint.ru");

	QApplication::removeTranslator(&translator);

	Common::WebMounter mainApp;

	Ui::ControlPanel* panel = new Ui::ControlPanel;
	mainApp.startApp( panel );

	QObject::connect(panel, SIGNAL(mount()), &mainApp, SLOT(mount()));
	QObject::connect(panel, SIGNAL(unmount()), &mainApp, SLOT(unmount())); 

	panel->show();

	int res = app.exec();

	delete panel;

	return res;
}
