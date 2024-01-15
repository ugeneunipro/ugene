/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MaContentFilterTask.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MsaObject.h>
#include <U2Core/MsaUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "../ProjectFilterNames.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// MaContentFilterTask
//////////////////////////////////////////////////////////////////////////

static bool patternFitsMaAlphabet(const MsaObject* maObject, const QString& pattern) {
    SAFE_POINT_NN(maObject, false);
    SAFE_POINT(!pattern.isEmpty(), "Empty pattern to search", false);

    const DNAAlphabet* alphabet = maObject->getAlphabet();
    SAFE_POINT_NN(alphabet, false);

    const QByteArray searchStr = pattern.toUpper().toLatin1();
    return alphabet->containsAll(searchStr.constData(), searchStr.length());
}

static bool maContainsPattern(const MsaObject* maObject, const QString& pattern) {
    SAFE_POINT_NN(maObject, false);
    SAFE_POINT(!pattern.isEmpty(), "Empty pattern to search", false);

    const MsaData* mData = maObject->getAlignment().data();
    const QByteArray searchStr = pattern.toUpper().toLatin1();

    for (int i = 0, n = mData->getRowCount(); i < n; ++i) {
        const MsaRow& row = mData->getRow(i);
        for (int j = 0; j < (mData->getLength() - searchStr.length() + 1); ++j) {
            char c = row->charAt(j);
            int alternateLength = 0;
            if (c != U2Msa::GAP_CHAR && MsaUtils::equalsIgnoreGaps(row, j, searchStr, alternateLength)) {
                return true;
            }
        }
    }
    return false;
}

static bool isFilteredByMAContent(const MsaObject* maObj, const ProjectTreeControllerModeSettings& settings) {
    CHECK(maObj != nullptr, false);

    foreach (const QString& pattern, settings.tokensToShow) {
        if (!patternFitsMaAlphabet(maObj, pattern)) {
            continue;
        }
        if (maContainsPattern(maObj, pattern)) {
            return true;
        }
    }
    return false;
}

static bool seqContainsPattern(const U2SequenceObject* seqObject, const QString& pattern) {
    SAFE_POINT_NN(seqObject, false);
    SAFE_POINT(!pattern.isEmpty(), "Empty pattern to search", false);

    U2OpStatusImpl op;
    QByteArray seqData = seqObject->getWholeSequenceData(op);
    CHECK_OP(op, false);

    const QByteArray searchStr = pattern.toUpper().toLatin1();
    return seqData.indexOf(searchStr) >= 0;
}

MsaContentFilterTask::MsaContentFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MSA_CONTENT_FILTER_NAME, docs) {
    filteredObjCountPerIteration = 1;
}

bool MsaContentFilterTask::filterAcceptsObject(GObject* obj) {
    return isFilteredByMAContent(qobject_cast<MsaObject*>(obj), settings);
}

McaReadContentFilterTask::McaReadContentFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_READ_CONTENT_FILTER_NAME, docs) {
    filteredObjCountPerIteration = 1;
}

bool McaReadContentFilterTask::filterAcceptsObject(GObject* obj) {
    return isFilteredByMAContent(qobject_cast<MsaObject*>(obj), settings);
}

McaReferenceContentFilterTask::McaReferenceContentFilterTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_REFERENCE_CONTENT_FILTER_NAME, docs) {
    filteredObjCountPerIteration = 1;
}

bool McaReferenceContentFilterTask::filterAcceptsObject(GObject* obj) {
    auto mcaObj = qobject_cast<MsaObject*>(obj);
    CHECK(mcaObj != nullptr, false);

    foreach (const QString& pattern, settings.tokensToShow) {
        if (!patternFitsMaAlphabet(mcaObj, pattern)) {
            continue;
        }
        U2SequenceObject* refObj = mcaObj->getReferenceObj();
        if (refObj != nullptr && seqContainsPattern(refObj, pattern)) {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
/// MaContentFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask* MsaContentFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                      const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new MsaContentFilterTask(settings, acceptedDocs);
}

AbstractProjectFilterTask* McaReadContentFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                          const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new McaReadContentFilterTask(settings, acceptedDocs);
}

AbstractProjectFilterTask* McaReferenceContentFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                                               const QList<QPointer<Document>>& docs) const {
    QList<QPointer<Document>> acceptedDocs = getAcceptedDocs(docs, {GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT});
    return acceptedDocs.isEmpty() ? nullptr : new McaReferenceContentFilterTask(settings, acceptedDocs);
}

}  // namespace U2
