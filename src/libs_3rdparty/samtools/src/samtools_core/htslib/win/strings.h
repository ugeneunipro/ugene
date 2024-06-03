// Based on https://github.com/mirror/mingw-org-wsl/blob/master/src/libcrt/posix/unistd/usleep.c

#include <string.h>

#ifndef _STRINGS_H
#define _STRINGS_H

#define strncasecmp(x,y,z) _strnicmp(x,y,z)

#define strcasecmp(x,y) _stricmp(x,y)

#endif /* strings.h  */
