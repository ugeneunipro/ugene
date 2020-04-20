/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <QFile>
#include <QVarLengthArray>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "IOLibUtils.h"
#include "SCFFormat.h"

/* TRANSLATOR U2::SCFFormat */

namespace U2 {

SCFFormat::SCFFormat(QObject* p) : DocumentFormat(p, BaseDocumentFormats::SCF, DocumentFormatFlag_SupportStreaming, QStringList("scf")) {
    formatName = tr("SCF");
    formatDescription = tr ("It is Standard Chromatogram Format");
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    supportedObjectTypes+=GObjectTypes::CHROMATOGRAM;
}

FormatCheckResult SCFFormat::checkRawData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    if (size <= 4 || data[0]!='.' || data[1]!='s' || data[2]!='c' || data[3]!='f') {
        return FormatDetection_NotMatched;
    }
    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    return hasBinaryData ? FormatDetection_Matched: FormatDetection_NotMatched;
}

Document* SCFFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){

    Document* doc = parseSCF(dbiRef, io, fs, os);
    CHECK_OP(os, NULL);
    CHECK_EXT(doc != NULL, os.setError(tr("Failed to parse SCF file: %1").arg(io->getURL().getURLString())), NULL);

    return doc;
}

template <class T> void dumpVector(const QVector<T>& v) {
    printf("Tha vector\n");
    for (int i = 0; i < v.count(); ++i ){
        printf("%d ", v[i]);
    }
    printf("\n\n");
}

/*
* Copyright (c) Medical Research Council 1994. All rights reserved.
*
* Permission to use, copy, modify and distribute this software and its
* documentation for any purpose is hereby granted without fee, provided that
* this copyright and notice appears in all copies.
*
* This file was written by James Bonfield, Simon Dear, Rodger Staden,
* as part of the Staden Package at the MRC Laboratory of Molecular
* Biology, Hills Road, Cambridge, CB2 2QH, United Kingdom.
*
* MRC disclaims all warranties with regard to this software.
*/

/*
Title:       read_scf.c

Purpose:     read IO of Standard Chromatogram Format sequences
Last update:   August 18 1994

Change log:
4 Feb 1992,  Now draft proposal version 2
20 Feb 1992, Grab info from comment lines
19 Aug 1992, If SCF file has clip information, don't clip automatically
10 Nov 1992  SCF comments now stored in seq data structure
18 Aug 1994  Renamed from  ReadIOSCF.c; now purely SCF IO (no Seq structs)

*/

/* The SCF magic number */
#define SCF_MAGIC ((int) ((((('.'<<8)+'s')<<8)+'c')<<8)+'f')

/* prior to this was a different format */
#define SCF_VERSION_OLDEST 2.00F
#define SCF_VERSION_OLD 2.02F

/* The current SCF format level */
#define SCF_VERSION 3.00

/*
 *-----------------------------------------------------------------------------
 * Structures and typedefs
 *-----------------------------------------------------------------------------
 */

/*
 * Type definition for the Header structure
 */
typedef struct {
    uint magic_number;       /* SCF_MAGIC */
    uint samples;            /* Number of elements in Samples matrix */
    uint samples_offset;     /* Byte offset from start of file */
    uint bases;              /* Number of bases in Bases matrix */
    uint bases_left_clip;    /* OBSOLETE: No. bases in left clip (vector) */
    uint bases_right_clip;   /* OBSOLETE: No. bases in right clip (qual) */
    uint bases_offset;       /* Byte offset from start of file */
    uint comments_size;      /* Number of bytes in Comment section */
    uint comments_offset;    /* Byte offset from start of file */
    char   version[4];         /* "version.revision" */
    uint sample_size;          /* precision of samples (in bytes) */
    uint code_set;         /* uncertainty codes used */
    uint private_size;       /* size of private data, 0 if none */
    uint private_offset;     /* Byte offset from start of file */
    uint spare[18];          /* Unused */
} Header;

/*
 * Header.sample_size == 1.
 */
typedef struct {
    uchar sample_A;         /* Sample for A trace */
    uchar sample_C;         /* Sample for C trace */
    uchar sample_G;         /* Sample for G trace */
    uchar sample_T;         /* Sample for T trace */
} Samples1;

/*
 * Header.sample_size == 2.
 */
typedef struct {
    ushort sample_A;            /* Sample for A trace */
    ushort sample_C;            /* Sample for C trace */
    ushort sample_G;            /* Sample for G trace */
    ushort sample_T;            /* Sample for T trace */
} Samples2;

/*
 * Type definition for the sequence data
 */
typedef struct {
    uint peak_index;        /* Index into Samples matrix for base position */
    uchar prob_A;            /* Probability of it being an A */
    uchar prob_C;            /* Probability of it being an C */
    uchar prob_G;            /* Probability of it being an G */
    uchar prob_T;            /* Probability of it being an T */
    char base;            /* Base called */
    uchar spare[3];          /* Spare */
} Bases;


