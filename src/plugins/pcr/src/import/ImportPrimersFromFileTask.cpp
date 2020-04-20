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

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>

#include "ImportPrimerFromObjectTask.h"
#include "ImportPrimersFromFileTask.h"
#include "PrimerStatistics.h"

namespace U2 {

ImportPrimersFromFileTask::ImportPrimersFromFileTask(const QString &filePath) :
    Task(tr("Import primers from local file: %1").arg(filePath),
         TaskFlags(TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported)),
    filePath(filePath),
    loadTask(NULL)
{
    SAFE_POINT_EXT(!filePath.isEmpty(), setError("File path is empty"), );
}

void ImportPrimersFromFileTask::prepare() {
    loadTask = LoadDocumentTask::getCommonLoadDocTask(GUrl(filePath));
    CHECK_EXT(NULL != loadTask, setError(tr("Internal error: can't load the file '%1'").arg(filePath)), );
    addSubTask(loadTask);
}

QList<Task *> ImportPrimersFromFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK(loadTask == subTask, result);
    CHECK_OP(stateInfo, result);

    Document *document = loadTask->getDocument();
    SAFE_POINT_EXT(NULL != document, setError(tr("Document wasn't loaded")), result);
    QList<GObject *> objects = document->findGObjectByType(GObjectTypes::SEQUENCE);
    CHECK_EXT(!objects.isEmpty(), setError(tr("There are no sequence objects in the file '%1' to convert them to primers").arg(filePath)), result);
    foreach (GObject *object, objects) {
        result << new ImportPrimerFromObjectTask(object);
    }
    return result;
}

QString ImportPrimersFromFileTask::generateReport() const {
    QString report = QString("<b>%1</b>: <font color='%2'>%3</font>").arg(filePath);
    if (isCanceled()) {
        return report.arg(Theme::errorColorLabelHtmlStr()).arg(tr("cancelled"));
    }

    if (hasError()) {
        return report.arg(Theme::errorColorLabelHtmlStr()).arg(tr("error")) + ": " + getError();
    }

    int tasksWithError = 0;
    int primersImported = 0;
    foreach (const QPointer<Task> &subTask, getSubtasks()) {
        if (Q_UNLIKELY(loadTask == subTask.data())) {
            continue;
        }

        if (subTask->hasError()) {
            tasksWithError++;
        } else {
            primersImported++;
        }
    }

    if (0 == primersImported) {
        return report.arg(Theme::errorColorLabelHtmlStr()).arg(tr("error")) + ": " + tr("there are no applicable sequences");
    }

    return report.arg(Theme::successColorLabelHtmlStr())
                 .arg(tr("success")) +
                     " (" + "primers imported: " + QString::number(primersImported) +
                     (tasksWithError > 0 ? "; sequences were not converted: " + QString::number(tasksWithError) : "")
                     + ")";
}

}   // namespace U2
