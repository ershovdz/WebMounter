#include <qglobal.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <QtGui/QMessageBox>
#include "single_application.h"

SingleInstance::SingleInstance( QObject *parent ) : QTcpServer(parent)
{
	setMaxPendingConnections(1);
	listen(QHostAddress::Any, 2222);
}

void SingleInstance::newConnection(int /*socket*/)
{
//  No need to implement anything!
//  The parameter gives a warning at compile-time but that's nothing to care about...
}

SingleApplication::SingleApplication(int &argc, char **argv) : QApplication(argc, argv)
{
}

bool SingleApplication::isSingle()
{
#ifdef Q_OS_WIN
	HANDLE hMutex = CreateMutex(0, TRUE, L"WebMounter");

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return false;
	}

	return true;
#endif
#ifdef Q_OS_LINUX
	_instance = new SingleInstance(this);
	return _instance->isListening();
#endif
}
