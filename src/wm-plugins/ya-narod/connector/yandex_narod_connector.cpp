
#include <curl/curl.h>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include "cp_rsa.h"
#include "base64.h"
#include "yandex_narod_connector.h"
#include "reg_exp.h"
#include <QFileInfo>

namespace Connector
{
	using namespace Common;
	using namespace Data;
	//	QMutex YandexNarodHTTPConnector::_connectorMutex;

	size_t YandexNarodHTTPConnector::writeStr(void *ptr, size_t size, size_t count, void *response)
	{
		if(response)
		{
			((QString*)response)->append(QString::fromUtf8((char*)ptr, count));
			return size*count;
		}
		return -1;
	}

	size_t YandexNarodHTTPConnector::fwrite_b(void *ptr, size_t size, size_t count, void *path)
	{
		//QMutexLocker locker(&_connectorMutex); 		
		if(path)
 		{
 			QFile file(*(QString*)path);
 			bool res = file.open(QIODevice::WriteOnly | QIODevice::Append);
 			size_t result = file.write((char*)ptr, size*count);
			file.flush();
 			return result;
 		}
		return -1;
	}

	YandexNarodHTTPConnector::YandexNarodHTTPConnector()
	{
	}

	YandexNarodHTTPConnector::~YandexNarodHTTPConnector()
	{
	}

	void YandexNarodHTTPConnector::setSettings(const QString& login
		, const QString& password
		, const QString& proxy
		, const QString& proxyLoginPwd)
	{
		_login = login.left(login.lastIndexOf("@"));
		_password = password;
		_proxy = proxy;
		_proxy_login_pwd = proxyLoginPwd;
	}

	int YandexNarodHTTPConnector::execQuery(const QString &url, const QString &header, const QString& postFields, QString* response)
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
 
			const char* urlStr = url.toUtf8().data();
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

	RESULT YandexNarodHTTPConnector::auth()
	{
		bool isConnected = false;
		cookies.clear();
		QString header = "";
		QString post = QString::fromAscii("login=") + _login + QString::fromAscii("&passwd=") + _password;
		QString response;
		int err = execQuery("http://passport.yandex.ru/passport?mode=auth", header, post, &response);
		if(err == 302)
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

	RESULT YandexNarodHTTPConnector::getFiles(QString& response)
	{
		RESULT res = eERROR;
		QString header = "Cookie: ";
		for(int i=0; i<cookies.count(); i++)
		{
			header += cookies.at(i) + "; ";
		}
		QString post = "";
		int err = execQuery("http://narod.yandex.ru/disk/all/page1/?sort=cdate%20desc", header, post, &response);
		const char* header_rsp = response.toUtf8().data();
		QString tmp(response);
		int page = 1;
		int cpos = 0;
		while(err == 200 && cpos >= 0)
		{
			QRegExp rxnp("<a\\sid=\"next_page\"\\shref=\"([^\"]+)\"");
			cpos = rxnp.indexIn(tmp);
			if(cpos>0 && rxnp.capturedTexts()[1].length())
			{
				tmp = "";
				QString url = "http://narod.yandex.ru/disk/all/page" + QString::number(++page) + "/?sort=cdate%20desc";
				err = execQuery(url, header, post, &tmp);
				response.append(tmp);
			}
		}
		return (err == 200) ? eNO_ERROR : eERROR;
	}

	RESULT YandexNarodHTTPConnector::deleteFile(const QString& id, const QString& token)
	{
		RESULT res = eERROR;
		QString header = "Cookie: ";
		for(int i=0; i<cookies.count(); i++)
		{
			header += cookies.at(i) + "; ";
		}
		QString post = QString::fromAscii("action=delete&fid=%1&token-%2=%3").arg(id).arg(id).arg(token);
		QString response;
		int err = execQuery("http://narod.yandex.ru/disk/all/", header, post, &response);

		return res;
	}

	RESULT YandexNarodHTTPConnector::getUploadLink(QString& progressLink, QString& uploadLink)
	{
		RESULT res = eERROR;
		CURL* p_curl = curl_easy_init();
		if(p_curl)
		{
			if((_proxy != "") && (_proxy != ":"))
			{
				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
			}

			curl_easy_setopt(p_curl, CURLOPT_URL, "http://narod.yandex.ru/disk/getstorage/");

			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			QString response;
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

			CURLcode _error = curl_easy_perform(p_curl);

			long code;
			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
			if(_error == CURLE_OK && code == 200)
			{
				QRegExp rx("\"url\":\"(\\S+)\".+\"hash\":\"(\\S+)\".+\"purl\":\"(\\S+)\"");
				if (rx.indexIn(response)>-1)
				{
					progressLink = rx.cap(3) + "?tid=" + rx.cap(2);
					const char* purlStr = progressLink.toUtf8().data();
					uploadLink = rx.cap(1) + "?tid=" + rx.cap(2);
					const char* uplink = uploadLink.toUtf8().data();
					int a = 0;
				}

				res = eNO_ERROR;
			}
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::doUpload(const QString& path, const QString& title, const QString& uploadLink, QString& response)
	{
		RESULT res = eERROR;
		QString header = "Cookie: ";
		for(int i=0; i<cookies.count(); i++)
		{
			header += cookies.at(i) + "; ";
		}

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

			struct curl_slist *chunk = NULL;
			chunk = curl_slist_append(chunk, qPrintable(header));
			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk);

			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(uploadLink));
			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);

			curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, qPrintable(path), CURLFORM_END);

