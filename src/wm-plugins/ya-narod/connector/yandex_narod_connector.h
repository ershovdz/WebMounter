#ifndef YANDEX_NAROD_CONNECTION_H
#define YANDEX_NAROD_CONNECTION_H

#include <QThread>
#include <QMutex>
#include "vfs_cache.h"
#include <curl/curl.h>

namespace Connector
{
	using namespace Data;

	class YandexNarodHTTPConnector
	{
	public:
		YandexNarodHTTPConnector();
		~YandexNarodHTTPConnector();

		void setSettings(const QString& login
			, const QString& password
			, const QString& proxy
			, const QString& proxyLoginPwd);
		RESULT auth();
		RESULT getFiles(QString& response);
		RESULT downloadFile(const QString& url, const QString& path);
		RESULT downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList);
		RESULT uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response);
		RESULT deleteFile(const QString& id, const QString& token);
	private:
		static size_t writeStr(void *ptr, size_t size, size_t count, void *response);
		static size_t fwrite_b(void *ptr, size_t size, size_t count, void *path); 
		//static size_t readStr(void *ptr, size_t size, size_t nmemb, void *stream);
		int execQuery(const QString &url, const QString &header, const QString &postFields, QString* response);
		RESULT getDownloadLink(const QString& url, QString& link);
		RESULT getUploadLink(QString& progressLink, QString& uploadLink);
		RESULT doUpload(const QString& path, const QString& title, const QString& uploadLink, QString& response);
		RESULT getProgress(const QString& progressLink, QString& response);
		RESULT getUploadedFileLink(QString& link);
	private:
		struct sPutData
		{
			const char *data;
			size_t len;
		};
	private:
//		static QMutex _connectorMutex;
		QList<QString> cookies;
		QString _login;
		QString _password;
		QString _proxy;
		QString _proxy_login_pwd;

		QMutex _connectorMutex;
	};
}

#endif // YANDEX_NAROD_CONNECTION_H
