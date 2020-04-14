#ifndef UGENE_CUSTOM_IO_H
#define UGENE_CUSTOM_IO_H

#include <stdio.h>

// The proxy functions for open, fopen standard library functions

// The functions accept filename (and mode) in multibyte string form
// then converts them to wide string form and call _wopen or _wfopen
// The conversion is with CP_THREAD_ACP by default

int   ugene_custom_open(const char *filename, int oflag);
int   ugene_custom_open2(const char *filename, int oflag, int pflag);
FILE* ugene_custom_fopen(const char *filename, const char* mode);

#endif // UGENE_CUSTOM_IO_H
