#ifndef _U2_HMMSEARCH_WORKER_H_
#define _U2_HMMSEARCH_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "uhmmsearch.h"

namespace U2 {

namespace LocalWorkflow {

class HMMSearchPrompter : public PrompterBase<HMMSearchPrompter> {
    Q_OBJECT
public:
    HMMSearchPrompter(Actor *p = 0)
        : PrompterBase<HMMSearchPrompter>(p) {
    }

protected:
    QString composeRichDoc();
};

class HMMSearchWorker : public BaseWorker {
    Q_OBJECT
public:
    HMMSearchWorker(Actor *a);
    virtual void init();
    virtual bool isReady() const;
    virtual Task *tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished(Task *);

protected:
    IntegralBus *hmmPort, *seqPort, *output;
    QString resultName;
    UHMMSearchSettings cfg;
    QList<plan7_s *> hmms;
};

class HMMSearchWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR;
    static void init();
    HMMSearchWorkerFactory()
        : DomainFactory(ACTOR) {
    }
    virtual Worker *createWorker(Actor *a) {
        return new HMMSearchWorker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif
