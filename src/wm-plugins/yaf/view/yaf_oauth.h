#ifndef __YAF_OAUTH_H__
#define __YAF_OAUTH_H__

#include <QtGui>
#include <QNetworkReply>
#include <QWebView>
#include "common_stuff.h"

//QT_BEGIN_NAMESPACE
//class QWebView;
//QT_END_NAMESPACE

using namespace Common;
namespace Ui
{
	class WebView : public QWebView
	{
		Q_OBJECT
	protected:
		void closeEvent(QCloseEvent *event);
		Q_SIGNALS:
			void finished(RESULT error);
	};

	class YafOAuth : public QObject
	{
		Q_OBJECT
	public:
		YafOAuth();
		~YafOAuth();
		void authenticate();
	Q_SIGNALS:
		void authFinished(RESULT error, const QString& login, const QString& token);
	public slots:
		void ignoreSSL( QNetworkReply *, const QList<QSslError> & );
		void finished(QNetworkReply *reply);
		void finished(RESULT error);
		void handleUnsupportedContent(QNetworkReply *reply);
	private slots:
		void slotOAuthTimeout();
	private:
		void initializeWebView();
	private:
		QString _token;
		WebView *_view;
		QTimer *_oAuthTimer;
	};
}


#endif // __YAF_OAUTH_H__