			curl_easy_setopt(p_curl, CURLOPT_HTTPPOST, post);

			curl_easy_setopt(p_curl, CURLOPT_WRITEHEADER, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_HEADERDATA, &response);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

			CURLcode _error = curl_easy_perform(p_curl);
			res = (_error == CURLE_OK) ? eNO_ERROR: eERROR;
			curl_formfree(post);
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::getProgress(const QString& progressLink, QString& response)
	{
		RESULT res = eERROR;

		QString header = "Cookie: ";
		for(int i=0; i<cookies.count(); i++)
		{
			header += cookies.at(i) + "; ";
		}

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

			struct curl_slist *chunk = NULL;
			chunk = curl_slist_append(chunk, qPrintable(header));
			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk);

			const char* prlink = progressLink.toUtf8().data();
			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(progressLink));
			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);

			curl_easy_setopt(p_curl, CURLOPT_WRITEHEADER, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_HEADERDATA, &response);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

			CURLcode _error = curl_easy_perform(p_curl);
			res = (_error == CURLE_OK) ? eNO_ERROR: eERROR;
			curl_formfree(post);
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::getUploadedFileLink(QString& link)
	{
		RESULT res = eERROR;

		QString header = "Cookie: ";
		for(int i=0; i<cookies.count(); i++)
		{
			header += cookies.at(i) + "; ";
		}

		CURL* p_curl = curl_easy_init();
		if(p_curl)
		{
			if((_proxy != "") && (_proxy != ":"))
			{
				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
			}

			struct curl_slist *chunk = NULL;
			chunk = curl_slist_append(chunk, qPrintable(header));
			curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk);

			curl_easy_setopt(p_curl, CURLOPT_URL, "http://narod.yandex.ru/disk/last/");
			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);

			curl_easy_setopt(p_curl, CURLOPT_WRITEHEADER, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_HEADERDATA, &link);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &link);

			CURLcode _error = curl_easy_perform(p_curl);
			res = (_error == CURLE_OK) ? eNO_ERROR: eERROR;
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response)
	{
		RESULT res = eERROR;
		QString progressLink;
 		QString uploadLink;
		getUploadLink(progressLink, uploadLink);

		res = doUpload(path, title, uploadLink, response);

		if(res == eNO_ERROR)
		{
			QString tmp;
			res = getProgress(progressLink, tmp);
			response.append(tmp);
			if(res == eNO_ERROR)
			{
				tmp = "";
				res = getUploadedFileLink(tmp);
				response.append(tmp);
			}
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::getDownloadLink(const QString& url, QString& link)
	{
		RESULT res = eERROR;
		CURL* p_curl = curl_easy_init();
		if(p_curl)
		{
			if((_proxy != "") && (_proxy != ":"))
			{
				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
			}

			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));

			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);
			curl_easy_setopt(p_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.0.3) Gecko/2008092417 Firefox/3.0.3 YB/3.5.3");
			QString response;
			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

			CURLcode _error = curl_easy_perform(p_curl);

			long code;
			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
			if(_error == CURLE_OK && code == 200)
			{
				link = RegExp::getByPattern("class=\"h\-link\" rel=\"yandex_bar\" href=\"(.*)\">", response);
				res = eNO_ERROR;
			}
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::downloadFile(const QString& url, const QString& path)
	{
		//QMutexLocker locker(&_connectorMutex);
		
		RESULT res = eERROR;

 		CURL* p_curl = curl_easy_init();
 		if(p_curl)
 		{
 			if((_proxy != "") && (_proxy != ":"))
 			{
 				curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
 				curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
 			}
 
 			curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));
 
 			curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(p_curl, CURLOPT_FOLLOWLOCATION, 1L);
 			curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, fwrite_b);
 			const QString* path1 = &path;
 			curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &path);
 
 			CURLcode _error = curl_easy_perform(p_curl);
 			long code;
 			curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
 			res = (_error == CURLE_OK && code == 200) ? eNO_ERROR : eERROR;
 			curl_easy_cleanup(p_curl);
 		}

		return res;
	}

	RESULT YandexNarodHTTPConnector::downloadFiles(const QList <QString>& urlList, const QList <QString>& pathList)
	{
		QMutexLocker locker(&_connectorMutex);
		
		RESULT res = eNO_ERROR;
		QList <CURL*> curls;
		CURLM* p_mcurl = curl_multi_init();

		for(int i = 0; i < urlList.size(); i++)
		{
			QString link;
			res = getDownloadLink(urlList.at(i), link);
			if(res == eNO_ERROR)
			{
				QString realUrl("http://narod.ru" + link);
				const char* linkStr = link.toUtf8().data();
				downloadFile(realUrl, pathList.at(i));
//  				CURL* p_curl = curl_easy_init();
//  				if(p_curl)
//  				{
//  					if((_proxy != "") && (_proxy != ":"))
//  					{
//  						curl_easy_setopt(p_curl, CURLOPT_PROXY, qPrintable(_proxy));
//  						curl_easy_setopt(p_curl, CURLOPT_PROXYUSERPWD, qPrintable(_proxy_login_pwd));
//  					}
//  
//  					curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(realUrl));
//  
//  					curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
//  					curl_easy_setopt(p_curl, CURLOPT_FOLLOWLOCATION, 1L);
//  					curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, fwrite_b);
//  					curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &pathList.at(i));
//  				}
//  				curls.append(p_curl);
//  				curl_multi_add_handle(p_mcurl, p_curl);
			}
		}

// 		int still_running = 0;
// 		int result = CURLM_CALL_MULTI_PERFORM;
// 		do
// 		{
// 			result = (int)curl_multi_perform(p_mcurl, &still_running);
// 
// 			if(still_running <= 0)
// 			{
// 				break;
// 			}
// 		}
// 		while(result == CURLM_CALL_MULTI_PERFORM || still_running > 0);
// 
//  		int msgs_in_queue = 0;
//  		do
//  		{
//  			CURLMsg* msg = curl_multi_info_read(p_mcurl, &msgs_in_queue);
//  			if((RESULT)msg->data.result == eERROR)
//  			{
//  				if(curl_easy_perform(msg->easy_handle) != CURLE_OK)
//  				{
//  					res = eERROR;
//  					break;
//  				}
//  			}
//  		}
//  		while(msgs_in_queue);
//  
//  		for(int i = 0; i < urlList.size(); i++)
//  		{
//  			curl_multi_remove_handle(p_mcurl, curls.at(i));
//  			curl_easy_cleanup(curls.at(i));
//  		}
//  
//  		curl_multi_cleanup(p_mcurl);
		return res;
	}
}