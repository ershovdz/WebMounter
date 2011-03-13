#ifndef VFS_CACHE_H
#define VFS_CACHE_H

#include "common_stuff.h"

#include <map>
#include <QString>
#include "vfs_element.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <QMutex>
#include <QMutexLocker>

#include <boost/shared_ptr.hpp>

using Common::RESULT;

namespace Data
{
	typedef boost::shared_ptr<VFSElement> VFSElementPtr;
	typedef std::map<QString, VFSElementPtr> VFSFileList;
	typedef VFSElement::VFSElementType ElementType;
	typedef std::pair <QString, VFSElementPtr> VFSFileList_Pair;
	
	class VFSCache_Iter
	{
		friend class VFSCache;
	
	public:
		typedef VFSFileList::iterator::iterator_category iterator_category;
		typedef VFSFileList::iterator::value_type value_type;
		typedef VFSFileList::iterator::difference_type difference_type;
		typedef difference_type distance_type;
		typedef VFSFileList::iterator::pointer pointer;
		typedef VFSFileList::iterator::reference reference;

	public:
		VFSCache_Iter operator++(int) // postfix form
		{
			VFSCache_Iter iter = _Iter;
			_Iter++;
			return iter;
		}

		VFSCache_Iter operator--(int) // postfix form
		{
			VFSCache_Iter iter = _Iter;
			_Iter--;
			return iter;
		}

		VFSCache_Iter& operator++() // prefix form
		{
			_Iter++;
			return *this;
		}

		VFSCache_Iter& operator--() // prefix form
		{
			_Iter--;
			return *this;
		}

		operator VFSFileList::iterator()
		{
			return _Iter;
		}

		VFSCache_Iter& operator*()
		{
			return *this;
		}
	
		bool operator!=(VFSFileList::iterator iter)
		{
			return (_Iter != iter);
		}

		bool operator==(VFSFileList::iterator iter)
		{
			return (_Iter == iter);
		}

		bool operator!=(VFSElement& elem)
		{
			return (*(_Iter->second.get()) != elem);
		}

		bool operator==(VFSElement& elem)
		{
			return (*(_Iter->second.get()) == elem);
		}

		const VFSElement* operator->()
		{
			return _Iter->second.get();
		}

	protected:
		
		VFSCache_Iter& operator=(VFSFileList::iterator iter)
		{
			_Iter = iter;
			return *this;
		}

		VFSCache_Iter& operator=(VFSElement elem)
		{
			*(_Iter->second.get()) = elem;
			return *this;
		}

		VFSCache_Iter(VFSFileList::iterator iter)
		{
			_Iter = iter;
		}
		VFSCache_Iter()
		{	
		}

	protected:
		VFSFileList::iterator getNatureIter()
		{
			return _Iter;
		}

	private:
		VFSFileList::iterator _Iter;
	};

	class VFSCache
	{
	protected:
		VFSCache(void);
		VFSCache(const QString& localDBPath);

	public:
		~VFSCache(void);

	public:
		typedef VFSCache_Iter iterator;

		static VFSCache* getCache(const QString& localDBPath = "");
		RESULT restoreCache();
		void insert(VFSElement& elem, bool dirty = false, bool db = true);
		RESULT erase(iterator& elem);
		RESULT erasePlugin(const QString& pluginName);
		RESULT flush();
		
		iterator find(const QString& key)
		{
			return _FileList.find(key);
		}

		iterator begin()
		{
			return _FileList.begin();
		}
		iterator end()
		{
			return _FileList.end();
		}

		iterator findById(int id)
		{
			
		}

		void setFlag(iterator& iter, uint set, uint unset = VFSElement::eFl_None);

	private:
		RESULT initDB();
	private:
		VFSFileList _FileList;
		static QSqlDatabase _DB;
		static VFSCache* _VFSCacheInstance;
		static QMutex _VFSCacheMutex;
		static QString _localDBPath;
	};
}

#endif