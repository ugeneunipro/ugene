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

#ifndef _U2_ADD_SEQUENCES_TO_ALIGNMENT_TASK_H_
#define _U2_ADD_SEQUENCES_TO_ALIGNMENT_TASK_H_

#include <QPointer>

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2Type.h>

namespace U2 {

class StateLock;
class LoadDocumentTask;
class DNASequence;
class U2MsaDbi;
class U2UseCommonUserModStep;

/** Utility methods used by the AddSequenceObjectsToAlignment tasks family. */

class U2CORE_EXPORT AddSequenceObjectsToAlignmentUtils : public QObject {
    Q_OBJECT
public:
    /**
     * Adds sequences to the MSA object. Changes MSA alphabet if needed.
     * Returns the result as a MaModificationInfo data structure.
     */
    static MaModificationInfo addObjectsToAlignment(U2OpStatus& os,
                                                    MultipleSequenceAlignmentObject* obj,
                                                    const QList<DNASequence>& sequenceList,
                                                    int insertRowIndex,
                                                    bool recheckNewSequenceAlphabetOnMismatch);
};

class U2CORE_EXPORT AddSequenceObjectsToAlignmentTask : public Task {
    Q_OBJECT
public:
    /**
     * Adds sequences to the alignment starting from the given insertRowIndex: making the first inserted row to have row index = insertRowIndex.
     * For example insertRowIndex = 0 pre-appends sequences to the alignment.
     * If insertRowIndex < 0 or insertRowIndex >= row count - appends sequences to the end of the alignment rows.
     */
    AddSequenceObjectsToAlignmentTask(MultipleSequenceAlignmentObject* obj,
                                      const QList<DNASequence>& sequenceList,
                                      int insertRowIndex = -1,
                                      bool recheckNewSequenceAlphabetOnMismatch = false);

    /** Runs addSequencesToAlignment to process sequence list. */
    void run() override;

    const MaModificationInfo& getMaModificationInfo() const;

protected:
    /** Original list of sequences to add into the alignment. */
    QList<DNASequence> sequenceList;

    /** Insert location for the sequence list. '-1' or any other illegal value results to appending after the last row in the selection.*/
    int insertMaRowIndex = -1;

    QPointer<MultipleSequenceAlignmentObject> maObj;

private:
    StateLock* stateLock = nullptr;
    MaModificationInfo mi;

    /*
     * If re-check alphabet is true and the alphabet of the new sequence is not the same as the alphabet of the alignment
     * the task will re-test the sequence data if it fits into the alignment alphabet first and fall-back to the sequence alphabet only if it does not.
     *
     * Example: paste symbol 'T' to amino alignment: 'T' is detected as Nucleic while it is also valid for Amino!
     */
    bool recheckNewSequenceAlphabetOnMismatch = false;
};

class U2CORE_EXPORT AddSequencesFromFilesToAlignmentTask : public AddSequenceObjectsToAlignmentTask {
    Q_OBJECT
public:
    AddSequencesFromFilesToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QStringList& urls, int insertMaRowIndex);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private slots:
    void sl_onCancel();

private:
    QStringList urlList;
    LoadDocumentTask* loadTask;
};

class U2CORE_EXPORT AddSequencesFromDocumentsToAlignmentTask : public AddSequenceObjectsToAlignmentTask {
    Q_OBJECT
public:
    AddSequencesFromDocumentsToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QList<Document*>& docs, int insertMaRowIndex, bool recheckNewSequenceAlphabets);

    void prepare() override;

private:
    QList<Document*> docs;
};

}  // namespace U2

#endif  //_U2_ADD_SEQUENCES_TO_ALIGNMENT_TASK_H_
