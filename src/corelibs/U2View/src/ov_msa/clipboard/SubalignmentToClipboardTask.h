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

#ifndef _U2_SUBALIGNMENT_TO_CLIPBOARD_TASK_H_
#define _U2_SUBALIGNMENT_TO_CLIPBOARD_TASK_H_

#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include <U2View/MSAEditor.h>

namespace U2 {

////////////////////////////////////////////////////////////////////////////////
class PrepareMsaClipboardDataTask : public Task {
    Q_OBJECT
public:
    PrepareMsaClipboardDataTask(const QList<qint64>& rowIds, const U2Region& collumnRange, TaskFlags taskFlags = TaskFlags_NR_FOSE_COSC);

    QList<qint64> rowIds;
    U2Region columnRange;
    QString resultText;
};

////////////////////////////////////////////////////////////////////////////////
class FormatsMsaClipboardTask : public PrepareMsaClipboardDataTask {
    Q_OBJECT
public:
    FormatsMsaClipboardTask(MultipleSequenceAlignmentObject* msaObj, const QList<qint64>& rowIds, const U2Region& columnRange, const DocumentFormatId& formatId);

    void prepare() override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    static CreateSubalignmentSettings createSettings(const QList<qint64>& rowIds, const U2Region& columnRange, const DocumentFormatId& formatId, U2OpStatus& os);

private:
    CreateSubalignmentTask* createSubalignmentTask;
    MultipleSequenceAlignmentObject* msaObj;
    DocumentFormatId formatId;
};

class RichTextMsaClipboardTask : public PrepareMsaClipboardDataTask {
public:
    RichTextMsaClipboardTask(MaEditor* maEditor, const QList<qint64>& rowIds, const U2Region& columnRange);
    void prepare() override;

private:
    MaEditor* maEditor;
};

////////////////////////////////////////////////////////////////////////////////
class MsaClipboardDataTaskFactory {
public:
    /** Returns a new PrepareMsaClipboardDataTask instance. */
    static PrepareMsaClipboardDataTask* newInstance(MSAEditor* context, const QList<qint64>& maRowIds, const U2Region& columnRange, const DocumentFormatId& formatId);
};

////////////////////////////////////////////////////////////////////////////////
class SubalignmentToClipboardTask : public Task {
    Q_OBJECT
public:
    SubalignmentToClipboardTask(MSAEditor* maEditor, const QList<qint64>& rowIds, const U2Region& columnRange, const DocumentFormatId& formatId);

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    DocumentFormatId formatId;
    PrepareMsaClipboardDataTask* prepareDataTask = nullptr;
};

}  // namespace U2

#endif
