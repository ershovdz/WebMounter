#ifndef COMMON_STUFF_H
#define COMMON_STUFF_H

#include <qglobal.h>

#ifdef Q_OS_WIN
#include <windows.h>
#define LVFS_DRIVER_H "win_lvfs_driver.h"
#endif
#ifdef Q_OS_LINUX
#define LVFS_DRIVER_H "linux_lvfs_driver.h"
#endif

//#define WM_VERSION_FULL

namespace Common
{
	enum RESULT
	{
		eNO_ERROR,
		eERROR
	};
};

#define LOCK(mutex) QMutexLocker locker(&mutex);

#include <QString>

#endif