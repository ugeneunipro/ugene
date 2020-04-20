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

#include <QApplication>
#include <QMessageBox>

#include "ReloadDocumentsTask.h"

#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/UnloadDocumentTask.h>

namespace U2 {

ReloadDocumentsTask::ReloadDocumentsTask(const QList<Document*>& _docs2Reload)
                : Task(tr("Reload documents task"), TaskFlag_NoRun |
                                                    TaskFlag_MinimizeSubtaskErrorText),
                  docs2Reload(_docs2Reload) {
    GCOUNTER(cvar, tvar, "ReloadDocumentsTask");

    foreach(Document* doc, docs2Reload) {
        QString unloadErr = UnloadDocumentTask::checkSafeUnload(doc);
        if (!unloadErr.isEmpty()) {
            QMessageBox::warning(QApplication::activeWindow(),
                                 U2_APP_TITLE,
                                 tr("Document '%1' can't be unloaded. '%2'")
                                    .arg(doc->getName(), unloadErr));
            doc->setLastUpdateTime();
            continue;
        }
    }
}

void ReloadDocumentsTask::prepare() {
    foreach(Document* doc, docs2Reload) {
        addSubTask(new ReloadDocumentTask(doc));
    }
}

Task::ReportResult ReloadDocumentsTask::report() {
    CHECK(!subTaskStateInfoErrors.isEmpty(), ReportResult_Finished);

    setConcatenateChildrenErrors(true);
    setReportingSupported(true);
    stateInfo.setError(tr("Document(s) reloading failed."));

    return ReportResult_Finished;
}

QString ReloadDocumentsTask::generateReport() const {
    QString report;
    report += tr("The following errors occurred during the document(s) reloading: <ul>");
    for (int i = 0; i < subTaskStateInfoErrors.size(); i++) {
        report += QString("<li>'%1': %2</li>")
                          .arg(i + 1)
                          .arg(subTaskStateInfoErrors[i]);
    }
    report += "</ul>";

    return report;
}

QList<Task*> ReloadDocumentsTask::onSubTaskFinished(Task* subTask) {
    if (subTask->hasError()) {
        subTaskStateInfoErrors << subTask->getError();
    }

    return QList<Task*>();
}

}// namespace U2
