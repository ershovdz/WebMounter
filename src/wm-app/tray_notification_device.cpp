#include "tray_notification_device.h"

#include <QMessageBox>

using namespace Ui;

TrayNotificationDevice::TrayNotificationDevice(QSystemTrayIcon* ptrTrayIcon) : queueMaxLength(5)
{
  pTrayIcon = ptrTrayIcon;
}

TrayNotificationDevice::~TrayNotificationDevice()
{
  delete pTrayIcon;
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
    {
      QMutexLocker locker(&mutex);

      if(!msgQueue.empty())
      {
        _DeviceMutex.lock();
        msg = msgQueue.front();

        showTrayMsg(msg.Type, msg.Title, msg.Description);

        msgQueue.pop();
        _DeviceMutex.unlock();
      }

      queueIsNotEmpty.wait(&mutex, 3000);
    }
  }
}