/*
 * Type definition for the comments
 */
typedef char Comments;      /* Zero terminated list of \n separated entries */

/*
* Reading SCF routines
* -----------------
* Return:
*    0 - success
*   -1 - failure
*/
int read_scf_header(SeekableBuf *fp, Header *h)
{
    int i;

    if (be_read_int_4(fp,&h->magic_number)==0)        return -1;

    if (h->magic_number != SCF_MAGIC)
        return -1;

    if (be_read_int_4(fp,&h->samples)==0)             return -1;
    if (be_read_int_4(fp,&h->samples_offset)==0)      return -1;
    if (be_read_int_4(fp,&h->bases)==0)               return -1;
    if (be_read_int_4(fp,&h->bases_left_clip)==0)     return -1;
    if (be_read_int_4(fp,&h->bases_right_clip)==0)    return -1;
    if (be_read_int_4(fp,&h->bases_offset)==0)        return -1;
    if (be_read_int_4(fp,&h->comments_size)==0)       return -1;
    if (be_read_int_4(fp,&h->comments_offset)==0)     return -1;
    if (!fp->read(&h->version[0],sizeof(h->version))) return -1;
    if (be_read_int_4(fp,&h->sample_size)==0)         return -1;
    if (be_read_int_4(fp,&h->code_set)==0)            return -1;
    if (be_read_int_4(fp,&h->private_size)==0)        return -1;
    if (be_read_int_4(fp,&h->private_offset)==0)      return -1;
    for (i=0;i<18;i++)
        if (be_read_int_4(fp,&h->spare[i])==0)        return -1;

    return 0;
}


int read_scf_sample1(SeekableBuf *fp, Samples1 *s)
{
    uchar buf[4];

    if (!fp->read((char*)buf, 4)) return -1;
    s->sample_A = buf[0];
    s->sample_C = buf[1];
    s->sample_G = buf[2];
    s->sample_T = buf[3];

    return 0;
}


int read_scf_sample2(SeekableBuf *fp, Samples2 *s)
{
    if (be_read_int_2(fp,&s->sample_A)==0) return -1;
    if (be_read_int_2(fp,&s->sample_C)==0) return -1;
    if (be_read_int_2(fp,&s->sample_G)==0) return -1;
    if (be_read_int_2(fp,&s->sample_T)==0) return -1;

    return 0;
}

int read_scf_samples1(SeekableBuf *fp, Samples1 *s, size_t num_samples)
{
    for (size_t i = 0; i < num_samples; i++) {
        if (-1 == read_scf_sample1(fp, &(s[i])))
            return -1;
    }

    return 0;
}


int read_scf_samples2(SeekableBuf *fp, Samples2 *s, size_t num_samples)
{
    for (size_t i = 0; i < num_samples; i++) {
        if (-1 == read_scf_sample2(fp, &(s[i])))
            return -1;
    }

    return 0;
}

void scf_delta_samples1 (char* samples, int num_samples)
{
    /* do the reverse to:
    change a series of sample points to a series of delta delta values:
    ie change them first: delta = current_value - previous_value
    then delta_delta = delta - previous_delta
    */
#ifdef CLEAR_BUT_SLOW
        uchar p_sample;

        p_sample = 0;
        for (i=0;i<num_samples;i++) {
            samples[i] = samples[i] + p_sample;
            p_sample = samples[i];
        }
        p_sample = 0;
        for (i=0;i<num_samples;i++) {
            samples[i] = samples[i] + p_sample;
            p_sample = samples[i];
        }
#else
        char p_sample1, p_sample2;

        p_sample1 = p_sample2 = 0;
        for (int i = 0; i < num_samples; i++) {
            p_sample1  = p_sample1 + samples[i];
            samples[i] = p_sample1 + p_sample2;
            p_sample2  = samples[i];
        }
#endif
}

/* define samples to delta_delta values */
#define DELTA_IT 1

