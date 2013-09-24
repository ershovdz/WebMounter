/* Copyright (c) 2013, Alexander Ershov
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 * Contact e-mail: Alexander Ershov <ershav@yandex.ru>
 */

#ifndef YAF_DRIVER_H
#define YAF_DRIVER_H

#include <QDir>
#include <QWaitCondition>
#include "rvfs_driver.h"
//#include "common_stuff.h"
#include "../connector/yaf_connector.h"

#include "data.h"


namespace RemoteDriver
{
	class YafRVFSDriver :
		public RVFSDriver
	{
		Q_OBJECT

	public:
		YafRVFSDriver(const QString& pluginName);
		virtual ~YafRVFSDriver(void);
		void connectHandlerStage2(RESULT error, PluginSettings pluginSettings);

	public:
		virtual RESULT downloadFiles() {return RVFSDriver::downloadFiles();};
		virtual RESULT downloadFiles(QList <QString>& urlList, QList <QString>& pathList);
		virtual RESULT uploadFile(const QString& path, const QString& title,  const QString& id, const QString& parentId);
		virtual RESULT modifyFile(const QString&);
		virtual RESULT renameElement(const QString& path, const QString& newTitle);
		virtual RESULT deleteDirectory(const QString& id);
		virtual RESULT deleteFile(const QString& id);
		virtual RESULT moveElement(const QString& path, const QString& newParentId);
		virtual RESULT createDirectory(const QString& path, const QString& parentId, const QString& title);
		virtual RESULT getElements(QList<VFSElement>& elements);
		virtual bool areFileAttributesValid(const QString& path, unsigned long attributes);

	private:
		RESULT getAlbums(QList<VFSElement>& elements);
		RESULT getPhotos(QList<VFSElement>& elements);

		void parseAlbumEntry(QString& xmlEntry, VFSElement& elem);
		void parsePhotoEntry(QString& xmlEntry, VFSElement& elem);

		virtual void connectHandler(PluginSettings& pluginSettings);
		virtual void disconnectHandler();
		virtual void syncHandler();
		virtual void stopSyncHandler();

		QString getElementPath(QList<VFSElement>& elements, VFSElement& element);
		int findParentIndex(const QList<VFSElement>& elemList, const VFSElement& elem);
		void markNameDuplicates(QList<VFSElement>& elemList);
		void handleNameDuplicates(QList<VFSElement>& elemList);
		QString addPathSuffix(ElementType type, const QString& path, const QString& suffix);

	private:
		Connector::YafHTTPConnector* m_httpConnector;
	};
}

#endif //YAF_DRIVER_H
