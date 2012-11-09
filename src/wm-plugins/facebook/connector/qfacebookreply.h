#ifndef QFACEBOOKREPLY_H
#define QFACEBOOKREPLY_H

//#include "qfacebook_global.h"
#include "graphapi.h"

#include <QObject>
#include <QNetworkReply>
#include <QSslError>

namespace Connector
{
	class QFacebookReply : public QObject
	{
		Q_OBJECT
	protected:
		explicit QFacebookReply(QNetworkReply *reply, QObject *parent = 0);

	public:
		QVariant data();
		QNetworkReply::NetworkError error() const;

signals:
		void finished();
		void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

		private slots:
			void onParseData();
			void doParse();
			void onError(QNetworkReply::NetworkError code);
			void onSslErrors(const QList<QSslError> &errors);

	private:
		QNetworkReply *m_reply;
		QVariant m_data;

		friend class GraphAPI;
	};
}

#endif // QFACEBOOKREPLY_H
