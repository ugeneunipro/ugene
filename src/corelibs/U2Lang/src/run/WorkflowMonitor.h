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

#ifndef _U2_WORKFLOWMONITOR_H_
#define _U2_WORKFLOWMONITOR_H_

#include <QTextStream>

#include <U2Core/global.h>
#include <U2Core/Task.h>
#include <U2Core/ExternalToolRunTask.h>

#include <U2Lang/SupportClass.h>
#include <U2Lang/WorkflowRunTask.h>

namespace U2 {
namespace Workflow {
class WDListener;

class Actor;

namespace Monitor {
    class U2LANG_EXPORT FileInfo {
    public:
        FileInfo( );
        FileInfo(const QString &url, const QString &producer, bool openBySystem = false);
        QString url;
        QString actor;
        bool    openBySystem;

        bool operator== (const FileInfo &other) const;
    };
    class U2LANG_EXPORT WorkerInfo {
    public:
        WorkerInfo();
        int ticks;
        qint64 timeMks;
    };
    class U2LANG_EXPORT WorkerParamsInfo {
    public:
        WorkerParamsInfo();
        QString workerName;
        QList<Attribute*> parameters;
        Actor *actor;
    };
    class U2LANG_EXPORT WorkerLogInfo {
    public:
        WorkerLogInfo(): runNumber(0){}
        ~WorkerLogInfo();
        int runNumber;
        QList<ExternalToolListener*> logs;
    };

    class U2LANG_EXPORT LogEntry {
    public:
        LogEntry() :
            runNumber(0),
            logType(0) {}
        QString toolName;
        QString actorName;
        int runNumber;
        int logType;
        QString lastLine;
    };

    enum U2LANG_EXPORT TaskState {
        RUNNING,
        RUNNING_WITH_PROBLEMS,
        FINISHED_WITH_PROBLEMS,
        CANCELLED,
        FAILED,
        SUCCESS
    };
} // Monitor

class U2LANG_EXPORT WorkflowMonitor : public QObject {
    Q_OBJECT
public:
    WorkflowMonitor(WorkflowAbstractIterationRunner *task, Schema *schema);

    const QList<Monitor::FileInfo> & getOutputFiles() const;
    const ProblemList & getProblems() const;
    const QMap<QString, Monitor::WorkerInfo> & getWorkersInfo() const;
    const QList<Monitor::WorkerParamsInfo>  & getWorkersParameters() const;
    const QMap<QString, Monitor::WorkerLogInfo> & getWorkersLog() const;
    const QMap<QString, QMultiMap<QString, QString> > &getWorkersReports() const;
    QString actorName(const QString &id) const;
    int getDataProduced(const QString &actor) const;
    bool containsOutputFile(const QString &url) const;

    void addOutputFile(const QString &url, const QString &producer, bool openBySystem = false);
    void addInfo(const QString &message, const QString &actor, const QString &type = Problem::U2_INFO);
    void addError(const QString &message, const QString &actor, const QString &type = Problem::U2_ERROR);
    /** Can be called only one time for the task */
    void addTaskError(Task *task, const QString &message = "");
    void addTaskWarning(Task *task, const QString &message = "");
    void addTime(qint64 timeMks, const QString &actor);
    void addTick(qint64 timeMks, const QString &actor);

    void start();
    void pause();
    void resume();

    void registerTask(Task *task, const QString &actor);

    void setOutputDir(const QString &dir);
    QString outputDir() const;

    void setSaveSchema(const Metadata &meta);

    QList<ExternalToolListener*> createWorkflowListeners(const QString& workerName, int listenersNumber = 1);

    void onLogChanged(const WDListener* listener, int messageType, const QString& message);

    static const QString WORKFLOW_FILE_NAME;

public slots:
    void sl_progressChanged();
    void sl_taskStateChanged();
    void sl_workerTaskFinished(Task *workerTask);

signals:
    void si_firstProblem();
    void si_newOutputFile(const U2::Workflow::Monitor::FileInfo &info);
    void si_newProblem(const Problem &info);
    void si_workerInfoChanged(const QString &actor, const U2::Workflow::Monitor::WorkerInfo &info);
    void si_progressChanged(int progress);
    void si_runStateChanged(bool paused);
    void si_taskStateChanged(Monitor::TaskState state);
    void si_updateProducers();
    void si_report();
    void si_dirSet(const QString &dir);
    void si_logChanged(U2::Workflow::Monitor::LogEntry entry);

private:
    Schema *schema;
    QScopedPointer<Metadata> meta;
    QPointer<WorkflowAbstractIterationRunner> task;
    QMap<QString, Actor*> procMap;
    QMap<Task*, Actor*> taskMap;
    QList<Task*> errorTasks;
    QList<Monitor::FileInfo> outputFiles;
    ProblemList problems;
    QMap<QString, Monitor::WorkerInfo> workers;
    QList<Monitor::WorkerParamsInfo> workersParamsInfo;
    QMap<QString, Monitor::WorkerLogInfo> workersLog;
    QMap<QString, QMultiMap<QString, QString> > workersReports;  // workerId<taskName, taskReport> >
    QString _outputDir;
    bool saveSchema;
    bool started;

protected:
    void setWorkerInfo(const QString &actorId, const Monitor::WorkerInfo &info);
    void setRunState(bool paused);
    void addProblem(const Problem &problem);
    bool hasWarnings() const;
    bool hasErrors() const;
};

class U2LANG_EXPORT MonitorUtils {
public:
    static QMap< QString, QList<Monitor::FileInfo> > filesByActor(const WorkflowMonitor *m);
    static QStringList sortedByAppearanceActorIds(const WorkflowMonitor *m);
    static QString toSlashedUrl(const QString &url);
};

class U2LANG_EXPORT WDListener: public ExternalToolListener{
public:
    WDListener(WorkflowMonitor* _monitor, const QString& _actorName, int _runNumber);

    void addNewLogMessage(const QString& message, int messageType);

    const QString& getActorName() const {return actorName;}

    int getRunNumber() const {return runNumber;}

    static QString getStandardOutputLogFileUrl(const QString &actorName, int runNumber);
    static QString getStandardErrorLogFileUrl(const QString &actorName, int runNumber);

private:
    void writeToFile(int messageType, const QString& message);
    static void writeToFile(QTextStream &logStream, const QString& message);

    WorkflowMonitor* monitor;
    QString actorName;
    int runNumber;

    QFile outputLogFile;
    QFile errorLogFile;
    QTextStream outputLogStream;
    QTextStream errorLogStream;
    bool outputHasMessages;
    bool errorHasMessages;
};

} // Workflow
} // U2

Q_DECLARE_METATYPE( U2::Workflow::Monitor::TaskState )
Q_DECLARE_METATYPE( U2::Workflow::Monitor::FileInfo )
Q_DECLARE_METATYPE( U2::Workflow::Monitor::WorkerInfo )
Q_DECLARE_METATYPE( U2::Workflow::Monitor::LogEntry )

#endif // _U2_WORKFLOWMONITOR_H_
