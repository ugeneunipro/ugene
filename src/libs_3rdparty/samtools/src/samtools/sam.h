#ifndef BAM_SAM_H
#define BAM_SAM_H

#include "bam.h"

/*!
  @header

  This file provides higher level of I/O routines and unifies the APIs
  for SAM and BAM formats. These APIs are more convenient and
  recommended.

  @copyright Genome Research Ltd.
 */

/*! @typedef
  @abstract SAM/BAM file handler
  @field  type    type of the handler; bit 1 for BAM, 2 for reading and bit 3-4 for flag format
  @field  bam   BAM file handler; valid if (type&1) == 1
  @field  tamr  SAM file handler for reading; valid if type == 2
  @field  tamw  SAM file handler for writing; valid if type == 0
  @field  header  header struct
 */
typedef struct {
	int type;
	union {
		tamFile tamr;
		bamFile bam;
		FILE *tamw;
	} x;
	bam_header_t *header;
} samfile_t;

extern const int READ_ERROR_CODE;
extern char * SAMTOOLS_ERROR_MESSAGE;

#ifdef __cplusplus
extern "C" {
#endif
	/*!
	  @abstract     Read one alignment
	  @param  fp    file handler
	  @param  b     alignment
	  @return       bytes read
	 */
	int samread(samfile_t *fp, bam1_t *b);

	/*!
	  @abstract     Write one alignment
	  @param  fp    file handler
	  @param  b     alignment
	  @return       bytes written
	 */
	int samwrite(samfile_t *fp, const bam1_t *b);

    /*!
     @abstract     Close a SAM/BAM handler
     @param  fp    file handler to be closed
    */
    void samclose(samfile_t *fp);

    bam_header_t *bam_header_dup(const bam_header_t *h0);

    void append_header_text(bam_header_t *header, char* text, int len);

#ifdef __cplusplus
}
#endif

#endif
