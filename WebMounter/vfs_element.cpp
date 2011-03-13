//#include "StdAfx.h"
#include "vfs_element.h"
#include <QFileInfo>

using namespace Data;

VFSElement::VFSElement(void) : m_Flags(eFl_None)
{
}

VFSElement::~VFSElement(void)
{
}

VFSElement& VFSElement::operator=(const VFSElement& elem)
{
	this->setFlags(elem.getFlags());
	this->setDirty(elem.isDirty());
	this->setDownloaded(elem.isDownloaded());
	this->setId(elem.getId());
	this->setModified(elem.getModified());
	this->setSrcUrl(elem.getSrcUrl());
	this->setEditMetaUrl(elem.getEditMetaUrl());
	this->setEditMediaUrl(elem.getEditMediaUrl());
	this->setName(elem.getName());
	this->setParentId(elem.getParentId());
	this->setType(elem.getType());
	this->setPath(elem.getPath());
	this->setPluginName(elem.getPluginName());

	return *this;
}

bool VFSElement::operator==(const VFSElement& elem)
{
	bool equal = (this->getPluginName() == elem.getPluginName()
		&& this->getParentId() == elem.getParentId()
		//&& this->getOrigUrl() == elem.getOrigUrl()
		//&& this->getSmallUrl() == elem.getSmallUrl()
		&& this->getType() == elem.getType()
		&& this->getId() == elem.getId()
		&& this->getModified() == elem.getModified()
		/*&& this->getFlags() == elem.getFlags()*/
		);
	
	if(equal)
	{
		QFileInfo fCurrent(this->getPath());
		QFileInfo fNew(elem.getPath());

		QString path = fCurrent.path();
		QString path2 = fNew.path();
		QString name =  fCurrent.baseName();
		QString name2 = fNew.baseName();

		if(fCurrent.path() == fNew.path()
			&& fCurrent.baseName() == fNew.baseName())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return equal;
}

bool VFSElement::operator!=(const VFSElement& elem)
{
	if(*this == elem)
	{
		return false;
	}
	else
	{
		return true;
	}
}

VFSElement::VFSElement(VFSElementType type
	, const QString &path
	, const QString &name
	, const QString &editMetaUrl
	, const QString &editMediaUrl
	, const QString &srcUrl
	, const QString& id
	, const QString& parent_id
	, const QString& modified
	, const QString& pluginName
	, int flags
	) 

	: m_eType(type)
	, m_Flags(flags)
	, m_ID(id)
	, m_parentID(parent_id)
	, m_Path(path)
	, m_Name(name)
	, m_srcUrl(srcUrl)
	, m_editMetaUrl(editMetaUrl)
	, m_editMediaUrl(editMediaUrl)
	, m_pluginName(pluginName)
	, m_Modified(modified)
{
}

VFSElement::VFSElement(const VFSElement& elem) 
	: m_eType(elem.getType())
	, m_Flags(eFl_None)
	, m_ID(elem.getId())
	, m_parentID(elem.getParentId())
	, m_Path(elem.getPath())
	, m_Name(elem.getName())
	, m_srcUrl(elem.getSrcUrl())
	, m_editMetaUrl(elem.getEditMetaUrl())
	, m_editMediaUrl(elem.getEditMediaUrl())
	, m_pluginName(elem.getPluginName())
	, m_Modified(elem.getModified())
{
	setFlags(elem.getFlags());
};

uint VFSElement::getFlags() const
{
	return m_Flags;
}

bool VFSElement::isDirty() const
{
	return m_Flags&eFl_Dirty;
}
bool VFSElement::isDownloaded() const
{
	return m_Flags&eFl_Downloaded;
}

const QString& VFSElement::getModified() const
{
	return this->m_Modified;
}
const QString& VFSElement::getPath() const
{
	return this->m_Path;
}
const QString& VFSElement::getName() const
{
	return this->m_Name;
}
const QString& VFSElement::getEditMetaUrl() const
{
	return this->m_editMetaUrl;
}
const QString& VFSElement::getEditMediaUrl() const
{
	return this->m_editMediaUrl;
}
const QString& VFSElement::getSrcUrl() const
{
	return this->m_srcUrl;
}
const QString& VFSElement::getParentId(void) const
{
	return this->m_parentID;
}
const QString& VFSElement::getId(void) const
{
	return this->m_ID;
}
VFSElement::VFSElementType VFSElement::getType(void) const
{
	return this->m_eType;
}
const QString& VFSElement::getPluginName() const
{
	return this->m_pluginName;
}

void VFSElement::setPath(const QString& path)
{
	m_Path = path;
}
void VFSElement::setName(const QString& name)
{
	m_Name = name;
}
void VFSElement::setEditMetaUrl(const QString& editMetaUrl)
{
	m_editMetaUrl = editMetaUrl;
}
void VFSElement::setSrcUrl(const QString& srcUrl)
{
	m_srcUrl = srcUrl;
}
void VFSElement::setEditMediaUrl(const QString& editMediaUrl)
{
	m_editMediaUrl = editMediaUrl;
}
void VFSElement::setParentId(const QString& parentId)
{
	this->m_parentID = parentId;
}
void VFSElement::setId(const QString& id)
{
	this->m_ID = id;
}
void VFSElement::setType(VFSElement::VFSElementType type)
{
	this->m_eType = type;
}
void VFSElement::setDirty(bool dirty)
{
	uint set = dirty ? eFl_Dirty : eFl_None;
	uint unset = !dirty ? eFl_Dirty : eFl_None;
	setFlags(set, unset);
}
void VFSElement::setDownloaded(bool downloaded)
{
	uint set = downloaded ? eFl_Downloaded : eFl_None;
	uint unset = !downloaded ? eFl_Downloaded : eFl_None;
	setFlags(set, unset);
}
void VFSElement::setModified(const QString& date)
{
	this->m_Modified = date;
}
void VFSElement::setPluginName(const QString& pluginName)
{
	this->m_pluginName = pluginName;
}

void VFSElement::setFlags(uint set, uint unset)
{
	m_Flags |= set;
	m_Flags &= ~unset;
}

void VFSElement::reset()
{
	m_Flags = eFl_None;
	this->setDirty(true);
	this->setDownloaded(false);
	this->setId(0);
	this->setModified("");
	this->setSrcUrl("");
	this->setEditMetaUrl("");
	this->setEditMediaUrl("");
	this->setName("");
	this->setParentId(0);
	this->setType(UNKNOWN);
	this->setPath("");
	this->setPluginName("");
}