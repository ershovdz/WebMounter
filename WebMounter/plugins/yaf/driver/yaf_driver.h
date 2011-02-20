#ifndef YAF_DRIVER_H
#define YAF_DRIVER_H

#include <QDir>
#include <QWaitCondition>
#include "rvfs_driver.h"
#include "common_stuff.h"
#include "plugins/yaf/connector/yaf_connector.h"
#include "notification_device.h"

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
	private:
		void updateDownloadStatus(RESULT downloadResult, const UINT uDownloaded, const UINT uNotDownloaded);
		RESULT downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList, UINT& uDownloaded, UINT& uNotDownloaded);
		virtual RESULT downloadFile(const QString& url, const QString& path);
		virtual RESULT uploadFile(const QString& path, const QString& title,  const QString& id, const QString& parentId);
		virtual RESULT modifyFile(const QString&);
		virtual RESULT renameElement(const QString& id, ElementType type, const QString& newTitle);
		virtual RESULT deleteDirectory(const QString& id);
		virtual RESULT deleteFile(const QString& id);
		virtual RESULT moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type);
		virtual RESULT createDirectory(const QString& path, const QString& parentId, const QString& title);
		virtual RESULT getElements();
		virtual RESULT sync();
		virtual bool areFileAttributesValid(const QString& path, unsigned long attributes);
	private:
		RESULT getAlbums(QList<VFSElement>& elements);
		RESULT getPhotos(QList<VFSElement>& elements);

		void parseAlbumEntry(QString& xmlEntry, VFSElement& elem);
		void parsePhotoEntry(QString& xmlEntry, VFSElement& elem);

		void notifyUser(Ui::Notification::_Types type, QString title, QString description) const;
		RESULT downloadFiles();

		UINT countNotDownloaded();

		void run();

		virtual void connectHandler(PluginSettings& pluginSettings);
		virtual void disconnectHandler();
		virtual void syncHandler();
		virtual void stopSyncHandler();

		void syncCacheWithFileSystem(const QString& path);
		int removeFolder(QDir& dir);
		int findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem);
		void markNameDuplicates(QList<VFSElement>& elemList);
		void handleNameDuplicates(QList<VFSElement>& elemList);
		QString addPathSuffix(ElementType type, const QString& path, const QString& suffix);
	private:
		Connector::YafHTTPConnector* _httpConnector;
		const QString _pluginName;
		QWaitCondition _forceSync;
		QMutex _syncMutex;
		QMutex _driverMutex;
	};
}

#endif //YAF_DRIVER_H