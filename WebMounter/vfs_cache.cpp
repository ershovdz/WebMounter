//#include "StdAfx.h"
#include "vfs_cache.h"
#include <QtCore/qfile.h>

#include <vector>
#include <QString>
#include <QVariant>

#include <WebMounter.h>

using std::vector;

namespace Data
{
	using namespace Common;

	QSqlDatabase VFSCache::_DB;
	VFSCache* VFSCache::_VFSCacheInstance = 0;
	QMutex VFSCache::_VFSCacheMutex;
	QString VFSCache::_localDBPath;

	VFSCache::VFSCache(void)
	{
	}

	VFSCache::VFSCache(const QString& localDBPath)
	{
		_localDBPath = localDBPath;

		_DB = QSqlDatabase::addDatabase("QSQLITE", "webmounter");
		_DB.setDatabaseName(localDBPath + "\\webmounter.db");

		if(!QFile::exists(localDBPath + "\\webmounter.db")) // Database does not exist yet
		{
			_DB.open(); // Open db and create db file.
			initDB(); // Populate database with plugin tables
		}
		else // Database exists. Just open it
		{
			_DB.open(); 
		}
	}

	VFSCache::~VFSCache(void)
	{
		_DB.close();
		_DB.removeDatabase("webmounter");
	}

	RESULT VFSCache::initDB()
	{
		QSqlQuery query(_DB);

		if(_DB.isOpen())
		{
			query.exec("PRAGMA synchronous=OFF");

			bool isOk = query.exec("create table Elements (path varchar(256),"
				"elemid varchar(256),"
				"parentid varchar(256),"
				"name varchar(256),"
				"type int,"
				"flags int,"
				"modified varchar(256),"
				"smallurl varchar(1024),"
				"origurl varchar(1024),"
				"pluginName varchar(256))");

			if(!isOk)
			{
				return eERROR;
			}
			
			return eNO_ERROR;
		}
		return eERROR;
	}

	VFSCache* VFSCache::getCache(const QString& localDBPath)
	{
		if(_VFSCacheInstance)
		{
			if(_DB.isOpen())
			{
				return _VFSCacheInstance;
			}
			else
			{
				bool isOk = _DB.open();
				if(isOk)
				{
					return _VFSCacheInstance;
				}
				else
				{
					delete _VFSCacheInstance;
					_VFSCacheInstance = 0;
					return 0;
				}
			}
		}
		else
		{
			_VFSCacheInstance = new VFSCache(localDBPath);
			if(_DB.isOpen())
			{
				if(_VFSCacheInstance->restoreCache() == eNO_ERROR)
				{
					return _VFSCacheInstance;
				}
				else
				{
					delete _VFSCacheInstance;
					_VFSCacheInstance = 0;
					return 0;
				}
			}
			else
			{
				delete _VFSCacheInstance;
				_VFSCacheInstance = 0;
				return 0;
			}
		}
	}

	void VFSCache::insert(VFSElement& elem, bool dirty, bool updatedb)
	{
		VFSCache::iterator iter = _FileList.find(elem.getPath());

		if(iter != _FileList.end()) // element exists
		{
			if(iter != elem ) // element not equal to existing one
			{
				iter.getNatureIter()->second->setFlags(VFSElement::eFl_Dirty);
				iter.getNatureIter()->second->setDownloaded(false);
				iter.getNatureIter()->second->setId(elem.getId());
				iter.getNatureIter()->second->setModified(elem.getModified());
				iter.getNatureIter()->second->setOrigUrl(elem.getOrigUrl());
				iter.getNatureIter()->second->setSmallUrl(elem.getSmallUrl());
				iter.getNatureIter()->second->setName(elem.getName());
				iter.getNatureIter()->second->setParentId(elem.getParentId());
				iter.getNatureIter()->second->setType(elem.getType());
				iter.getNatureIter()->second->setPath(elem.getPath());
				iter.getNatureIter()->second->setPluginName(elem.getPluginName());

				if(updatedb)
				{
					flush();
				}
			}
			else if(dirty) // element equal to existing one. Let's mark as dirty
			{
				iter.getNatureIter()->second->setDirty(true);
			}
		}
		else // element does not exist or moved. Let's insert it to DB and to list
		{
			for(iterator iter = this->begin(); iter != this->end(); ++iter) 
			{
				if(iter == elem) // HACK!! Identical elements, except file extension.
				{
					elem.setDownloaded(iter->isDownloaded());
					elem.setName(iter->getName());
					elem.setPath(iter->getPath());
					
					erase(iter);

					elem.setDirty(dirty);

					VFSElementPtr elemPtr (new VFSElement(elem)); 

					_FileList.insert(VFSFileList_Pair(elem.getPath(), elemPtr));

					if(updatedb)
					{
						flush();
					}
					return;
				}

				if(iter->getId() == elem.getId() && iter->getType() == elem.getType()) // element moved or renamed
				{
					//iter.getNatureIter()->second->setPath(elem.getPath());
					//iter.getNatureIter()->second->setDirty(dirty);
					//iter.getNatureIter()->first. = elem.getPath();
					elem.setDownloaded(iter->isDownloaded());
					erase(iter);

					elem.setDirty(dirty);

					VFSElementPtr elemPtr (new VFSElement(elem)); 

					_FileList.insert(VFSFileList_Pair(elem.getPath(), elemPtr));

					if(updatedb)
					{
						flush();
					}
					return;
				}
				
			}

			elem.setDirty(dirty);

			VFSElementPtr elemPtr (new VFSElement(elem)); 

			_FileList.insert(VFSFileList_Pair(elem.getPath(), elemPtr));

			//if(updatedb)
			//{
				QSqlQuery query(_DB);

				query.prepare("INSERT INTO Elements(path, elemid, parentid, name, type, flags, modified, smallurl, origurl, pluginName) "
					"VALUES(:path_elem, :elemid,"
					":parentid,"
					":name,"
					":type,"
					":flags,"
					":modified,"
					":smallurl,"
					":origurl,"
					":pluginName)");

				//query.bindValue(":path", QVariant(iter->getPath()));
				query.bindValue(":path_elem", QVariant(elem.getPath()));
				query.bindValue(":elemid", QVariant(elem.getId()));
				query.bindValue(":parentid", QVariant(elem.getParentId()));
				query.bindValue(":name", QVariant(elem.getName()));
				query.bindValue(":type", QVariant(elem.getType()));
				query.bindValue(":flags", QVariant(elem.getFlags()));
				query.bindValue(":modified", QVariant(elem.getModified()));
				query.bindValue(":smallurl", QVariant(elem.getSmallUrl()));
				query.bindValue(":origurl", QVariant(elem.getOrigUrl()));
				query.bindValue(":pluginName", QVariant(elem.getPluginName()));

				query.exec();
			//}
		}
	}

