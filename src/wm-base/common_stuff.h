#ifndef COMMON_STUFF_H
#define COMMON_STUFF_H

#include <qglobal.h>

#define VERSION "1.0.0"

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
		eERROR_GENERAL,
		eERROR_CANCEL,
		eERROR_NOT_SUPPORTED,
		eERROR_FILE_SYSTEM
	};
};

#define LOCK(mutex) QMutexLocker locker(&mutex);

#include <QString>

#define IPP_VERSION

#define WmInterface struct

#endif
