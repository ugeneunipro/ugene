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

#ifndef _SCHEMA_WORKER_H_
#define _SCHEMA_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class SchemaWorker : public BaseWorker {
    Q_OBJECT
public:
    SchemaWorker(Actor *a);

    bool isReady() const;
    bool isDone() const;
    Task *tick();
    void init();
    void cleanup();
};

class SchemaWorkerFactory : public DomainFactory {
public:
    SchemaWorkerFactory(QString name) : DomainFactory(name) {}
    static bool init(Schema *schema, const QString &name, const QString &actorFilePath);
    virtual Worker* createWorker(Actor* a) {return new SchemaWorker(a);}
};

class SchemaWorkerPrompter : public PrompterBase<SchemaWorkerPrompter> {
    Q_OBJECT
public:
    SchemaWorkerPrompter(Actor *p = NULL): PrompterBase<SchemaWorkerPrompter>(p) {}
    QString composeRichDoc();
};

} // LocalWorkflow
} // U2


#endif // _SCHEMA_WORKER_H_
