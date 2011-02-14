#pragma once

//#include "LVFSDriver.h"

namespace LocalDriver
{

	class FuseDriver
	{
	public:
		FuseDriver(void);
		virtual ~FuseDriver(void);

	public:
		/*virtual int getattr (const char *name, struct stat *st) = 0;
		virtual int readlink (const char *name, char *buf, size_t size) = 0;
		virtual int getdir (const char *name, fuse_dirh_t dh, fuse_dirfil_t fill) = 0;
		virtual int mknod (const char *name, mode_t mode, dev_t dev) = 0;
		virtual int mkdir (const char *name, mode_t mode) = 0;
		virtual int unlink (const char *name) = 0;
		virtual int rmdir (const char *name) = 0;
		virtual int symlink (const char *from, const char *to) = 0;
		virtual int rename (const char *from, const char *to) = 0;
		virtual int link (const char *from, const char *to) = 0;
		virtual int chmod (const char *name, mode_t mode) = 0;
		virtual int chown (const char *name, uid_t uid, gid_t gid) = 0;
		virtual int truncate (const char *name, off_t off) = 0;
		virtual int utime (const char *name, struct utimbuf *time) = 0;
		virtual int open (const char *name, struct fuse_file_info *info) = 0;
		virtual int read (const char *name, char *buf, size_t size, off_t off, struct fuse_file_info *info) = 0;
		virtual int write (const char *name, const char *buf, size_t size, off_t off, struct fuse_file_info *info) = 0;
		virtual int statfs (const char *name, struct statvfs *buf) = 0;
		virtual int flush (const char *name, struct fuse_file_info *info) = 0;
		virtual int release (const char *name, struct fuse_file_info *info) = 0;
		virtual int fsync (const char *name, int datasync, struct fuse_file_info *info) = 0;
		virtual int setxattr (const char *name, const char *attr, const char *value, size_t size, int flags) = 0;
		virtual int getxattr (const char *name, const char *attr, char *value, size_t size) = 0;
		virtual int listxattr (const char *name, char *attr, size_t size) = 0;
		virtual int removexattr (const char *name, const char *attr) = 0;
		virtual int opendir (const char *name, struct fuse_file_info *info) = 0;
		virtual int readdir (const char *name, void *buf, fuse_fill_dir_t fill, off_t off, struct fuse_file_info *info) = 0;
		virtual int releasedir (const char *name, struct fuse_file_info *info) = 0;
		virtual int fsyncdir (const char *name, int datasync, struct fuse_file_info *info) = 0;
		virtual int access (const char *name, int mode) = 0;
		virtual int create (const char *name, mode_t mode, struct fuse_file_info *info) = 0;
		virtual int ftruncate (const char *name, off_t off, struct fuse_file_info *info) = 0;
		virtual int fgetattr (const char *name, struct stat *st, struct fuse_file_info *info) = 0;*/
	};
}