void scf_delta_samples2 ( ushort samples[], int num_samples, int job) {

    /* If job == DELTA_IT:
    change a series of sample points to a series of delta delta values:
    ie change them first: delta = current_value - previous_value
    then delta_delta = delta - previous_delta

    else
    do the reverse
    */

    register int i;

    if ( DELTA_IT == job ) {
#ifdef CLEAR_BUT_SLOW
        register ushort p_delta, p_sample;

        p_delta  = 0;
        for (i=0;i<num_samples;i++) {
            p_sample = samples[i];
            samples[i] = samples[i] - p_delta;
            p_delta  = p_sample;
        }
        p_delta  = 0;
        for (i=0;i<num_samples;i++) {
            p_sample = samples[i];
            samples[i] = samples[i] - p_delta;
            p_delta  = p_sample;
        }
#else
        for (i = num_samples-1 ; i > 1; i--) {
            samples[i] = samples[i] - 2*samples[i-1] + samples[i-2];
        }
        samples[1] = samples[1] - 2*samples[0];
#endif

    } else {

#ifdef CLEAR_BUT_SLOW
        register ushort p_sample;

        p_sample = 0;
        for (i=0;i<num_samples;i++) {
            samples[i] = samples[i] + p_sample;
            p_sample = samples[i];
        }
        p_sample = 0;
        for (i=0;i<num_samples;i++) {
            samples[i] = samples[i] + p_sample;
            p_sample = samples[i];
        }
#else
        ushort p_sample1, p_sample2;

        p_sample1 = p_sample2 = 0;
        for (i = 0; i < num_samples; i++) {
            p_sample1  = p_sample1 + samples[i];
            samples[i] = p_sample1 + p_sample2;
            p_sample2  = samples[i];
        }
#endif
    }
}



int read_scf_samples32(SeekableBuf *fp, Samples2 *s, size_t num_samples)
{
    size_t i;
    QVarLengthArray<ushort> arr(num_samples);
    ushort* samples_out = arr.data();

    /* version to read delta delta data in 2 bytes */

    for (i = 0; i < num_samples; i++) {
        if (be_read_int_2(fp, samples_out + i) == 0) return -1;
    }
    scf_delta_samples2 ( samples_out, num_samples,0);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_A = samples_out[i];
    }

    for (i = 0; i < num_samples; i++) {
        if (be_read_int_2(fp, samples_out + i) == 0) return -1;
    }
    scf_delta_samples2 ( samples_out, num_samples,0);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_C = samples_out[i];
    }

    for (i = 0; i < num_samples; i++) {
        if (be_read_int_2(fp, samples_out + i) == 0) return -1;
    }
    scf_delta_samples2 ( samples_out, num_samples,0);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_G = samples_out[i];
    }

    for (i = 0; i < num_samples; i++) {
        if (be_read_int_2(fp, samples_out + i) == 0) return -1;
    }
    scf_delta_samples2 ( samples_out, num_samples,0);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_T = samples_out[i];
    }
    return 0;
}

int read_scf_samples31(SeekableBuf *fp, Samples1 *s, size_t num_samples)
{
    size_t i;
    QVarLengthArray<char> arr(num_samples);
    char* samples_out = arr.data();

    /* version to read delta delta data in 1 byte */

    if (fp->read(samples_out, num_samples)) return -1;
    scf_delta_samples1 ( samples_out, num_samples);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_A = samples_out[i];
    }

    if (fp->read(samples_out, num_samples)) return -1;
    scf_delta_samples1 ( samples_out, num_samples);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_C = samples_out[i];
    }

    if (fp->read(samples_out, num_samples)) return -1;
    scf_delta_samples1 ( samples_out, num_samples);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_G = samples_out[i];
    }

    if (fp->read(samples_out, num_samples)) return -1;
    scf_delta_samples1 ( samples_out, num_samples);
    for (i = 0; i < num_samples; i++) {
        (&s[i])->sample_T = samples_out[i];
    }

    return 0;
}

int read_scf_base(SeekableBuf *fp, Bases *b)
{
    uchar buf[12];

    if (!fp->read((char*)buf, 12)) return -1;
    b->peak_index = be_int4(buf);
    b->prob_A = buf[4];
    b->prob_C = buf[5];
    b->prob_G = buf[6];
    b->prob_T = buf[7];
    b->base   = buf[8];
    b->spare[0] = buf[9];
    b->spare[1] = buf[10];
    b->spare[2] = buf[11];

    return 0;
}

int read_scf_bases(SeekableBuf *fp, Bases *b, size_t num_bases) {
    size_t i;

    for (i = 0; i < num_bases; i++) {
        if (-1 == read_scf_base(fp, &(b[i])))
            return -1;
    }

    return 0;
}

int read_scf_bases3(SeekableBuf *fp, Bases *b, size_t num_bases)
{
    size_t i;
    QVarLengthArray<uchar> arr1(num_bases * 8);
    uchar *buf1 = arr1.data();

    for (i = 0; i < num_bases; i++) {
        uint buf4;
        if (be_read_int_4(fp, &buf4) == 0) return -1;
        (&b[i])->peak_index = buf4;
    }

    if (!fp->read((char*)buf1, 8 * num_bases)) return -1;

    for (i=0; i < num_bases; i++) {
        (&b[i])->prob_A   = buf1[i];
        (&b[i])->prob_C   = buf1[i+num_bases];
        (&b[i])->prob_G   = buf1[i+2*num_bases];
        (&b[i])->prob_T   = buf1[i+3*num_bases];
        (&b[i])->base     = buf1[i+4*num_bases];
        (&b[i])->spare[0] = buf1[i+5*num_bases];
        (&b[i])->spare[1] = buf1[i+6*num_bases];
        (&b[i])->spare[2] = buf1[i+7*num_bases];
    }

    return 0;
}

