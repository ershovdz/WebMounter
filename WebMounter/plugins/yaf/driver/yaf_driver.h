#ifndef YAF_DRIVER_H
#define YAF_DRIVER_H

#include <QDir>
#include <QWaitCondition>
#include "rvfs_driver.h"
#include "common_stuff.h"
#include "plugins/yaf/connector/yaf_connector.h"

#include "data.h"


namespace RemoteDriver
{
	class YafRVFSDriver :
		public RVFSDriver
	{
		Q_OBJECT
	
	public:
		YafRVFSDriver(const QString& pluginName);
		virtual ~YafRVFSDriver(void);
		void connectHandlerStage2(RESULT error, PluginSettings pluginSettings, const QString& token);
	
	public:
		virtual RESULT downloadFiles() {return RVFSDriver::downloadFiles();};
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
		RESULT getAlbums(QList<VFSElement>& elements);
		RESULT getPhotos(QList<VFSElement>& elements);

		void parseAlbumEntry(QString& xmlEntry, VFSElement& elem);
		void parsePhotoEntry(QString& xmlEntry, VFSElement& elem);

		void run();

		virtual void connectHandler(PluginSettings& pluginSettings);
		virtual void disconnectHandler();
		virtual void syncHandler();
		virtual void stopSyncHandler();

		QString getElementPath(QList<VFSElement>& elements, VFSElement& element);
		int findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem);
		void markNameDuplicates(QList<VFSElement>& elemList);
		void handleNameDuplicates(QList<VFSElement>& elemList);
		QString addPathSuffix(ElementType type, const QString& path, const QString& suffix);
	
	private:
		Connector::YafHTTPConnector* _httpConnector;
		//const QString _pluginName;
		QWaitCondition _forceSync;
		QMutex _syncMutex;
	};
}

#endif //YAF_DRIVER_H