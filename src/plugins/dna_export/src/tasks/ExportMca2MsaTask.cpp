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

#include <U2Core/Counter.h>
#include <U2Core/GHints.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>

#include "ConvertMca2MsaTask.h"
#include "ExportMca2MsaTask.h"
#include "ExportTasks.h"

namespace U2 {

ExportMca2MsaTask::ExportMca2MsaTask(MultipleChromatogramAlignmentObject *mcaObject,
                                     const QString &fileName,
                                     const DocumentFormatId &formatId,
                                     bool includeReference)
    : DocumentProviderTask(tr("Export Sanger reads task"), TaskFlags_NR_FOSE_COSC),
      mcaObject(mcaObject),
      fileName(fileName),
      formatId(formatId),
      includeReference(includeReference),
      convertTask(NULL),
      exportTask(NULL)
{
    GCOUNTER(cvar, tvar, "ExportMca2MsaTask");
    SAFE_POINT_EXT(NULL != mcaObject, setError(L10N::nullPointerError("MCA object")), );
}

void ExportMca2MsaTask::prepare() {
    convertTask = new ConvertMca2MsaTask(mcaObject, includeReference);
    addSubTask(convertTask);
}

QList<Task *> ExportMca2MsaTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubTasks;
    CHECK_OP(stateInfo, newSubTasks);

    if (convertTask == subTask) {
        exportTask = new ExportAlignmentTask(convertTask->getMsa(), fileName, formatId);
        newSubTasks << exportTask;
    } else if (exportTask == subTask) {
        resultDocument = exportTask->takeDocument();
        resultDocument->getGHints()->set(DocumentReadingMode_SequenceAsAlignmentHint, true);
    }

    return newSubTasks;
}

}   // namespace U2
