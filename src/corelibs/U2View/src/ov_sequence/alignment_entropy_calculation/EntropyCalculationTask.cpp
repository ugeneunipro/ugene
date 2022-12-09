/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "EntropyCalculationTask.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/DNASequenceObject.h>

namespace U2 {
EntropyCalculationTask::EntropyCalculationTask(AnnotatedDNAView* _annotatedDNAView, const QString& _alignmentFilePath, const QString& _saveToPath): 
    Task(tr("Alignment entropy calculation"), TaskFlag_None), 
    annotatedDNAView(_annotatedDNAView),
    alignmentFilePath(_alignmentFilePath), 
    saveToPath(_saveToPath),
    loadDocumentTask(nullptr){
}

void EntropyCalculationTask::prepare() {
    QVariantMap hints;
    hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    loadDocumentTask = LoadDocumentTask::getDefaultLoadDocTask(alignmentFilePath, hints);
    if (loadDocumentTask == nullptr) {
        setError(tr("Cannot open file: %1").arg(alignmentFilePath));
        return;
    }
    addSubTask(loadDocumentTask);
}

void EntropyCalculationTask::run() {
}

QList<Task*> EntropyCalculationTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask == loadDocumentTask) {
        Document* doc = loadDocumentTask->getDocument();
        auto objects = doc->getObjects();
        if (objects.size() != 1) {
            setError(tr("More than 1 object found at %1").arg(alignmentFilePath));
            return res;
        }
        alignment = qobject_cast<MultipleSequenceAlignmentObject*>(objects.at(0));
        if (alignment == nullptr) {
            setError(tr("Cannot cast to MultipleSequenceAlignmentObject: %1").arg(alignmentFilePath));
            return res;
        }
        U2OpStatusImpl os;
        addSequenceTask = new AddSequenceObjectsToAlignmentTask(alignment, 
            {annotatedDNAView->getSequenceObjectsWithContexts().first()->getWholeSequence(os)});
        res << addSequenceTask;
    }
    
    return res;
}

}  // namespace U2
