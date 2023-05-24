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

#include <U2Core/AddDocumentTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

namespace U2 {

class Document;
class LoadUnloadedDocumentTask;
class LoadRemoteDocumentTask;
class DocumentProviderTask;

class U2GUI_EXPORT LoadUnloadedDocumentAndOpenViewTask : public Task {
    Q_OBJECT
public:
    LoadUnloadedDocumentAndOpenViewTask(Document* d);

    Document* getDocument();

protected:
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    void clearResourceUse();

    LoadUnloadedDocumentTask* loadUnloadedTask;
};

class U2GUI_EXPORT LoadRemoteDocumentAndAddToProjectTask : public Task {
    Q_OBJECT
public:
    LoadRemoteDocumentAndAddToProjectTask(const QString& accId,
                                          const QString& dbName,
                                          const QString& fullpath,
                                          const QString& format = QString(),
                                          const QVariantMap& hints = QVariantMap(),
                                          bool openView = true);
    LoadRemoteDocumentAndAddToProjectTask(const GUrl& url);
    void prepare() override;
    QString generateReport() const override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask);

    LoadRemoteDocumentAndAddToProjectTask(const QVariantMap& hints);

private:
    QString accNumber;
    QString databaseName;
    QString fileFormat;
    QString fullpath;
    GUrl docUrl;
    QVariantMap hints;
    bool openView = true;
    LoadRemoteDocumentTask* loadRemoteDocTask = nullptr;
};

class U2GUI_EXPORT OpenViewTask : public Task {
    Q_OBJECT
public:
    OpenViewTask(Document* d);

    static const int MAX_DOC_NUMBER_TO_OPEN_VIEWS;
    static const QString IGNORE_MODAL_WIDGET;

protected:
    void prepare();

private:
    Document* doc;
};

class U2GUI_EXPORT AddDocumentAndOpenViewTask : public Task {
    Q_OBJECT
public:
    AddDocumentAndOpenViewTask(Document* d, const AddDocumentTaskConfig& conf = AddDocumentTaskConfig());
    AddDocumentAndOpenViewTask(DocumentProviderTask* d, const AddDocumentTaskConfig& conf = AddDocumentTaskConfig());

protected:
    QList<Task*> onSubTaskFinished(Task* t);
};

}  // namespace U2
