#ifndef TRAYNOTIFICATIONDEVICE_H
#define TRAYNOTIFICATIONDEVICE_H

#include "notification_device.h"

#include <queue>
#include <QMutex>
#include <QSystemTrayIcon>
#include <QWaitCondition>
#include <QMessageBox>

namespace Ui
{
	using namespace std;

	class TrayNotificationDevice : public NotificationDevice				   
	{
		Q_OBJECT

	public:
		TrayNotificationDevice(QSystemTrayIcon*);
		~TrayNotificationDevice();
		//void showNotification(const Notification&);

		//public slots:	
		virtual void showNotification(const Notification&);
		//virtual void initiateDevice();
signals:
		void showMsgBox(const QString&, const QString&);
		void showTrayMsg(int, const QString&, const QString&);

	protected:
		void run();

	private:
		const unsigned int queueMaxLength;
		queue<Notification> msgQueue;
		QDialog* pMsgBox;
		QSystemTrayIcon* pTrayIcon;
		QMutex _DeviceMutex;
		QWaitCondition queueIsNotEmpty;

	};
}
#endif // TRAYNOTIFICATIONDEVICE_H
