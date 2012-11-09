#include <curl/curl.h>
#include <qregexp.h>
#include <QStringList>
#include <QCryptographicHash>
#include <QFile>
#include <QEventLoop>
#include "facebook_connector.h"
#include "reg_exp.h"
#include <QJson/Parser>

namespace Connector
{
	using namespace Common;

	// FacebookHTTPConnector
	FacebookHTTPConnector::FacebookHTTPConnector()
	{
	}

	FacebookHTTPConnector::~FacebookHTTPConnector()
	{
	}

	void FacebookHTTPConnector::setSettings(const QString& login
		, const QString& password
		, const QString& proxy
		, const QString& proxyLoginPwd
		, bool isOAuth
		, const QString& token)
	{
		_login = login;
		_password = password;
		_proxy = proxy;
		_proxy_login_pwd = proxyLoginPwd;
		_isOAuth = isOAuth;
		_token = token;
	}

	size_t FacebookHTTPConnector::fwrite_b(void *ptr, size_t size, size_t count, void *path) 
	{
		if(path)
		{
			QFile file(*(QString*)path);
			bool res = file.open(QIODevice::WriteOnly | QIODevice::Append);
			size_t result = file.write((char*)ptr, size*count);
			return result;
		}
		return -1;
	}

	size_t FacebookHTTPConnector::writeStr(void *ptr, size_t size, size_t count, void *response)
	{
		if(response)
		{
			((QString*)response)->append(QString::fromUtf8((char*)ptr, count));
			return size*count;
		}
		return -1;
	}

	RESULT FacebookHTTPConnector::getAlbums(QVariant& response, int& errorCode)
	{
		RESULT res = eERROR_GENERAL;
		GraphAPI* graphAPI = new GraphAPI(NULL);
		graphAPI->setSettings(_proxy, _proxy_login_pwd, _token);
		QFacebookReply* reply = graphAPI->getObject("me/albums");

		if (reply)
		{
			QEventLoop loop;
			connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();

			res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 
			response = reply->data();
		}

		delete graphAPI;
		return res;
	}

	RESULT FacebookHTTPConnector::getPhotos(const QString& albumId, QVariant& response)
	{
		RESULT res = eERROR_GENERAL;
		GraphAPI* graphAPI = new GraphAPI(NULL);
		graphAPI->setSettings(_proxy, _proxy_login_pwd, _token);
		QFacebookReply* reply = graphAPI->getObject(albumId + "/photos");

		if (reply)
		{
			QEventLoop loop;
			connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();

			res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 
			response = reply->data();
		}

		delete graphAPI;
		return res;
	}

	/*RESULT FacebookHTTPConnector::downloadFile(const QString& url, const QString& path)
	{
	RESULT res = eERROR_GENERAL;
	return res;
	}*/

