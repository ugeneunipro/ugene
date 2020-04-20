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

#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include "CalculateCoveragePerBaseTask.h"

namespace U2 {

CalculateCoveragePerBaseOnRegionTask::CalculateCoveragePerBaseOnRegionTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const U2Region &region) :
    Task(tr("Calculate coverage per base for assembly on region (%1, %2)").arg(region.startPos).arg(region.endPos()), TaskFlag_None),
    dbiRef(dbiRef),
    assemblyId(assemblyId),
    region(region),
    results(new QVector<CoveragePerBaseInfo>)
{
    SAFE_POINT_EXT(dbiRef.isValid(), setError(tr("Invalid database reference")), );
    SAFE_POINT_EXT(!assemblyId.isEmpty(), setError(tr("Invalid assembly ID")), );
}

CalculateCoveragePerBaseOnRegionTask::~CalculateCoveragePerBaseOnRegionTask() {
    delete results;
}

void CalculateCoveragePerBaseOnRegionTask::run() {
    DbiConnection con(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    U2AssemblyDbi *assemblyDbi = con.dbi->getAssemblyDbi();
    SAFE_POINT_EXT(NULL != assemblyDbi, setError(tr("Assembly DBI is NULL")), );

    results->resize(region.length);

    QScopedPointer<U2DbiIterator<U2AssemblyRead> > readsIterator(assemblyDbi->getReads(assemblyId, region, stateInfo));
    while (readsIterator->hasNext()) {
        const U2AssemblyRead read = readsIterator->next();
        processRead(read);
        CHECK_OP(stateInfo, );
    }
}

const U2Region &CalculateCoveragePerBaseOnRegionTask::getRegion() const {
    return region;
}

QVector<CoveragePerBaseInfo> *CalculateCoveragePerBaseOnRegionTask::takeResult() {
    QVector<CoveragePerBaseInfo> *result = results;
    results = NULL;
    return result;
}

void CalculateCoveragePerBaseOnRegionTask::processRead(const U2AssemblyRead &read) {
    const qint64 startPos = qMax(read->leftmostPos, region.startPos);
    const qint64 endPos = qMin(read->leftmostPos + read->effectiveLen, region.endPos());
    const U2Region regionToProcess = U2Region(startPos, endPos - startPos);

    // we have used effective length of the read, so insertions/deletions are already taken into account
    // cigarVector can be longer than needed
    QVector<U2CigarOp> cigarVector;
    foreach (const U2CigarToken &cigar, read->cigar) {
        cigarVector += QVector<U2CigarOp>(cigar.count, cigar.op);
    }

    if (read->leftmostPos < regionToProcess.startPos) {
        cigarVector = cigarVector.mid(regionToProcess.startPos - read->leftmostPos);//cut unneeded cigar string
    }

    for (int positionOffset = 0, cigarOffset = 0, deletionsCount = 0, insertionsCount = 0; regionToProcess.startPos + positionOffset < regionToProcess.endPos(); positionOffset++) {
        char currentBase = 'N';
        CoveragePerBaseInfo &info = (*results)[regionToProcess.startPos + positionOffset - region.startPos];
        const U2CigarOp cigarOp = nextCigarOp(cigarVector, cigarOffset, insertionsCount);
        CHECK_OP(stateInfo, );

        switch(cigarOp) {
        case U2CigarOp_I:
        case U2CigarOp_S:
            // skip the insertion
            continue;
        case U2CigarOp_D:
            // skip the deletion
            deletionsCount++;
            continue;
        case U2CigarOp_N:
            // skip the deletion
            deletionsCount++;
            continue;
        default:
            currentBase = read->readSequence[positionOffset - deletionsCount + insertionsCount];
            break;
        }
        info.basesCount[currentBase] = info.basesCount[currentBase] + 1;
        info.coverage++;
    }
}

U2CigarOp CalculateCoveragePerBaseOnRegionTask::nextCigarOp(const QVector<U2CigarOp> &cigarVector, int &index, int &insertionsCount) {
    U2CigarOp cigarOp = U2CigarOp_Invalid;

    do {
        SAFE_POINT_EXT(index < cigarVector.length(), setError(tr("Cigar string: out of bounds")), U2CigarOp_Invalid);
        cigarOp = cigarVector[index];
        index++;

        if (U2CigarOp_I == cigarOp || U2CigarOp_S == cigarOp) {
            insertionsCount++;
        }
    } while (U2CigarOp_I == cigarOp || U2CigarOp_S == cigarOp || U2CigarOp_P == cigarOp);

    return cigarOp;
}

CalculateCoveragePerBaseTask::CalculateCoveragePerBaseTask(const U2DbiRef &_dbiRef, const U2DataId &_assemblyId) :
    Task(tr("Calculate coverage per base for assembly"), TaskFlags_NR_FOSE_COSC),
    dbiRef(_dbiRef),
    assemblyId(_assemblyId),
    getLengthTask(NULL)
{
    SAFE_POINT_EXT(dbiRef.isValid(), setError(tr("Invalid database reference")), );
    SAFE_POINT_EXT(!assemblyId.isEmpty(), setError(tr("Invalid assembly ID")), );
}

CalculateCoveragePerBaseTask::~CalculateCoveragePerBaseTask() {
    qDeleteAll(results.values());
}

void CalculateCoveragePerBaseTask::prepare() {
    getLengthTask = new GetAssemblyLengthTask(dbiRef, assemblyId);
    addSubTask(getLengthTask);
}

QList<Task *> CalculateCoveragePerBaseTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;
    CHECK_OP(stateInfo, res);

    if(subTask == getLengthTask){
        const qint64 length = getLengthTask->getAssemblyLength();
        qint64 tasksCount = length / MAX_REGION_LENGTH + (length % MAX_REGION_LENGTH > 0 ? 1 : 0);
        for (qint64 i = 0; i < tasksCount; i++) {
            const U2Region region(i * MAX_REGION_LENGTH, (i == tasksCount - 1 ? length % MAX_REGION_LENGTH : MAX_REGION_LENGTH));
            res.append(new CalculateCoveragePerBaseOnRegionTask(dbiRef, assemblyId, region));
        }
    }else{
        CalculateCoveragePerBaseOnRegionTask *calculateTask = qobject_cast<CalculateCoveragePerBaseOnRegionTask *>(subTask);
        SAFE_POINT_EXT(NULL != calculateTask, setError(tr("An unexpected subtask")), res);

        results.insert(calculateTask->getRegion().startPos, calculateTask->takeResult());
        emit si_regionIsProcessed(calculateTask->getRegion().startPos);
    }

    return res;
}

bool CalculateCoveragePerBaseTask::isResultReady(qint64 startPos) const {
    return results.contains(startPos);
}

bool CalculateCoveragePerBaseTask::areThereUnprocessedResults() const {
    return !results.isEmpty();
}

QVector<CoveragePerBaseInfo> *CalculateCoveragePerBaseTask::takeResult(qint64 startPos) {
    QVector<CoveragePerBaseInfo> *result = results.value(startPos, NULL);
    results.remove(startPos);
    return result;
}

void GetAssemblyLengthTask::run(){
    DbiConnection con(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    U2AttributeDbi *attributeDbi = con.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, setError(tr("Attribute DBI is NULL")), );

    const U2IntegerAttribute lengthAttribute = U2AttributeUtils::findIntegerAttribute(attributeDbi, assemblyId, U2BaseAttributeName::reference_length, stateInfo);
    CHECK_OP(stateInfo, );
    CHECK_EXT(lengthAttribute.hasValidId(), setError(tr("Can't get the assembly length: attribute is missing")), );

    length = lengthAttribute.value;
    SAFE_POINT_EXT(0 < length, setError(tr("Assembly has zero length")), );
}
}   // namespace U2
