/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ClipboardController.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/MsaObject.h>

namespace U2 {

const QString U2Clipboard::UGENE_MIME_TYPE = "text/ugene";

///////////////////
/// PasteFactory
PasteFactory::PasteFactory(QObject* parent)
    : QObject(parent) {
}

///////////////////
/// PasteTask
PasteTask::PasteTask(TaskFlags flags)
    : Task(tr("Paste data"), flags) {
}

void PasteTask::processDocument(Document*) {
}

///////////////////
/// PasteUtils
QList<DNASequence> PasteUtils::getSequences(const QList<Document*>& docs, U2OpStatus& os) {
    QList<DNASequence> res;

    for (Document* doc : qAsConst(docs)) {
        for (GObject* seqObj : doc->findGObjectByType(GObjectTypes::SEQUENCE)) {
            auto casted = qobject_cast<U2SequenceObject*>(seqObj);
            if (casted == nullptr) {
                continue;
            }
            DNASequence seq = casted->getWholeSequence(os);
            if (os.hasError()) {
                continue;
            }
            seq.alphabet = casted->getAlphabet();
            res.append(seq);
        }
        const QList<GObject*> msaObjectList = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        for (GObject* msaObj : qAsConst(msaObjectList)) {
            auto casted = qobject_cast<MsaObject*>(msaObj);
            if (casted == nullptr) {
                continue;
            }
            QVector<MsaRow> msaRowList = casted->getAlignment()->getRows();
            for (const MsaRow& row : qAsConst(msaRowList)) {
                DNASequence seq = row->getSequence();
                seq.seq = row->getData();
                seq.alphabet = casted->getAlphabet();
                res.append(seq);
            }
        }
    }

    return res;
}

}  // namespace U2
