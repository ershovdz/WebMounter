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
 		QMessageBox::information(0, QObject::tr("WebMounter"),
 									QObject::tr("WebMounter is already running."));
 
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
 	app.setApplicationName(QObject::tr("WebMounter"));
 	app.setApplicationVersion(VERSION);
  app.setOrganizationName("WebMounter");
  app.setOrganizationDomain("webmounter.ru");
 
 	QApplication::removeTranslator(&translator);
 
 	Common::WebMounter MainApp;
 
 	Ui::ControlPanel* panel = new Ui::ControlPanel;
 	MainApp.startApp((*panel)());
 	panel->show();
 
 	int res = app.exec();
 
 	delete panel;
 
 	return res;
}
