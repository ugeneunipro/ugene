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

#include <time.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNAInfo.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ABIFormat.h"
#include "DocumentFormatUtils.h"
#include "IOLibUtils.h"

/* TRANSLATOR U2::ABIFormat */

namespace U2 {

ABIFormat::ABIFormat(QObject* p) : DocumentFormat(p, BaseDocumentFormats::ABIF, DocumentFormatFlag_SupportStreaming, QStringList() << "ab1" << "abi" << "abif")
{
    formatName = tr("ABIF");
    formatDescription = tr("A chromatogram file format");
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    supportedObjectTypes+=GObjectTypes::CHROMATOGRAM;
}

FormatCheckResult ABIFormat::checkRawData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();

    if (size <= 4 || data[0]!='A' || data[1]!='B' || data[2]!='I' || data[3]!='F') {
        /*
        * Maybe we've got a file in MacBinary format in which case
        * we'll have an extra offset 128 bytes to add.
        */
        data += 128;
        size -= 128;
        if (size <= 4 || data[0]!='A' || data[1]!='B' || data[2]!='I' || data[3]!='F') {
            return FormatDetection_NotMatched;
        }
    }
    bool hasBinaryBlocks = TextUtils::contains(TextUtils::BINARY, data, size);
    return hasBinaryBlocks ? FormatDetection_Matched : FormatDetection_NotMatched;
}

Document* ABIFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QByteArray readBuff;
    QByteArray block(BUFF_SIZE, 0);
    quint64 len = 0;
    while ((len=io->readBlock(block.data(),BUFF_SIZE)) > 0) {
        readBuff.append(QByteArray(block.data(), len));
        CHECK_EXT(readBuff.size() <= CHECK_MB, os.setError(L10N::errorFileTooLarge(io->getURL())), NULL);
    }

    SeekableBuf sf;
    sf.head = readBuff.constData();
    sf.pos = 0;
    sf.size = readBuff.size();
    Document* doc = parseABI(dbiRef, &sf, io, fs, os);
    CHECK_OP(os, NULL)
    CHECK_EXT(doc != NULL, os.setError(tr("Not a valid ABIF file: %1").arg(io->toString())), NULL);
    return doc;
}

