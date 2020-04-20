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

#ifndef _U2_FILTER_ANNOTATIONS_BY_QUALIFIER_WORKER_H_
#define _U2_FILTER_ANNOTATIONS_BY_QUALIFIER_WORKER_H_

#include <U2Core/AnnotationData.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>


namespace U2 {
namespace LocalWorkflow {

class FilterAnnotationsByQualifierPrompter : public PrompterBase<FilterAnnotationsByQualifierPrompter> {
    Q_OBJECT
public:
    FilterAnnotationsByQualifierPrompter(Actor* p = 0) : PrompterBase<FilterAnnotationsByQualifierPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class FilterAnnotationsByQualifierWorker : public BaseWorker {
    Q_OBJECT
public:
    FilterAnnotationsByQualifierWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {};

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();
private slots:
    void sl_taskFinished(Task *t);
private:
    IntegralBus *input, *output;
    QList<SharedAnnotationData> inputAnns;
};

class FilterAnnotationsByQualifierWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    FilterAnnotationsByQualifierWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) { return new FilterAnnotationsByQualifierWorker(a); }
};

class FilterAnnotationsByQualifierTask : public Task {
    Q_OBJECT
public:
    FilterAnnotationsByQualifierTask(QList<SharedAnnotationData> &annotations, const QString &qName, const QString& qVal, bool acceptAnns)
        : Task(tr("Filter annotations by qualifier task"), TaskFlag_None), anns(annotations), qualName(qName), qualFilterVal(qVal), accept(acceptAnns)
    {

    }

    void run();

private:
    QList<SharedAnnotationData> &anns;
    QString qualName, qualFilterVal;
    bool accept;
};

} // LocalWorkflow namespace
} // U2 namespace

#endif //_U2_FILTER_ANNOTATIONS_BY_QUALIFIER_WORKER_H_