static uchar getMaxProb(uchar probA, uchar probC, uchar probG, uchar probT ) {
    if (probA > probC && probA > probG && probA > probT) {
        return probA;
    } else if (probC > probA && probC > probG && probC > probT) {
        return probC;
    } else if (probG > probA && probG > probC && probG > probT) {
        return probG;
    } else if (probT > probA && probT > probC && probT > probG) {
        return probT;
    } else {
        return 0;
    }
}

Document* SCFFormat::parseSCF(const U2DbiRef& dbiRef, IOAdapter* io, const QVariantMap& fs, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    DNASequence dna;
    DNAChromatogram cd;
    if (!loadSCFObjects(io, dna, cd, os)) {
        return NULL;
    }

    const QString seqObjName = dna.getName().isEmpty() ? "Sequence" : dna.getName() + " sequence";
    const QString chromaObjName = dna.getName().isEmpty() ? "Chromatogram" : dna.getName() + " chromatogram";
    dna.setName(seqObjName);
    QVariantMap hints;
    hints.insert(DBI_FOLDER_HINT, fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
    QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    hints.insert(DBI_FOLDER_HINT, folder);
    U2EntityRef ref = U2SequenceUtils::import(os, dbiRef, folder, dna);
    U2SequenceObject* seqObj = new U2SequenceObject(seqObjName, ref);
    CHECK_OP(os, NULL);
    SAFE_POINT(seqObj != NULL, "DocumentFormatUtils::addSequenceObject returned NULL but didn't set error", NULL);
    seqObj->setQuality(dna.quality);

    DNAChromatogramObject* chromObj = DNAChromatogramObject::createInstance(cd, chromaObjName, dbiRef, os, hints);
    CHECK_OP(os, NULL);

    QList<GObject*> objects;
    objects.append(chromObj);
    objects.append(seqObj);

    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
    chromObj->addObjectRelation(GObjectRelation(GObjectReference(seqObj), ObjectRole_Sequence));
    return doc;
}


/*
* Copyright (c) Medical Research Council 1994. All rights reserved.
*
* Permission to use, copy, modify and distribute this software and its
* documentation for any purpose is hereby granted without fee, provided that
* this copyright and notice appears in all copies.
*
* This file was written by James Bonfield, Simon Dear, Rodger Staden,
* as part of the Staden Package at the MRC Laboratory of Molecular
* Biology, Hills Road, Cambridge, CB2 2QH, United Kingdom.
*
* MRC disclaims all warranties with regard to this software.
*/

/*
Title:       write_scf.c

Purpose:     Output of Standard Chromatogram Format sequences
Last update: August 18 1994

Change log:
4 Feb 1992, Now draft proposal version 2
23 Nov 92,  SCF 2.0 + LaDeana's changes
11 Aug 93, Version 2.01 containing confidence values
18 Aug 1994  Renamed from  writeSCF.c; now purely SCF IO (no Seq structs)

Oct 95 major rewrite to make files more easily compressed.
gzip now gets files to around 40% of original
Version raised to 3.00
* We store in order:
*     Header
*     Samples
*     Bases
*     Comments
*     Private

Two main types of change:
1: write data in lane order instead of all lanes together
eg write Sample values for A, then Sample values for C, etc.

2: where appropriate write delta delta values instead of complete ones.
ie write the differences in the differences between successive values

*/


const static int scf_version = 3;

/* ---- Imports ---- */

// #include <ctype.h>
// #include <stdio.h>    /* IMPORT: fopen, fclose, fseek, ftell, fgetc,
// EOF */
// #include <string.h>
// #include "io_lib/scf.h"      /* IMPORT: scf structures */
// #include "io_lib/mach-io.h"  /* IMPORT: be_write_int_1, be_write_int_2, be_write_int_4 */
// #include "io_lib/xalloc.h"
//
// #include "io_lib/stdio_hack.h"

/* ---- Exports ---- */


int write_scf_header(FILE *fp, Header *h)
{
    int i;

    if (be_write_int_4(fp,&h->magic_number)==false)     return -1;
    if (be_write_int_4(fp,&h->samples)==false)          return -1;
    if (be_write_int_4(fp,&h->samples_offset)==false)   return -1;
    if (be_write_int_4(fp,&h->bases)==false)            return -1;
    if (be_write_int_4(fp,&h->bases_left_clip)==false)  return -1;
    if (be_write_int_4(fp,&h->bases_right_clip)==false) return -1;
    if (be_write_int_4(fp,&h->bases_offset)==false)     return -1;
    if (be_write_int_4(fp,&h->comments_size)==false)    return -1;
    if (be_write_int_4(fp,&h->comments_offset)==false)  return -1;
    if (fwrite(h->version,sizeof(h->version),1,fp)!=1)  return -1;
    if (be_write_int_4(fp,&h->sample_size)==false)      return -1;
    if (be_write_int_4(fp,&h->code_set)==false)         return -1;
    if (be_write_int_4(fp,&h->private_size)==false)     return -1;
    if (be_write_int_4(fp,&h->private_offset)==false)   return -1;
    for (i=0;i<18;i++)
        if (be_write_int_4(fp,&h->spare[i])==false)     return -1;

    return 0;
}


// int write_scf_sample1(FILE *fp, Samples1 *s)
// {
//     uchar buf[4];
//
//     buf[0] = s->sample_A;
//     buf[1] = s->sample_C;
//     buf[2] = s->sample_G;
//     buf[3] = s->sample_T;
//     if (4 != fwrite(buf, 1, 4, fp)) return -1;
//
//     return 0;
// }
//
//
// int write_scf_sample2(FILE *fp, Samples2 *s)
// {
//     ushort buf[4];
//
//     buf[0] = be_int2(reinterpret_cast<uchar*> (&s->sample_A) );
//     buf[1] = be_int2(reinterpret_cast<uchar*> (&s->sample_C) );
//     buf[2] = be_int2(reinterpret_cast<uchar*> (&s->sample_G) );
//     buf[3] = be_int2(reinterpret_cast<uchar*> (&s->sample_T) );
//     if (4 != fwrite(buf, 2, 4, fp)) return -1;
//
//     return 0;
// }
//
//
// int write_scf_samples1(FILE *fp, Samples1 *s, size_t num_samples) {
//     size_t i;
//
//     for (i = 0; i < num_samples; i++) {
//         if (-1 == write_scf_sample1(fp, &(s[i])))
//             return -1;
//     }
//
//     return 0;
// }


// int write_scf_samples2(FILE *fp, Samples2 *s, size_t num_samples) {
//     size_t i;
//
//     for (i = 0; i < num_samples; i++) {
//         if (-1 == write_scf_sample2(fp, &(s[i])))
//             return -1;
//     }
//
//     return 0;
// }


 int write_scf_samples31(FILE *fp, Samples1 *s, size_t num_samples) {
     size_t i;

     if (!num_samples)
         return 0;

     QVarLengthArray<uchar> ar(num_samples);
     uchar *samples_out=ar.data();

     for (i = 0; i < num_samples; i++) {
         samples_out[i] = (&s[i])->sample_A;
     }
     scf_delta_samples1 ( reinterpret_cast<char*> (samples_out), num_samples);
     if (num_samples != fwrite(samples_out, 1, num_samples, fp)) {
         return -1;
     }

     for (i = 0; i < num_samples; i++) {
         samples_out[i] = (&s[i])->sample_C;
     }
     scf_delta_samples1 ( reinterpret_cast<char*>(samples_out), num_samples);
     if (num_samples != fwrite(samples_out, 1, num_samples, fp)) {
         return -1;
     }

     for (i = 0; i < num_samples; i++) {
         samples_out[i] = (&s[i])->sample_G;
     }
     scf_delta_samples1 ( reinterpret_cast<char*>(samples_out), num_samples);
     if (num_samples != fwrite(samples_out, 1, num_samples, fp)) {
         return -1;
     }

     for (i = 0; i < num_samples; i++) {
         samples_out[i] = (&s[i])->sample_T;
     }
     scf_delta_samples1 ( reinterpret_cast<char*>(samples_out), num_samples);
     if (num_samples != fwrite(samples_out, 1, num_samples, fp)) {
         return -1;
     }

     return 0;
 }

int write_scf_samples32(FILE *fp, Samples2 *s, size_t num_samples) {
    size_t i;

    if (!num_samples)
        return 0;


    QVarLengthArray<ushort> ar(num_samples);
    ushort *samples_out = ar.data();

    for (i = 0; i < num_samples; i++) {
        samples_out[i] = (&s[i])->sample_A;
    }
    scf_delta_samples2 ( samples_out, num_samples,1);
    for (i = 0; i < num_samples; i++) {
        samples_out[i] = be_int2(reinterpret_cast<uchar*>(&samples_out[i]));
    }
    if (num_samples != fwrite(samples_out, 2, num_samples, fp)) return -1;


    for (i = 0; i < num_samples; i++) {
        samples_out[i] = (&s[i])->sample_C;
    }
    scf_delta_samples2 ( samples_out, num_samples,1);
    for (i = 0; i < num_samples; i++) {
        samples_out[i] = be_int2(reinterpret_cast<uchar*>(&samples_out[i]));
    }
    if (num_samples != fwrite(samples_out, 2, num_samples, fp)) return -1;


    for (i = 0; i < num_samples; i++) {
        samples_out[i] = (&s[i])->sample_G;
    }
    scf_delta_samples2 ( samples_out, num_samples,1);
    for (i = 0; i < num_samples; i++) {
        samples_out[i] = be_int2(reinterpret_cast<uchar*>(&samples_out[i]));
    }
    if (num_samples != fwrite(samples_out, 2, num_samples, fp)) return -1;


    for (i = 0; i < num_samples; i++) {
        samples_out[i] = (&s[i])->sample_T;
    }
    scf_delta_samples2 ( samples_out, num_samples,1);
    for (i = 0; i < num_samples; i++) {
        samples_out[i] = be_int2(reinterpret_cast<uchar*>(&samples_out[i]));
    }
    if (num_samples != fwrite(samples_out, 2, num_samples, fp)) return -1;

    return 0;
}


int write_scf_base(FILE *fp, Bases *b)
{
    uchar buf[12];

    uchar *bufC;
    uint bufBegin[1];

    *bufBegin = be_int4(reinterpret_cast<uchar*>(&b->peak_index));
    bufC = (uchar*)bufBegin;
    buf[0] = bufC[0];
    buf[1] = bufC[1];
    buf[2] = bufC[2];
    buf[3] = bufC[3];

    buf[4] = b->prob_A;
    buf[5] = b->prob_C;
    buf[6] = b->prob_G;
    buf[7] = b->prob_T;
    buf[8] = b->base;
    buf[9] = b->spare[0];
    buf[10] = b->spare[1];
    buf[11] = b->spare[2];

    if (12 != fwrite(buf, 1, 12, fp)) return -1;

    return 0;
}


int write_scf_bases(FILE *fp, Bases *b, size_t num_bases)
{
    size_t i;

    for (i = 0; i < num_bases; i++) {
        if (-1 == write_scf_base(fp, &(b[i])))
            return -1;
    }

    return 0;
}

int write_scf_bases3(FILE *fp, Bases *b, size_t num_bases)
{
    size_t i;
    QVarLengthArray<uint> ar4(4 * num_bases);
    QVarLengthArray<uchar> ar1(8 * num_bases);

    uint *buf4 = ar4.data();
    uchar *buf1 = ar1.data();

    for (i = 0; i < num_bases; i++) {
        buf4[i] = be_int4(reinterpret_cast<uchar*>( &(&b[i])->peak_index) );
    }
    fwrite(buf4, 4, num_bases, fp);

    for (i=0; i < num_bases; i++) {
        buf1[i            ] = (&b[i])->prob_A;
        buf1[i+  num_bases] = (&b[i])->prob_C;
        buf1[i+2*num_bases] = (&b[i])->prob_G;
        buf1[i+3*num_bases] = (&b[i])->prob_T;
        buf1[i+4*num_bases] = (&b[i])->base;
        buf1[i+5*num_bases] = (&b[i])->spare[0];
        buf1[i+6*num_bases] = (&b[i])->spare[1];
        buf1[i+7*num_bases] = (&b[i])->spare[2];
    }
    if (8 * num_bases != (fwrite(buf1, 1, 8 * num_bases, fp))) {
        return -1;
    }

    return 0;
}


int write_scf_comment(FILE *fp, Comments *c, size_t s)
{
    if (fwrite(c, 1, s, fp) != s) return -1;

    return 0;
}

/*
* All of the above structs in a single scf format.
*/
typedef struct {
    Header header;
    union Samples {
        Samples1 *samples1;
        Samples2 *samples2;
    } samples;
    Bases *bases;
    Comments *comments;
    char *private_data;
} Scf;


/*
* Request which (major) version of scf to use when writing.
* Defaults to the latest. Currently suitable fields are
* 2 and 3.
*
* Returns 0 for success, -1 for failure.
*/
// int set_scf_version(int version) {
//     if (version != 2 && version != 3)
//         return -1;
//
//     scf_version = version;
//     return 0;
// }



inline QString scf_version_float2str(float f) {
    QString result = QString().sprintf("%1.2f", f);
    return result;
}


/*
* Write Seq out as a .scf file to the 'fp' FILE *
*/
 int fwrite_scf(Scf *scf, FILE *fp) {
     uint size;
     int err;

     /*
     * Init header offsets.
     *
     * We store in order:
     *     Header
     *     Samples
     *     Bases
     *     Comments
     *     Private
     */
     scf->header.samples_offset = (uint)sizeof(Header);
     size = scf->header.samples * (scf->header.sample_size == 1 ?
         sizeof(Samples1) : sizeof(Samples2));
     scf->header.bases_offset = (uint)(scf->header.samples_offset +
         size);
     size = scf->header.bases * sizeof(Bases);
     scf->header.comments_offset = (uint)(scf->header.bases_offset + size);

     size = scf->header.comments_size;
     scf->header.private_offset = (uint)(scf->header.comments_offset + size);

     /* Init a few other things, such as the magic number */
     scf->header.magic_number = SCF_MAGIC;

     if (scf_version == 3) {
         memcpy(scf->header.version, scf_version_float2str(SCF_VERSION).toLatin1().constData(), 4);
     } else {
         memcpy(scf->header.version, scf_version_float2str(SCF_VERSION_OLD).toLatin1().constData(), 4);
     }

     /* Write header */
     if (write_scf_header(fp, &scf->header) == -1)
         return -1;

     if (scf_version == 3) {
         /* Write Samples */
         if (scf->header.sample_size == 1)
             err = write_scf_samples31(fp, scf->samples.samples1,
             scf->header.samples);
         else
             err = write_scf_samples32(fp, scf->samples.samples2,
             scf->header.samples);
         if (-1 == err)
             return -1;

         /* Write Bases */
         if (-1 == write_scf_bases3(fp, scf->bases, scf->header.bases))
             return -1;

     }

     /* Write Comments */
     if (-1 == write_scf_comment(fp, scf->comments,
         scf->header.comments_size))
         return -1;

     /* Write private data */
     if (scf->header.private_size) {
         if (scf->header.private_size  != fwrite(scf->private_data, 1,
             scf->header.private_size, fp))
             return -1;
     }

     return 0;
 }


 static void saveChromatogramToSCF(const DNAChromatogram& c, const QByteArray& seq, FILE* fp)
 {
    Scf scf;
    scf.comments = NULL;
    scf.private_data = NULL;

    scf.header.bases = c.seqLength;
    scf.header.samples = c.traceLength;

    scf.header.bases_left_clip = 0;
    scf.header.bases_right_clip = 0;
    scf.header.code_set = 2; // TODO: not sure if correct
    scf.header.comments_size = 0;
    scf.header.comments_offset = 0;
    scf.header.private_size = 0;
    // Fixed precision for saving
    scf.header.sample_size = 2;

    assert(c.seqLength == seq.length());

    QVector<Bases> bases(c.seqLength);
    for (int i = 0; i < c.seqLength; ++i) {
        bases[i].base = seq.at(i);
        bases[i].prob_A = c.prob_A.at(i);
        bases[i].prob_C = c.prob_C.at(i);
        bases[i].prob_G = c.prob_G.at(i);
        bases[i].prob_T = c.prob_T.at(i);
        bases[i].peak_index = c.baseCalls.at(i);
    }

    scf.bases = bases.data();

    QVector<Samples2> samples(c.traceLength);
    for (int i = 0; i < c.traceLength; ++i) {
        samples[i].sample_A = c.A[i];
        samples[i].sample_C = c.C[i];
        samples[i].sample_G = c.G[i];
        samples[i].sample_T = c.T[i];
    }
    scf.samples.samples2 = samples.data();

    fwrite_scf(&scf, fp);


 }


void SCFFormat::exportDocumentToSCF( const QString& fileName, const DNAChromatogram& cd, const QByteArray& seq, U2OpStatus& ts ) {

    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            ts.setError(L10N::errorOpeningFileWrite(fileName));
            return;
        }
        file.close();
    }

    FILE* fp = NULL;
