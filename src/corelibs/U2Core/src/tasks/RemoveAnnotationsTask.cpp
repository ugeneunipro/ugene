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

#include <U2Core/U2SafePoints.h>

#include "RemoveAnnotationsTask.h"

namespace U2 {

RemoveAnnotationsTask::RemoveAnnotationsTask(AnnotationTableObject *ao, const QString &gName)
    : Task("Remove Annotations Task",TaskFlag_NoRun), aobj(ao), groupName(gName)
{
    SAFE_POINT(!aobj.isNull(), "Invalid annotation table detected!", );
}

Task::ReportResult RemoveAnnotationsTask::report() {
    AnnotationGroup *rootGroup = aobj->getRootGroup();
    AnnotationGroup *subGroup = rootGroup->getSubgroup(groupName, false);
    if (subGroup == NULL) { // subgroup having @groupName does not exist
        return ReportResult_Finished;
    }

    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    if (aobj->isStateLocked()) {
        stateInfo.setDescription(tr("Waiting for object lock released"));
        return ReportResult_Finished;
    }

    if (!subGroup->hasAnnotations()) {
        return ReportResult_Finished;
    }

    rootGroup->removeSubgroup(subGroup);

    return ReportResult_Finished;
}

} // namespace U2
