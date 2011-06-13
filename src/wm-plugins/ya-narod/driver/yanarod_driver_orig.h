#ifndef YANDEX_NAROD_DRIVER_H
#define YANDEX_NAROD_DRIVER_H

//#include <QDir>
//#include <QWaitCondition>
#include "rvfs_driver.h"
//#include "common_stuff.h"
//#include "../connector/yanarod_connector.h"

#include "data.h"


//namespace RemoteDriver
//{
	class YaNarodRVFSDriver //: public RemoteDriver::RVFSDriver
	{
		//Q_OBJECT
	
	public:
		YaNarodRVFSDriver(const QString& pluginName);
		virtual ~YaNarodRVFSDriver(void);
	public:
		//virtual RESULT downloadFiles() { return RVFSDriver::downloadFiles(); }
		virtual RESULT downloadFiles(QList <QString>& urlList, QList <QString>& pathList);
		virtual RESULT uploadFile(const QString& path, const QString& title,  const QString& id, const QString& parentId);
		virtual RESULT modifyFile(const QString&);
		virtual RESULT renameElement(const QString& path, const QString& newTitle);
		virtual RESULT deleteDirectory(const QString& id);
		virtual RESULT deleteFile(const QString& id);
		virtual RESULT moveElement(const QString& path, const QString& newParentId);
		virtual RESULT createDirectory(const QString& path, const QString& parentId, const QString& title);
		virtual RESULT getElements();
		virtual RESULT sync();
		virtual bool areFileAttributesValid(const QString& path, unsigned long attributes);
	
	private:
		RESULT getFiles(QList<RemoteDriver::VFSElement>& elements);

		void parseEntry(QString& xmlEntry, RemoteDriver::VFSElement& elem);

		void run();

		virtual void connectHandler(RemoteDriver::PluginSettings& pluginSettings);
		virtual void disconnectHandler();
		virtual void syncHandler();
		virtual void stopSyncHandler();
	private:
		//Connector::YaNarodHTTPConnector* _httpConnector;
		//const QString _pluginName;
		//QWaitCondition _forceSync;
		//QMutex _syncMutex;
	};
//}

#endif //YANDEX_NAROD_DRIVER_H
