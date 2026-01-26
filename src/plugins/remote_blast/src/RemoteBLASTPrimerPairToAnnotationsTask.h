/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

#include "RemoteBLASTTask.h"

#include <QPointer>

namespace U2 {

class AnnotationTableObject;
class CreateAnnotationsTask;
class U2SequenceObject;

// This task is required to find common BLAST results of left and right Primer3 primers.
// This task BLASTs left and right primers (usually, Primer3 results), gathers both BLAST results
// and looks for common organisms these primers accessible at.
// This is the equivalent of the Primer-BLAST tool, look here for details:
// https://www.ncbi.nlm.nih.gov/tools/primer-blast/primerinfo.html
class RemoteBLASTPrimerPairToAnnotationsTask : public Task {
    Q_OBJECT
public:
    // \param pairName Name of primer group. Looks like "pair [num]".
    // \param seqObj Sequence object pointer.
    // \param ato Annotation table object pointer.
    // \param leftPrimer Left primer annotations data.
    // \param rightPrimer Right primer annotations data.
    // \param cfg Blast task configurations.
    // \param groupPath Full path to this primer annotation group.
    RemoteBLASTPrimerPairToAnnotationsTask(const QString& pairName,
                                           const QPointer<U2SequenceObject>& seqObj,
                                           const QPointer<AnnotationTableObject>& ato,
                                           const SharedAnnotationData& leftPrimer,
                                           const SharedAnnotationData& rightPrimer,
                                           const RemoteBLASTTaskSettings& cfg,
                                           const QString& groupPath);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    RemoteBLASTTask* getBlastTaskForAnnotationRegion(const SharedAnnotationData& ann);
    void removeAlreadyFoundBlastResults(QList<SharedAnnotationData>& resultAnnotations);

    QString pairName;
    QPointer<U2SequenceObject> seqObj;
    QPointer<AnnotationTableObject> ato;
    SharedAnnotationData leftPrimer;
    SharedAnnotationData rightPrimer;
    RemoteBLASTTaskSettings cfg;
    QString groupPath;

    RemoteBLASTTask* leftPrimerBlastTask = nullptr;
    RemoteBLASTTask* rightPrimerBlastTask = nullptr;
    CreateAnnotationsTask* createAnnotationsTask = nullptr;
};

}