#ifdef Q_OS_WIN
    errno_t err = fopen_s(&fp, qPrintable(fileName), "wb+");
    if (err != 0) {
        ts.setError(L10N::errorOpeningFileWrite(fileName));
        return;
    }
#else
    fp = fopen(qPrintable(fileName), "wb+");
#endif

    if (fp == NULL) {
        ts.setError(L10N::errorOpeningFileWrite(fileName));
        return;
    }

    saveChromatogramToSCF(cd, seq, fp);

    fclose(fp);
}

bool SCFFormat::loadSCFObjects( IOAdapter* io, DNASequence& dna, DNAChromatogram& cd, U2OpStatus& os )
{
    GUrl url = io->getURL();
    QByteArray readBuff;
    QByteArray block(BUFF_SIZE, 0);
    quint64 len = 0;
    while ((len=io->readBlock(block.data(),BUFF_SIZE)) > 0) {
        readBuff.append(QByteArray(block.data(), len));
        if (readBuff.size()>CHECK_MB) {
            os.setError(L10N::errorFileTooLarge(url));
            break;
        }
    }

    CHECK_OP(os, false);

    SeekableBuf sf;
    sf.head = readBuff.constData();
    sf.pos = 0;
    sf.size = readBuff.size();

    Header h;
    float scf_version;
    int sections = READ_ALL;

    SeekableBuf* fp = &sf;

    /* Read header */
    if (read_scf_header(fp, &h) == -1) {
        return false;
    }

    /* fake things for older style SCF -- SD */
    if (h.sample_size != 1 && h.sample_size != 2) h.sample_size = 1;

    QString ver(h.version);
    ver.chop(4);
    scf_version = ver.toFloat();

    /* Allocate memory */
    QByteArray sequence(h.bases, 0);
    QByteArray qualVals(h.bases, 0);

    if (sections & READ_SAMPLES) {
        /* Read samples */
        if (SeekBuf(fp, h.samples_offset, 0 /* SEEK_SET */) != 0) {
            return false;
        }

        cd.A.resize(h.samples);
        cd.C.resize(h.samples);
        cd.G.resize(h.samples);
        cd.T.resize(h.samples);
        cd.traceLength = h.samples;
        int err;

        if (h.sample_size == 1) {
            QVector<Samples1> samples(h.samples);
            if ( 2.9 > scf_version ) {
                err= read_scf_samples1(fp, samples.data(), h.samples);
            } else {
                err= read_scf_samples31(fp, samples.data(), h.samples);
            }
            if (-1 == err) {
                return false;
            }
            for (uint i = 0; i < h.samples; i++) {
                cd.A[i] = samples[i].sample_A;
                cd.C[i] = samples[i].sample_C;
                cd.G[i] = samples[i].sample_G;
                cd.T[i] = samples[i].sample_T;
            }
        }
        else {
            QVector<Samples2> samples(h.samples);
            if (2.9 > scf_version ) {
                err= read_scf_samples2(fp, samples.data(), h.samples);
            } else {
                err= read_scf_samples32(fp, samples.data(), h.samples);
            }
            if (-1 == err) {
                return false;
            }
            for (uint i = 0; i < h.samples; i++) {
                cd.A[i] = samples[i].sample_A;
                cd.C[i] = samples[i].sample_C;
                cd.G[i] = samples[i].sample_G;
                cd.T[i] = samples[i].sample_T;
            }
        }
    }

    if (sections & READ_BASES) {

        /* Read bases */
        if (SeekBuf(fp, h.bases_offset, 0 /* SEEK_SET */) != 0) {
            return false;
        }

        QVector<Bases> bases(h.bases);

        if ( 2.9 > scf_version ) {
            if (-1 == read_scf_bases(fp, bases.data(), h.bases)) {
                return false;
            }
        }
        else {
            if (-1 == read_scf_bases3(fp, bases.data(), h.bases)) {
                return false;
            }
        }

        cd.seqLength = h.bases;
        cd.baseCalls.resize(h.bases);
        cd.prob_A.resize(h.bases);
        cd.prob_C.resize(h.bases);
        cd.prob_G.resize(h.bases);
        cd.prob_T.resize(h.bases);

        for (uint i = 0; i<h.bases; i++) {
            cd.prob_A[i] = bases[i].prob_A;
            cd.prob_C[i] = bases[i].prob_C;
            cd.prob_G[i] = bases[i].prob_G;
            cd.prob_T[i] = bases[i].prob_T;
            cd.baseCalls[i] = bases[i].peak_index;
            sequence[i] = bases[i].base;
            uchar maxProb = getMaxProb(cd.prob_A[i], cd.prob_C[i],cd.prob_G[i],cd.prob_T[i]);
            qualVals[i] = DNAQuality::encode( maxProb, DNAQualityType_Sanger );

        }
    }

    QString comments;
    if (sections & READ_COMMENTS) {
        /* Try reading comments */
        if (SeekBuf(fp,(h.comments_offset), 0) == 0) {
            QByteArray arr(h.comments_size, 0);
            if (fp->read(arr.data(), h.comments_size) ) {
                comments.append(arr);
            }
        }
    }

    cd.hasQV = true;

    QString sampleName;
    QStringList vals = comments.split("\n");
    // detect sample name per http://www.ncbi.nlm.nih.gov/Traces/trace.cgi?cmd=show&f=formats&m=doc&s=format
    foreach(const QString& val, vals) {
        if (val.startsWith("NAME=")) {
            sampleName = val.mid(5);
            break;
        }
    }
    if (sampleName.isEmpty()) {
        sampleName = url.baseFileName();
    }

    cd.name = sampleName + " chromatogram";

    dna.setName(sampleName);
    dna.seq = sequence;
    dna.info.insert(DNAInfo::COMMENT, vals);
    dna.quality.qualCodes = qualVals;

    return true;
}

DNASequence* SCFFormat::loadSequence( IOAdapter *io, U2OpStatus &os )
{
    if (io->isEof()) {
        return NULL;
    }

    DNASequence* seq = new DNASequence();
    DNAChromatogram cd;

    if (!loadSCFObjects(io, (*seq), cd, os)) {
        os.setError(tr("Failed to load sequence from SCF file %1").arg(io->toString()));
    }

    return seq;

}

}//namespace
