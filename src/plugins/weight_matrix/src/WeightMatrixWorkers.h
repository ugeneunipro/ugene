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

#ifndef _U2_WEIGHT_MATRIX_WORKER_H_
#define _U2_WEIGHT_MATRIX_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "WeightMatrixAlgorithm.h"
#include "WeightMatrixSearchTask.h"

namespace U2 {

namespace LocalWorkflow {

class PWMatrixBuildPrompter : public PrompterBase<PWMatrixBuildPrompter> {
    Q_OBJECT
public:
    PWMatrixBuildPrompter(Actor* p = 0) : PrompterBase<PWMatrixBuildPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class PFMatrixBuildPrompter : public PrompterBase<PFMatrixBuildPrompter> {
    Q_OBJECT
public:
    PFMatrixBuildPrompter(Actor* p = 0) : PrompterBase<PFMatrixBuildPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class PWMatrixSearchPrompter : public PrompterBase<PWMatrixSearchPrompter> {
    Q_OBJECT
public:
    PWMatrixSearchPrompter(Actor* p = 0) : PrompterBase<PWMatrixSearchPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class PWMatrixBuildWorker : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    static void registerProto();

    PWMatrixBuildWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    IntegralBus    *input, *output;
    PMBuildSettings         cfg;
    DataTypePtr             mtype;
};

class PFMatrixBuildWorker : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    static void registerProto();

    PFMatrixBuildWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    IntegralBus    *input, *output;
    PMBuildSettings         cfg;
    DataTypePtr             mtype;
};

class PFMatrixConvertPrompter : public PrompterBase<PFMatrixConvertPrompter> {
    Q_OBJECT
public:
    PFMatrixConvertPrompter(Actor* p = 0) : PrompterBase<PFMatrixConvertPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class PFMatrixConvertWorker : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    static void registerProto();

    PFMatrixConvertWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    IntegralBus    *input, *output;
    PMBuildSettings         cfg;
    DataTypePtr             mtype;
};

class PWMatrixSearchWorker : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    static void registerProto();

    PWMatrixSearchWorker(Actor* a) : BaseWorker(a, false),
        modelPort(NULL), dataPort(NULL), output(NULL), strand(0) {}
    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished(Task*);

protected:
    IntegralBus *modelPort, *dataPort, *output;
    QString resultName;
    QList<PWMatrix> models;
    int strand;
    WeightMatrixSearchCfg cfg;
};

class PWMatrixWorkerFactory : public DomainFactory {
public:
    static const Descriptor WEIGHT_MATRIX_CATEGORY();
    static const QString WEIGHT_MATRIX_MODEL_TYPE_ID;
    static DataTypePtr const WEIGHT_MATRIX_MODEL_TYPE();
    static const Descriptor WMATRIX_SLOT;
    static void init();
    PWMatrixWorkerFactory(const Descriptor& d) : DomainFactory(d) {}
    virtual Worker* createWorker(Actor* a);
};

class PFMatrixWorkerFactory : public DomainFactory {
public:
    static const Descriptor FREQUENCY_MATRIX_CATEGORY();
    static const QString FREQUENCY_MATRIX_MODEL_TYPE_ID;
    static DataTypePtr const FREQUENCY_MATRIX_MODEL_TYPE();
    static const Descriptor FMATRIX_SLOT;
    static void init();
    PFMatrixWorkerFactory(const Descriptor& d) : DomainFactory(d) {}
    virtual Worker* createWorker(Actor* a);
};

} // Workflow namespace
} // U2 namespace

#endif
