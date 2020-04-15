#include <io.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "ugene_custom_io.h"

// The proxy functions for open, fopen standard library functions

// The func accepts filename in multibyte string form
// then converts it to wide string form and call _wopen
// The conversion is with CP_THREAD_ACP by default
int ugene_custom_open(const char *filename, int oflag) {
#ifndef _WIN32
    return open(filename, oflag);
#else
    int wchars_num = MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, NULL, 0);
    wchar_t* w_filename = malloc(sizeof(wchar_t) * wchars_num);
    MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, w_filename, wchars_num);

    int fd = _wopen(w_filename, oflag);
    free(w_filename);
    return fd;
#endif
}

// The func accepts filename in multibyte string form
// then converts it to wide string form and call _wopen
// The conversion is with CP_THREAD_ACP by default
int ugene_custom_open2(const char *filename, int oflag, int pflag) {
#ifndef _WIN32
    return open(filename, oflag);
#else
    int wchars_num = MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, NULL, 0);
    wchar_t* w_filename = malloc(sizeof(wchar_t) * wchars_num);
    MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, w_filename, wchars_num);

    int fd = _wopen(w_filename, oflag, pflag);
    free(w_filename);
    return fd;
#endif
}

// The func accepts filename and mode in multibyte string form
// then converts it to wide string form and call _wfopen
// The conversion is with CP_THREAD_ACP by default
FILE* ugene_custom_fopen(const char *filename, const char* mode) {
#ifndef _WIN32
    return fopen(filename, oflag);
#else
    int wchars_num = MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, NULL, 0);
    wchar_t* w_filename = malloc(sizeof(wchar_t) * wchars_num);
    MultiByteToWideChar(CP_THREAD_ACP, 0, filename, -1, w_filename, wchars_num);

    wchars_num = MultiByteToWideChar(CP_THREAD_ACP, 0, mode, -1, NULL, 0);
    wchar_t* w_mode = malloc(sizeof(wchar_t) * wchars_num);
    MultiByteToWideChar(CP_THREAD_ACP, 0, mode, -1, w_mode, wchars_num);

    FILE* fd = _wfopen(w_filename, w_mode);
    free(w_filename);
    free(w_mode);
    return fd;
#endif
}
