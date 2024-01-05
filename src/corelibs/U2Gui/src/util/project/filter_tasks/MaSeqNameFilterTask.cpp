/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MaSeqNameFilterTask.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/MsaObject.h>
#include <U2Core/U2SafePoints.h>

#include "../ProjectFilterNames.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// MaSeqNameFilterTask
//////////////////////////////////////////////////////////////////////////

static bool isFilteredByMASequenceName(const MsaObject* maObj, const ProjectTreeControllerModeSettings& settings) {
    CHECK(maObj != nullptr, false);
    for (int i = 0, n = maObj->getRowCount(); i < n; ++i) {
        if (settings.nameFilterAcceptsString(maObj->getRow(i)->getName())) {
            return true;
        }
    }
    return false;
}

MsaSeqNameFilterTask::MsaSeqNameFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MSA_SEQ_NAME_FILTER_NAME, docs) {
}

bool MsaSeqNameFilterTask::filterAcceptsObject(GObject* obj) {
    return isFilteredByMASequenceName(qobject_cast<MsaObject*>(obj), settings);
}

McaReadNameFilterTask::McaReadNameFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_READ_NAME_FILTER_NAME, docs) {
}

bool McaReadNameFilterTask::filterAcceptsObject(GObject* obj) {
    return isFilteredByMASequenceName(qobject_cast<MsaObject*>(obj), settings);
}

McaReferenceNameFilterTask::McaReferenceNameFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_REFERENCE_NAME_FILTER_NAME, docs) {
}

bool McaReferenceNameFilterTask::filterAcceptsObject(GObject* obj) {
    auto mcaObj = qobject_cast<MsaObject*>(obj);
    CHECK(mcaObj != nullptr, false);
    U2SequenceObject* refObj = mcaObj->getReferenceObj();
    if (refObj != nullptr && settings.nameFilterAcceptsString(refObj->getSequenceName())) {
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
/// MaSeqNameFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask* MsaSeqNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                      const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new MsaSeqNameFilterTask(settings, acceptedDocs);
}

AbstractProjectFilterTask* McaReadNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                       const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new McaReadNameFilterTask(settings, acceptedDocs);
}

AbstractProjectFilterTask* McaReferenceNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                            const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new McaReferenceNameFilterTask(settings, acceptedDocs);
}

}  // namespace U2
