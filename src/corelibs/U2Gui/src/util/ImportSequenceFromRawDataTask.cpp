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

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ImportSequenceFromRawDataTask.h"

namespace U2 {

ImportSequenceFromRawDataTask::ImportSequenceFromRawDataTask(const U2DbiRef &dbiRef, const QString &folder, const DNASequence &sequence) :
    Task(tr("Import sequence from raw data"), TaskFlag_None),
    dbiRef(dbiRef),
    folder(folder),
    sequence(sequence)
{
    SAFE_POINT_EXT(dbiRef.isValid(), setError("Destination dbi ref is invalid"), );
}

QString ImportSequenceFromRawDataTask::getSequenceName() const {
    return sequence.getName();
}

const U2EntityRef & ImportSequenceFromRawDataTask::getEntityRef() const {
    return sequenceRef;
}

void ImportSequenceFromRawDataTask::run() {
    sequenceRef = U2SequenceUtils::import(stateInfo, dbiRef, folder, sequence);
}

}   // namespace U2
