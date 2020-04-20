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

#ifndef _U2_GENOME_ALIGNER_INDEX_PART_H_
#define _U2_GENOME_ALIGNER_INDEX_PART_H_

#include <U2Core/global.h>
#include <QFile>

typedef quint64 BMType;
typedef quint32 SAType;
#define MAX_BIT_MASK_LENGTH 31

namespace U2 {
class GenomeAlignerIndexTask;

class IndexPart {
    friend class GenomeAlignerIndexTask;
    friend class GenomeAlignerIndex;
public:
    IndexPart();
    ~IndexPart();
    SAType          *sArray;
    BMType          *bitMask;
    char            *seq;

    SAType getLoadedPartSize() const;
    SAType getMaxLength();
    SAType getLoadedSeqStart();
    int getCurrentPart() {return currentPart;}
    bool load(int part);
    void writePart(int part, quint32 arrLen);
private:
    int             partCount;
    int             currentPart;
    SAType          *seqStarts;
    SAType          *seqLengths;
    SAType          *saLengths;

    QFile           *refFile;
    QFile           **partFiles;

    BMType getBitValue(uchar *seq, SAType idx);
};

bool isLittleEndian();

} //U2

#endif //_U2_GENOME_ALIGNER_INDEX_PART_H_
