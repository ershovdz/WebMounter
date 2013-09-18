#ifndef RVFS_DRIVER_H
#define RVFS_DRIVER_H

#include <map>
#include <QWaitCondition>

#include "common_stuff.h"

#include "data.h"

#include "vfs_cache.h"
//#include "view.h"
#include "notification_device.h"

#include <QThread>
#include <QDir>

using namespace std;
using Common::RESULT;

#define MAX_DOWNLOAD_RETRY 5
#define MAX_UPLOAD_RETRY 5
#define DOWNLOAD_CHUNK_SIZE 30

namespace RemoteDriver
{
	using namespace Data;

	enum DriverState
	{
		eNotConnected
		, eAuthInProgress
		, eAuthorized
		, eSync
		, eConnected
		, eSyncStopping
	};

	class WEBMOUNTER_EXPORT RVFSDriver : public QThread
	{
		Q_OBJECT

	public:
		virtual RESULT downloadFiles();

		virtual RESULT downloadFiles(QList <QString>& urlList, QList <QString>& pathList) = 0;

		virtual RESULT uploadFile(const QString& path, const QString& title,  const QString& id, const QString& parentId) = 0;

		virtual RESULT modifyFile(const QString&) = 0;

		virtual RESULT renameElement( const QString& path, const QString& newTitle) = 0;

		virtual RESULT deleteDirectory( const QString& path) = 0;

		virtual RESULT deleteFile( const QString& path) = 0;

		virtual RESULT moveElement( const QString& path, const QString& newParentId) = 0;

		virtual RESULT createDirectory(const QString& path,  const QString& parentid, const QString& title) = 0;

		virtual RESULT createFile(const QString& path, const QString& title,  const QString& id, const QString& parentId);

		virtual RESULT getElements(QList<VFSElement>& elements) = 0;

		virtual RESULT sync();

		virtual bool areFileAttributesValid(const QString& path, unsigned long attributes);

	protected:
		virtual void notifyUser(Ui::Notification::_Types type, QString title, QString description) const;
		virtual void updateState(int progress, DriverState newState);
		virtual void syncCacheWithFileSystem(const QString& path);
		virtual void updateDownloadStatus(RESULT downloadResult, const unsigned int uDownloaded, const unsigned int uNotDownloaded);
		unsigned int countNotDownloaded();
		unsigned int countObjects(VFSElement::VFSElementType type);
		void updateSyncStatus(double currentStatus);
		void updateChildrenPath(const VFSElement& elem);
		void mergeToCache(QList<VFSElement>& elements);
		
		void run();
Q_SIGNALS:

		void updateView(int, int);
		void fileUploaded(/*QString, RESULT*/);

		public Q_SLOTS:

			virtual void startPlugin(Data::PluginSettings&);

			virtual void stopPlugin();

			virtual void startSync();

			virtual void stopSync();

	public:

		virtual void connectHandler(PluginSettings& pluginSettings);

		virtual void disconnectHandler();

		virtual void syncHandler();

		virtual void stopSyncHandler();

		virtual DriverState getState() {return _state;};

		virtual void setState(DriverState state) {_state = state;};

	protected:
		DriverState _state;
		QString _pluginName;
		QMutex _driverMutex;
		QWaitCondition _forceSync;
		QMutex _syncMutex;
		struct SyncProgressData
		{
			double _currProgress;
			int _maxValue;
		} _progressData;
	};
}

#endif