DNASequence *ABIFormat::loadSequence(IOAdapter *io, U2OpStatus &os)
{
    if (io->isEof()) {
        return NULL;
    }

    CHECK_EXT((io != NULL) && (io->isOpen() == true), os.setError(L10N::badArgument("IO adapter")), NULL);
    QByteArray readBuff;
    QByteArray block(BUFF_SIZE, 0);
    quint64 len = 0;
    while ((len=io->readBlock(block.data(),BUFF_SIZE)) > 0) {
        readBuff.append(QByteArray(block.data(), len));
        CHECK_EXT(readBuff.size() <= CHECK_MB, os.setError(L10N::errorFileTooLarge(io->getURL())), NULL);
    }

    SeekableBuf sf;
    sf.head = readBuff.constData();
    sf.pos = 0;
    sf.size = readBuff.size();

    DNASequence* seq = new DNASequence();
    DNAChromatogram cd;

    if (!loadABIObjects(&sf, (*seq), cd)) {
        os.setError(tr("Failed to load sequence from ABI file %1").arg(io->toString()));
    }

    return seq;

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
* Title:    seqIOABI
*
* File:     seqIOABI.c
* Purpose:  Reading (not writing) of ABI sequences
* Last update: Fri Sep 02, 1994
*/

/*
* The ABI magic number - "ABIF"
*/
#define ABI_MAGIC    ((int) ((((('A'<<8)+'B')<<8)+'I')<<8)+'F')

/*
* The index is located towards the end of the ABI trace file.
* It's location is given by a longword at a fixed place.
*/
#define IndexPO 26

#define IndexEntryLength 28

/*
* Here are some labels we will be looking for, four chars packed
* into an int_4
*/
#define LABEL(a) ((int) ((((((a)[0]<<8)+(a)[1])<<8)+(a)[2])<<8)+(a)[3])
#define DataEntryLabel    LABEL("DATA")
#define BaseEntryLabel    LABEL("PBAS")
#define BasePosEntryLabel LABEL("PLOC")
#define SpacingEntryLabel LABEL("SPAC")
#define SignalEntryLabel  LABEL("S/N%")
#define FWO_Label         LABEL("FWO_")
#define MCHNLabel         LABEL("MCHN")
#define PDMFLabel         LABEL("PDMF")
#define SMPLLabel         LABEL("SMPL")
#define PPOSLabel         LABEL("PPOS")
#define CMNTLabel         LABEL("CMNT")
#define GelNameLabel      LABEL("GELN")
#define LANELabel         LABEL("LANE")
#define RUNDLabel         LABEL("RUND")
#define RUNTLabel         LABEL("RUNT")
#define MTXFLabel         LABEL("MTXF")
#define SPACLabel         LABEL("SPAC")
#define SVERLabel         LABEL("SVER")
#define MODLLabel         LABEL("MODL")
#define BaseConfLabel     LABEL("PCON")

#define baseIndex(B) ((B)=='C'?0:(B)=='A'?1:(B)=='G'?2:3)

/*
* Gets the offset of the ABI index.
* Returns -1 for failure, 0 for success.
*/
static int getABIIndexOffset(SeekableBuf* fp, uint *indexO) {
    uint magic = 0;

    /*
    * Initialise header_fudge.
    *
    * This is usually zero, but maybe we've transfered a file in MacBinary
    * format in which case we'll have an extra 128 bytes to add to all
    * our fseeks.
    */
    be_read_int_4(fp, &magic);
    if (magic != ABI_MAGIC) {
        fp->head += 128;
        fp->size -= 128;
    }

    if ((SeekBuf(fp, IndexPO, 0) != 0) || (!be_read_int_4(fp, indexO)))
        return -1;
    else
        return 0;
}

/*
* From the ABI results file connected to `fp' whose index starts
* at byte offset `indexO', return in `val' the `lw'th long word
* from the `count'th entry labelled `label'.
* The result is 0 for failure, or index offset for success.
*/
int getABIIndexEntryLW(SeekableBuf* fp, int indexO, uint label, uint count, int lw, uint *val) {
    int entryNum=-1;
    int i;
    uint entryLabel, entryLw1;

    do {
        entryNum++;

        if (SeekBuf(fp, indexO+(entryNum*IndexEntryLength), 0) != 0)
            return 0;

        if (!be_read_int_4(fp, &entryLabel))
            return 0;

        if (!be_read_int_4(fp, &entryLw1))
            return 0;
    } while (!(entryLabel == label && entryLw1 == count));

    for(i=2; i<=lw; i++) {
        if (!be_read_int_4(fp, val))
            return 0;
    }

    return indexO+(entryNum*IndexEntryLength);
}

/*
* From the ABI results file connected to `fp' whose index starts
* at byte offset `indexO', return in `val' the `sw'th short word
* from the `count'th entry labelled `label'.
* The result is 0 for failure, or index offset for success.
*/
int getABIIndexEntrySW(SeekableBuf* fp, int indexO, uint label, uint count, int sw, ushort *val) {
    int entryNum=-1;
    int i;
    uint entryLabel, entryLw1;

    do {
        entryNum++;

        if (SeekBuf(fp, indexO+(entryNum*IndexEntryLength), 0) != 0)
            return 0;

        if (!be_read_int_4(fp, &entryLabel))
            return 0;

        if (!be_read_int_4(fp, &entryLw1))
            return 0;
    } while (!(entryLabel == label && entryLw1 == count));

    for(i=4; i<=sw; i++) {
        if (!be_read_int_2(fp, val))
            return 0;
    }

    return indexO+(entryNum*IndexEntryLength);
}

/*
* Get an "ABI String". These strings are either pointed to by the index
* offset, or held in the offset itself when the string is <= 4 characters.
* The "type" of the index entry is either 0x12 (a pascal string in which
* case the first byte of the string determines its length) or a 0x02 (a
* C-style string with length coming from the abi index).
*
* "string" will be max 256 bytes for the pascal string, but is of unknown
* (and hence potentially buggy) length for C-strings. For now we live with
* it as this entire file needs rewriting from scratch anyway.
*
* Returns -1 for failure, string length for success.
*/
int getABIString(SeekableBuf *fp, int indexO, uint label, uint count, char *string) {
    uint off;
    uint len;
    quint16 type;

    off = getABIIndexEntrySW(fp, indexO, label, count, 4, &type);
    if (!off)
        return -1;

    if ((off = getABIIndexEntryLW(fp, indexO, label, count, 4, &len))) {
        uchar len2 = 0;

        if (!len)
            return 0;

        /* Determine offset */
        if (len <= 4)
            off += 20;
        else
            getABIIndexEntryLW(fp, indexO, label, count, 5, &off);

        /* Read length byte */
        if (type == 0x12) {
            SeekBuf(fp, off, 0);
            be_read_int_1(fp, &len2);
        } else {
            len2 = len;
        }

        /* Read data (max 255 bytes) */
        fp->read(string, len2);
        string[len2] = 0;

        return len2;
    } else {
        return -1;
    }
}

/*
* Get an "ABI Int_1". This is raw 1-byte integer data pointed to by the
* offset, or held in the offset itself when the data is <= 4 characters.
*
* If indexO is 0 then we do not search for (or indeed use) label and count,
* but simply assume that we are already at the correct offset and read from
* here. (NB: This negates the length <= 4 check.)
*
* Returns -1 for failure, length desired for success (it'll only fill out
* up to max_data_len elements, but it gives an indication of whether there
* was more to come).
*/
int getABIint1(SeekableBuf *fp, int indexO, uint label, uint count, uchar *data, int max_data_len) {
    uint off;
    uint len, len2;

    if (indexO) {
        if (!(off = getABIIndexEntryLW(fp, indexO, label, count, 4, &len)))
            return -1;

        if (!len)
            return 0;

        /* Determine offset */
        if (len <= 4)
            off += 20;
        else
            getABIIndexEntryLW(fp, indexO, label, count, 5, &off);

        len2 = qMin((uint)max_data_len, len);

        SeekBuf(fp, off, 0);
    } else {
        len = len2 = max_data_len;
    }

    fp->read((char*)data, len2);

    return len;
}

/*
* Get an "ABI Int_2". This is raw 2-byte integer data pointed to by the
* offset, or held in the offset itself when the data is <= 4 characters.
*
* Returns -1 for failure, length desired for success (it'll only fill out
* up to max_data_len elements, but it gives an indication of whether there
* was more to come).
*/
int getABIint2(SeekableBuf *fp, int indexO, uint label, uint count, ushort *data, int max_data_len) {
    int len, l2;
    int i;

    len = getABIint1(fp, indexO, label, count, (uchar *)data, max_data_len*2);
    if (-1 == len)
        return -1;

    len /= 2;
    l2 = qMin(len, max_data_len);
    for (i = 0; i < l2; i++) {
        data[i] = be_int2((uchar*)(data+i));
    }

    return len;
}

/*
* Get an "ABI Int_4". This is raw 4-byte integer data pointed to by the
* offset, or held in the offset itself when the data is <= 4 characters.
*
* Returns -1 for failure, length desired for success (it'll only fill out
* up to max_data_len elements, but it gives an indication of whether there
* was more to come).
*/
int getABIint4(SeekableBuf *fp, int indexO, uint label, uint count, uint *data, int max_data_len) {
    int len, l2;
    int i;

    len = getABIint1(fp, indexO, label, count, (uchar *)data, max_data_len*4);
    if (-1 == len)
        return -1;

    len /= 4;
    l2 = qMin(len, max_data_len);
    for (i = 0; i < l2; i++) {
        data[i] = be_int4((uchar*)(data+i));
    }

    return len;
}

static void replace_nl(char *string) {
    char *cp;

    for (cp = string; *cp; cp++) {
        if (*cp == '\n') *cp = ' ';
    }
}

Document* ABIFormat::parseABI(const U2DbiRef& dbiRef, SeekableBuf* fp, IOAdapter* io, const QVariantMap& fs, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);
    DNASequence dna;
    DNAChromatogram cd;

    if (!loadABIObjects(fp, dna, cd)) {
        return NULL;
    }
    if (dna.getName().isEmpty()) {
        dna.setName("Sequence");
    }

    QList<GObject*> objects;
    QVariantMap hints;
    QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    hints.insert(DBI_FOLDER_HINT, folder);
    if (dna.alphabet == NULL) {
        dna.alphabet = U2AlphabetUtils::findBestAlphabet(dna.seq);
        CHECK_EXT(dna.alphabet != NULL, os.setError(tr("Undefined sequence alphabet")), NULL);
    }
    U2EntityRef ref = U2SequenceUtils::import(os, dbiRef, folder, dna, dna.alphabet->getId());
    CHECK_OP(os, NULL);
    U2SequenceObject* seqObj = new U2SequenceObject(dna.getName(), ref);
    objects.append(seqObj);

    DNAChromatogramObject* chromObj = DNAChromatogramObject::createInstance(cd, "Chromatogram", dbiRef, os, hints);
    CHECK_OP(os, NULL);
    objects.append(chromObj);

    QString seqComment = dna.info.value(DNAInfo::COMMENT).toStringList().join("\n");
    TextObject *textObj = TextObject::createInstance(seqComment, "Info", dbiRef, os, hints);
    CHECK_OP(os, NULL);
    objects.append(textObj);

    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
    chromObj->addObjectRelation(GObjectRelation(GObjectReference(seqObj), ObjectRole_Sequence));
    return doc;
}

