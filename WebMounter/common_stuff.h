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

namespace Common
{
	enum RESULT
	{
		eNO_ERROR,
		eERROR
	};
};

//using Common::RESULT;

#include <QString>

#endif