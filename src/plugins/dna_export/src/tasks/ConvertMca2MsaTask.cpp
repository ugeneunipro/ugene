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

#include "ConvertMca2MsaTask.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/MsaObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

ConvertMca2MsaTask::ConvertMca2MsaTask(MsaObject* mcaObject, bool includeReference)
    : Task(tr("Convert MCA to MSA task"), TaskFlag_None),
      mcaObject(mcaObject),
      includeReference(includeReference),
      locker(StateLocker(mcaObject)) {
    SAFE_POINT_EXT(mcaObject != nullptr, setError(L10N::nullPointerError("MCA object")), );
}

Msa ConvertMca2MsaTask::getMsa() const {
    return msa;
}

void ConvertMca2MsaTask::prepare() {
    locker.lock();
}

void ConvertMca2MsaTask::run() {
    msa = Msa(mcaObject->getGObjectName(), mcaObject->getAlphabet());

    if (includeReference) {
        U2SequenceObject* referenceObject = mcaObject->getReferenceObj();
        msa->addRow(referenceObject->getSequenceName(), referenceObject->getWholeSequenceData(stateInfo));
        CHECK_OP(stateInfo, );
    }

    foreach (const MsaRow& mcaRow, mcaObject->getAlignment()->getRows()) {
        msa->addRow(mcaRow->getName(), mcaRow->getSequence(), mcaRow->getGaps(), stateInfo);
        CHECK_OP(stateInfo, );
    }
}

Task::ReportResult ConvertMca2MsaTask::report() {
    locker.unlock();
    return ReportResult_Finished;
}

}  // namespace U2
