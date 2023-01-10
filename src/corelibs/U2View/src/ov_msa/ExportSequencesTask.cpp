/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ExportSequencesTask.h"

#include <QFile>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

ExportSequencesTask::ExportSequencesTask(const MultipleSequenceAlignment& msa, const QSet<qint64>& rowIds, bool trimGaps, bool addToProjectFlag, const QString& dirUrl, const DocumentFormatId& format, const QString& extension, const QString& customFileName)
    : Task(tr("Export selected sequences from alignment"), TaskFlags_NR_FOSE_COSC),
      addToProjectFlag(addToProjectFlag),
      dirUrl(dirUrl),
      format(format),
      extension(extension),
      customFileName(customFileName) {
    sequences = MSAUtils::convertMsaToSequenceList(msa, stateInfo, trimGaps, rowIds);
}

void ExportSequencesTask::prepare() {
    QList<Task*> tasks;
    QSet<QString> existingFilenames;
    for (const DNASequence& sequence : qAsConst(sequences)) {
        QString filename = GUrlUtils::fixFileName(customFileName.isEmpty() ? sequence.getName() : customFileName);
        QString filePath = GUrlUtils::prepareFileLocation(dirUrl + "/" + filename + "." + extension, stateInfo);
        CHECK_OP(stateInfo, );

        filePath = GUrlUtils::rollFileName(filePath, "_", existingFilenames);
        existingFilenames.insert(filePath);
        GUrl url(filePath);
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(format);
        SAFE_POINT(df != nullptr, "Cant get DocuemtFormat by given DocumentFormatId", );

        QScopedPointer<Document> doc(df->createNewLoadedDocument(iof, filePath, stateInfo));
        CHECK_OP(stateInfo, );

        QVariantMap hints;
        hints.insert(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER);
        U2EntityRef ref = U2SequenceUtils::import(stateInfo, doc->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, sequence, sequence.alphabet->getId());
        CHECK_OP(stateInfo, );

        auto sequenceObject = new U2SequenceObject(sequence.getName(), ref);
        doc->addObject(sequenceObject);

        Document* takenDoc = doc.take();
        auto saveTask = new SaveDocumentTask(takenDoc, takenDoc->getIOAdapterFactory(), takenDoc->getURL());
        saveTask->addFlag(addToProjectFlag ? SaveDoc_OpenAfter : SaveDoc_DestroyAfter);
        addSubTask(saveTask);
    }
}

}  // namespace U2
