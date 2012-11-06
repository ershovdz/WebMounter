#ifndef IGUI_H
#define IGUI_H

#include "common_stuff.h"
#include "notification_device.h"

namespace Ui
{
  WmInterface IGui
  {
    virtual ~IGui() {}

    virtual void mounted() = 0;
    virtual void unmounted() = 0;
    virtual NotificationDevice* getNotificationDevice() = 0;
  };
}

#endif // IGUI_H