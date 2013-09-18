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
