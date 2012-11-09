#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include <QtNetwork/QTcpServer>
#include <QtGui/QApplication>

class SingleInstance : public QTcpServer
{
	Q_OBJECT
public:
	SingleInstance( QObject *parent=0 );
	void newConnection( int );
};

class SingleApplication : public QApplication
{
	Q_OBJECT
public:
	SingleApplication(int &argc, char **argv);
	bool isSingle();
private:
#ifdef Q_OS_LINUX
	SingleInstance* _instance;
#endif
};


#endif // SINGLE_APPLICATION_H
