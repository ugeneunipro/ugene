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

#include <U2Core/Timer.h>
#include <U2Core/Counter.h>
#include <U2Algorithm/BinaryFindOpenCL.h>
#include <U2Algorithm/SyncSort.h>
#include <QFile>
#include <QtEndian>
#include "GenomeAlignerFindTask.h"
#include "GenomeAlignerTask.h"
#include "GenomeAlignerIndex.h"

namespace U2 {

const QString GenomeAlignerIndex::HEADER_EXTENSION("idx");
const QString GenomeAlignerIndex::SARRAY_EXTENSION("sarr");
const QString GenomeAlignerIndex::REF_INDEX_EXTENSION("ref");
const QString GenomeAlignerIndex::HEADER("#UGENE suffix array index\n");
const QString GenomeAlignerIndex::PARAMETERS("#file \"%1\", sequence's length = %2, w = %3\n");
const int GenomeAlignerIndex::charsInMask = MAX_BIT_MASK_LENGTH;
const int GenomeAlignerIndex::overlapSize = 10000;
const QString COMMENT1("#sequence length, window size, bitChar's length, seqPartSize, count of sequences\n");
const QString COMMENT2("#reference name\n");
const QString COMMENT3("#offsets of sequences in the reference\n");
const QString COMMENT4("#seqStart, seqLength, saStart, saLength\n");

GenomeAlignerIndex::GenomeAlignerIndex()
: memIdx(NULL), memBM(NULL), objLens(NULL)
{
    bitTable = bt.getBitMaskCharBits(DNAAlphabet_NUCL);
    bitCharLen = bt.getBitMaskCharBitsNum(DNAAlphabet_NUCL);
    seqLength = 0;
    w = 0;
    partsInMemCache = 0;
    objCount = 0;
    currentPart = -1;
    build = true;
    seqPartSize = -1;
}

GenomeAlignerIndex::~GenomeAlignerIndex() {
    delete[] memIdx;
    delete[] memBM;
    delete[] objLens;
}

void GenomeAlignerIndex::serialize(const QString &refFileName) {
    QString indexFileName = baseFileName + QString(".") + HEADER_EXTENSION;
    QFile file(indexFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QByteArray data;
    data = HEADER.toLatin1();
    data += PARAMETERS.arg(refFileName).arg(seqLength).arg(w).toLatin1();
    data += COMMENT1 + COMMENT2 + COMMENT3 + COMMENT4;
    data += QByteArray::number(seqLength, 10) + ", ";
    data += QByteArray::number(w, 10) + ", ";
    data += QByteArray::number(bitCharLen, 10) + ", ";
    data += QByteArray::number(seqPartSize, 10) + ", ";
    data += QByteArray::number(objCount, 10) + "\n";
    data += firstSequenceObjectName + "\n";
    for (qint64 i=0; i < objCount; i++) {
        data += QByteArray::number(objLens[i], 10);
        if (objCount-1 == i) {
            data += "\n";
        } else {
            data += ", ";
        }
    }
    file.write(data);

    file.close();
}

inline quint32 getNextInt(QByteArray &data, bool &eol, bool &intErr) {
    int commaIdx = data.indexOf(',');
    if (-1 == commaIdx) {
        commaIdx = data.length();
        eol = true;
    }

    QByteArray result = data.left(commaIdx).trimmed();
    data = data.mid(commaIdx+1).trimmed();

    for (int i = 0; i < result.length(); i++) {
        char c = result[i];
        if (c <'0' || c>'9') {
            intErr = true;
            return -1;
        }
    }
    return result.toUInt();
}

bool GenomeAlignerIndex::deserialize(QByteArray &error) {
    QString indexFileName = baseFileName + QString(".") + HEADER_EXTENSION;
    QFile file(indexFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        error = "Can't open file-index.";
        return false;
    }
    QByteArray data;
    bool eol = false;
    bool intErr = false;

    do {
        data = file.readLine().trimmed();
    } while (data.length() > 0 && '#' == data[0]);

    if (0 == data.length()) {
        error = "Empty parameters' line in the file-index.";
        file.close();
        return false;
    }
    seqLength = getNextInt(data, eol, intErr);
    w = getNextInt(data, eol, intErr);
    int bitCharLen = getNextInt(data, eol, intErr);
    seqPartSize = getNextInt(data, eol, intErr);
    if (eol) {
        error = "Too little amount of parameters in the file-index.";
        file.close();
        return false;
    }
    objCount = getNextInt(data, eol, intErr);
    if (intErr) {
        error = "Bad integer for some parameter in the file-index.";
        file.close();
        return false;
    }
    if (objCount <= 0) {
        error = "Count of sequences must be >0.";
        file.close();
        return false;
    }

    firstSequenceObjectName = file.readLine().trimmed();
    data = file.readLine().trimmed();
    objLens = new quint32[objCount];
    eol = false;
    intErr = false;
    for (int i=0; i<objCount; i++) {
        objLens[i] = getNextInt(data, eol, intErr);
        if (i<objCount-1) {
            if (eol) {
                error = "Too little amount of parameters in the file-index.";
                file.close();
                return false;
            }
        }
    }
    if (intErr) {
        error = "Bad integer for some parameter in the file-index.";
        file.close();
        return false;
    }

    if (w <= 0) {
        error = "Negative index's parameters.";
        file.close();
        return false;
    }

    if (this->bitCharLen != bitCharLen) {
        error = "This index was built for a sequence with another sequence type.";
        file.close();
        return false;
    } else {
        this->bitCharLen = bitCharLen;
    }

    file.close();
    return true;
}

bool GenomeAlignerIndex::openIndexFiles() {
    indexPart.refFile = new QFile(baseFileName+QString(".")+GenomeAlignerIndex::REF_INDEX_EXTENSION);
    return indexPart.refFile->open(QIODevice::ReadOnly);
}

BMType GenomeAlignerIndex::getBitValue(const char *seq, int length) const {
    BMType bitValue = 0;
    for (int i = 0; i < length; i++) {
        bitValue = (bitValue << bitCharLen) | bitTable[uchar(*(seq+i))];
    }

    bitValue <<= 2*charsInMask - bitCharLen*length;

    return bitValue;
}

bool GenomeAlignerIndex::loadPart(int part) {
    currentPart = part;
    if (build) {
        GTIMER(c, v, "GenomeAlignerIndex::build");
        SAType arrLen = 0;
        sArray = indexPart.sArray;
        bitMask = indexPart.bitMask;
        buildPart(indexPart.seqStarts[part], indexPart.seqLengths[part], arrLen);
        indexPart.saLengths[part] = arrLen;
        indexPart.currentPart = part;

        qint64 t0=GTimer::currentTimeMicros();
        SyncSort<BMType, SAType> s(bitMask, sArray, 0, arrLen);
        s.sort();
        qint64 t1=GTimer::currentTimeMicros();
        algoLog.trace(QString("loadPart::build sort time %1 ms").arg((t1 - t0) / double(1000), 0, 'f', 3));

        GTIMER(c2, v2, "GenomeAlignerIndex::writePart");
        indexPart.writePart(part, arrLen);
        qint64 t2=GTimer::currentTimeMicros();
        algoLog.trace(QString("loadPart::build write time %1 ms").arg((t2 - t1) / double(1000), 0, 'f', 3));

        v2.stop();
        sArray = NULL;
        bitMask = NULL;

        if (part == indexPart.partCount - 1) {
            build = false;
            serialize(baseFileName + "." + REF_INDEX_EXTENSION);
        }
        return true;
    } else {
        GTIMER(c, v, "GenomeAlignerIndex::load");
        return indexPart.load(part);
    }
}

BinarySearchResult GenomeAlignerIndex::bitMaskBinarySearch(BMType bitValue, BMType bitFilter) {
    int low = 0;
    int high = indexPart.getLoadedPartSize() - 1;
    BMType *a = indexPart.bitMask;
    qint64 rc = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        rc = (a[mid]&bitFilter) - (bitValue&bitFilter);
        if (rc < 0) {
            low = mid + 1;
        } else if (rc > 0) {
            high = mid - 1;
        } else {
            for(high=mid-1;high >= 0 && (a[high]&bitFilter)==(bitValue&bitFilter); high--){};
            high++;
            break;
        }
    }
    if (0==rc) {
        return high;
    }
    return -1;
}

#ifdef OPENCL_SUPPORT
BinarySearchResult *GenomeAlignerIndex::bitMaskBinarySearchOpenCL(const BMType *bitValues, int size, const int *windowSizes) {

    taskLog.trace(QString("Binary search on GPU for %1 Mb search-values in %2 Mb base values")
        .arg((8*size)/(1 << 20)).arg((8*indexPart.getLoadedPartSize())/(1 << 20)));
    assert(indexPart.getLoadedPartSize() != 0);
    assert(size > 0);

    BinaryFindOpenCL bf((NumberType*)indexPart.bitMask, indexPart.getLoadedPartSize(), (NumberType*)bitValues, size, windowSizes);

    NumberType *ans = bf.launch();

    return (BinarySearchResult*)ans;
}
#endif

bool GenomeAlignerIndex::isValidPos(SAType offset, int startPos, int length, SAType &fisrtSymbol, SearchQuery *qu, SAType &loadedSeqStart) {
    assert(offset<objLens[objCount-1]);
    if ((qint64)offset - loadedSeqStart < startPos) {
        return false;
    }
    fisrtSymbol = offset-startPos;
    if (qu->contains(fisrtSymbol)) {
        return false;
    }

    //binary search in reference objects
    int low = 0;
    int high = objCount;
    int mid = 0;
    qint64 rc = 0;
    SAType minBorder = 0;
    while (low <= high) {
        mid = (low + high) / 2;
        rc = objLens[mid] - (qint64)offset;
        minBorder = mid>0?objLens[mid-1]:0;
        if (((qint64)offset >= minBorder) && (rc > 0)) {
            break;
        } else if (rc <= 0) {
            low = mid;
        } else {
            high = mid;
        }
    }

    if (fisrtSymbol < minBorder) {
        return false;
    }
    if (offset + (length - startPos - 1) >= objLens[mid]) {
        return false;
    }

    return true;
}

bool GenomeAlignerIndex::compare(const char *sourceSeq, const char *querySeq, int startPos, int w, int &c, int CMAX, int length) {
    // forward collect
    for (int i=startPos+w; i<length && c <= CMAX; i++) {
        c += (querySeq[i] == sourceSeq[i])?0:1;
    }
    if (c>CMAX) {
        return false;
    }
    // backward collect
    for (int i=startPos-1; i>=0 && c <= CMAX; i--) {
        c += (querySeq[i] == sourceSeq[i])?0:1;
    }
    if (c <= CMAX) {
        return true;
    }
    return false;
}

//this method contains big copy-paste but it works very fast because of it.
void GenomeAlignerIndex::alignShortRead(SearchQuery *qu, BMType bitValue, int startPos, BinarySearchResult firstResult, AlignContext *settings, BMType bitFilter, int w)
{
    if (firstResult < 0) {
        return;
    }

    SAType fisrtSymbol = 0;
    const char* querySeq = qu->constData();
    const int queryLen = qu->length();
    char *refBuff = NULL;

    int CMAX = settings->nMismatches;
    if (!settings->absMismatches) {
        CMAX = (queryLen * settings->ptMismatches) / MAX_PERCENTAGE;
    }

    int bestC = CMAX + 1;
    SAType bestResult = 0;
    bool found = false;
    if (settings->bestMode) {
        if (qu->haveResult()) {
            bestC = qu->firstMCount();
        }
        if (NULL != qu->getRevCompl() && qu->getRevCompl()->haveResult()) {
            int cRev = qu->getRevCompl()->firstMCount();
            if (cRev < bestC) {
                bestC = cRev;
            }
        }
    }
    CMAX = bestC - 1;
    SAType loadedPartSize = indexPart.getLoadedPartSize();
    SAType loadedSeqStart = indexPart.getLoadedSeqStart();
    quint64 rightOverlapStart = (quint64)loadedSeqStart + (quint64)indexPart.seqLengths[indexPart.currentPart];
    if (indexPart.currentPart != indexPart.partCount - 1) {
        rightOverlapStart -= 2*overlapSize;
    }
    for (SAType k=firstResult; (k<loadedPartSize) && (bitValue&bitFilter)==(indexPart.bitMask[k]&bitFilter); k++) {
        if (!isValidPos(indexPart.sArray[k] + loadedSeqStart, startPos, queryLen,
            fisrtSymbol, qu, loadedSeqStart)) {
            continue;
        }

        refBuff = &(indexPart.seq[fisrtSymbol - loadedSeqStart]);

        int c = 0;
        if (compare(refBuff, querySeq, startPos, w, c, CMAX, queryLen)) {
            if (settings->bestMode) {
                found = true;
                bestC = c;
                bestResult = fisrtSymbol;
                CMAX = bestC - 1;

                if (0 == c) {
                    break;
                } else {
                    continue;
                }
            }
            if (fisrtSymbol >= rightOverlapStart) {
                qu->addOveplapResult(fisrtSymbol);
            } else {
                qu->addResult(fisrtSymbol, c);
            }
        }
    }
    if (settings->bestMode && found) {
        qu->clear();
        qu->addResult(bestResult, bestC);
    }
}

/*build index*/
void GenomeAlignerIndex::buildPart(SAType start, SAType length, SAType &arrLen) {
    qint64 t0 = GTimer::currentTimeMicros();
    initSArray(start, length, arrLen);
    qint64 t1 = GTimer::currentTimeMicros();
    algoLog.trace(QString("initSArray time %1 ms, len %2").arg((t1 - t0) / double(1000), 0, 'f', 3).arg(length));

    const char *seq = indexPart.seq;
    SAType *arunner = sArray;
    BMType *mrunner = bitMask;
    BMType bitValue = 0;
    SAType expectedNext = 0;
    quint32 wCharsInMask1 = w - 1;

    for (BMType *end = mrunner + arrLen; mrunner < end; arunner++, mrunner++) {
        const char* s = seq + *arunner;
        if (*arunner == expectedNext && expectedNext != 0) { //pop first bit, push wCharsInMask1 char to the mask
            bitValue = ((bitValue << bitCharLen) | bitTable[uchar(*(s + wCharsInMask1))]) & bitFilter;
#ifdef _DEBUG
            // double check that optimization doesn't break anything
            BMType bitValue2 = getBitValue(s, w);
            assert(bitValue == bitValue2);
#endif
        } else {
            //recompute the mask if we have some symbols skipped
            bitValue = getBitValue(s, w);
        }
        expectedNext = (s + 1) - seq;
        *mrunner = bitValue;
    }
    qint64 t2 = GTimer::currentTimeMicros();
    algoLog.trace(QString("buildPart bitValue time %1 ms, len %2").arg((t2 - t1) / double(1000), 0, 'f', 3).arg(length));
}

void GenomeAlignerIndex::initSArray(SAType start, SAType length, SAType &arrLen) {
    indexPart.refFile->seek(start);
    qint64 l = indexPart.refFile->read(indexPart.seq, length);
    if (length != l) {
        //setError("Index .ref file is corrupted.");
        return;
    }

    const char *seq = indexPart.seq;

    SAType *arunner = sArray;
    SAType idx = start;
    SAType seqIdx = 0;
    SAType tmpIdx = 0;

    int curObj = 0;
    for (; curObj<objCount; curObj++) {
        if (idx < objLens[curObj]) {
            break;
        }
    }
    assert(curObj < objCount);

    bool goodSuff = false;
    arrLen = 0;
    SAType last = start + length - w;
    for (quint32 i=0; idx <= last; i++) { //initializing sArray and arrLen
        if (idx > (objLens[curObj]-w) && idx < objLens[curObj]) {
            seqIdx += objLens[curObj] - idx;
            idx = objLens[curObj];
            curObj++;
            goodSuff = false;
        }
        if (idx >= seqLength || seqIdx >= length) {
            break;
        }

        if (goodSuff) {
            if (seq[seqIdx + w-1]==unknownChar) {
                goodSuff = false;
                seqIdx += w;
                idx += w;
                i--;
                continue;
            }
        }
        if (!goodSuff) {
            int goodChars = 0;
            tmpIdx = idx;
            SAType j = seqIdx;
            for (; j<length && goodChars<w; j++) {
                if (tmpIdx == objLens[curObj]) {
                    j += objLens[curObj] - tmpIdx;
                    tmpIdx = objLens[curObj];
                    curObj++;
                    goodChars = 0;
                    j--;
                    continue;
                }
                if (seq[j]!=unknownChar) {
                    goodChars++;
                } else {
                    goodChars = 0;
                }
                tmpIdx++;
            }

            idx = tmpIdx - goodChars;
            seqIdx = j - w;
            if (goodChars != w) {
                break;
            }
            goodSuff = true;
        }

        *arunner = idx - start; //-start for positioning at a part of the sequence
        arrLen++;
        idx++;
        arunner++;
        seqIdx++;
    }
}


} //U2
