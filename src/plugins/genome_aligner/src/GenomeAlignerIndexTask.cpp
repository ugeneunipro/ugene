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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequence.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Timer.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
#include "U2Formats/StreamSequenceReader.h"
#include <QtEndian>
#include "GenomeAlignerIndex.h"

#include "GenomeAlignerIndexTask.h"

namespace U2 {

GenomeAlignerIndexTask::GenomeAlignerIndexTask(const GenomeAlignerIndexSettings &settings)
    : Task("Building genome aligner's index", TaskFlag_None), objLens(NULL), objCount(0), unknownChar('N')
{
    GUrl i = settings.indexFileName;
    baseFileName = i.dirPath() + "/" + i.baseFileName();
    w = MAX_BIT_MASK_LENGTH;

    bitTable = bt.getBitMaskCharBits(DNAAlphabet_NUCL);
    bitCharLen = bt.getBitMaskCharBitsNum(DNAAlphabet_NUCL);

    index = new GenomeAlignerIndex();
    index->baseFileName = baseFileName;
    index->unknownChar = unknownChar;
    index->bitFilter = ((BMType)1<<(bitCharLen * w))-1;
    this->settings = settings;
}

GenomeAlignerIndexTask::~GenomeAlignerIndexTask() {
}

SAType getPartStartPos(SAType seqLength, int parts, int curPart) {
    SAType start = (seqLength/parts)*curPart;
    if (curPart > 0) {
        start -= GenomeAlignerIndex::overlapSize;
    }
    return start;
}

SAType getPartLength(SAType seqLength, int parts, int curPart) {
    SAType length = 0;
    if (curPart < parts - 1) {
        length = (seqLength/parts) + GenomeAlignerIndex::overlapSize;
    } else {
        length = seqLength - (seqLength/parts)*curPart;
    }
    if (curPart > 0) {
        length += GenomeAlignerIndex::overlapSize;
    }
    return length;
}

void GenomeAlignerIndexTask::run() {
    QByteArray error;
    bool res = index->deserialize(error);
    if (settings.prebuiltIndex) {
        index->build = false;
        if (!res) {
            setError("Index file is corrupted. Try to create index another time.");
            return;
        }
        QString refName = baseFileName + QString(".") + GenomeAlignerIndex::REF_INDEX_EXTENSION;
        if (!QFile::exists(refName)) {
            setError(tr("File %1 is not found. Try to create index another time.").arg(refName));
            return;
        }
        seqLength = index->seqLength;
        settings.seqPartSize = index->seqPartSize;
    } else {
        if (!res) {
            algoLog.details(error + " Index file is not defined. I will try to create a new index file.");
        }
        index->build = !(res && (index->seqPartSize == settings.seqPartSize));
        if (!index->build) {
            index->build = !QFile::exists(baseFileName + QString(".") + GenomeAlignerIndex::REF_INDEX_EXTENSION);
            seqLength = index->seqLength;
        }
        if (index->build) {
            reformatSequence();
            if (isCanceled() || hasError()) {
                return;
            }
            seqLength = objLens[objCount-1];
            index->seqLength = seqLength;
            index->w = w;
            index->seqPartSize = settings.seqPartSize;
        }
    }

    MAX_ELEM_COUNT_IN_MEMORY = settings.seqPartSize*1024*1024;
    int parts = seqLength/(MAX_ELEM_COUNT_IN_MEMORY - 2*GenomeAlignerIndex::overlapSize) + 1;
    index->indexPart.partCount = parts;
    index->indexPart.seqStarts = new SAType[parts];
    index->indexPart.seqLengths = new SAType[parts];
    index->indexPart.saLengths = new SAType[parts];
    index->indexPart.partFiles = new QFile*[parts];

    SAType start = 0;
    SAType length = 0;
    for (int i=0; i<parts; i++) {
        start = getPartStartPos(seqLength, parts, i);
        length = getPartLength(seqLength, parts, i);

        index->indexPart.seqStarts[i] = start;
        index->indexPart.seqLengths[i] = length;
        index->indexPart.partFiles[i] = new QFile(baseFileName + "." + QByteArray::number(i) + "." + GenomeAlignerIndex::SARRAY_EXTENSION);
        if (!index->build) {
            if (!index->build && !index->indexPart.partFiles[i]->exists()) {
                if (settings.prebuiltIndex) {
                    setError(tr("File %1 is not found. Try to create index another time.").arg(index->indexPart.partFiles[i]->fileName()));
                    return;
                }
                index->build = true;
            } else {
                index->build = false;
            }
        } else {
            index->indexPart.partFiles[i]->remove();
        }
    }


    if (!index->openIndexFiles()) {
        setError("Can't open some of index files");
        return;
    }
    memFreeSize = MEM_FOR_READS*1024*1024;
    gpuFreeSize = memFreeSize;
    SAType maxLength = index->indexPart.getMaxLength();

    try {
        assert(0!=maxLength);
        index->indexPart.bitMask = new BMType[maxLength];
        index->indexPart.sArray = new SAType[maxLength];
        index->indexPart.seq = new char[maxLength];
    } catch(std::bad_alloc &e) {
        Q_UNUSED(e);
        setError("Can't allocate this amount of memory. Try to close some of your programs or to decrease \"maxMemorySize\"-option");
        return;
    }
    if (settings.justBuildIndex) {
        for (int i=0; i<parts; i++) {
            index->build = true;
            index->loadPart(i);
            index->indexPart.partFiles[i]->close();
            stateInfo.progress += 100/parts;
        }
    }
}

void GenomeAlignerIndexTask::reformatSequence() {
    StreamSequenceReader seqReader;
    QList<GUrl> urls;
    urls.append(GUrl(settings.refFileName));
    bool init = seqReader.init(urls);
    if (!init) {
        setError(tr("Can not init short reads loader. %1").arg(seqReader.getErrorMessage()));
        return;
    }

    objCount = 0;
    QList<quint32> seqLens;

    QFile newRefFile(baseFileName + QString(".") + GenomeAlignerIndex::REF_INDEX_EXTENSION);
    newRefFile.open(QIODevice::WriteOnly);
    bool firstSeq = true;
    while (seqReader.hasNext()) {
        objCount++;
        const DNASequence *seq = seqReader.getNextSequenceObject();
        if (NULL == seq) {
            setError("Reference object type must be a sequence, but not a multiple alignment");
            return;
        }
        if(NULL == seq->alphabet){
            setError("Cannot define an alphabet for the reference sequence");
            return;
        }
        if (DNAAlphabet_NUCL != seq->alphabet->getType()) {
            setError("Unsupported file format: alphabet type is not NUCL");
            return;
        }
        seqLens.append(seq->length());
        newRefFile.write(seq->constData());
        if (firstSeq) {
            index->firstSequenceObjectName = seq->getName();// + QString("_and_others");
            firstSeq = false;
        }
        CHECK_OP_EXT(stateInfo, newRefFile.close(), );
    }
    newRefFile.close();

    if (0 == objCount) {
        setError(QString("Too large sequence, unsupported file format or empty reference in %1").arg(settings.refFileName));
        return;
    }
    index->objLens = new quint32[objCount];
    index->objCount = objCount;
    objLens = index->objLens;
    int i = 0;
    quint32 prev = 0;
    foreach (quint32 len, seqLens) {
        objLens[i] = prev + len;
        prev = objLens[i];
        i++;
    }
}

} //U2
