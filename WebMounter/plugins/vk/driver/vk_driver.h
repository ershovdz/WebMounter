#ifndef VK_DRIVER_H
#define VK_DRIVER_H

#include <QDir>
#include <QWaitCondition>
#include "common_stuff.h"
#include "rvfs_driver.h"
#include "plugins/vk/connector/vk_connector.h"
#include "data.h"
#include "notification_device.h"

namespace RemoteDriver
{
	#define DEMO_ALBUMS_LIMIT			2
	#define DEMO_PHOTOS_PER_ALBUM_LIMIT	5

	class VkRVFSDriver : public RVFSDriver
	{
		Q_OBJECT 

	public:
		VkRVFSDriver(const QString& pluginName);
		virtual ~VkRVFSDriver(void);
	
	public:
		virtual RESULT downloadFiles() {return RVFSDriver::downloadFiles();};
		virtual void updateDownloadStatus(RESULT downloadResult, const UINT uDownloaded, const UINT uNotDownloaded);
		virtual RESULT downloadFiles(QList <QString>& urlList, QList <QString>& pathList);
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

		//RESULT downloadFiles();

		//UINT countNotDownloaded();
		void run();

		virtual void connectHandler(PluginSettings& pluginSettings);
		virtual void disconnectHandler();
		virtual void syncHandler();
		virtual void stopSyncHandler();
		virtual RESULT checkKey(const PluginSettings& pluginSettings);

		//int removeFolder(QDir& dir);
		int findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem);
		//void syncCacheWithFileSystem(const QString& path);
		void markNameDuplicates(QList<VFSElement>& elemList);
		void handleNameDuplicates(QList<VFSElement>& elemList);
		QString addPathSuffix(ElementType type, const QString& path, const QString& suffix);
	private:
		Connector::VkHTTPConnector* _httpConnector;
		const QString _pluginName;
		QWaitCondition _forceSync;
		QMutex _syncMutex;
		QMutex _driverMutex;
		bool _isDemo;
	};
}

#endif //VK_DRIVER_H