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

#ifndef __IMPORT_ANNOTATIONS_WORKER_H_
#define __IMPORT_ANNOTATIONS_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Core/AnnotationData.h>

namespace U2 {
namespace LocalWorkflow {

class ImportAnnotationsWorker : public BaseWorker {
    Q_OBJECT
public:
    ImportAnnotationsWorker(Actor * p) : BaseWorker(p), inPort(NULL), outPort(NULL) {}

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

private slots:
    void sl_docsLoaded(Task *);

private:
    IntegralBus * inPort;
    IntegralBus * outPort;
    QMap<Task *, QList<SharedAnnotationData> > annsMap;

private:
    void addTaskAnnotations(const QVariant &data, Task *t);
}; // ImportAnnotationsWorker

class ImportAnnotationsWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    ImportAnnotationsWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker * createWorker(Actor* a);

}; // ImportAnnotationsWorkerFactory

} // LocalWorkflow
} // U2

#endif // __IMPORT_ANNOTATIONS_WORKER_H_
