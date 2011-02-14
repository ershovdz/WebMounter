#pragma once

#include <QThread>
#include <QMutex>
#include "common_stuff.h"
#include "vfs_cache.h"
#include <curl/curl.h>

namespace Connector
{
	using namespace Data;

	class VkHTTPConnector
	{
	public:	
		VkHTTPConnector();
		~VkHTTPConnector();

		void setSettings(
			const QString& login 
			, const QString& password 
			, const QString& proxy = ""//"proxy.te.mera.ru:8080"
			, const QString& proxyLoginPwd = ""//"login:password"
			);
		RESULT downloadFile(const QList <QString>& urlList, const QList <QString>& pathList);
		RESULT auth();
		RESULT getAlbums(QString& response);
		RESULT getPhotos(int offset, QString& response);
		RESULT uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response);
		RESULT downloadFile(const QString& url, const QString& path);
		RESULT deleteFile(const QString& id);
		RESULT deleteAlbum(const QString& id);
		RESULT createDirectory(const QString& title, QString& response);
		RESULT moveFile(const QString& id, const QString& oldParentId, const QString& newParentId);
		RESULT renameAlbum(const QString& id, const QString& newTitle);
	private:
		RESULT authStep1();
		RESULT authStep2();
		RESULT authStep3();
		RESULT authStep4();
		RESULT authStep5();
		RESULT authStep6();
		RESULT authStep7();

		QString genQuery(const QString &method, const QStringList &params);
		RESULT execQuery(const QString &url, const QString &header, const QString &postFields, QString* response);

		QString getUploadServer(const QString& albumId);
		RESULT uploadPhoto(const QString& uploadServer, const QString& path);
		RESULT savePhoto(const QString& parentId, QString& response);
	private:
		static size_t writeStr(void *ptr, size_t size, size_t count, void *response);
		static size_t fwrite_b(void *ptr, size_t size, size_t count, void *path); 
	private:
		struct sVkSession
		{
			QString _expire;     
			QString _mid;   		
			QString _secret;   
			QString _sid;
		};

		struct sUploadData
		{
			QString _server;     
			QString _photos_list;   		
			QString _aid;   
			QString _hash;
		};
	private:
		CURL* _curl;
		QString _login;
		QString _password;
		QString _proxy;
		QString _proxy_login_pwd;
		QMutex _connectorMutex;

		QString _id;
		static QString _appId;
		QString _hash;
		QString _settings_hash;
		QString _s;
		QString _p;
		sVkSession _session;
		sUploadData _upload;
		long _responseCode;
	};
}