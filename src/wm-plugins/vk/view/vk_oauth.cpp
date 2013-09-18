#include <QtGui>
#include <QtWebKit>
#include "webmounter.h"
#include "reg_exp.h"
#include "vk_oauth.h"

namespace Ui
{
	using namespace Common;

	void WebView::closeEvent(QCloseEvent* /*event*/)
	{
		emit finished(eERROR_CANCEL);
	}


	VkOAuth::VkOAuth() : _view(NULL)
	{
		_oAuthTimer = new QTimer();
		connect(_oAuthTimer, SIGNAL(timeout()), this, SLOT(slotOAuthTimeout()));
	}

	VkOAuth::~VkOAuth()
	{
		delete _oAuthTimer;
	}

	void VkOAuth::initializeWebView()
	{
		_view = new WebView();
		_view->setWindowTitle(tr("Vkontakte | Authentication"));
		QNetworkAccessManager * manager = new QNetworkAccessManager(this);

		connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));
		connect(manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(ignoreSSL( QNetworkReply *, const QList<QSslError> & )));
		connect(_view, SIGNAL(finished(RESULT)), this, SLOT(finished(RESULT)));

		_view->page()->setNetworkAccessManager(manager);
		_view->page()->triggerAction(QWebPage::Forward);

		_view->resize(800, 500);
	}

	void VkOAuth::finished(QNetworkReply *reply) 
	{
		int attr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

		if(attr == 302) // redirect
		{
			_oAuthTimer->stop();

			QString url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
			if(url.contains("access_token="))
			{
				_token = Data::RegExp::getByPattern("access_token=(.*)&expires_in=", url);

				delete _view;
				_view = NULL;

				emit authFinished(eNO_ERROR, "", _token);
			}
			else if(url.contains("error=access_denied"))
			{
				delete _view;
				_view = NULL;

				_oAuthTimer->stop();

				emit authFinished(eERROR_CANCEL, "", "");
			}
		}
	}

	void VkOAuth::ignoreSSL(QNetworkReply* reply, const QList<QSslError>& /*list*/)
	{
		reply->ignoreSslErrors();
	}

	void VkOAuth::authenticate()
	{
		if(!_view)
		{
			initializeWebView();
		}

		GeneralSettings generalSettings; 
		WebMounter::getSettingStorage()->getData(generalSettings);
		if(generalSettings.proxyAddress.length())
		{
			QNetworkProxy proxy;
			proxy.setType(QNetworkProxy::HttpProxy);
			proxy.setHostName(generalSettings.proxyAddress.left(generalSettings.proxyAddress.lastIndexOf(":")));
			QString portStr = generalSettings.proxyAddress.right(generalSettings.proxyAddress.length() - generalSettings.proxyAddress.lastIndexOf(":")-1);
			proxy.setPort(portStr.toInt());
			proxy.setUser(generalSettings.proxyLogin);
			proxy.setPassword(generalSettings.proxyPassword);

			_view->page()->networkAccessManager()->setProxy(proxy);
		}

		_view->load(QUrl("https://oauth.vk.com/authorize?client_id=2950346&scope=4&response_type=token"));
		_view->show();
		if (!_oAuthTimer->isActive())
			_oAuthTimer->start(60*1000);
	}

	void VkOAuth::finished(RESULT error)
	{
		delete _view;
		_view = NULL;

		_oAuthTimer->stop();

		emit authFinished(error, "", "");
	}

	void VkOAuth::slotOAuthTimeout()
	{
		_oAuthTimer->stop();
		delete _view;
		_view = NULL;

		emit authFinished(eERROR_GENERAL, "", "");
	}
}
