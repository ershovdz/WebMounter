#include "tray_notification_device.h"

#include <QMessageBox>

using namespace Ui;

TrayNotificationDevice::TrayNotificationDevice(QSystemTrayIcon* ptrTrayIcon) : queueMaxLength(5)
{
	pTrayIcon = ptrTrayIcon;
}

TrayNotificationDevice::~TrayNotificationDevice()
{
}

void TrayNotificationDevice::showNotification(const Notification& msg)
{
	QMutexLocker locker(&_DeviceMutex);
	
	if(msgQueue.size() < queueMaxLength)
	{
		msgQueue.push(msg);
	}
	
	if(!isRunning())
	{
		start();
	}

	queueIsNotEmpty.wakeAll();
}

void TrayNotificationDevice::run()
{
	Notification msg;
	QMutex mutex;
	forever
	{
		mutex.lock();

		if(!msgQueue.empty())
		{
			_DeviceMutex.lock();
			msg = msgQueue.front();

			//if(msg.Type == Notification::eINTERACTIVE)
			//{
				//emit showMsgBox(msg.Title
				//	, msg.Description);
				//QMessageBox* ptrMsgBox = new QMessageBox();
				/*pMsgBox-critical(0, msg.Title,
					msg.Description, QMessageBox::Cancel);*/
				//ptrMsgBox->setWindowTitle("dsd");
				//ptrMsgBox->setText("sdsdsdsds");
				
				//QThread::post
				//pMsgBox->show();

			//}
			//else
			//{

				QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon((int)msg.Type);
				pTrayIcon->showMessage(msg.Title
					, msg.Description
					, icon
					, 3 * 1000
					);
			//}

			msgQueue.pop();
			_DeviceMutex.unlock();
		}
		
		queueIsNotEmpty.wait(&mutex, 3000);
		mutex.unlock();
	}
}