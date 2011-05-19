#ifndef FILE_PROXY_H
#define FILE_PROXY_H

#include "common_stuff.h"
#include "rvfs_driver.h"
#include "notification_device.h"

#include <map>
#include <set>
#include <QString>
#include <QMutex>
#include <QFuture>
#include <QtCore>

namespace Common
{
	using RemoteDriver::RVFSDriver;
	
	class WEBMOUNTER_EXPORT FileProxy
	{
		FileProxy(void);
		//FileProxy(RVFSDriver* pDriver);
	public:
		static FileProxy* CreateFileProxy();
		~FileProxy(void);

		RESULT CreateDirectoryW(QString path);
		RESULT CreateFileW(QString path);

		RESULT MoveElement(QString from, QString to, bool bFirstRequest = true);

		void OpenDirectory(QString path);
		RESULT ReadFile(QString path);
		RESULT RemoveFile(QString path);
		RESULT RemoveDir(QString& path);

		//void Sync(QString path, bool bFullSync = false);
		RESULT UnCheckFile(QString path);
                unsigned int CheckFile(QString path);
		void WriteFile(QString path);
		//void RegisterRVFSDriver(RVFSDriver*);
		bool CheckFileAttributes(const QString& path, unsigned long attributes);

                static unsigned int GetNotUploadedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			return uNotUploaded;
		}
                static unsigned int GetUploadedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			return uUploaded;
		}
		static void IncreaseUploadedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			uUploaded++;
		}
		static void IncreaseNotUploadedCounter()
		{
			//QMutexLocker locker(&_FileProxyMutex);
			if(uNotUploaded == 0)
			{
				notifyUser(Ui::Notification::eINFO
					, QObject::tr("Info")
					, QObject::tr("Upload has been started !"));
			}
			uNotUploaded++;
		}

                static unsigned int getNotDeletedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			return _uNotDeleted;
		}
                static unsigned int getDeletedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			return _uDeleted;
		}
		static void increaseDeletedCounter()
		{
			QMutexLocker locker(&_FileProxyMutex);
			_uDeleted++;
		}
		static void increaseNotDeletedCounter()
		{
			if(_uNotDeleted == 0)
			{
				notifyUser(Ui::Notification::eINFO
					, QObject::tr("Info")
					, QObject::tr("Deletion has been started !"));
			}
			_uNotDeleted++;
		}

 public:
	 void fileUploaded(QString filePath, RESULT result);
	 void fileDeleted(const QString& filePath, RESULT result);

	private:
		static void notifyUser(Ui::Notification::_Types type, QString title, QString description);
		RVFSDriver* extractPlugin(const QString& path) const;
	
	private:
		static QMutex _FileProxyMutex;
		static set<QString> _currentFiles;

                static unsigned int uUploaded;
                static unsigned int uNotUploaded;

		static QList<QString> _uploadQueue;

                static unsigned int _uDeleted;
                static unsigned int _uNotDeleted;
		static QList<QString> _deleteQueue;
	public:
		static FileProxy* _pFileProxyInstance;
	};
}

#endif
