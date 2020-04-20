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

#ifndef _U2_OPEN_DOCUMENT_TASK_H_
#define _U2_OPEN_DOCUMENT_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/GUrl.h>
#include <U2Core/AddDocumentTask.h>

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

enum LoadRemoteDocumentMode {
    LoadRemoteDocumentMode_LoadOnly = 1,
    LoadRemoteDocumentMode_AddToProject = 2,
    LoadRemoteDocumentMode_OpenView = 3
};

class U2GUI_EXPORT LoadRemoteDocumentAndAddToProjectTask : public Task {
    Q_OBJECT
public:
  
    LoadRemoteDocumentAndAddToProjectTask(const QString& accId, const QString& dbName);
    LoadRemoteDocumentAndAddToProjectTask(const QString& accId,
                                      const QString& dbName,
                                      const QString& fullpath,
                                      const QString& format = QString(),
                                      const QVariantMap& hints = QVariantMap(),
                                      LoadRemoteDocumentMode mode = LoadRemoteDocumentMode_OpenView);
    LoadRemoteDocumentAndAddToProjectTask(const GUrl& url);
    virtual void prepare();
    virtual QString generateReport() const;
protected:
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    QString     accNumber;
    QString     databaseName;
    QString     fileFormat;
    QString     fullpath;
    GUrl        docUrl;
    QVariantMap hints;
    LoadRemoteDocumentMode mode;
    LoadRemoteDocumentTask* loadRemoteDocTask;
};

class U2GUI_EXPORT OpenViewTask : public Task {
    Q_OBJECT
public:
    OpenViewTask(Document* d);

    static const int MAX_DOC_NUMBER_TO_OPEN_VIEWS;
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

}//namespace

#endif
