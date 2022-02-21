#ifndef _U2_HMMCALIBRATE_TASK_H_
#define _U2_HMMCALIBRATE_TASK_H_

#include <U2Core/Task.h>

#include "uhmmcalibrate.h"

struct HMMERTaskLocalData;

namespace U2 {

class HMMCalibrateParallelSubTask;
class HMMCalibrateAbstractTask;
class HMMReadTask;

class HMMCalibrateAbstractTask;

class HMMCalibrateToFileTask : public Task {
    Q_OBJECT
public:
    HMMCalibrateToFileTask(const QString& _inFile, const QString& _outFile, const UHMMCalibrateSettings& s);

    const plan7_s* getHMM() const {
        return hmm;
    }
    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    QString generateReport() const;

protected:
    plan7_s* hmm;
    QString inFile;
    QString outFile;
    UHMMCalibrateSettings settings;
    HMMReadTask* readTask;
    HMMCalibrateAbstractTask* calibrateTask;
};

class HMMCalibrateAbstractTask : public Task {
    Q_OBJECT
public:
    HMMCalibrateAbstractTask(const QString& _name, plan7_s* _hmm, const UHMMCalibrateSettings& s, TaskFlags fl = TaskFlag_None)
        : Task(_name, fl), hmm(_hmm), settings(s) {
    }
    plan7_s* getHMM() {
        return hmm;
    }
    const UHMMCalibrateSettings& getSettings() const {
        return settings;
    }

protected:
    plan7_s* hmm;
    UHMMCalibrateSettings settings;
};

class HMMCalibrateTask : public HMMCalibrateAbstractTask {
    Q_OBJECT
public:
    HMMCalibrateTask(plan7_s* hmm, const UHMMCalibrateSettings& s);
    void run();
};

//////////////////////////////////////////////////////////////////////////
// parallel version

class HMMCreateWPoolTask;

// 1 creates WPOOL
// runs N parallel subtasks
class HMMCalibrateParallelTask : public HMMCalibrateAbstractTask {
    Q_OBJECT
public:
    HMMCalibrateParallelTask(plan7_s* hmm, const UHMMCalibrateSettings& s);
    ~HMMCalibrateParallelTask() {
        cleanup();
    }

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    void run();
    ReportResult report();
    void cleanup();

    WorkPool_s* getWorkPool() {
        return &wpool;
    }

private:
    HMMCreateWPoolTask* initTask;
    WorkPool_s wpool;
};

// initializes pool that will be used by parallel calibrate subtasks
class HMMCreateWPoolTask : public Task {
    Q_OBJECT
public:
    HMMCreateWPoolTask(HMMCalibrateParallelTask* t);
    ~HMMCreateWPoolTask() {
        cleanup();
    }

    void run();
    void runUnsafe();

    HMMCalibrateParallelTask* pt;
};

class HMMCalibrateParallelSubTask : public Task {
    Q_OBJECT
public:
    HMMCalibrateParallelSubTask(HMMCalibrateParallelTask* pt);
    void run();

private:
    HMMCalibrateParallelTask* pt;
};

}  // namespace U2
#endif
