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

#ifndef _U2_GENOME_ALIGNER_INDEX_TASK_H_
#define _U2_GENOME_ALIGNER_INDEX_TASK_H_

#include <U2Core/Task.h>
#include <U2Algorithm/BitsTable.h>
#include <QVector>
#include <QFile>
#include "GenomeAlignerIndex.h"

namespace U2 {

class GenomeAlignerIndex;
class U2SequenceObject;

class GenomeAlignerIndexSettings {
public:
    QString     indexFileName;
    QString     refFileName;
    bool        justBuildIndex;
    int         seqPartSize;            //in Mb
    bool        prebuiltIndex;
};

class GenomeAlignerIndexTask: public Task {
    Q_OBJECT
public:
    GenomeAlignerIndexTask(const GenomeAlignerIndexSettings &settings);
    ~GenomeAlignerIndexTask();
    void run();
    qint64 getFreeMemSize() {return memFreeSize;}
    qint64 getFreeGPUSize() {return gpuFreeSize;}

    GenomeAlignerIndex    *index;

private:
    SAType          seqLength;      //reference sequence's length
    int             w;              //window size
    QString         baseFileName;   //base of the index's file name
    BitsTable       bt;
    const quint32*  bitTable;
    int             bitCharLen;
    BMType          bitFilter;
    SAType          *objLens;
    int             objCount;
    char            unknownChar;

    qint64           memFreeSize;
    qint64           gpuFreeSize;

    GenomeAlignerIndexSettings settings;

    quint32 MAX_ELEM_COUNT_IN_MEMORY;
    static const int BUFF_SIZE = 6291456; //6Mb. Must be divided by 8
    static const int MEM_FOR_READS = 400; //Mb for aligning reads


private:
    void reformatSequence();
};

} //U2

#endif // _U2_GENOME_ALIGNER_INDEX_TASK_H_
