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

#ifndef FILESYSTEMTOOLS_H
#define FILESYSTEMTOOLS_H

#include <QObject>
#include <QDir>

#include "common_stuff.h"

namespace Common
{
	class FileSystemTools : public QObject
	{
		Q_OBJECT

	public:
		static RESULT removeFolder( QDir& dir );
		static RESULT removeFolderContent( QDir& dir );
	};
}

#endif // FILESYSTEMTOOLS_H
