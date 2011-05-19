#ifndef VIEW_H
#define VIEW_H

#include <QWidget>

#include "data.h"

#if defined(WEBMOUNTER_UI_LIBRARY)
#  define WEBMOUNTER_UI_EXPORT Q_DECL_EXPORT
#else
#  define WEBMOUNTER_UI_EXPORT Q_DECL_IMPORT
#endif

namespace Ui
{
	class WEBMOUNTER_UI_EXPORT View : public QWidget
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