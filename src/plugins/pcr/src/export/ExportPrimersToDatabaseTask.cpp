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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ExportPrimersToDatabaseTask.h"

namespace U2 {

ExportPrimersToDatabaseTask::ExportPrimersToDatabaseTask(const QList<Primer> &primers, const U2DbiRef &dbiRef, const QString &folder) :
    Task(tr("Export primers"), TaskFlags_FOSE_COSC | TaskFlag_OnlyNotificationReport),
    primers(primers),
    dbiRef(dbiRef),
    folder(folder),
    dbiSequences(dbiRef, stateInfo),
    dbiAnnotations(dbiRef, stateInfo)
{
    SAFE_POINT_EXT(!primers.isEmpty(), setError(L10N::badArgument("primers list")), );
    SAFE_POINT_EXT(dbiRef.isValid(), setError(L10N::badArgument("shared database reference")), );
    SAFE_POINT_EXT(folder.startsWith(U2ObjectDbi::ROOT_FOLDER), setError(L10N::badArgument("database folder")), );
}

void ExportPrimersToDatabaseTask::run() {
    foreach (const Primer &primer, primers) {
        U2SequenceImporter importer;
        importer.startSequence(stateInfo, dbiRef, folder, primer.name, false);
        CHECK_OP(stateInfo, );
        importer.addBlock(primer.sequence.toLocal8Bit().constData(), primer.sequence.length(), stateInfo);
        CHECK_OP(stateInfo, );
        const U2DataId sequenceId = importer.finalizeSequence(stateInfo).id;
        dbiSequences.objects << sequenceId;
        CHECK_OP(stateInfo, );

        SharedAnnotationData annotation(new AnnotationData);
        annotation->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_primer_bind).text;
        annotation->location->regions << U2Region(0, primer.sequence.length());
        annotation->qualifiers << U2Qualifier("sequence", primer.sequence);
        annotation->qualifiers.append(U2Qualifier("tm", QString::number(primer.tm)));
        annotation->qualifiers.append(U2Qualifier("gc%", QString::number(primer.gc)));

        QVariantMap hints;
        hints[DocumentFormat::DBI_FOLDER_HINT] = folder;
        AnnotationTableObject annotationTable(primer.name + " features", dbiRef, hints);
        annotationTable.addAnnotations(QList<SharedAnnotationData>() << annotation);
        dbiAnnotations.objects << annotationTable.getEntityRef().entityId;

        const GObjectReference sequenceRef(U2DbiUtils::ref2Url(dbiRef), primer.name, GObjectTypes::SEQUENCE, U2EntityRef(dbiRef, sequenceId));
        annotationTable.addObjectRelation(GObjectRelation(sequenceRef, ObjectRole_Sequence));
    }
}

Task::ReportResult ExportPrimersToDatabaseTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    while (!dbiSequences.objects.isEmpty() && !dbiAnnotations.objects.isEmpty()) {
        importedObjectIds.insert(dbiSequences.objects.takeFirst(), dbiAnnotations.objects.takeFirst());
    }
    SAFE_POINT_EXT(dbiSequences.objects.isEmpty() && dbiAnnotations.objects.isEmpty(), setError(tr("Unexpected objects count")), ReportResult_Finished);
    return ReportResult_Finished;
}

const QMap<U2DataId, U2DataId> &ExportPrimersToDatabaseTask::getImportedObjectIds() const {
    return importedObjectIds;
}

}   // namespace U2
