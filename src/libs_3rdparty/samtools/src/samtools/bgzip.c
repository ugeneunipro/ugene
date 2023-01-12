/* The MIT License

   Copyright (c) 2008 Broad Institute / Massachusetts Institute of Technology

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>
#include "bgzf.h"

static const int WINDOW_SIZE = 64 * 1024;

static int bgzip_main_usage()
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:   bgzip [options] [file] ...\n\n");
	fprintf(stderr, "Options: -c      write on standard output, keep original files unchanged\n");
	fprintf(stderr, "         -d      decompress\n");
	fprintf(stderr, "         -f      overwrite files without asking\n");
	fprintf(stderr, "         -b INT  decompress at virtual file pointer INT\n");
	fprintf(stderr, "         -s INT  decompress INT bytes in the uncompressed file\n");
	fprintf(stderr, "         -h      give this help\n");
	fprintf(stderr, "\n");
	return 1;
}

static int write_open(const char *fn, int is_forced)
{
	int fd = -1;
	char c;
	if (!is_forced) {
		if ((fd = open(fn, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 0666)) < 0 && errno == EEXIST) {
			fprintf(stderr, "[bgzip] %s already exists; do you wish to overwrite (y or n)? ", fn);
			scanf("%c", &c);
			if (c != 'Y' && c != 'y') {
				fprintf(stderr, "[bgzip] not overwritten\n");
				exit(1);
			}
		}
	}
	if (fd < 0) {
		if ((fd = open(fn, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
			fprintf(stderr, "[bgzip] %s: Fail to write\n", fn);
			exit(1);
		}
	}
	return fd;
}

static void fail(BGZF* fp)
{
    fprintf(stderr, "Error: %s\n", fp->error);
    exit(1);
}
