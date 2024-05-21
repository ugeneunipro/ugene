#ifndef _UNISTD_H
#define _UNISTD_H

// Based on https://github.com/mirror/mingw-org-wsl/blob/master/src/libcrt/posix/unistd/usleep.c
int __cdecl usleep(unsigned int useconds);

// Based on https://stackoverflow.com/a/62371749/7273346
#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

 /* Standard file descriptors from unistd.h */
#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */

#endif /* unistd.h  */
