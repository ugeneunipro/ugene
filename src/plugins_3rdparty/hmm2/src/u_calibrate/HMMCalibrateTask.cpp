#include "HMMCalibrateTask.h"
#include <hmmer2/funcs.h>

#include <HMMIO.h>
#include <TaskLocalStorage.h>

#include <U2Core/Counter.h>

#include <QFileInfo>

namespace U2 {

HMMCalibrateTask::HMMCalibrateTask(plan7_s* hmm, const UHMMCalibrateSettings& s) 
    : HMMCalibrateAbstractTask("", hmm, s)
{
    GCOUNTER( cvar, tvar, "HMMCalibrateTask" );
    setTaskName(tr("HMM calibrate '%1'").arg(hmm->name));
    tpm = Task::Progress_Manual;
}


void HMMCalibrateTask::run() {
    TaskLocalData::createHMMContext(getTaskId(), true);
    try {
        UHMMCalibrate::calibrate(hmm, settings, stateInfo);
    } catch (const HMMException &e) {
        stateInfo.setError(e.error);
    }
    TaskLocalData::freeHMMContext(getTaskId());
}

//////////////////////////////////////////////////////////////////////////
//top-lebel parallel calibration subtask
HMMCalibrateParallelTask::HMMCalibrateParallelTask(plan7_s* _hmm, const UHMMCalibrateSettings& s) 
: HMMCalibrateAbstractTask(tr("HMM calibrate '%1'").arg(_hmm->name), _hmm, s), initTask(NULL)
{
    wpool.hmm = hmm;
    wpool.progress = &stateInfo.progress;
    tpm = Task::Progress_Manual;
}

void HMMCalibrateParallelTask::prepare() {
    TaskLocalData::createHMMContext(getTaskId(), false);

    initTask = new HMMCreateWPoolTask(this);
    addSubTask(initTask);
    for(int i=0;i < settings.nThreads;i++){
        addSubTask(new HMMCalibrateParallelSubTask(this));
    }
    
    setMaxParallelSubtasks(1);
}

QList<Task*> HMMCalibrateParallelTask::onSubTaskFinished(Task* subTask) {
    if (subTask == initTask) {
        setMaxParallelSubtasks(settings.nThreads);
    }
    return QList<Task*>();
}

void HMMCalibrateParallelTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }
    
    TaskLocalData::bindToHMMContext(getTaskId());
    try {
        histogram_s* hist = getWorkPool()->hist;
        if (!ExtremeValueFitHistogram(hist, TRUE, 9999.)) {
            stateInfo.setError("fit failed; num sequences may be set too small?\n");
        } else {
            hmm->flags |= PLAN7_STATS;
            hmm->mu     = hist->param[EVD_MU];
            hmm->lambda = hist->param[EVD_LAMBDA];
        }
    } catch (const HMMException &e) {
        stateInfo.setError(e.error);
    }
    TaskLocalData::detachFromHMMContext();
}

Task::ReportResult HMMCalibrateParallelTask::report() {
    TaskLocalData::freeHMMContext(getTaskId());
    return ReportResult_Finished;
}

void HMMCalibrateParallelTask::cleanup() {
    if (wpool.hist!=NULL) {
        FreeHistogram(wpool.hist); // exception safe function -> does not depend on hmm context
        wpool.hist = NULL;
    }
}


//////////////////////////////////////////////////////////////////////////
// task to init wpool state
HMMCreateWPoolTask::HMMCreateWPoolTask(HMMCalibrateParallelTask* t)
: Task(tr("Initialize parallel context"), TaskFlag_None), pt(t)
{
}

void HMMCreateWPoolTask::run() {
    TaskLocalData::bindToHMMContext(pt->getTaskId());
    try {
        runUnsafe();
    } catch (const HMMException &e) {
        stateInfo.setError(e.error);
    }
    TaskLocalData::detachFromHMMContext();
}

void HMMCreateWPoolTask::runUnsafe() {
    const UHMMCalibrateSettings& settings = pt->getSettings();
    WorkPool_s* wpool = pt->getWorkPool();

    SetAlphabet(wpool->hmm->atype);
    sre_srandom(settings.seed);

    wpool->fixedlen = settings.fixedlen;
    wpool->hist = AllocHistogram(-200, 200, 100);
    wpool->lenmean = settings.lenmean;
    wpool->lensd = settings.lensd;
    wpool->nsample = settings.nsample;
    wpool->nseq = 0;
    wpool->randomseq.resize(MAXABET);
    wpool->max_score = -FLT_MAX;

        
    float  p1;
    P7Logoddsify(wpool->hmm, TRUE);
    P7DefaultNullModel(wpool->randomseq.data(), &p1);
}

