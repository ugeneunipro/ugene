/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ENSEMBLE_CLASSIFICATION_H_
#define _U2_ENSEMBLE_CLASSIFICATION_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "TaxonomySupport.h"

namespace U2 {
namespace LocalWorkflow {

class EnsembleClassificationPrompter;
typedef PrompterBase<EnsembleClassificationPrompter> EnsembleClassificationBase;

class EnsembleClassificationPrompter : public EnsembleClassificationBase {
    Q_OBJECT
public:
    EnsembleClassificationPrompter(Actor* p = 0) : EnsembleClassificationBase(p) {}
protected:
    QString composeRichDoc();
};

class EnsembleClassificationWorker: public BaseWorker {
    Q_OBJECT
public:
    EnsembleClassificationWorker(Actor *a);

protected:
    bool isReady() const;
    void init();
    Task * tick();
    void cleanup() {}

private slots:
    void sl_taskFinished(Task *task);

private:
    bool isReadyToRun() const;
    bool dataFinished() const;
    QString checkSimultaneousFinish() const;

    IntegralBus *input1;
    IntegralBus *input2;
    IntegralBus *input3;
    IntegralBus *output;
    QString outputFile;
    bool tripleInput;
};

class EnsembleClassificationWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    static void cleanup();
    EnsembleClassificationWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new EnsembleClassificationWorker(a); }
};

class EnsembleClassificationTask : public Task {
    Q_OBJECT
public:
    EnsembleClassificationTask(const QList<TaxonomyClassificationResult> &taxData, const bool tripleInput, const QString &outputFile, const QString &workingDir);

    bool foundMismatches() const {return hasMissing;}
    const QString &getOutputFile() const {return outputFile;}

private:
    void run();

    QList<TaxonomyClassificationResult> taxData;
    const bool tripleInput;
    const QString workingDir;

    QString outputFile;
    bool hasMissing;
};

} //LocalWorkflow
} //U2

#endif
