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
        bool    isDir;

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
        WorkerLogInfo(): workerRunNumber(0){}
        int workerRunNumber;
        QList<ExternalToolListener*> logs;
    };

    class U2LANG_EXPORT LogEntry {
    public:
        LogEntry() :
            actorRunNumber(0),
            toolRunNumber(0),
            contentType(0) {}
        QString toolName;
        QString actorId;
        QString actorName;
        int actorRunNumber;
        int toolRunNumber;
        int contentType;
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
    const NotificationsList & getNotifications() const;
    const QMap<QString, Monitor::WorkerInfo> & getWorkersInfo() const;
    const QList<Monitor::WorkerParamsInfo>  & getWorkersParameters() const;
    const QMap<QString, Monitor::WorkerLogInfo> & getWorkersLog() const;
    const QMap<QString, QMultiMap<QString, QString> > &getWorkersReports() const;
    QString actorName(const QString &id) const;
    int getDataProduced(const QString &actor) const;
    bool containsOutputFile(const QString &url) const;

    void addOutputFile(const QString &url, const QString &producer, bool openBySystem = false);
    void addOutputFolder(const QString &url, const QString &producer);
    void addInfo(const QString &message, const QString &actor, const QString &type = WorkflowNotification::U2_INFO);
    void addError(const QString &message, const QString &actor, const QString &type = WorkflowNotification::U2_ERROR);
    /** Can be called only one time for the task */
    void addTaskError(Task *task, const QString &message = "");
    void addTaskWarning(Task *task, const QString &message = "");
    void addTime(qint64 timeMks, const QString &actor);
    void addTick(qint64 timeMks, const QString &actor);

    void start();
    void pause();
    void resume();
    bool isExternalToolScheme() const;

    void registerTask(Task *task, const QString &actor);

    void setOutputDir(const QString &dir);
    QString outputDir() const;
    QString getLogsDir() const;
    QString getLogUrl(const QString &actorId, int actorRunNumber, const QString &toolName, int toolRunNumber, int contentType) const;

    void setSaveSchema(const Metadata &meta);

    QList<ExternalToolListener*> createWorkflowListeners(const QString &workerId, const QString& workerName, int listenersNumber = 1);
    WDListener *getListener(const QString &actorId, int actorRunNumber, const QString &toolName, int toolRunNumber) const;
    int getNewToolRunNumber(const QString &actorId, int actorRunNumber, const QString &toolName);

    void onLogChanged(const WDListener* listener, int messageType, const QString& message);

    static const QString WORKFLOW_FILE_NAME;

public slots:
    void sl_progressChanged();
    void sl_taskStateChanged();
    void sl_workerTaskFinished(Task *workerTask);

signals:
    void si_firstNotification();
    void si_newOutputFile(const Monitor::FileInfo &info);
    void si_newNotification(const WorkflowNotification &info, int count);
    void si_workerInfoChanged(const QString &actor, const Monitor::WorkerInfo &info);
    void si_progressChanged(int progress);
    void si_runStateChanged(bool paused);
    void si_taskStateChanged(Monitor::TaskState state);
    void si_updateProducers();
    void si_report();
    void si_dirSet(const QString &dir);
    void si_logChanged(Monitor::LogEntry entry);

private:
    Schema                                      *schema;
    QScopedPointer<Metadata>                    meta;
    QPointer<WorkflowAbstractIterationRunner>   task;
    QMap<QString, QPointer<Actor> >             procMap;
    StrStrMap                                   processNames;
    QMap<Task*, Actor*>                         taskMap;
    QList<Task*>                                errorTasks;
    QList<Monitor::FileInfo>                    outputFiles;
    NotificationsList                           notifications;
    QMap<QString, Monitor::WorkerInfo>          workers;
    QList<Monitor::WorkerParamsInfo>            workersParamsInfo;
    QMap<QString, Monitor::WorkerLogInfo>       workersLog;
    QMap<QString, QMultiMap<QString, QString> > workersReports;  // workerId<taskName, taskReport> >
    QString                                     _outputDir;
    bool saveSchema;
    bool started;
    bool externalTools;

protected:
    void setWorkerInfo(const QString &actorId, const Monitor::WorkerInfo &info);
    void setRunState(bool paused);
    void addNotification(const WorkflowNotification &notification);
    bool hasWarnings() const;
    bool hasErrors() const;
};

class U2LANG_EXPORT MonitorUtils {
public:
    static QMap< QString, QList<Monitor::FileInfo> > filesByActor(const WorkflowMonitor *m);
    static QStringList sortedByAppearanceActorIds(const WorkflowMonitor *m);
    static QString toSlashedUrl(const QString &url);
};

class U2LANG_EXPORT WDListener: public ExternalToolListener {
public:
    WDListener(WorkflowMonitor *monitor, const QString& actorId, const QString &actorName, int actorRunNumber);

    void addNewLogMessage(const QString& message, int messageType) override;

    void setToolName(const QString& toolName) override;

    const QString& getActorId() const {return actorId;}
    const QString& getActorName() const {return actorName;}

    int getActorRunNumber() const {return actorRunNumber;}
    int getToolRunNumber() const {return toolRunNumber;}

    QString getStdoutLogFileUrl();
    QString getStderrLogFileUrl();

private:
    static QString getStdoutLogFileUrl(const QString &actorName, int actorRunNumber, const QString &toolName, int toolRunNumber);
    static QString getStderrLogFileUrl(const QString &actorName, int actorRunNumber, const QString &toolName, int toolRunNumber);

    void initLogFile(int contentType);

    void writeToFile(int messageType, const QString& message);
    static void writeToFile(QTextStream &logStream, const QString& message);

    WorkflowMonitor* monitor;
    QString actorId;
    QString actorName;
    int actorRunNumber;
    int toolRunNumber;

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
Q_DECLARE_METATYPE( U2::Workflow::Monitor::WorkerParamsInfo )

#endif // _U2_WORKFLOWMONITOR_H_
