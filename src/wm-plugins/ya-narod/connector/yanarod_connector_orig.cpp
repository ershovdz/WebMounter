
#include <curl/curl.h>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include "yanarod_connector.h"
#include "reg_exp.h"
#include <QFileInfo>

namespace Connector
{
	using namespace Common;
	using namespace Data;

	size_t YaNarodHTTPConnector::writeStr(void *ptr, size_t size, size_t count, void *response)
	{
		if(response)
		{
			((QString*)response)->append(QString::fromUtf8((char*)ptr, count));
			return size*count;
		}
		return -1;
	}

	size_t YaNarodHTTPConnector::fwrite_b(void *ptr, size_t size, size_t count, void *path)
	{
		//QMutexLocker locker(&_connectorMutex); 		
		if(path)
 		{
 			QFile file(*(QString*)path);
 			bool res = file.open(QIODevice::WriteOnly | QIODevice::Append);
 			size_t result = file.write((char*)ptr, size*count);
 			return result;
 		}
		return -1;
	}

	size_t YaNarodHTTPConnector::readStr(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t res = 0;
		if(stream)
		{
			sPutData* userdata = (sPutData*)stream;

			size_t curl_size = nmemb * size;
			res = (userdata->len < curl_size) ? userdata->len : curl_size;
			memcpy(ptr, userdata->data, res);
			userdata->len -= res;
			userdata->data += res;
 		}

		return res;
	} 

	YaNarodHTTPConnector::YaNarodHTTPConnector()
	{
	}

	YaNarodHTTPConnector::~YaNarodHTTPConnector()
	{
	}

	void YaNarodHTTPConnector::setSettings(const QString& login
		, const QString& password
		, const QString& proxy
		, const QString& proxyLoginPwd)
	{
		_login = login.left(login.lastIndexOf("@"));
		_password = password;
		_proxy = proxy;
		_proxy_login_pwd = proxyLoginPwd;
	}

	int YaNarodHTTPConnector::execQuery(const QString &url, const QString &header, const QString& postFields, QString* response)
	{
		int res = 0;
 		CURL* p_curl = curl_easy_init();
 		if(p_curl)
 		{
 			struct curl_httppost *post = NULL;
 			struct curl_httppost *last = NULL;
 
 			if((_proxy != "") && (_proxy != ":"))
 			{
 				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
 				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
 			}
 
 			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));
 			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
 
 			struct curl_slist *chunk = NULL;
 			chunk = curl_slist_append(chunk, "Cache-Control: no-store, no-cache, must-revalidate");
 
