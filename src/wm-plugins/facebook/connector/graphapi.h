#ifndef __GRAPHAPI_H__
#define __GRAPHAPI_H__

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "qfacebookreply.h"

namespace Connector
{
	class QFacebookReply;

	class GraphAPI : public QObject
	{
	public:
		enum HttpMethod 
		{
			eGET = 0,
			ePOST,
			ePUT,
			eDELETE
		};

		GraphAPI(QObject *parent = 0);
		void setSettings(const QString& proxy = ""//"proxy.te.mera.ru:8080"
			, const QString& proxyLoginPwd = ""//"login:password"
			, const QString& token = "");

		QFacebookReply *getObject(QString id);
		QFacebookReply *getConnections(QString id, QString connectionName);
		QFacebookReply *putObject(QString id, QString connection,
			QByteArray data = QByteArray());
		QFacebookReply *deleteObject(QString id);
		QFacebookReply *request(HttpMethod method, QString path,
			QByteArray postArgs = QByteArray());

		QNetworkReply *get(QUrl url);
		QNetworkReply *put(QUrl url, QByteArray data);
		QNetworkReply *del(QUrl url);
		QNetworkReply *request(HttpMethod method, QUrl url,
			QByteArray postArgs = QByteArray());

	private:
		QString m_accessToken;
		QNetworkAccessManager *m_networkManager;
	};
}

#endif // __GRAPHAPI_H__
