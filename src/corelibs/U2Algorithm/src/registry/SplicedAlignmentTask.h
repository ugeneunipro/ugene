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

#ifndef _U2_SPLICED_ALIGNMENT_TASK_H_
#define _U2_SPLICED_ALIGNMENT_TASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Task.h>

namespace U2 {


class SplicedAlignmentTaskConfig {
public:
    SplicedAlignmentTaskConfig(U2SequenceObject* cDna, U2SequenceObject* genomic) : cDnaObj(cDna), genomicSeqObj(genomic) {

    }

    U2SequenceObject* getGenomicSequence() { return genomicSeqObj; }
    U2SequenceObject* getCDnaSequence() { return cDnaObj; }

private:
    U2SequenceObject* cDnaObj;
    U2SequenceObject* genomicSeqObj;
    QVariantMap customOptions;
};

// The task performs mRNA to genomic sequence alignment

class U2ALGORITHM_EXPORT SplicedAlignmentTask : public Task
{
    Q_OBJECT
public:
    SplicedAlignmentTask(const SplicedAlignmentTaskConfig& cfg);
    SplicedAlignmentTask(const QString& taskName, TaskFlags flags, const SplicedAlignmentTaskConfig& cfg);
    virtual QList<SharedAnnotationData> getAlignmentResult() = 0;
protected:
    SplicedAlignmentTaskConfig config;
};

// Factory creates SplicedAligmentTask

class U2ALGORITHM_EXPORT SplicedAlignmentTaskFactory {
public:
    virtual SplicedAlignmentTask* createTaskInstance(const SplicedAlignmentTaskConfig& config) = 0;
    virtual ~SplicedAlignmentTaskFactory() {}
};


} // namespace

#endif // _U2_SPLICED_ALIGNMENT_TASK_H
