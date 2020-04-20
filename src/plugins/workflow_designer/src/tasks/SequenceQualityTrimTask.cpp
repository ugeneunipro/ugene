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

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2SafePoints.h>

#include "SequenceQualityTrimTask.h"

namespace U2 {

SequenceQualityTrimTaskSettings::SequenceQualityTrimTaskSettings()
    : qualityTreshold(30),
      minSequenceLength(0),
      trimBothEnds(true)
{

}

SequenceQualityTrimTask::SequenceQualityTrimTask(const SequenceQualityTrimTaskSettings &settings)
    : Task(tr("Trim sequence by quality"), TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      settings(settings),
      trimmedSequenceObject(NULL),
      trimmedChromatogramObject(NULL),
      isFilteredOut(false)
{
    SAFE_POINT_EXT(NULL != settings.sequenceObject, setError("Sequence object is NULL"), );
}

SequenceQualityTrimTask::~SequenceQualityTrimTask() {
    delete trimmedSequenceObject;
    delete trimmedChromatogramObject;
}

U2SequenceObject *SequenceQualityTrimTask::takeTrimmedSequence() {
    U2SequenceObject *result = trimmedSequenceObject;
    trimmedSequenceObject = NULL;
    return result;
}

void SequenceQualityTrimTask::run() {
    cloneObjects();
    CHECK_OP(stateInfo, );

    const U2Region acceptedRegion = trimSequence();
    CHECK_OP(stateInfo, );

    trimChromatogram(acceptedRegion);
}

QString SequenceQualityTrimTask::generateReport() const {
    CHECK(isFilteredOut, "");
    return tr("The sequence '%1' was filtered out by quality").arg(settings.sequenceObject->getSequenceName());
}

void SequenceQualityTrimTask::cloneObjects() {
    cloneSequence();
    CHECK_OP(stateInfo, );
    cloneChromatogram();
    CHECK_OP(stateInfo, );
    restoreRelation();
    CHECK_OP(stateInfo, );
}

void SequenceQualityTrimTask::cloneSequence() {
    const U2DbiRef dbiRef = settings.sequenceObject->getEntityRef().dbiRef;
    trimmedSequenceObject = qobject_cast<U2SequenceObject *>(settings.sequenceObject->clone(dbiRef, stateInfo));
}

void SequenceQualityTrimTask::cloneChromatogram() {
    const U2DbiRef dbiRef = settings.sequenceObject->getEntityRef().dbiRef;
    const U2EntityRef chromatogramRef = ChromatogramUtils::getChromatogramIdByRelatedSequenceId(stateInfo, settings.sequenceObject->getEntityRef());
    CHECK_OP(stateInfo, );
    CHECK(chromatogramRef.isValid(), );

    const QString chromatogramName = ChromatogramUtils::getChromatogramName(stateInfo, chromatogramRef);
    CHECK_OP(stateInfo, );

    QScopedPointer<DNAChromatogramObject> chromatogramObject(new DNAChromatogramObject(chromatogramName, chromatogramRef));
    trimmedChromatogramObject = qobject_cast<DNAChromatogramObject *>(chromatogramObject->clone(dbiRef, stateInfo));
    CHECK_OP(stateInfo, );
}

void SequenceQualityTrimTask::restoreRelation() {
    CHECK(NULL != trimmedChromatogramObject, );
    SAFE_POINT_EXT(NULL != trimmedSequenceObject, setError("Cloned sequence object is NULL"), );

    U2ObjectRelation dbRelation;
    dbRelation.id = trimmedChromatogramObject->getEntityRef().entityId;
    dbRelation.referencedName = trimmedSequenceObject->getGObjectName();
    dbRelation.referencedObject = trimmedSequenceObject->getEntityRef().entityId;
    dbRelation.referencedType = trimmedSequenceObject->getGObjectType();
    dbRelation.relationRole = ObjectRole_Sequence;

    DbiConnection connection(settings.sequenceObject->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    connection.dbi->getObjectRelationsDbi()->createObjectRelation(dbRelation, stateInfo);
    CHECK_OP(stateInfo, );
}

U2Region SequenceQualityTrimTask::trimSequence() {
    DNASequence sequence = trimmedSequenceObject->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, U2Region());
    CHECK_EXT(sequence.hasQualityScores(), stateInfo.addWarning(tr("The trimming was skipped for the sequences without quality information.")), U2Region(0, trimmedSequenceObject->getSequenceLength()));
    const U2Region acceptedRegion = DNASequenceUtils::trimByQuality(sequence, settings.qualityTreshold, settings.minSequenceLength, settings.trimBothEnds);
    isFilteredOut = acceptedRegion.isEmpty();
    trimmedSequenceObject->setWholeSequence(sequence);
    return acceptedRegion;
}

void SequenceQualityTrimTask::trimChromatogram(const U2Region &regionToCrop) {
    CHECK(NULL != trimmedChromatogramObject, );
    DNAChromatogram chromatogram = trimmedChromatogramObject->getChromatogram();
    ChromatogramUtils::crop(chromatogram, regionToCrop.startPos, regionToCrop.length);
    trimmedChromatogramObject->setChromatogram(stateInfo, chromatogram);
    CHECK_OP(stateInfo, );
}

}   // namespace U2
