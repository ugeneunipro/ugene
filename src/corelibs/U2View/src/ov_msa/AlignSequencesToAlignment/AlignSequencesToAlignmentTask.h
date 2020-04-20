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

#ifndef _U2_ALIGN_SEQUENCES_TO_ALIGNMENT_TASK_H_
#define _U2_ALIGN_SEQUENCES_TO_ALIGNMENT_TASK_H_

#include <QPointer>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/Task.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/U2AlphabetUtils.h>

#include <QPointer>

#include <U2Algorithm/AlignSequencesToAlignmentTaskSettings.h>

namespace U2 {

class StateLock;
class MSAEditor;

class SequenceObjectsExtractor {
public:
    SequenceObjectsExtractor();

    void setAlphabet(const DNAAlphabet* newAlphabet);

    void extractSequencesFromDocuments(const QList<Document*>& documentsList);

    void extractSequencesFromDocument(Document* doc);

    void extractSequencesFromObjects(const QList<GObject*>& objects);

    const QStringList& getErrorList() const;

    const DNAAlphabet* getAlphabet() const;

    const QList<U2EntityRef>& getSequenceRefs() const;

    const QStringList& getSequenceNames() const;

    qint64 getMaxSequencesLength() const;

    const QList<Document*>& getUsedDocuments() const;

    const DNAAlphabet* resultingAlphabet() const;

private:
    void checkAlphabet(const DNAAlphabet* alphabet, const QString& objectName);

    QList<U2EntityRef> sequenceRefs;
    QStringList errorList;
    const DNAAlphabet* seqsAlphabet;
    bool extractFromMsa;
    QStringList sequenceNames;
    qint64 sequencesMaxLength;
    QList<Document*> usedDocuments;
};

class LoadSequencesTask : public Task {
    Q_OBJECT
public:
    LoadSequencesTask(const DNAAlphabet* msaAlphabet, const QStringList& filesWithSequences);
    void prepare();

    QList<Task*> onSubTaskFinished(Task* subTask);

    ReportResult report();

    const SequenceObjectsExtractor& getExtractor() const;

private:
    const DNAAlphabet*          msaAlphabet;
    QStringList                 urls;

    SequenceObjectsExtractor    extractor;

    static const int maxErrorListSize;
private:
    void setupError();
};

class AlignSequencesToAlignmentTask : public Task {
    Q_OBJECT
public:
    AlignSequencesToAlignmentTask(MultipleSequenceAlignmentObject* obj, const SequenceObjectsExtractor& extractor, bool forceUseUgeneNativeAligner = false);
    void prepare();
    ReportResult report();
private:
    void fillSettingsByDefault();

    QPointer<MultipleSequenceAlignmentObject>  maObj;
    QStringList                 urls;
    StateLock*                  stateLock;
    StateLock*                  docStateLock;
    qint64 sequencesMaxLength;
    AlignSequencesToAlignmentTaskSettings settings;
    QList<Document*> usedDocuments;
    const DNAAlphabet *initialMsaAlphabet;
    SequenceObjectsExtractor extr;
};

class LoadSequencesAndAlignToAlignmentTask : public Task {
    Q_OBJECT
public:
    LoadSequencesAndAlignToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QStringList& urls, bool forceUseUgeneNativeAligner = false);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    bool propagateSubtaskError() override;

private:
    QStringList                 urls;
    QPointer<MultipleSequenceAlignmentObject>  maObj;
    LoadSequencesTask*  loadSequencesTask;
    bool forceUseUgeneNativeAligner;
};

}// namespace

#endif //_U2_ALIGN_SEQUENCES_TO_ALIGNMENT_TASK_H_
