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

#ifndef _STATISTIC_WORKERS_H_
#define _STATISTIC_WORKERS_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class DNAStatWorker:public BaseWorker {
    Q_OBJECT
public:
    DNAStatWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}

    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {};

private:
    float calcGCContent(const QByteArray &seq);
    float calcGC1Content(const QByteArray &seq);
    float calcGC2Content(const QByteArray &seq);
    float calcGC3Content(const QByteArray &seq);
    IntegralBus *input, *output;
};

class DNAStatWorkerFactory: public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    DNAStatWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) { return new DNAStatWorker(a); }
};

class DNAStatWorkerPrompter: public PrompterBase<DNAStatWorkerPrompter> {
    Q_OBJECT
public:
    DNAStatWorkerPrompter(Actor* p = 0) : PrompterBase<DNAStatWorkerPrompter>(p) {}
protected:
    QString composeRichDoc();
};


}//LocalWorkflow
}//U2

#endif
