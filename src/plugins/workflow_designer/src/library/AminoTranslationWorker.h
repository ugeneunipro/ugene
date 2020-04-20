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

#ifndef _U2_TRANSLATE_SEQUENCE_WORKER_H_
#define _U2_TRANSLATE_SEQUENCE_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <QSharedPointer>
#include <U2Core/DNATranslation.h>

namespace U2 {

namespace LocalWorkflow {

class AminoTranslationSettings{
public:
    QString resultName;
    QVector<U2Region> regionsDirect;
    QVector<U2Region> regionsComplementary;
    QSharedPointer<U2SequenceObject> seqObj;
    DNATranslation* aminoTT;
};

class TranslateSequence2AminoTask : public Task{
    Q_OBJECT
public:
    TranslateSequence2AminoTask(const AminoTranslationSettings &configs, const U2DbiRef &dbiRef);
    virtual void run();
    QList<U2SequenceObject*> popResults(){return results;}
private:
    QList<U2SequenceObject*> results;
    AminoTranslationSettings configs;
    const U2DbiRef dbiRef;
};

class AminoTranslationPrompter;
typedef PrompterBase<AminoTranslationPrompter> AminoTranslationPrompterBase;

class AminoTranslationPrompter : public AminoTranslationPrompterBase {
    Q_OBJECT
public:
    AminoTranslationPrompter(Actor* p = 0) : AminoTranslationPrompterBase(p) {}
protected:
    QString composeRichDoc();
};

class AminoTranslationWorker : public BaseWorker {
    Q_OBJECT
public:
    AminoTranslationWorker(Actor* a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

    private slots:
        void sl_taskFinished();

protected:
    IntegralBus *input, *output;
};

class AminoTranslationWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    AminoTranslationWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new AminoTranslationWorker(a);}
};

}// Workflow namespace

}// U2 namespace

#endif
