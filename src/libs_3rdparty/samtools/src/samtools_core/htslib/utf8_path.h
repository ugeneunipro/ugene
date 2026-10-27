/* UGENE passes filesystem paths to HTSlib as UTF-8 on every platform. */
#ifndef UGENE_HTSLIB_UTF8_PATH_H
#define UGENE_HTSLIB_UTF8_PATH_H

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>

static inline wchar_t *hts_path_to_wide(const char *path)
{
    int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    wchar_t *wide;
    if (length == 0) { errno = EINVAL; return NULL; }
    wide = (wchar_t *) malloc(length * sizeof(wchar_t));
    if (wide == NULL) { errno = ENOMEM; return NULL; }
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, length) == 0) {
        free(wide);
        errno = EINVAL;
        return NULL;
    }
    return wide;
}

static inline int hts_open_utf8(const char *path, int flags, int mode)
{
    wchar_t *wide = hts_path_to_wide(path);
    int result, saved_errno;
    if (wide == NULL) return -1;
    result = _wopen(wide, flags, mode);
    saved_errno = errno;
    free(wide);
    errno = saved_errno;
    return result;
}

static inline int hts_stat_utf8(const char *path, struct stat *buffer)
{
    wchar_t *wide = hts_path_to_wide(path);
    struct _stat info;
    int result, saved_errno;
    if (wide == NULL) return -1;
    result = _wstat(wide, &info);
    saved_errno = errno;
    free(wide);
    if (result == 0) {
        buffer->st_dev = info.st_dev;
        buffer->st_ino = info.st_ino;
        buffer->st_mode = info.st_mode;
        buffer->st_nlink = info.st_nlink;
        buffer->st_uid = info.st_uid;
        buffer->st_gid = info.st_gid;
        buffer->st_rdev = info.st_rdev;
        buffer->st_size = info.st_size;
        buffer->st_atime = info.st_atime;
        buffer->st_mtime = info.st_mtime;
        buffer->st_ctime = info.st_ctime;
    }
    errno = saved_errno;
    return result;
}

static inline int hts_unlink_utf8(const char *path)
{
    wchar_t *wide = hts_path_to_wide(path);
    int result, saved_errno;
    if (wide == NULL) return -1;
    result = _wunlink(wide);
    saved_errno = errno;
    free(wide);
    errno = saved_errno;
    return result;
}
#else
#include <unistd.h>
#define hts_open_utf8 open
#define hts_stat_utf8 stat
#define hts_unlink_utf8 unlink
#endif

#endif
