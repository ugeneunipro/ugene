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

#ifndef _U2_ADD_DOCUMENT_TASK_H_
#define _U2_ADD_DOCUMENT_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

class Document;
class DocumentProviderTask;

class AddDocumentTaskConfig {
public:
    AddDocumentTaskConfig() : createProjectIfNeeded(true), unloadExistingDocument(false) {}

    bool createProjectIfNeeded;
    bool unloadExistingDocument;
};
/**  Adds document to active project. Waits for locks if any */
class U2CORE_EXPORT AddDocumentTask : public Task {
    Q_OBJECT
public:
    AddDocumentTask(Document * d, const AddDocumentTaskConfig& c = AddDocumentTaskConfig());
    AddDocumentTask(DocumentProviderTask* dp, const AddDocumentTaskConfig& c = AddDocumentTaskConfig());

    ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);
    Document* getDocument() { return document; }
private:

    Document*               document;
    DocumentProviderTask*   dpt;
    AddDocumentTaskConfig   conf;
};


}//namespace

#endif
