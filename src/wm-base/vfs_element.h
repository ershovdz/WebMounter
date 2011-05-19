#pragma once
#include <QString>
#include "common_stuff.h"

namespace Data
{
	#define ROOT_ID "0"

	class WEBMOUNTER_EXPORT VFSElement
	{
	public:
                enum VFSElementType {UNKNOWN, DIRECTORY, FILE};
		enum eVFSFlags
		{
			eFl_None		= 0x0,
			eFl_Downloaded	= 0x1,
			eFl_Dirty		= 0x2,
			eFl_SelfMade	= 0x4,
			eFl_NameDup		= 0x8,
			eFl_Downloading	= 0x10,

			eFl_All			= 0xffffff
		};

		VFSElement(void);
		VFSElement(const VFSElement& elem);
		VFSElement& operator=(const VFSElement& elem);
		virtual ~VFSElement(void);
		VFSElement(VFSElementType type
					, const QString& path
					, const QString& name
					, const QString& editMetaURL
					, const QString& editMediaURL
					, const QString& srcURL
					, const QString& id
					, const QString& parent_id
					, const QString& modified
					, const QString& pluginName
					, int flags = eFl_None);

	public:
		bool operator==(const VFSElement& elem);
		bool operator!=(const VFSElement& elem);
		VFSElementType getType() const;
		const QString& getId() const;
		const QString& getParentId() const;
		const QString& getPath() const;
		const QString& getName() const;
		const QString& getEditMetaUrl() const;
		const QString& getEditMediaUrl() const;
		const QString& getSrcUrl() const;
		const QString& getPluginName() const;
		uint getFlags() const;
		bool isDirty() const;
		bool isDownloaded() const;
		const QString& getModified() const;
	
		void setDirty(bool);
		void setDownloaded(bool);
		void setModified(const QString&);
		void setParentId(const QString&);
		void setId(const QString&);
		void setType(VFSElementType);
		void setPath(const QString&);
		void setName(const QString&);
		void setEditMediaUrl(const QString&);
		void setEditMetaUrl(const QString&);
		void setSrcUrl(const QString&);
		void setPluginName(const QString& pluginName);
		void setFlags(uint set, uint unset = eFl_None);

		void reset();

	private:
		VFSElementType m_eType;
		uint m_Flags;
		QString m_Path;
		QString m_Name;

		QString m_ID;
		QString m_parentID;

		QString m_editMetaUrl;
		QString m_editMediaUrl;
		QString m_srcUrl;

		QString m_Modified;
		QString m_pluginName;
	};
}
