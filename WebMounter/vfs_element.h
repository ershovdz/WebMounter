#pragma once
#include <QString>

namespace Data
{
	#define ROOT_ID "0"

	class VFSElement
	{
	public:
		static enum VFSElementType {UNKNOWN, DIRECTORY, FILE};
		enum eFlags
		{
			eFl_None		= 0x0,
			eFl_Downloaded	= 0x1,
			eFl_Dirty		= 0x2,
			eFl_SelfMade	= 0x4,
			eFl_NameDup		= 0x8,

			eFl_All			= 0xffffff
		};

		VFSElement(void);
		VFSElement(const VFSElement& elem);
		VFSElement& operator=(const VFSElement& elem);
		virtual ~VFSElement(void);
		VFSElement(VFSElementType type
			, const QString& path
			, const QString& name
			, const QString& smallURL
			, const QString& origURL
			, const QString& id
			, const QString& parent_id
			, const QString& modified
			, const QString& pluginName
			, int flags = eFl_None
			);

	public:
		bool operator==(const VFSElement& elem);
		bool operator!=(const VFSElement& elem);
		VFSElementType getType() const;
		const QString& getId() const;
		const QString& getParentId() const;
		const QString& getPath() const;
		const QString& getName() const;
		const QString& getSmallUrl() const;
		const QString& getOrigUrl() const;
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
		void setSmallUrl(const QString&);
		void setOrigUrl(const QString&);
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

		QString m_smallUrl;
		QString m_origUrl;

		QString m_Modified;
		QString m_pluginName;
	};
}