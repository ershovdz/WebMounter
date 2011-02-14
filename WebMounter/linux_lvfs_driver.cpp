//#include "StdAfx.h"
#include "linux_lvfs_driver.h"

namespace LocalDriver
{
	FuseDriver::FuseDriver(void)
	{
	}

	FuseDriver::~FuseDriver(void)
	{
	}

	//static void* init()
	//{
	//	assert(settings.permchain != NULL);
	//	assert(settings.mntsrc_fd > 0);
	//
	//	if (fchdir(settings.mntsrc_fd) != 0) {
	//		fprintf(
	//			stderr,
	//			"Could not change working directory to '%s': %s\n",
	//			settings.mntsrc,
	//			strerror(errno)
	//			);
	//		fuse_exit(fuse_get_context()->fuse);
	//	}
	//
	//	return NULL;
	//}
	//
	//static void destroy(void *private_data)
	//{
	//}
	//
	//static int getattr(const char *path, struct stat *stbuf)
	//{
	//	path = process_path(path);
	//
	//	if (lstat(path, stbuf) == -1)
	//		return -errno;
	//	return getattr_common(path, stbuf);
	//}
	//
	//static int fgetattr(const char *path, struct stat *stbuf,
	//struct fuse_file_info *fi)
	//{
	//	path = process_path(path);
	//
	//	if (fstat(fi->fh, stbuf) == -1)
	//		return -errno;
	//	return getattr_common(path, stbuf);
	//}
	//
	//static int readlink(const char *path, char *buf, size_t size)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	/* No need to check for access to the link itself, since symlink
	//	permissions don't matter. Access to the path components of the symlink
	//	are automatically queried by FUSE. */
	//
	//	res = readlink(path, buf, size - 1);
	//	if (res == -1)
	//		return -errno;
	//
	//	buf[res] = '\0';
	//	return 0;
	//}
	//
	//static int opendir(const char *path, struct fuse_file_info *fi)
	//{
	//	DIR *dp;
	//
	//	path = process_path(path);
	//
	//	dp = opendir(path);
	//	if (dp == NULL)
	//		return -errno;
	//
	//	fi->fh = (unsigned long) dp;
	//	return 0;
	//}
	//
	//static inline DIR *get_dirp(struct fuse_file_info *fi)
	//{
	//	return (DIR *) (uintptr_t) fi->fh;
	//}
	//
	//static int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
	//				   off_t offset, struct fuse_file_info *fi)
	//{
	//	DIR *dp = get_dirp(fi);
	//	struct dirent *de;
	//
	//	(void) path;
	//	seekdir(dp, offset);
	//	while ((de = readdir(dp)) != NULL) {
	//		struct stat st;
	//		memset(&st, 0, sizeof(st));
	//		st.st_ino = de->d_ino;
	//		st.st_mode = de->d_type << 12;
	//		if (filler(buf, de->d_name, &st, telldir(dp)))
	//			break;
	//	}
	//
	//	return 0;
	//}
	//
	//static int releasedir(const char *path, struct fuse_file_info *fi)
	//{
	//	DIR *dp = get_dirp(fi);
	//	(void) path;
	//	closedir(dp);
	//	return 0;
	//}
	//
	//static int mknod(const char *path, mode_t mode, dev_t rdev)
	//{
	//	int res;
	//	struct fuse_context *fc;
	//	uid_t file_owner = -1;
	//	gid_t file_group = -1;
	//
	//	path = process_path(path);
	//
	//	mode = permchain_apply(settings.create_permchain, mode);
	//
	//	if (S_ISFIFO(mode))
	//		res = mkfifo(path, mode);
	//	else
	//		res = mknod(path, mode, rdev);
	//	if (res == -1)
	//		return -errno;
	//
	//	if (settings.create_policy == CREATE_AS_USER) {
	//		fc = fuse_get_context();
	//		file_owner = fc->uid;
	//		file_group = fc->gid;
	//	}
	//
	//	if (settings.create_for_uid != -1)
	//		file_owner = settings.create_for_uid;
	//	if (settings.create_for_gid != -1)
	//		file_group = settings.create_for_gid;
	//
	//	if ((file_owner != -1) || (file_group != -1)) {
	//		if (chown(path, file_owner, file_group) == -1) {
	//			DPRINTF("Failed to chown new device node (%d)", errno);
	//		}
	//	}
	//
	//	return 0;
	//}
	//
	//static int mkdir(const char *path, mode_t mode)
	//{
	//	int res;
	//	struct fuse_context *fc;
	//	uid_t file_owner = -1;
	//	gid_t file_group = -1;
	//
	//	path = process_path(path);
	//
	//	mode |= S_IFDIR; /* tell permchain_apply this is a directory */
	//	mode = permchain_apply(settings.create_permchain, mode);
	//
	//	res = mkdir(path, mode & 0777);
	//	if (res == -1)
	//		return -errno;
	//
	//	if (settings.create_policy == CREATE_AS_USER) {
	//		fc = fuse_get_context();
	//		file_owner = fc->uid;
	//		file_group = fc->gid;
	//	}
	//
	//	if (settings.create_for_uid != -1)
	//		file_owner = settings.create_for_uid;
	//	if (settings.create_for_gid != -1)
	//		file_group = settings.create_for_gid;
	//
	//	if ((file_owner != -1) || (file_group != -1)) {
	//		if (chown(path, file_owner, file_group) == -1) {
	//			DPRINTF("Failed to chown new directory (%d)", errno);
	//		}
	//	}
	//
	//	return 0;
	//}
	//
	//static int unlink(const char *path)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	res = unlink(path);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int rmdir(const char *path)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	res = rmdir(path);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int symlink(const char *from, const char *to)
	//{
	//	int res;
	//	struct fuse_context *fc;
	//	uid_t file_owner = -1;
	//	gid_t file_group = -1;
	//
	//	to = process_path(to);
	//
	//	res = symlink(from, to);
	//	if (res == -1)
	//		return -errno;
	//
	//	if (settings.create_policy == CREATE_AS_USER) {
	//		fc = fuse_get_context();
	//		file_owner = fc->uid;
	//		file_group = fc->gid;
	//	}
	//
	//	if (settings.create_for_uid != -1)
	//		file_owner = settings.create_for_uid;
	//	if (settings.create_for_gid != -1)
	//		file_group = settings.create_for_gid;
	//
	//	if ((file_owner != -1) || (file_group != -1)) {
	//		if (lchown(to, file_owner, file_group) == -1) {
	//			DPRINTF("Failed to lchown new symlink (%d)", errno);
	//		}
	//	}
	//
	//	return 0;
	//}
	//
	//static int rename(const char *from, const char *to)
	//{
	//	int res;
	//
	//	from = process_path(from);
	//	to = process_path(to);
	//
	//	res = rename(from, to);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int link(const char *from, const char *to)
	//{
	//	int res;
	//
	//	from = process_path(from);
	//	to = process_path(to);
	//
	//	res = link(from, to);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int chmod(const char *path, mode_t mode)
	//{
	//	int file_execute_only = 0;
	//	struct stat st;
	//	mode_t diff = 0;
	//
	//	path = process_path(path);
	//
	//	if (settings.chmod_allow_x) {
	//		/* Get the old permission bits and see which bits would change. */
	//		if (lstat(path, &st) == -1)
	//			return -errno;
	//
	//		if (S_ISREG(st.st_mode)) {
	//			diff = (st.st_mode & 07777) ^ (mode & 07777);
	//			file_execute_only = 1;
	//		}
	//	}
	//
	//	switch (settings.chmod_policy) {
	//case CHMOD_NORMAL:
	//	if (chmod(path, mode) == -1)
	//		return -errno;
	//	return 0;
	//case CHMOD_IGNORE:
	//	if (file_execute_only) {
	//		diff &= 00111; /* See which execute bits were flipped.
	//					   Forget about other differences. */
	//		if (chmod(path, st.st_mode ^ diff) == -1)
	//			return -errno;
	//	}
	//	return 0;
	//case CHMOD_DENY:
	//	if (file_execute_only) {
	//		if ((diff & 07666) == 0) {
	//			/* Only execute bits have changed, so we can allow this. */
	//			if (chmod(path, mode) == -1)
	//				return -errno;
	//			return 0;
	//		}
	//	}
	//	return -EPERM;
	//default:
	//	assert(0);
	//	}
	//}
	//
	//static int chown(const char *path, uid_t uid, gid_t gid)
	//{
	//	int res;
	//
	//	if (uid != -1) {
	//		switch (settings.chown_policy) {
	//case CHOWN_NORMAL:
	//	break;
	//case CHOWN_IGNORE:
	//	uid = -1;
	//	break;
	//case CHOWN_DENY:
	//	return -EPERM;
	//		}
	//	}
	//
	//	if (gid != -1) {
	//		switch (settings.chgrp_policy) {
	//case CHGRP_NORMAL:
	//	break;
	//case CHGRP_IGNORE:
	//	gid = -1;
	//	break;
	//case CHGRP_DENY:
	//	return -EPERM;
	//		}
	//	}
	//
	//	if (uid != -1 || gid != -1) {
	//		path = process_path(path);
	//		res = lchown(path, uid, gid);
	//		if (res == -1)
	//			return -errno;
	//	}
	//
	//	return 0;
	//}
	//
	//static int truncate(const char *path, off_t size)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	res = truncate(path, size);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int ftruncate(const char *path, off_t size,
	//struct fuse_file_info *fi)
	//{
	//	int res;
	//
	//	(void) path;
	//
	//	res = ftruncate(fi->fh, size);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int utime(const char *path, struct utimbuf *buf)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	res = utime(path, buf);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int create(const char *path, mode_t mode, struct fuse_file_info *fi)
	//{
	//	int fd;
	//	struct fuse_context *fc;
	//	uid_t file_owner = -1;
	//	gid_t file_group = -1;
	//
	//	path = process_path(path);
	//
	//	mode |= S_IFREG; /* tell permchain_apply this is a regular file */
	//	mode = permchain_apply(settings.create_permchain, mode);
	//
	//	fd = open(path, fi->flags, mode & 0777);
	//	if (fd == -1)
	//		return -errno;
	//
	//	if (settings.create_policy == CREATE_AS_USER) {
	//		fc = fuse_get_context();
	//		file_owner = fc->uid;
	//		file_group = fc->gid;
	//	}
	//
	//	if (settings.create_for_uid != -1)
	//		file_owner = settings.create_for_uid;
	//	if (settings.create_for_gid != -1)
	//		file_group = settings.create_for_gid;
	//
	//	if ((file_owner != -1) || (file_group != -1)) {
	//		if (chown(path, file_owner, file_group) == -1) {
	//			DPRINTF("Failed to chown new file (%d)", errno);
	//		}
	//	}
	//
	//	fi->fh = fd;
	//	return 0;
	//}
	//
	//static int open(const char *path, struct fuse_file_info *fi)
	//{
	//	int fd;
	//
	//	path = process_path(path);
	//
	//	fd = open(path, fi->flags);
	//	if (fd == -1)
	//		return -errno;
	//
	//	fi->fh = fd;
	//	return 0;
	//}
	//
	//static int read(const char *path, char *buf, size_t size, off_t offset,
	//struct fuse_file_info *fi)
	//{
	//	int res;
	//
	//	(void) path;
	//	res = pread(fi->fh, buf, size, offset);
	//	if (res == -1)
	//		res = -errno;
	//
	//	return res;
	//}
	//
	//static int write(const char *path, const char *buf, size_t size,
	//				 off_t offset, struct fuse_file_info *fi)
	//{
	//	int res;
	//
	//	(void) path;
	//	res = pwrite(fi->fh, buf, size, offset);
	//	if (res == -1)
	//		res = -errno;
	//
	//	return res;
	//}
	//
	//static int statfs(const char *path, struct statvfs *stbuf)
	//{
	//	int res;
	//
	//	path = process_path(path);
	//
	//	res = statvfs(path, stbuf);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//static int release(const char *path, struct fuse_file_info *fi)
	//{
	//	(void) path;
	//	close(fi->fh);
	//
	//	return 0;
	//}
	//
	//static int fsync(const char *path, int isdatasync,
	//struct fuse_file_info *fi)
	//{
	//	int res;
	//	(void) path;
	//
	//#ifndef HAVE_FDATASYNC
	//	(void) isdatasync;
	//#else
	//	if (isdatasync)
	//		res = fdatasync(fi->fh);
	//	else
	//#endif
	//		res = fsync(fi->fh);
	//	if (res == -1)
	//		return -errno;
	//
	//	return 0;
	//}
	//
	//#ifdef HAVE_SETXATTR
	///* If HAVE_L*XATTR is not defined, we assume Mac/BSD -style *xattr() */
	//
	//static int setxattr(const char *path, const char *name, const char *value,
	//					size_t size, int flags)
	//{
	//	DPRINTF("setxattr %s %s=%s", path, name, value);
	//
	//	if (settings.xattr_policy == XATTR_READ_ONLY)
	//		return -EACCES;
	//
	//	/* fuse checks permissions for us */
	//	path = process_path(path);
	//#ifdef HAVE_LSETXATTR
	//	if (lsetxattr(path, name, value, size, flags) == -1)
	//#else
	//	if (setxattr(path, name, value, size, 0, flags | XATTR_NOFOLLOW) == -1)
	//#endif
	//		return -errno;
	//	return 0;
	//}
	//
	//static int getxattr(const char *path, const char *name, char *value,
	//					size_t size)
	//{
	//	int res;
	//
	//	DPRINTF("getxattr %s %s", path, name);
	//
	//	path = process_path(path);
	//	/* fuse checks permissions for us */
	//#ifdef HAVE_LGETXATTR
	//	res = lgetxattr(path, name, value, size);
	//#else
	//	res = getxattr(path, name, value, size, 0, XATTR_NOFOLLOW);
	//#endif
	//	if (res == -1)
	//		return -errno;
	//	return res;
	//}
	//
	//static int listxattr(const char *path, char *list, size_t size)
	//{
	//	int res;
	//
	//	DPRINTF("listxattr %s", path);
	//
	//	path = process_path(path);
	//	/* fuse checks permissions for us */
	//#ifdef HAVE_LLISTXATTR
	//	res = llistxattr(path, list, size);
	//#else
	//	res = listxattr(path, list, size, XATTR_NOFOLLOW);
	//#endif
	//	if (res == -1)
	//		return -errno;
	//	return res;
	//}
	//
	//static int removexattr(const char *path, const char *name)
	//{
	//	DPRINTF("removexattr %s %s", path, name);
	//
	//	if (settings.xattr_policy == XATTR_READ_ONLY)
	//		return -EACCES;
	//
	//	path = process_path(path);
	//	/* fuse checks permissions for us */
	//#ifdef HAVE_LREMOVEXATTR
	//	if (lremovexattr(path, name) == -1)
	//#else
	//	if (removexattr(path, name, XATTR_NOFOLLOW) == -1)
	//#endif
	//		return -errno;
	//	return 0;
	//}
	//#endif /* HAVE_SETXATTR */
}