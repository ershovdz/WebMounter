#include "common_stuff.h"

#include "WebMounter.h"

//#include LVFS_DRIVER_H

#include <QtGui>
#include <QtGui/QApplication>
#include <QMutex>
#include <QCryptographicHash>

#ifdef Q_OS_WIN
bool Alone()
{
	HANDLE hMutex = CreateMutex(0, TRUE, L"WebMounter");

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return false;
	}

	return true;
}
#endif
#ifdef Q_OS_LINUX
bool Alone()
{
	return true; // TODO: Add code for Linux
}
#endif

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(controlpanel);

    QApplication app(argc, argv);

	QTranslator translator;
	translator.load(QString("webmounter_" + QLocale::system().name()));
	QApplication::installTranslator(&translator);

	if(!Alone())
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

	QApplication::removeTranslator(&translator);
	Common::WebMounter MainApp;
	MainApp.startApp();

	return app.exec();
}
