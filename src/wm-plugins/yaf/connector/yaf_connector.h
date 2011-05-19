#ifndef YAF_CONNECTION_H
#define YAF_CONNECTION_H

#include <QThread>
#include <QMutex>
//#include "common_stuff.h"
#include "vfs_cache.h"
#include <curl/curl.h>

namespace Connector
{
	using namespace Data;

	class YafHTTPConnector
	{
	public:
		YafHTTPConnector();
		~YafHTTPConnector();

		void setSettings(const QString& login
			, const QString& password
			, const QString& proxy
			, const QString& proxyLoginPwd
			, bool isOAuth);
		RESULT auth();
		RESULT getTreeElements(const QString& path, QString& response);
		RESULT downloadFile(const QString& url, const QString& path);
		RESULT downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList);
		RESULT uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response);
		RESULT deleteFile(const QString& path, QString& response);
		RESULT createDirectory(const QString& title, const QString& parentId, QString& response);
		RESULT moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type, QString& response);
		RESULT renameElement(const QString& id, ElementType type, const QString& newTitle, QString& response);
		void setToken(const QString& token);
	private:
		static size_t writeStr(void *ptr, size_t size, size_t count, void *response);
		static size_t fwrite_b(void *ptr, size_t size, size_t count, void *path); 
		static size_t readStr(void *ptr, size_t size, size_t nmemb, void *stream);
		RESULT getCredentials();
		RESULT postCredentials();
		std::string encrypt(const char* public_key, std::string text);
		int execQuery(const QString &url, const QString &header, const QString &postFields, QString* response);
	private:
		struct sPutData
		{
			const char *data;
			size_t len;
		};
	private:
//		static QMutex _connectorMutex;
		CURL* _curl;
		QString _login;
		QString _password;
		QString _proxy;
		QString _proxy_login_pwd;
		bool _isOAuth;

		QString _token;
		QString _requestId;
		QString _key;
		QMutex _connectorMutex;
	};
}

#endif // YAF_CONNECTION_H