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

#ifndef _U2_FIND_WORKER_H_
#define _U2_FIND_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Algorithm/FindAlgorithm.h>
#include <U2Algorithm/FindAlgorithmTask.h>

namespace U2 {

namespace LocalWorkflow {

class FindPrompter : public PrompterBase<FindPrompter> {
    Q_OBJECT
public:
    FindPrompter(Actor* p = 0) : PrompterBase<FindPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class FindWorker : public BaseWorker {
    Q_OBJECT
public:
    FindWorker(Actor* a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished(Task*);

protected:
    IntegralBus *input, *output;
    QString resultName;
    QMap<Task*, QByteArray> patterns;
    QMap<Task*, QPair< QString, QByteArray> > filePatterns;
    QList<QPair<QString, QString> > namesPatterns;
    bool patternFileLoaded;
    bool useNames;
};

class FindWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    FindWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new FindWorker(a);}
};

class FindAllRegionsTask : public Task { //FIXME this is temporary solution until FindAlgorithmTask moved to SequenceWalker
    Q_OBJECT
public:
    FindAllRegionsTask(const FindAlgorithmTaskSettings& s, const QList<AnnotationData> &);
    virtual void prepare();
    QList<FindAlgorithmResult> getResult();
private:
    FindAlgorithmTaskSettings cfg;
    QList<AnnotationData> regions;
};

} // Workflow namespace
} // U2 namespace

#endif
