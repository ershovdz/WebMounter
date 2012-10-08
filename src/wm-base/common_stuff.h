#ifndef COMMON_STUFF_H
#define COMMON_STUFF_H

#include <qglobal.h>

#define VERSION "0.3.2"

#ifdef Q_OS_WIN
#include <windows.h>
#define LVFS_DRIVER_H "win_lvfs_driver.h"
#endif
#ifdef Q_OS_LINUX
#define LVFS_DRIVER_H "linux_lvfs_driver.h"
#endif

#define WM_FULL_VERSION

#if defined(WEBMOUNTER_LIBRARY)
#  define WEBMOUNTER_EXPORT Q_DECL_EXPORT
#else
#  define WEBMOUNTER_EXPORT Q_DECL_IMPORT
#endif

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
