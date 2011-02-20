
#include <curl/curl.h>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include "CP_RSA.h"
#include "base64.h"
#include "yaf_connector.h"
#include "reg_exp.h"

namespace Connector
{
	using namespace Common;
	using namespace Data;
	//	QMutex YafHTTPConnector::_connectorMutex;

	size_t YafHTTPConnector::writeStr(void *ptr, size_t size, size_t count, void *response)
	{
		if(response)
		{
			((QString*)response)->append(QString::fromUtf8((char*)ptr, count));
			return size*count;
		}
		return -1;
	}

	size_t YafHTTPConnector::fwrite_b(void *ptr, size_t size, size_t count, void *path) 
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

	size_t YafHTTPConnector::readStr(void *ptr, size_t size, size_t nmemb, void *stream)
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

	YafHTTPConnector::YafHTTPConnector()
	{
		_curl = curl_easy_init();
	}

	YafHTTPConnector::~YafHTTPConnector()
	{
		curl_easy_cleanup(_curl);
	}

	void YafHTTPConnector::setSettings(const QString& login
		, const QString& password
		, const QString& proxy
		, const QString& proxyLoginPwd
		, bool isOAuth)
	{
		_login = login.left(login.lastIndexOf("@"));
		_password = password;
		_proxy = proxy;
		_proxy_login_pwd = proxyLoginPwd;
		_isOAuth = isOAuth;
	}

	int YafHTTPConnector::execQuery(const QString &url, const QString &header, const QString& postFields, QString* response)
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

	RESULT YafHTTPConnector::getCredentials()
	{
		RESULT res = eERROR;

		QString response;
		int err = execQuery("http://auth.mobile.yandex.ru/yamrsa/key/", "", "", &response);
		if(err == 200) 
		{
			res = eNO_ERROR;
			_key = RegExp::getByPattern("<key>(.*)</key>", response);
			_requestId = RegExp::getByPattern("<request_id>(.*)</request_id>", response);

			if(_key.length() == 0 || _requestId.length() == 0)
			{
				res = eERROR;
			}
		}

		return res;
	}

	std::string YafHTTPConnector::encrypt(const char* public_key, std::string text)
	{
		CCryptoProviderRSA encrypter;
		encrypter.ImportPublicKey(public_key);

		char crypted_text[MAX_CRYPT_BITS / sizeof(char)] = "\0";
		size_t crypted_len = 0;
		encrypter.Encrypt(text.c_str(), text.size(), crypted_text, crypted_len);

		std::string b64_crypted_text = base64_encode((unsigned char *)crypted_text, crypted_len);

		return b64_crypted_text;
	}

	RESULT YafHTTPConnector::postCredentials()
	{
		RESULT res = eERROR;

		QString credStr = QString("<credentials login=\"%1\" password=\"%2\"/>").arg(_login).arg(_password);
		std::string cred = encrypt(_key.toStdString().c_str(), credStr.toStdString());
		QString postReq = QString("request_id=%1&credentials=%2").arg(_requestId).arg(QString::fromStdString(cred));

		QString response;
		int err = execQuery("http://auth.mobile.yandex.ru/yamrsa/token/", "", postReq, &response);
		if(err == 200) 
		{
			_token = RegExp::getByPattern("<token>(.*)</token>", response);
			res = eNO_ERROR;
		}

		return res;
	}

	RESULT YafHTTPConnector::auth()
	{
		bool isConnected = false;
 		int reqCount = 0;
 		RESULT err = eNO_ERROR;
		if(!_isOAuth)
		{
			while(!isConnected && reqCount <= 20)
			{
				err = getCredentials();
				if(err)
				{
					break;
				}

				err = postCredentials();

				isConnected = (_token.length() != 0);
				reqCount++;
			}
		}
		return !isConnected ? eERROR: eNO_ERROR;
	}