	void VFSCache::setDownloaded(iterator& iter, bool value)
	{
		iter.getNatureIter()->second->setDirty(true);
		iter.getNatureIter()->second->setDownloaded(value);
		flush();
	}

	RESULT VFSCache::erasePlugin(const QString& pluginName)
	{
		for(VFSCache::iterator iter = _FileList.begin(); 
			iter != _FileList.end() && iter->getPluginName() == pluginName;
			++iter)
		{
			erase(iter);
		}
		return eNO_ERROR;
	}

	RESULT VFSCache::erase(iterator& iter)
	{
		QSqlQuery query(_DB);

		if(iter != _FileList.end())
		{
			query.prepare("DELETE FROM Elements WHERE elemid=:id");
			query.bindValue(":id", iter->getId());

			if(!query.exec())
			{
				return eERROR;
			}

			_FileList.erase(iter);
			return eNO_ERROR;
		}
		return eNO_ERROR;
	}

	RESULT VFSCache::restoreCache()
	{
		//QMutexLocker locker(&_VFSCacheMutex);

		QString rootPath = Common::WebMounter::getSettingStorage()->getAppStoragePath();
		//vector<QString> plugins = WebMounter::getAllPluginsName();

		_FileList.clear();

		/*VFSElement elem(VFSElement(VFSElement::DIRECTORY
		, rootPath
		, "ROOT"
		, ""
		, ""
		, 0
		, 0
		, ""
		, "root"
		, false
		, true));*/

		//VFSElementPtr elemPtr (new VFSElement(elem)); 

		//_FileList.insert(VFSFileList_Pair(elem.getPath(), elemPtr));

		QSqlQuery query(_DB);

		//for(int i = 0; i < plugins.size(); i++) 
		//{
		bool isOk = query.exec("SELECT path, elemid, parentid, name, type, smallurl, origurl, flags, modified, pluginName FROM Elements WHERE 1");

		//query.bindValue(":pluginName", QVariant(plugins[i]));
		if(!isOk)
		{
			return eERROR;
		}

		while (query.next()) 
		{
			QString path = query.value(0).toString();
			QString elemid = query.value(1).toString()/*.toInt()*/;
			QString parentid = query.value(2).toString()/*.toInt()*/;
			QString name = query.value(3).toString();
			VFSElement::VFSElementType type = (VFSElement::VFSElementType)query.value(4).toInt();
			QString smallurl = query.value(5).toString();
			QString origurl = query.value(6).toString();
			int flags = query.value(7).toInt();
			QString modified = query.value(8).toString();
			QString pluginName = query.value(9).toString();

			flags &= ~VFSElement::eFl_Dirty; // we get it from database, thus it is not dirty
			VFSElement elem(VFSElement(type
				, path
				, name
				, smallurl
				, origurl
				, elemid
				, parentid
				, modified
				, pluginName
				, flags));

			VFSElementPtr elemPtr (new VFSElement(elem)); 

			_FileList.insert(VFSFileList_Pair(elem.getPath(), elemPtr));
		}
		return eNO_ERROR;
	}

	RESULT VFSCache::flush()
	{
		//QMutexLocker locker(&_VFSCacheMutex);
		QSqlQuery query(_DB);

		for(iterator iter = _FileList.begin(); iter != _FileList.end(); iter++)
		{
			if(iter->isDirty())
			{
				query.prepare("UPDATE Elements SET "
					"elemid=:elemid,"
					"parentid=:parentid,"
					"name=:name,"
					"type=:type,"
					"flags=:flags,"
					"modified=:modified,"
					"smallurl=:smallurl,"
					"origurl=:origurl,"
					"pluginName=:pluginName WHERE path=:path_elem");

				//query.bindValue(":path", QVariant(iter->getPath()));
				query.bindValue(":path_elem", QVariant(iter->getPath()));
				query.bindValue(":elemid", QVariant(iter->getId()));
				query.bindValue(":parentid", QVariant(iter->getParentId()));
				query.bindValue(":name", QVariant(iter->getName()));
				query.bindValue(":type", QVariant(iter->getType()));
				query.bindValue(":flags", QVariant(iter->getFlags()));
				query.bindValue(":modified", QVariant(iter->getModified()));
				query.bindValue(":smallurl", QVariant(iter->getSmallUrl()));
				query.bindValue(":origurl", QVariant(iter->getOrigUrl()));
				query.bindValue(":pluginName", QVariant(iter->getPluginName()));

				if(!query.exec())
				{
					return eERROR;
				}
				iter.getNatureIter()->second->setDirty(false);
			}
		}
		return eNO_ERROR;
	}
}