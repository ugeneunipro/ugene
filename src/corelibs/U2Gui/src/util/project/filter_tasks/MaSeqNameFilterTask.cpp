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

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include "../ProjectFilterNames.h"

#include "MaSeqNameFilterTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// MaSeqNameFilterTask
//////////////////////////////////////////////////////////////////////////

static bool isFilteredByMASequenceName(const MultipleAlignmentObject* maObj, const ProjectTreeControllerModeSettings &settings) {
    CHECK(NULL != maObj, false);
    for (int i = 0, n = maObj->getNumRows(); i < n; ++i) {
        if (settings.nameFilterAcceptsString(maObj->getRow(i)->getName())) {
            return true;
        }
    }
    return false;
}

MsaSeqNameFilterTask::MsaSeqNameFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs) 
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MSA_SEQ_NAME_FILTER_NAME, docs) {
}

bool MsaSeqNameFilterTask::filterAcceptsObject(GObject *obj) {
    return isFilteredByMASequenceName(qobject_cast<MultipleSequenceAlignmentObject *>(obj), settings);
}

McaReadNameFilterTask::McaReadNameFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs) 
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_READ_NAME_FILTER_NAME, docs) {
}


bool McaReadNameFilterTask::filterAcceptsObject(GObject *obj) {
    return isFilteredByMASequenceName(qobject_cast<MultipleChromatogramAlignmentObject *>(obj), settings);
}

McaReferenceNameFilterTask::McaReferenceNameFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::MCA_REFERENCE_NAME_FILTER_NAME, docs)
{
}

bool McaReferenceNameFilterTask::filterAcceptsObject(GObject *obj) {
    MultipleChromatogramAlignmentObject* mcaObj = qobject_cast<MultipleChromatogramAlignmentObject*>(obj);
    CHECK(NULL != mcaObj, false);
    U2SequenceObject* refObj = mcaObj->getReferenceObj();
    if (refObj != NULL && settings.nameFilterAcceptsString(refObj->getSequenceName())) {
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
/// MaSeqNameFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask * MsaSeqNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new MsaSeqNameFilterTask(settings, docs);
}

AbstractProjectFilterTask * McaReadNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new McaReadNameFilterTask(settings, docs);
}

AbstractProjectFilterTask * McaReferenceNameFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new McaReferenceNameFilterTask(settings, docs);
}


} // namespace U2