bool ABIFormat::loadABIObjects(SeekableBuf* fp, DNASequence &dna, DNAChromatogram &cd)
{
    uint numPoints, numBases;
    uint signalO;
    int no_bases = 0;
    int sections = READ_ALL;

    uint fwo_;     /* base -> lane mapping */
    uint indexO;   /* File offset where the index is */
    uint baseO;    /* File offset where the bases are stored */
    uint dataCO;   /* File offset where the C trace is stored */
    uint dataAO;   /* File offset where the A trace is stored */
    uint dataGO;   /* File offset where the G trace is stored */
    uint dataTO;   /* File offset where the T trace is stored */
    uint offset;   /* Generic offset */
    uint offset2;  /* Generic offset */
    uint offset3;  /* Generic offset */
    uint offset4;  /* Generic offset */

    /* DATA block numbers for traces, in order of FWO_ */
    int DataCount[4] = {9, 10, 11, 12};

    QString sequenceName;
    QString sequenceComment;
    QByteArray sequence;
    DNAQuality quality;

    /* Get the index offset */
    if (-1 == getABIIndexOffset(fp, &indexO))
        return false;

    /* Get the number of points */
    if (!getABIIndexEntryLW(fp,indexO,DataEntryLabel,DataCount[0], 3,&numPoints))
        return false;

    /* Get the number of bases */
    if (!getABIIndexEntryLW(fp,indexO,BaseEntryLabel,1,3,&numBases)) {
        no_bases = 1;
        numBases = 0;
    }


    /* Allocate the sequence */
    /* Allocate space for the bases - 1 extra for the ->base field so
    * that we can treat it as a NULL terminated string.
    */
    if (sections & READ_BASES) {
        cd.prob_A.resize(numBases+1);
        cd.prob_C.resize(numBases+1);
        cd.prob_G.resize(numBases+1);
        cd.prob_T.resize(numBases+1);
        cd.baseCalls.resize(numBases+1);
    }

    if (sections & READ_SAMPLES) {
        cd.A.resize(numPoints+1);
        cd.C.resize(numPoints+1);
        cd.G.resize(numPoints+1);
        cd.T.resize(numPoints+1);
    }

    /* Get the Filter Wheel Order (FWO_) field ... */
    if (!getABIIndexEntryLW(fp,indexO,FWO_Label,1,5,&fwo_)) {
        /* Guess at CAGT */
        fwo_ = 0x43414754;
    }

    /*
    * The order of the DATA fields is determined by the field FWO_
    * Juggle around with data pointers to get it right
    */
    if (sections & READ_SAMPLES) {
        uint *dataxO[4];

        dataxO[0] = &dataCO;
        dataxO[1] = &dataAO;
        dataxO[2] = &dataGO;
        dataxO[3] = &dataTO;

        /*Get the positions of the four traces */
        if (!(getABIIndexEntryLW(fp, indexO, DataEntryLabel,
            DataCount[0], 5,
            dataxO[baseIndex((char)(fwo_>>24&255))]) &&
            getABIIndexEntryLW(fp, indexO, DataEntryLabel,
            DataCount[1], 5,
            dataxO[baseIndex((char)(fwo_>>16&255))]) &&
            getABIIndexEntryLW(fp, indexO, DataEntryLabel,
            DataCount[2], 5,
            dataxO[baseIndex((char)(fwo_>>8&255))]) &&
            getABIIndexEntryLW(fp, indexO, DataEntryLabel,
            DataCount[3], 5,
            dataxO[baseIndex((char)(fwo_&255))]))) {
                return false;
        }
    }


    /*************************************************************
    * Read the traces and bases information
    *************************************************************/

    if (sections & READ_SAMPLES) {
        /* Read in the C trace */
        if (SeekBuf(fp, dataCO, 0) == -1) return false;
        getABIint2(fp, 0, 0, 0, cd.C.data(), numPoints);

        /* Read in the A trace */
        if (SeekBuf(fp, dataAO, 0) == -1) return false;
        getABIint2(fp, 0, 0, 0, cd.A.data(), numPoints);

        /* Read in the G trace */
        if (SeekBuf(fp, dataGO, 0) == -1) return false;
        getABIint2(fp, 0, 0, 0, cd.G.data(), numPoints);

        /* Read in the T trace */
        if (SeekBuf(fp, dataTO, 0) == -1) return false;
        getABIint2(fp, 0, 0, 0, cd.T.data(), numPoints);

        /* Compute highest trace peak */
        /*for (i=0; i < numPoints; i++) {
            if (read->maxTraceVal < read->traceA[i])
                read->maxTraceVal = read->traceA[i];
            if (read->maxTraceVal < read->traceC[i])
                read->maxTraceVal = read->traceC[i];
            if (read->maxTraceVal < read->traceG[i])
                read->maxTraceVal = read->traceG[i];
            if (read->maxTraceVal < read->traceT[i])
                read->maxTraceVal = read->traceT[i];
        }*/
    }

    if (no_bases || !(sections & READ_BASES)) {
        goto skip_bases;
    }

    /* Read in base confidence values */
    {
    QVector<uchar> conf(numBases);
    int res = getABIint1(fp, indexO, BaseConfLabel, 2, conf.data(), numBases);

    /* Read in the bases */
    if (!(getABIIndexEntryLW(fp, indexO, BaseEntryLabel, 1, 5, &baseO) && (SeekBuf(fp, baseO, 0) == 0) )) {
        return false;
    }

    sequence = QByteArray(numBases, 0);
    if (!fp->read(sequence.data(), numBases)) {
        return false;
    }

    QByteArray qualCodes(numBases, 0);

    if (res != -1 ) {
        for (uint i = 0; i < numBases; i++) {
            qualCodes[i] = DNAQuality::encode(conf[i],DNAQualityType_Sanger);
            switch(sequence[i]) {
            case 'A':
            case 'a':
                cd.prob_A[i] = conf[i];
                cd.prob_C[i] = 0;
                cd.prob_G[i] = 0;
                cd.prob_T[i] = 0;
                break;

            case 'C':
            case 'c':
                cd.prob_A[i] = 0;
                cd.prob_C[i] = conf[i];
                cd.prob_G[i] = 0;
                cd.prob_T[i] = 0;
                break;

            case 'G':
            case 'g':
                cd.prob_A[i] = 0;
                cd.prob_C[i] = 0;
                cd.prob_G[i] = conf[i];
                cd.prob_T[i] = 0;
                break;

            case 'T':
            case 't':
                cd.prob_A[i] = 0;
                cd.prob_C[i] = 0;
                cd.prob_G[i] = 0;
                cd.prob_T[i] = conf[i];
                break;

            default:
                cd.prob_A[i] = 0;
                cd.prob_C[i] = 0;
                cd.prob_G[i] = 0;
                cd.prob_T[i] = 0;
                break;
            }
        }
    }

    quality.setQualCodes(qualCodes);

    }

    /* Read in the base positions */
    if (-1 == getABIint2(fp, indexO, BasePosEntryLabel, 1, cd.baseCalls.data(), numBases)) {
        return false;
    }

    /*
    * Check for corrupted traces where the bases are positioned on sample
    * coordinates which do not exist. Witnessed on some MegaBACE files.
    */
    if (cd.baseCalls[numBases-1] > numPoints) {
        int n = cd.baseCalls[numBases-1]+1;
        cd.A.resize(n);
        cd.C.resize(n);
        cd.G.resize(n);
        cd.T.resize(n);
        numPoints = n;
    }

skip_bases:
    /*************************************************************
    * Gather useful information - the comments field
    *************************************************************/
    if (sections & READ_COMMENTS) {
        char buffer[257];
        char commstr[256];
        int spacing;
        ushort i2;
        uint i4;

        /* The ABI comments */
        int clen = getABIString(fp, indexO, CMNTLabel, 1, commstr);
        if (clen != -1) {
            commstr[clen] = 0;
            char *commstrp = commstr;
            char *p;
            do {
                if ((p = strchr(commstrp, '\n'))){
                    *p++ = 0;
                }
                sequenceComment.append(QString("ABI Comment: %1\n").arg(commstrp));
            } while((commstrp = p));
        }


        /* Get Sample Name Offset */
        if (-1 != getABIString(fp, indexO, SMPLLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("Sample: %1\n").arg(buffer));
            sequenceName.append(buffer);
        }

        /* LANE */
        if (-1 != getABIint2(fp, indexO, LANELabel, 1, &i2, 1)) {
            sequenceComment.append(QString("LANE=%1\n").arg(i2));
        }

        /* Get Signal Strength Offset */
        if (getABIIndexEntryLW(fp, indexO, SignalEntryLabel, 1, 5,
            &signalO)) {
                short C,A,G,T;
                short *base[4];
                base[0] = &C;
                base[1] = &A;
                base[2] = &G;
                base[3] = &T;

                if (SeekBuf(fp, signalO, 0) != -1 &&
                    be_read_int_2(fp, (ushort *)
                    base[baseIndex((char)(fwo_>>24&255))]) &&
                    be_read_int_2(fp, (ushort *)
                    base[baseIndex((char)(fwo_>>16&255))]) &&
                    be_read_int_2(fp, (ushort *)
                    base[baseIndex((char)(fwo_>>8&255))]) &&
                    be_read_int_2(fp, (ushort *)
                    base[baseIndex((char)(fwo_&255))]))
                {
                    sequenceComment.append(QString("SIGN=A=%1,C=%2,G=%3,T=%4\n").arg(A).arg(C).arg(G).arg(T));
                }
        }

        /* Get the spacing.. it's a float but don't worry yet */
        float fspacing = 0;
        if (-1 != getABIint4(fp, indexO, SpacingEntryLabel, 1, (uint *)&spacing, 1)) {
            fspacing = int_to_float(spacing);
            sequenceComment.append(QString("SPAC=%1\n").arg(fspacing)); //-6.2f",
        }
        /* Correction for when spacing is negative. Why does this happen? */
        if (fspacing <= 0) {
            if (numBases > 1) {
                if (sections & READ_BASES)
                    fspacing = (float)(cd.baseCalls[numBases-1] - cd.baseCalls[0]) / (float) (numBases-1);
                else
                    fspacing = (float) numPoints / (float) numBases;
            } else {
                fspacing = 1;
            }
        }


        /* Get primer position */
        if (getABIIndexEntryLW(fp, indexO, PPOSLabel, 1, 5, (uint *)&i4)) {
                /* ppos stores in MBShort of pointer */
            sequenceComment.append(QString("PRIM=%1\n").arg(i4>>16));
        }

        /* RUND/RUNT */
        if (getABIIndexEntryLW(fp, indexO, RUNDLabel, 1, 5, &offset) &&
            getABIIndexEntryLW(fp, indexO, RUNDLabel, 2, 5, &offset2) &&
            getABIIndexEntryLW(fp, indexO, RUNTLabel, 1, 5, &offset3) &&
            getABIIndexEntryLW(fp, indexO, RUNTLabel, 2, 5, &offset4)) {
                //char buffer[1025];
                char buffer_s[1025];
                char buffer_e[1025];
                struct tm t;
                uint rund_s, rund_e, runt_s, runt_e;

                rund_s = offset;
                rund_e = offset2;
                runt_s = offset3;
                runt_e = offset4;

//                 sprintf(buffer, "%04d%02d%02d.%02d%02d%02d - %04d%02d%02d.%02d%02d%02d",
//                     rund_s >> 16, (rund_s >> 8) & 0xff, rund_s & 0xff,
//                     runt_s >> 24, (runt_s >> 16) & 0xff, (runt_s >> 8) & 0xff,
//                     rund_e >> 16, (rund_e >> 8) & 0xff, rund_e & 0xff,
//                     runt_e >> 24, (runt_e >> 16) & 0xff, (runt_e >> 8) & 0xff);
                QString buffer = QString("%1%2%3.%4%5%6 - %7%8%9.%10%11%12")
                    .arg((rund_s >> 16), 4,10,QLatin1Char('0')).arg((rund_s >> 8) & 0xff, 2,10,QLatin1Char('0'))
                    .arg((rund_s & 0xff), 2,10,QLatin1Char('0')).arg(runt_s >> 24, 2,10,QLatin1Char('0'))
                    .arg((runt_s >> 16) & 0xff, 2,10,QLatin1Char('0')).arg((runt_s >> 8) & 0xff, 2,10,QLatin1Char('0'))
                    .arg(rund_e >> 16, 4,10,QLatin1Char('0')).arg((rund_e >> 8) & 0xff, 2,10,QLatin1Char('0'))
                    .arg(rund_e & 0xff, 2,10,QLatin1Char('0')).arg(runt_e >> 24, 2,10,QLatin1Char('0'))
                    .arg((runt_e >> 16) & 0xff, 2,10,QLatin1Char('0')).arg((runt_e >> 8) & 0xff, 2,10,QLatin1Char('0'));

                memset(&t, 0, sizeof(t));
                t.tm_mday = rund_s & 0xff;
                t.tm_mon = ((rund_s >> 8) & 0xff) - 1;
                t.tm_year = (rund_s >> 16) - 1900;
                t.tm_hour = runt_s >> 24;
                t.tm_min = (runt_s >> 16) & 0xff;
                t.tm_sec = (runt_s >> 8) & 0xff;
                t.tm_isdst = -1;
                /*
                * Convert struct tm to time_t. We ignore the time_t value, but
                * the conversion process will update the tm_wday element of
                * struct tm.
                */
                mktime(&t);
                strftime(buffer_s, 1024, "%a %d %b %H:%M:%S %Y", &t);

                t.tm_mday = rund_e & 0xff;
                t.tm_mon = ((rund_e >> 8) & 0xff) - 1;
                t.tm_year = (rund_e >> 16) - 1900;
                t.tm_hour = runt_e >> 24;
                t.tm_min = (runt_e >> 16) & 0xff;
                t.tm_sec = (runt_e >> 8) & 0xff;
                t.tm_isdst = -1;
                /*
                * Convert struct tm to time_t. We ignore the time_t value, but
                * the conversion process will update the tm_wday element of
                * struct tm.
                */
                mktime(&t);
                strftime(buffer_e, 1024, "%a %d %b %H:%M:%S %Y", &t);

                sequenceComment.append(QString("DATE=%1 to %2\nRUND=%3\n").arg(buffer_s).arg(buffer_e).arg(buffer));
        }


        /* Get Dye Primer Offset */
        if (-1 != getABIString(fp, indexO, PDMFLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("DYEP=%1\n").arg(buffer));
        }

        /* Get Machine Name Offset */
        if (-1 != getABIString(fp, indexO, MCHNLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("MACH=%1\n").arg(buffer));
        }

        /* Machine model */
        if (-1 != getABIString(fp, indexO, MODLLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("MODL=%1\n").arg(buffer));
        }

        /* Matrix file */
        if (-1 != getABIString(fp, indexO, MTXFLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("MTXF=%1\n").arg(buffer));
        }

        /* Base calling version */
        if (-1 != getABIString(fp, indexO, SPACLabel, 2, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("BCAL=%1\n").arg(buffer));
        }

        /* Software versions */
        if (-1 != getABIString(fp, indexO, SVERLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("VER1=%1\n").arg(buffer));
        }
        if (-1 != getABIString(fp, indexO, SVERLabel, 2, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("VER2=%1\n").arg(buffer));
        }

        /* Get Gel Name Offset */
        if (-1 != getABIString(fp, indexO, GelNameLabel, 1, buffer)) {
            replace_nl(buffer);
            sequenceComment.append(QString("GELN=%1\n").arg(buffer));
        }
    }

    /* SUCCESS */
    cd.name = sequenceName.isEmpty() ? "chromatogram" : sequenceName + " chromatogram";
    cd.hasQV = true;
    cd.seqLength = sequence.size();
    assert(sequence.size() == int(numBases));
    cd.traceLength = numPoints;
    assert(cd.A.size() == int(numPoints + 1));

    dna.setName(sequenceName);
    dna.seq = sequence;
    dna.info.insert(DNAInfo::COMMENT, sequenceComment.split("\n"));
    dna.quality = quality;

    return true;

}

}//namespace