 			if(header.length())
 			{
 				chunk = curl_slist_append(chunk, qPrintable(header));
 			}
 
 			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk); 
 
 			QByteArray postArray = postFields.toUtf8();
 			if(postFields.length())
 			{
 				curl_easy_setopt(p_curl, CURLOPT_POSTFIELDS, postArray.constData());
 				curl_easy_setopt(p_curl, CURLOPT_POST, 1L);
 			}
 
 			curl_easy_setopt(p_curl, CURLOPT_WRITEHEADER, writeStr);
 			curl_easy_setopt(p_curl, CURLOPT_HEADERDATA, response);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, response);
 
 			CURLcode error = curl_easy_perform(p_curl);
 			long code;
 			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
 			res = (error == CURLE_OK) ? code : error;
 			curl_easy_cleanup(p_curl);
 			curl_slist_free_all(chunk);
 		}
		return res;
	}

	RESULT YaNarodHTTPConnector::auth()
	{
		bool isConnected = false;
		QString header = "";
		QString post = QString::fromAscii("login=") + _login + QString::fromAscii("&passwd=") + _password;
		QString response;
		int err = execQuery("http://passport.yandex.ru/passport?mode=auth", header, post, &response);
		if(!err)
		{
			QString pattern = QString::fromAscii("Set-Cookie: (.*); path");
			QRegExp rx(pattern);
			rx.setCaseSensitivity(Qt::CaseSensitive);
			rx.setMinimal(true);
			rx.setPatternSyntax(QRegExp::RegExp);

			int pos = 0;
			while((pos = rx.indexIn(response, pos)) != -1)
			{
				cookies.append(rx.cap(1));
				pos += rx.matchedLength();
			}

			isConnected = err && (cookies.count() > 0);
		}

		return !isConnected ? eERROR: eNO_ERROR;
	}

	RESULT YaNarodHTTPConnector::getTreeElements(const QString& path, QString& response)
	{
//		QString url = QString("http://api-fotki.yandex.ru//api/users/%1/%2").arg(_login).arg(path);
//		QString header = "";

//		int err = execQuery(url, header, "", &response);
//		return (err == 200) ? eNO_ERROR : eERROR;
	}

	RESULT YaNarodHTTPConnector::createDirectory(const QString& title, const QString& parentId, QString& response)
	{
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodHTTPConnector::deleteFile(const QString& path, QString& response)
	{
		RESULT res = eERROR;

/* 		CURL* p_curl = curl_easy_init();
 		if(p_curl)
 		{
 			struct curl_httppost *post = NULL;
 			struct curl_httppost *last = NULL;
 
 			if((_proxy != "") && (_proxy != ":"))
 			{
 				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
 				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
 			}
 
 			QString url = QString::fromAscii("http://api-fotki.yandex.ru/api/users/%1/%2").arg(_login).arg(path);
 
 			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));
 
 			struct curl_slist *chunk = NULL;
 			QString header = _isOAuth ? QString("Authorization: OAuth %1").arg(_token)
 				: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);
 
 			chunk = curl_slist_append(chunk, qPrintable(header));
 
 			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
 			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk);
 			curl_easy_setopt(p_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
 			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);
 
 			CURLcode _error = curl_easy_perform(p_curl);
 			long code;
 			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
 
 			res = (_error == CURLE_OK && code == 204) ? eNO_ERROR : eERROR;
 			curl_easy_cleanup(p_curl);
 		}
*/
		return res;
	}

	RESULT YaNarodHTTPConnector::uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response)
	{
		RESULT res = eERROR;
 
	/*	CURL* p_curl = curl_easy_init();
 		if(p_curl)
 		{
 			struct curl_httppost *post = NULL;
 			struct curl_httppost *last = NULL;
 			struct curl_slist *headerlist = NULL;
 			QString expect = QString::fromAscii("Expect:");
 			
 			if((_proxy != "") && (_proxy != ":"))
 			{
 				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
 				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
 			}
 
 			QByteArray pathArray = path.toLocal8Bit();
 			
 			QString fileName = title + "." + QFileInfo(path).suffix().toLower();
 			QByteArray titleArray = fileName.toUtf8();
 
 			QString access = "public";
 			curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_FILE, pathArray.constData(), CURLFORM_END);
 			curl_formadd(&post, &last, CURLFORM_COPYNAME, "title", CURLFORM_COPYCONTENTS, titleArray.constData(), CURLFORM_END);
 			curl_formadd(&post, &last, CURLFORM_COPYNAME, "album", CURLFORM_COPYCONTENTS, qPrintable(parentId), CURLFORM_END);
 			curl_formadd(&post, &last, CURLFORM_COPYNAME, "access_type", CURLFORM_COPYCONTENTS, qPrintable(access), CURLFORM_END);
 			curl_formadd(&post, &last, CURLFORM_COPYNAME, "yaru", CURLFORM_COPYCONTENTS, "0", CURLFORM_END);
 
 			headerlist = curl_slist_append(headerlist, qPrintable(expect));
 
 			QString auth = _isOAuth ? QString("Authorization: OAuth %1").arg(_token) 
 				: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);
 			headerlist = curl_slist_append(headerlist, qPrintable(auth));
 
 			curl_easy_setopt(p_curl, CURLOPT_URL, "http://api-fotki.yandex.ru/post/");
 
 			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, headerlist);
 			curl_easy_setopt(p_curl, CURLOPT_HTTPPOST, post);
 			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);
 			
 			CURLcode error = curl_easy_perform(p_curl);
 			long code;
 			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
 			res = (error == CURLE_OK && code == 200) ? eNO_ERROR : eERROR;
 
			curl_formfree(post);
			curl_slist_free_all(headerlist);
 			curl_easy_cleanup(p_curl);
 		}
*/
		return res;
	}

	RESULT YaNarodHTTPConnector::downloadFile(const QString& url, const QString& path)
	{
		QMutexLocker locker(&_connectorMutex);
		
		RESULT res = eERROR;
 /*		CURL* p_curl = curl_easy_init();
 		if(p_curl)
 		{
 			if((_proxy != "") && (_proxy != ":"))
 			{
 				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
 				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
 			}
 
 			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));
 
 			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, fwrite_b);
 			const QString* path1 = &path;
 			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &path);
 
 			CURLcode _error = curl_easy_perform(p_curl);
 			long code;
 			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
 			res = (_error == CURLE_OK && code == 200) ? eNO_ERROR : eERROR;
 			curl_easy_cleanup(p_curl);
 		}
*/
		return res;
	}

	RESULT YaNarodHTTPConnector::downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList)
	{
		QMutexLocker locker(&_connectorMutex);
		
		RESULT res = eNO_ERROR;
 /*		QList <CURL*> curls;
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
 			if((RESULT)msg->data.result == eERROR)
 			{
 				if(curl_easy_perform(msg->easy_handle) != CURLE_OK)
 				{
 					res = eERROR;
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
	*/
		return res;
	}


	RESULT YaNarodHTTPConnector::moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type, QString& response)
	{
		RESULT res = eERROR;
		return res;
	}

	RESULT YaNarodHTTPConnector::renameElement(const QString& id, ElementType type, const QString& newTitle, QString& response)
	{
		RESULT res = eERROR;
		return res;
	}
}