//////////////////////////////////////////////////////////////////////////
//parallel calibrate subtask 
HMMCalibrateParallelSubTask::HMMCalibrateParallelSubTask(HMMCalibrateParallelTask* t) 
:Task(tr("Parallel HMM calibration subtask"), TaskFlag_None), pt(t)
{
    tpm = Task::Progress_Manual;
}


void HMMCalibrateParallelSubTask::run() {
    TaskLocalData::bindToHMMContext(pt->getTaskId());
    try {
        UHMMCalibrate::calibrateParallel(pt->getWorkPool(), stateInfo);
    } catch (const HMMException &e) {
        stateInfo.setError(e.error);
    }

    TaskLocalData::detachFromHMMContext();
}


//////////////////////////////////////////////////////////////////////////
// task to save calibration results to file
HMMCalibrateToFileTask::HMMCalibrateToFileTask(const QString& _inFile, const QString& _outFile, const UHMMCalibrateSettings& s)
: Task("", TaskFlags_NR_FOSCOE|TaskFlag_ReportingIsSupported), hmm(NULL), inFile(_inFile), outFile(_outFile), settings(s) 
{
    setVerboseLogMode(true);
    QString tn = tr("HMM calibrate '%1'").arg(QFileInfo(inFile).fileName());
    setTaskName(tn);
    readTask = NULL;
    calibrateTask = NULL;
}


void HMMCalibrateToFileTask::prepare() {
    readTask = new HMMReadTask(inFile);
    readTask->setSubtaskProgressWeight(0);
    addSubTask(readTask);
}

QList<Task*> HMMCalibrateToFileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask == readTask) {
        hmm = readTask->getHMM();
        if (hmm == nullptr) {
            assert(false);
            stateInfo.setError(tr("HMMReadTask didn't generate \"hmm\" object, stop."));
            return res;
        }
        if (settings.nThreads == 1) {
            calibrateTask = new HMMCalibrateTask(hmm, settings);
        } else {
            calibrateTask = new HMMCalibrateParallelTask(hmm, settings);
        }
        res.append(calibrateTask);
    } else if (subTask == calibrateTask) {
        Task* t = new HMMWriteTask(outFile, hmm);
        t->setSubtaskProgressWeight(0);
        res.append(t);
    }
    return res;
}

QString HMMCalibrateToFileTask::generateReport() const {
    QString res;
    res+="<table>";
    res+="<tr><td width=200><b>" + tr("Source profile") + "</b></td><td>" + QFileInfo(inFile).absoluteFilePath() + "</td></tr>";

    if (hasError() || isCanceled()) {
        res+="<tr><td width=200><b>" + tr("Task was not finished") + "</b></td><td></td></tr>";
        res+="</table>";
        return res;
    }

    res+="<tr><td><b>" + tr("Result profile") + "</b></td><td>" + QFileInfo(outFile).absoluteFilePath() + "</td></tr>";
    res+="<tr><td><b>" + tr("Expert options") + "</b></td><td></td></tr>";
    
    res+="<tr><td><b>" + tr("Number of random sequences to sample") + "</b></td><td>"+QString::number(settings.nsample)+"</td></tr>";
    res+="<tr><td><b>" + tr("Random number seed") + "</b></td><td>"+QString::number(settings.seed)+"</td></tr>";
    res+="<tr><td><b>" + tr("Mean of length distribution") + "</b></td><td>"+QString::number(settings.lenmean)+"</td></tr>";
    res+="<tr><td><b>" + tr("Standard deviation of length distribution") + "</b></td><td>"+QString::number(settings.lensd)+"</td></tr>";
    
    res+="<tr><td><b>" + tr("Calculated evidence (mu , lambda)") + "</b></td><td>"+QString::number(hmm->mu, 'f', 6) + ", " + QString::number(hmm->lambda, 'f', 6) + "</td></tr>";
    
    res+="</table>";
    return res;
}

}//endif
