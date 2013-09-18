#include "filesystemtools.h"

using namespace Common;

RESULT FileSystemTools::removeFolder( QDir& dir )
{
  if ( eNO_ERROR != removeFolderContent( dir ) )
  {
    return eERROR_FILE_SYSTEM;
  }
  else if ( !QDir().rmdir(dir.absolutePath()) )
  {
    return eERROR_FILE_SYSTEM;
  }
  return eNO_ERROR;
}

RESULT FileSystemTools::removeFolderContent( QDir& dir )
{
  RESULT res = eNO_ERROR;

  QStringList lstDirs  = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
  QStringList lstFiles = dir.entryList(QDir::Files);

  foreach (QString entry, lstFiles)
  {
    QString entryAbsPath = dir.absolutePath() + "/" + entry;

    QFile::Permissions permissions = QFile::permissions(entryAbsPath);
    permissions |= (QFile::WriteGroup|QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther);
    QFile::setPermissions(entryAbsPath, permissions);

    QFile::remove(entryAbsPath);
  }

  foreach (QString entry, lstDirs)
  {
    QString entryAbsPath = dir.absolutePath() + "/" + entry;
    QDir internalDir(entryAbsPath);

    if ( eNO_ERROR != removeFolderContent( internalDir )  )
    {
      res = eERROR_FILE_SYSTEM;
    }
    else if( !QDir().rmdir( internalDir.absolutePath() ))
    {
      res = eERROR_FILE_SYSTEM;
    }
  }

  return res;
}