	RESULT YafHTTPConnector::getTreeElements(const QString& path, QString& response)
	{
		QString url = QString("http://api-fotki.yandex.ru/api/users/%1/%2").arg(_login).arg(path);
		QString header = _isOAuth ? QString("Authorization: OAuth %1").arg(_token) 
			: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token); 

		int err = execQuery(url, header, "", &response);
		return (err == 200) ? eNO_ERROR : eERROR;
	}

	RESULT YafHTTPConnector::createDirectory(const QString& title, const QString& parentId, QString& response)
	{
 		QString url = QString("http://api-fotki.yandex.ru/api/users/%1/albums/").arg(_login);
 		QString header = _isOAuth ? QString("Content-Type: application/atom+xml; charset=utf-8; type=entry\nAuthorization: OAuth %1").arg(_token)
			: QString("Content-Type: application/atom+xml; charset=utf-8; type=entry\nAuthorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);
		
		QString postParams = QString("<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:f=\"yandex:fotki\">\r\n <title>%1</title>\r\n <summary> Test </summary>\r\n </entry>").arg(title);
		int err = execQuery(url, header, postParams, &response);

 		return (err == 201) ? eNO_ERROR : eERROR;
	}

	RESULT YafHTTPConnector::deleteFile(const QString& path, QString& response)
	{
		RESULT res = eERROR;

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

		return res;
	}

	RESULT YafHTTPConnector::uploadFile(const QString& path, const QString& title, const QString& parentId, QString& response)
	{
		RESULT res = eERROR;

		CURL* p_curl = curl_easy_init();
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
			
			QString fileName = title;
			QByteArray titleArray = fileName.toUtf8();

			curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_FILE, pathArray.constData(), CURLFORM_END);
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "title", CURLFORM_COPYCONTENTS, titleArray.constData(), CURLFORM_END);
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "album", CURLFORM_COPYCONTENTS, qPrintable(parentId), CURLFORM_END);

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

			/* then cleanup the formpost chain */ 
			curl_formfree(post);
			/* free slist */ 
			curl_slist_free_all(headerlist);
			curl_easy_cleanup(p_curl);
		}

		return res;
	}

	RESULT YafHTTPConnector::downloadFile(const QString& url, const QString& path)
	{
		QMutexLocker locker(&_connectorMutex);
		
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




	RESULT YafHTTPConnector::downloadFile(const QList <QString>& urlList, const QList <QString>& pathList)
	{
		QMutexLocker locker(&_connectorMutex);
		
		RESULT res = eNO_ERROR;
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
		return res;
	}


	RESULT YafHTTPConnector::moveElement(const QString& id, const QString& oldParentId, const QString& newParentId, ElementType type, QString& response)
	{
		RESULT res = eERROR;
		QString typeStr = type == (VFSElement::DIRECTORY) ? QString("album") : QString("photo");
		QString url = QString("http://api-fotki.yandex.ru/api/users/%1/%2/%3/").arg(_login).arg(typeStr).arg(id);
		QString header = _isOAuth ? QString("Authorization: OAuth %1").arg(_token) 
			: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);

		int err = execQuery(url, header, "", &response);
		if(err == 200)
		{
			res = eNO_ERROR;
			QString entry = RegExp::getByPattern("<entry (.*)</entry>", response);
			QString from = QString("<link href=\"http://api-fotki.yandex.ru/api/users/%1/album/%2/\" rel=\"album\" />").arg(_login).arg(oldParentId);
			QString to = QString("<link href=\"http://api-fotki.yandex.ru/api/users/%1/album/%2/\" rel=\"album\" />").arg(_login).arg(newParentId);
			
			if(entry.indexOf(from) != -1)
			{
				if(newParentId == ROOT_ID)
				{
					entry.remove(from);
				}
				else
				{
					entry.replace(from, to);
				}
			}
			else
			{
				if(newParentId == ROOT_ID)
				{
					res = eERROR; // Moving from root to root. This should be handled in file-proxy
				}
				else
				{
					int index = entry.indexOf("<published>");
					entry.insert(index, to + "\r");
				}
			}

			CURL* p_curl = curl_easy_init();
			if(p_curl) 
			{
				curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));

				curl_easy_setopt(p_curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(p_curl, CURLOPT_PUT, 1L);
				curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
				
				struct curl_slist *chunk = NULL;
 				QString contentType = "Content-Type: application/atom+xml; charset=utf-8; type=entry";
 				chunk = curl_slist_append(chunk, qPrintable(contentType));
				QString authorization = _isOAuth ? QString("Authorization: OAuth %1").arg(_token)
					: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);
 				chunk = curl_slist_append(chunk, qPrintable(authorization));

				curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk); 

				QString datastr = QString("<entry %1</entry>").arg(entry);
				sPutData userdata;
				QByteArray arr = datastr.toUtf8();
				userdata.data = arr.constData();
				userdata.len = strlen(userdata.data);

				curl_easy_setopt(p_curl, CURLOPT_READFUNCTION, readStr);
				curl_easy_setopt(p_curl, CURLOPT_READDATA, &userdata);
				curl_easy_setopt(p_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)userdata.len);
				curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);

				curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

				CURLcode err = curl_easy_perform(p_curl);
				long code;
				curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
				res = (err == CURLE_OK && code == 200) ? eNO_ERROR : eERROR;

				curl_easy_cleanup(p_curl);
			}
		}

		return res;
	}

	RESULT YafHTTPConnector::renameElement(const QString& id, ElementType type, const QString& newTitle, QString& response)
	{
		RESULT res = eERROR;

		QString typeStr = type == (VFSElement::DIRECTORY) ? QString("album") : QString("photo");
		QString url = QString("http://api-fotki.yandex.ru/api/users/%1/%2/%3/").arg(_login).arg(typeStr).arg(id);
		QString header = _isOAuth ? QString("Authorization: OAuth %1").arg(_token)
			: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);

		int err = execQuery(url, header, "", &response);
		if(err == 200)
		{
			QString entry = RegExp::getByPattern("<entry (.*)</entry>", response);
			QString title = QString("<title>%1</title>").arg(RegExp::getByPattern("<title>(.*)</title>", entry));
			QString to = QString("<title>%1</title>").arg(newTitle);
			entry.replace(title, to);

			CURL* p_curl = curl_easy_init();
			if(p_curl) 
			{
				curl_easy_setopt(p_curl, CURLOPT_URL, qPrintable(url));

				curl_easy_setopt(p_curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(p_curl, CURLOPT_PUT, 1L);
				curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);

				struct curl_slist *chunk = NULL;
 				QString contentType = "Content-Type: application/atom+xml; charset=utf-8; type=entry";
 				chunk = curl_slist_append(chunk, qPrintable(contentType));
				QString authorization = _isOAuth ? QString("Authorization: OAuth %1").arg(_token) 
					: QString("Authorization: FimpToken realm=\"fotki.yandex.ru\", token=\"%1\"").arg(_token);
 				chunk = curl_slist_append(chunk, qPrintable(authorization));

				curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, chunk); 

				QString datastr = QString("<entry %1</entry>").arg(entry);
				sPutData userdata;
				QByteArray arr = datastr.toUtf8();
				userdata.data = arr.constData();
				userdata.len = strlen(userdata.data);

				curl_easy_setopt(p_curl, CURLOPT_READFUNCTION, readStr);
				curl_easy_setopt(p_curl, CURLOPT_READDATA, &userdata);
				curl_easy_setopt(p_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)userdata.len);
				curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, writeStr);

				curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, &response);

				CURLcode err = curl_easy_perform(p_curl);
				long code;
				curl_easy_getinfo(p_curl, CURLINFO_RESPONSE_CODE, &code);
				res = (err == CURLE_OK && code == 200) ? eNO_ERROR : eERROR;

				curl_easy_cleanup(p_curl);
			}
		}

		return res;
	}

	void YafHTTPConnector::setToken(const QString& token)
	{
		_token = token;
	}
}
