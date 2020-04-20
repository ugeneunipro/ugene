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

#include <U2Core/SaveDocumentTask.h>

#include "BaseDocWriter.h"

namespace U2 {
namespace LocalWorkflow {

class BaseWriteAssemblyWorker : public BaseDocWriter {
    Q_OBJECT
public:
    BaseWriteAssemblyWorker(Actor *a);

protected:
    virtual void data2doc(Document *doc, const QVariantMap &data);
    virtual bool hasDataToWrite(const QVariantMap &data) const;
    virtual QSet<GObject *> getObjectsToWrite(const QVariantMap &data) const;
}; // BaseWriteAssemblyWorker

class WriteBAMWorker : public BaseWriteAssemblyWorker {
    Q_OBJECT
public:
    WriteBAMWorker(Actor *a);

protected:
    virtual bool isStreamingSupport() const;
    virtual Task * getWriteDocTask(Document *doc, const SaveDocFlags &flags);
    virtual void takeParameters(U2OpStatus &os);

private:
    bool buildIndex;
}; // WriteBAMWorker

class WriteAssemblyWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    WriteAssemblyWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // WriteAssemblyWorkerFactory

class WriteBAMTask : public Task {
public:
    WriteBAMTask(Document *doc, bool buildIndex, const SaveDocFlags &flags);

    virtual void run();

private:
    Document *doc;
    bool buildIndex;
    SaveDocFlags flags;
}; // WriteBAMTask

} // LocalWorkflow
} // U2
