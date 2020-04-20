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

#ifndef _U2_CREATE_SEQUENCE_FROM_TEXT_AND_OPEN_VIEW_TASK_H_
#define _U2_CREATE_SEQUENCE_FROM_TEXT_AND_OPEN_VIEW_TASK_H_

#include <U2Core/DNASequence.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

namespace U2 {

class Document;
class DocumentFormat;
class ImportSequenceFromRawDataTask;

class CreateSequenceFromTextAndOpenViewTask : public Task {
    Q_OBJECT
public:
    CreateSequenceFromTextAndOpenViewTask(const QList<DNASequence> &sequences, const QString &formatId, const GUrl &saveToPath, bool saveImmediately);

private:
    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

    QList<Task *> prepareImportSequenceTasks();
    Document * createEmptyDocument();
    void addDocument();

    const QList<DNASequence> sequences;
    DocumentFormat *format;
    const GUrl saveToPath;
    const bool saveImmediately;
    Task *openProjectTask;
    QList<Task *> importTasks;
    int importedSequences;
    Document *document;
};

}   // namespace U2

#endif // _U2_CREATE_SEQUENCE_FROM_TEXT_AND_OPEN_VIEW_TASK_H_
