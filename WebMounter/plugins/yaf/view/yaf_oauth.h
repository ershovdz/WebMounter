#include <QtGui>
#include <QNetworkReply>
#include "common_stuff.h"

QT_BEGIN_NAMESPACE
class QWebView;
QT_END_NAMESPACE

using namespace Common;
namespace Ui
{
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
	private slots:
		void slotOAuthTimeout();
	private:
		void initializeWebView();
	private:
		QString _token;
		QWebView *_view;
		QTimer *_oAuthTimer;
	};
}