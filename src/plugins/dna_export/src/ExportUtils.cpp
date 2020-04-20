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

#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "ExportSequenceTask.h"
#include "ExportSequencesDialog.h"
#include "ExportTasks.h"
#include "ExportUtils.h"
#include "dialogs/ExportMca2MsaDialog.h"
#include "tasks/ExportMca2MsaTask.h"

namespace U2 {

void ExportUtils::loadDNAExportSettingsFromDlg(ExportSequenceTaskSettings& s, ExportSequencesDialog *d)  {
    s.fileName = d->file;
    s.merge = d->merge;
    s.mergeGap = d->mergeGap;
    s.allAminoFrames = d->translateAllFrames;
    s.strand = d->strand;
    s.formatId = d->formatId;
    s.mostProbable = d->mostProbable;
    s.saveAnnotations = d->withAnnotations;
    s.sequenceName = d->sequenceName;
}

Task* ExportUtils::wrapExportTask(DocumentProviderTask* t, bool addToProject) {
    if (!addToProject) {
        return t;
    }
    return new AddExportedDocumentAndOpenViewTask(t);
}

QString ExportUtils::genUniqueName(const QSet<QString>& names, QString prefix) {
    if (!names.contains(prefix)) {
        return prefix;
    }
    QString name = prefix;
    int i=0;
    do {
        if (!names.contains(name)) {
            break;
        }
        name = prefix + "_" + QString::number(++i);
    } while(true);
    return name;
}

void ExportUtils::launchExportMca2MsaTask(MultipleChromatogramAlignmentObject *mcaObject) {
    SAFE_POINT(NULL != mcaObject, "Can't cast the object to MultipleChromatogramAlignmentObject", );

    Document *document = mcaObject->getDocument();
    QString defaultUrl = GUrlUtils::getNewLocalUrlByFormat(document->getURL(), mcaObject->getGObjectName(), BaseDocumentFormats::UGENEDB, "");

    QObjectScopedPointer<ExportMca2MsaDialog> dialog = new ExportMca2MsaDialog(defaultUrl, AppContext::getMainWindow()->getQMainWindow());
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );
    CHECK(result != QDialog::Rejected, );

    Task *task = ExportUtils::wrapExportTask(new ExportMca2MsaTask(mcaObject,
                                                                   dialog->getSavePath(),
                                                                   dialog->getFormatId(),
                                                                   dialog->getIncludeReferenceOption()),
                                             dialog->getAddToProjectOption());
    TaskWatchdog::trackResourceExistence(mcaObject, task, tr("A problem occurred during export MCA to MSA. The MCA is no more available."));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

}   // namespace U2
