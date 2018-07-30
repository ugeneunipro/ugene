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

#ifndef _U2_EXTRACTMSACONSENSUSWORKER_H_
#define _U2_EXTRACTMSACONSENSUSWORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
    class MSAConsensusAlgorithm;
    class ExportConsensusTask;
    class SpinBoxDelegate;
namespace LocalWorkflow {
using namespace Workflow;

class ExtractMSAConsensusTaskHelper;

class ExtractMSAConsensusWorker : public BaseWorker {
    Q_OBJECT
public:
    ExtractMSAConsensusWorker(Actor *actor);

    void init();
    Task * tick();
    void cleanup();
protected:
    virtual ExtractMSAConsensusTaskHelper* createTask(const MultipleSequenceAlignment &msa) = 0;
    virtual void finish() = 0;
    virtual void sendResult(const SharedDbiDataHandler &seqId) = 0;
    ExtractMSAConsensusTaskHelper *extractMsaConsensus;
private slots:
    void sl_taskFinished();

private:
    bool hasMsa() const;
    MultipleSequenceAlignment takeMsa(U2OpStatus &os);
};

class ExtractMSAConsensusStringWorker : public ExtractMSAConsensusWorker {
    Q_OBJECT
public:
    ExtractMSAConsensusStringWorker(Actor *actor);
protected:
    virtual ExtractMSAConsensusTaskHelper* createTask(const MultipleSequenceAlignment &msa);
    virtual void finish();
    virtual void sendResult(const SharedDbiDataHandler &seqId);
};

class ExtractMSAConsensusSequenceWorker : public ExtractMSAConsensusWorker {
    Q_OBJECT
public:
    ExtractMSAConsensusSequenceWorker(Actor *actor);
protected:
    virtual ExtractMSAConsensusTaskHelper* createTask(const MultipleSequenceAlignment &msa);
    virtual void finish();
    virtual void sendResult(const SharedDbiDataHandler &seqId);
};

class ExtractMSAConsensusTaskHelper : public Task {
    Q_OBJECT
public:
    ExtractMSAConsensusTaskHelper(const QString &algoId, int threshold, bool keepGaps, const MultipleSequenceAlignment &msa, const U2DbiRef &targetDbi);


    void prepare();
    U2EntityRef getResult() const;
    QByteArray getResultAsText() const;

private:
    MSAConsensusAlgorithm * createAlgorithm();
    QString getResultName () const;

    const QString algoId;
    const int threshold;
    const bool keepGaps;
    MultipleSequenceAlignment msa;
    const U2DbiRef targetDbi;
    U2Sequence resultSequence;
    QByteArray resultText;

};

class ExtractMSAConsensusSequenceWorkerFactory : public DomainFactory {
public:
    ExtractMSAConsensusSequenceWorkerFactory();

    Worker * createWorker(Actor *actor);

    static void init();

    static const QString ACTOR_ID;
};

class ExtractMSAConsensusStringWorkerFactory : public DomainFactory {
public:
    ExtractMSAConsensusStringWorkerFactory();

    Worker * createWorker(Actor *actor);

    static void init();

    static const QString ACTOR_ID;
};

class ExtractMSAConsensusWorkerPrompter : public PrompterBase<ExtractMSAConsensusWorkerPrompter>{
    Q_OBJECT
public:
    ExtractMSAConsensusWorkerPrompter(Actor *actor = NULL);

protected:
    QString composeRichDoc();
};

class SpinBoxDelegatePropertyRelation : public AttributeRelation {
public:
    SpinBoxDelegatePropertyRelation(const QString &relatedAttrId): AttributeRelation(relatedAttrId){}

    virtual RelationType getType() const {return PROPERTY_CHANGER;}
    SpinBoxDelegatePropertyRelation *clone() const;

    virtual QVariant getAffectResult(const QVariant &influencingValue, const QVariant &dependentValue,
        DelegateTags *infTags, DelegateTags *depTags) const;
    virtual void updateDelegateTags(const QVariant &influencingValue, DelegateTags *dependentTags) const;
};

} // LocalWorkflow
} // U2

#endif // _U2_EXTRACTMSACONSENSUSWORKER_H_
