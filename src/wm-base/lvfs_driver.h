#ifndef ILVFS_DRIVER_H
#define ILVFS_DRIVER_H

#define VLFS_CALLBACK __stdcall

#include "data.h"
#include <QThread>

namespace LocalDriver
{
	using namespace Data;
	class ILVFSDriver : public QThread
	{
		Q_OBJECT
			public slots:
				virtual void mount(Data::GeneralSettings& generalSettings) = 0;
				virtual void unmount() = 0;

signals:
				void mounted();
				void unmounted();
	};
};

#endif