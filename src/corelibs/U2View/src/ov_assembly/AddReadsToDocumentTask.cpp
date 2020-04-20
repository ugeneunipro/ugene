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

#include <QCoreApplication>

#include <U2Core/L10n.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/DocumentFormatUtils.h>

#include "AddReadsToDocumentTask.h"

namespace U2 {

AddReadsToDocumentTask::AddReadsToDocumentTask(const QList<U2AssemblyRead> &reads, const QPointer<Document> &doc)
    : Task(tr("Add short reads to document"), TaskFlag_None), reads(reads), doc(doc)
{
    SAFE_POINT_EXT(!doc.isNull(), setError(L10N::nullPointerError("document")), );
    dbiRef = doc->getDbiRef();
    SAFE_POINT_EXT(dbiRef.isValid(), setError(tr("Invalid database reference detected")), );

    tpm = Progress_Manual;
}

void AddReadsToDocumentTask::run() {
    TmpDbiObjects objectsGuard(dbiRef, stateInfo);

    foreach (const U2AssemblyRead &r, reads) {
        DNASequence dna(r->name, r->readSequence);
        dna.quality = DNAQuality(r->quality, DNAQualityType_Sanger);
        U2EntityRef ref = U2SequenceUtils::import(stateInfo, dbiRef, dna);
        QScopedPointer<U2SequenceObject> seqObj(new U2SequenceObject(dna.getName(), ref));
        CHECK_OP(stateInfo, );

        seqNameById[seqObj->getEntityRef().entityId] = seqObj->getGObjectName();

        objectsGuard.objects.append(seqObj->getEntityRef().entityId);

        stateInfo.setProgress(100 * objectsGuard.objects.size() / reads.size());
    }
}

Task::ReportResult AddReadsToDocumentTask::report() {
    foreach (const U2DataId &seqId, seqNameById.keys()) {
        doc->addObject(new U2SequenceObject(seqNameById[seqId], U2EntityRef(dbiRef, seqId)));
    }
    return ReportResult_Finished;
}

} // namespace U2
