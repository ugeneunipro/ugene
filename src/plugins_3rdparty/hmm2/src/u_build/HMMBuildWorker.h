#ifndef _U2_HMM_BUILD_WORKER_H_
#define _U2_HMM_BUILD_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "u_calibrate/uhmmcalibrate.h"
#include "uhmmbuild.h"

namespace U2 {

namespace LocalWorkflow {

class HMMBuildPrompter : public PrompterBase<HMMBuildPrompter> {
    Q_OBJECT
public:
    HMMBuildPrompter(Actor* p = 0)
        : PrompterBase<HMMBuildPrompter>(p) {
    }

protected:
    QString composeRichDoc();
};

class HMMBuildWorker : public BaseWorker {
    Q_OBJECT
public:
    HMMBuildWorker(Actor* a);

    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual bool isDone() const;
    virtual void cleanup();

private slots:
    void sl_taskFinished(Task*);
    void sl_taskFinished();

protected:
    IntegralBus *input, *output;
    UHMMBuildSettings cfg;
    UHMMCalibrateSettings calSettings;
    bool calibrate;
    Task* nextTick;
};

class HMMBuildWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR;
    static void init();
    static void cleanup();
    HMMBuildWorkerFactory()
        : DomainFactory(ACTOR) {
    }
    virtual Worker* createWorker(Actor* a) {
        return new HMMBuildWorker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif
