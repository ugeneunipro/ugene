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

#ifndef _U2_EXPORT_SELECTED_SEQ_REGIONS_TASK_H_
#define _U2_EXPORT_SELECTED_SEQ_REGIONS_TASK_H_

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/U2Region.h>

#include "ExportSequenceTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// CreateExportItemsFromSeqRegionsTask
//////////////////////////////////////////////////////////////////////////

class CreateExportItemsFromSeqRegionsTask : public Task {
    Q_OBJECT
public:
    CreateExportItemsFromSeqRegionsTask(const QPointer<U2SequenceObject> &seqObject, const QList<QPointer<AnnotationTableObject> > &connectedAts,
        const QVector<U2Region> &regions, const ExportSequenceTaskSettings &exportSettings, const DNATranslation *aminoTrans,
        const DNATranslation *backTranslation, const DNATranslation *complTrans);

    void run();

    const ExportSequenceTaskSettings & getExportSettings() const;

private:
    QList<SharedAnnotationData> findAnnotationsInRegion(const U2Region &region);

    QPointer<U2SequenceObject> seqObject;
    QList<QPointer<AnnotationTableObject> > annotations;
    const QVector<U2Region> regions;
    ExportSequenceTaskSettings exportSettings;
    const DNATranslation *aminoTrans;
    const DNATranslation *backTranslation;
    const DNATranslation *complTrans;
};

//////////////////////////////////////////////////////////////////////////
/// ExportSelectedSeqRegionsTask
//////////////////////////////////////////////////////////////////////////

class ExportSelectedSeqRegionsTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportSelectedSeqRegionsTask(U2SequenceObject *seqObject, const QSet<AnnotationTableObject *> &connectedAts,
        const QVector<U2Region> &regions, const ExportSequenceTaskSettings &exportSettings, const DNATranslation *aminoTrans,
        const DNATranslation *backTrans, const DNATranslation *complTrans);

    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    QPointer<U2SequenceObject> seqObject;
    QList<QPointer<AnnotationTableObject> > annotations;
    const QVector<U2Region> regions;
    ExportSequenceTaskSettings exportSettings;
    const DNATranslation *aminoTrans;
    const DNATranslation *backTrans;
    const DNATranslation *complTrans;
};

} // namespace U2

#endif // _U2_EXPORT_SELECTED_SEQ_REGIONS_TASK_H_
