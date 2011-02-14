#ifndef RVFS_DRIVER_H
#define RVFS_DRIVER_H

#include <map>

#include "common_stuff.h"

#include "data.h"

#include "vfs_cache.h"
#include "view.h"

#include <QThread>

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

	class RVFSDriver : public QThread
	{
		Q_OBJECT

	public:
		virtual RESULT downloadFile(const QString& url, const QString& path) = 0;

		virtual RESULT uploadFile(const QString& path, const QString& title,  const QString& id, const QString& parentId) = 0;

		virtual RESULT modifyFile(const QString&) = 0;

		virtual RESULT renameElement( const QString& id, ElementType type, const QString& newTitle) = 0;

		virtual RESULT deleteDirectory( const QString& id) = 0;

		virtual RESULT deleteFile( const QString& id) = 0;

		virtual RESULT moveElement( const QString& id,  const QString& oldParentId, const QString& newParentId, ElementType type) = 0;

		virtual RESULT createDirectory(const QString& path,  const QString& parentid, const QString& title) = 0;

		virtual RESULT createFile(const QString& path, const QString& title,  const QString& id, const QString& parentId);

		virtual RESULT getElements() = 0;

		virtual RESULT sync() = 0;

		virtual bool areFileAttributesValid(const QString& path, unsigned long attributes);

		//virtual void registerView(Ui::View*) = 0;
		virtual RESULT checkKey(const PluginSettings& pluginSettings);

	protected:
		virtual void updateState(int progress, DriverState newState);

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

	};
}

#endif