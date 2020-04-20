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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include "../ProjectFilterNames.h"

#include "SequenceAccFilterTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// SequenceAccFilterTask
//////////////////////////////////////////////////////////////////////////

SequenceAccFilterTask::SequenceAccFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::SEQUENCE_ACC_FILTER_NAME, docs)
{

}

bool SequenceAccFilterTask::filterAcceptsObject(GObject *obj) {
    U2SequenceObject *seqObject = qobject_cast<U2SequenceObject *>(obj);
    CHECK(NULL != seqObject, false);
    return settings.nameFilterAcceptsString(seqObject->getSequenceInfo()[DNAInfo::ACCESSION].toString());
}

//////////////////////////////////////////////////////////////////////////
/// SequenceAccFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask * SequenceAccFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new SequenceAccFilterTask(settings, docs);
}

} // namespace U2
