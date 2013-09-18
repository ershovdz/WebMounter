#include <QDebug>

#include "qfacebookreply.h"
#ifdef Q_OS_WIN
#include <qjson/parser>
#endif
#ifdef Q_OS_LINUX
#include <qjson/parser.h>
#endif

namespace Connector
{
	QFacebookReply::QFacebookReply(QNetworkReply *reply, QObject *parent)
		: QObject(parent),
		m_reply(reply),
		m_data(QVariant())
	{
		if (m_reply) {
			//connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
			connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
			connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
			connect(m_reply, SIGNAL(finished()), this, SLOT(onParseData()));
		}
	}

	QVariant QFacebookReply::data()
	{
		return m_data;
	}

	QNetworkReply::NetworkError QFacebookReply::error() const
	{
		if (m_reply)
			return m_reply->error();

		return QNetworkReply::NoError;
	}

	void QFacebookReply::onParseData()
	{
		if (m_reply->error() != QNetworkReply::NoError) {
			qDebug() << "Request fail " << m_reply->errorString();
		} else {
			qDebug() << "Request success.";
			//TODO: Check about put and delete requests
			// before do the parse.
			doParse();
		}

		emit finished();
	}

	void QFacebookReply::doParse()
	{
		bool ok;
		QJson::Parser parser;
		QByteArray replyResult = m_reply->readAll();

		QVariant parsedResult = parser.parse(replyResult, &ok);

		if (!ok) {
			qWarning() << "Error while parsing network reply: " << replyResult;
			return;
		}

		m_data = parsedResult;
	}

	void QFacebookReply::onError(QNetworkReply::NetworkError code)
	{
		qDebug() << "Error: " << code;
	}

	void QFacebookReply::onSslErrors(const QList<QSslError> &errors)
	{
		qDebug() << "Ssl errors:";
		/*foreach(QSslError error, errors) {
		qDebug() << err.errorString();
		}*/

		qDebug() << "Ignoring ssl errors ...";
		m_reply->ignoreSslErrors();
	}
}
