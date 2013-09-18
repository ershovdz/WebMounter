#pragma once

#include <QObject>
#include <QThread>
#include <QMutex>
#include "common_stuff.h"
#include "vfs_cache.h"
//#include <curl/curl.h>
#include "graphapi.h"
#include "boost/shared_ptr.hpp"

namespace Connector
{
	using namespace Data;

	class FacebookHTTPConnector : public QObject
	{
		Q_OBJECT
	public:	
		FacebookHTTPConnector();
		~FacebookHTTPConnector();

		void setSettings(
			const QString& login 
			, const QString& password 
			, const QString& proxy = ""//"proxy.te.mera.ru:8080"
			, const QString& proxyLoginPwd = ""//"login:password"
			, bool isOAuth = true
			, const QString& token = "");
		static size_t fwrite_b(void *ptr, size_t size, size_t count, void *path);
		static size_t writeStr(void *ptr, size_t size, size_t count, void *response);

		RESULT downloadFiles(QList <QString>& urlList, QList <QString>& pathList);
		RESULT getAlbums(QVariant& response);
		RESULT getPhotos(const QString& albumId, QVariant& response);
		RESULT uploadFile(const QString& path, const QString& title, const QString& parentId, QVariant& response);
		//RESULT downloadFile(const QString& url, const QString& path);
		RESULT deleteObject(const QString& id);
		RESULT createDirectory(const QString& title, QVariant& response);
		RESULT moveFile(const QString& id, const QString& oldParentId, const QString& newParentId);
		RESULT renameAlbum(const QString& id, const QString& newTitle);
		void setToken(const QString& token);
	private:
		QString _login;
		QString _password;
		QString _proxy;
		QString _proxy_login_pwd;
		bool _isOAuth;
		QString _token;
		QMutex _connectorMutex;
	};
}