#ifndef VIEW_H
#define VIEW_H

#include <QWidget>

#include "data.h"

namespace Ui
{
	class View : public QWidget
	{
		Q_OBJECT

	public slots:
		virtual void updateView(int progress, int state) = 0;
	
signals:
		void startSync();
		void stopSync();
		void connectPlugin(Data::PluginSettings&);
		void disconnectPlugin();
	};
}

#endif