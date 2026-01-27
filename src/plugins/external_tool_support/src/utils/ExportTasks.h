/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Msa.h>
#include <U2Core/Task.h>

namespace U2 {

class CloneObjectTask;

/** Save Alignment Task (to CLUSTAL, NEXUS, ...) */
class SaveAlignmentTask : public Task {
    Q_OBJECT
public:
    SaveAlignmentTask(const Msa& ma, const QString& fileName, DocumentFormatId f, const QVariantMap& hints = QVariantMap());

    void run() override;

    virtual Document* getDocument() const;
    const QString& getUrl() const;
    const Msa& getMAlignment() const;

private:
    Msa ma;
    QString fileName;
    QVariantMap hints;
    DocumentFormatId format;
    QScopedPointer<Document> doc;
};

/**Export Alignment to Sequence Format (FASTA, ...) */
class SaveMSA2SequencesTask : public Task {
    Q_OBJECT
public:
    SaveMSA2SequencesTask(const Msa& ma, const QString& url, bool trimAli, const DocumentFormatId& format);

    void run() override;

    virtual Document* getDocument() const {
        return doc.data();
    }

private:
    QList<DNASequence> sequenceList;
    QString url;
    DocumentFormatId documentFormatId;
    QScopedPointer<Document> doc;
};

class SaveSequenceTask : public Task {
    Q_OBJECT
public:
    SaveSequenceTask(const QPointer<U2SequenceObject>& sequence, const QString& url, const DocumentFormatId& formatId);

private:
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    QPointer<U2SequenceObject> sequence;
    const QString url;
    const DocumentFormatId formatId;

    StateGuard locker;
    CloneObjectTask* cloneTask;
};

}  // namespace U2