	RESULT FacebookHTTPConnector::uploadFile(const QString& path, const QString& /*title*/, const QString& parentId, QVariant& response)
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		CURL* p_curl = curl_easy_init();
		if(p_curl)
		{
			struct curl_httppost *post = NULL;
			struct curl_httppost *last = NULL;

			QString resp;

			if((_proxy != "") && (_proxy != ":"))
			{
				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
			}

			QString url = QString("https://graph.facebook.com/%1/photos").arg(parentId);
			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));
			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYHOST, 0L);

			curl_formadd(&post, &last, CURLFORM_COPYNAME, "access_token", CURLFORM_COPYCONTENTS, qPrintable(_token), CURLFORM_END);
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "file1", CURLFORM_FILE, qPrintable(path), CURLFORM_END);

			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(p_curl, CURLOPT_HTTPPOST, post);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &resp);

			CURLcode _error = curl_easy_perform(p_curl);
			long code;
			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
			res = (_error == CURLE_OK) && (code == 200) ? eNO_ERROR: eERROR_GENERAL;
			curl_formfree(post);
			curl_easy_cleanup(p_curl);

			if(res == eNO_ERROR)
			{
				QJson::Parser parser;
				bool ok;
				QByteArray replyResult = resp.toLocal8Bit();
				QVariant parsedResult = parser.parse(replyResult, &ok);

				GraphAPI* graphAPI = new GraphAPI(NULL);
				graphAPI->setSettings(_proxy, _proxy_login_pwd, _token);
				QVariantMap map = parsedResult.toMap();
				QFacebookReply* reply = graphAPI->getObject(map["id"].toString());

				if (reply)
				{
					QEventLoop loop;
					connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();

					res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 
					response = reply->data();
				}

				delete graphAPI;
			}
		}

		return res;
	}

	RESULT FacebookHTTPConnector::downloadFiles(QList <QString>& urlList, QList <QString>& pathList)
	{
		RESULT res = eNO_ERROR;
		QMutexLocker locker(&_connectorMutex);

		QList <CURL*> curls;
		CURLM* p_mcurl = curl_multi_init();

		for(int i = 0; i < urlList.size(); i++)
		{
			CURL* p_curl = curl_easy_init();
			if(p_curl)
			{
				if((_proxy != "") && (_proxy != ":"))
				{
					curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
					curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
				}

				curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(urlList.at(i)));

				curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
				curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, fwrite_b);
				curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &pathList.at(i));
			}
			curls.append(p_curl);
			curl_multi_add_handle(p_mcurl, p_curl);
		}

		int still_running = 0;
		int result = CURLM_CALL_MULTI_PERFORM;
		do
		{
			result = (int)curl_multi_perform(p_mcurl, &still_running);

			if(still_running <= 0)
			{
				break;
			}
		}
		while(result == CURLM_CALL_MULTI_PERFORM || still_running > 0);

		int msgs_in_queue = 0;
		do
		{
			CURLMsg* msg = curl_multi_info_read(p_mcurl, &msgs_in_queue);
			if((RESULT)msg->data.result != eNO_ERROR)
			{
				if(curl_easy_perform(msg->easy_handle) != CURLE_OK)
				{
					res = eERROR_GENERAL;
					break;
				}
			}
		}
		while(msgs_in_queue);

		for(int i = 0; i < urlList.size(); i++)
		{
			curl_multi_remove_handle(p_mcurl, curls.at(i)); 
			curl_easy_cleanup(curls.at(i));
		}

		curl_multi_cleanup(p_mcurl);

		return res;
	}

	RESULT FacebookHTTPConnector::deleteObject(const QString& id)
	{
		RESULT res = eERROR_GENERAL;
		GraphAPI* graphAPI = new GraphAPI(NULL);
		graphAPI->setSettings(_proxy, _proxy_login_pwd, _token);
		QFacebookReply* reply = graphAPI->deleteObject(id);

		if (reply)
		{
			QEventLoop loop;
			connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();

			QNetworkReply::NetworkError err = reply->error();
			res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 
		}

		delete graphAPI;

		return res;
	}

	RESULT FacebookHTTPConnector::createDirectory(const QString& title, QVariant& response)
	{
		RESULT res = eERROR_GENERAL;
		QString name = QString("name=%1").arg(title); 
		GraphAPI* graphAPI = new GraphAPI(NULL);
		graphAPI->setSettings(_proxy, _proxy_login_pwd, _token);
		QFacebookReply* reply = graphAPI->putObject("me", "albums", name.toLocal8Bit());

		if (reply)
		{
			QEventLoop loop;
			connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();

			QNetworkReply::NetworkError err = reply->error();
			res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 

			if(res == eNO_ERROR)
			{
				QVariantMap map = reply->data().toMap();
				reply = graphAPI->getObject(map["id"].toString());

				if (reply)
				{
					QEventLoop loop;
					connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();

					res = (reply->error() == QNetworkReply::NoError) ? eNO_ERROR : eERROR_GENERAL; 
					response = reply->data();
				}
			}
		}

		delete graphAPI;

		return res;
	}

	RESULT FacebookHTTPConnector::moveFile(const QString& id, const QString& /*oldParentId*/, const QString& newParentId)
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		return res;
	}

	RESULT FacebookHTTPConnector::renameAlbum(const QString& id, const QString& newTitle)
	{
		RESULT res = eERROR_NOT_SUPPORTED;
		return res;
	}
}
