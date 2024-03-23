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

#pragma once

#include <QPointer>

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/UnloadedObject.h>

namespace U2 {

class Document;
class SaveDocumentTask;

enum UnloadDocumentTask_SaveMode {
    UnloadDocumentTask_SaveMode_Ask,
    UnloadDocumentTask_SaveMode_NotSave,
    UnloadDocumentTask_SaveMode_Save
};

class U2GUI_EXPORT UnloadDocumentTask : public Task {
    Q_OBJECT
public:
    UnloadDocumentTask(Document* doc, bool save);
    ReportResult report() override;

    static QList<Task*> runUnloadTaskHelper(const QList<Document*>& docs, UnloadDocumentTask_SaveMode sm);
    static QString checkSafeUnload(Document* d);

private:
    QPointer<Document> doc;
    SaveDocumentTask* saveTask;
    StateLock* lock;
    static const QString ACTIVE_VIEW_ERROR;
};

class U2GUI_EXPORT ReloadDocumentTask : public Task {
    Q_OBJECT
public:
    ReloadDocumentTask(Document* d);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    void saveObjectRelationsFromDoc();
    void restoreObjectRelationsForDoc();
    static void restoreObjectRelationsForObject(GObject* obj,
                                                const QList<GObjectRelation>& relations);

    Document* doc;
    GUrl url;
    Task* removeDocTask;
    Task* openDocTask;
    QMultiMap<QString, GObjectRelation> savedObjectRelations;
};

}  // namespace U2
