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

#ifndef _U2_EXTERNAL_TOOL_RUN_TASK_H
#define _U2_EXTERNAL_TOOL_RUN_TASK_H

#include <U2Core/AnnotationData.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Task.h>
#include <QProcess>


namespace U2 {

class ExternalToolLogParser;
class ExternalToolRunTaskHelper;
class SaveDocumentTask;
class ExternalToolListener;

//using namespace Workflow;

class U2CORE_EXPORT ProcessRun {
public:
    QProcess *process;
    QString program;
    QStringList arguments;
};

class U2CORE_EXPORT ExternalToolRunTask: public Task {
    Q_OBJECT
    Q_DISABLE_COPY(ExternalToolRunTask)
    friend class ExternalToolRunTaskHelper;
public:
    /**
     * Don't delete logParser, it will be deleted automatically.
     */
    ExternalToolRunTask(const QString& toolId, const QStringList& arguments, ExternalToolLogParser* logParser,
        const QString& workingDirectory = "", const QStringList& additionalPaths = QStringList(),
        const QString &additionalProcessToKill = QString(), bool parseOutputFile = false);
    ~ExternalToolRunTask();

    void addOutputListener(ExternalToolListener* outputListener);

    void run();

    void setStandartInputFile(const QString& file) { inputFile = file; }
    void setStandartOutputFile(const QString& file) { outputFile = file; }
    void setAdditionalEnvVariables(const  QMap<QString, QString> &envVariable) {additionalEnvVariables = envVariable; }

    static void killProcess(QProcess *process, QString childProcesses = "");
private:
    static QList<long> getChildPidsRecursive(long parentPid);
    QString parseStandartOutputFile() const;
    void parseError(const QString& error) const;

    QStringList             arguments;
    ExternalToolLogParser*  logParser;
    const QString           toolId;
    QString                 toolName;
    QString                 workingDirectory;
    QString                 inputFile;
    QString                 outputFile;
    QStringList             additionalPaths;
    QMap <QString, QString> additionalEnvVariables;
    QProcess*               externalToolProcess;
    QScopedPointer<ExternalToolRunTaskHelper> helper;
    ExternalToolListener*   listener;
    QString                 additionalProcessToKill;
    bool                    parseOutputFile;
};

class U2CORE_EXPORT ExternalToolSupportTask: public Task{
public:
    ExternalToolSupportTask(const QString& _name, TaskFlags f)
        : Task(_name, f) {}
    virtual ~ExternalToolSupportTask(){}

    void addListeners(const QList<ExternalToolListener*>& _listeners){listeners.append(_listeners);}
    const QList<ExternalToolListener*>& getListeners() const{return listeners;}

    ExternalToolListener* getListener(int listenerNumber);
protected:
    virtual void setListenerForTask(ExternalToolRunTask* runTask, int listenerNumber = 0);
    virtual void setListenerForHelper(ExternalToolRunTaskHelper* helper, int listenerNumber = 0);
private:
    QList<ExternalToolListener*> listeners;
};

/** Part of ExternalToolRunTask that belongs to task run  thread -> get signals from that thread directly */
class U2CORE_EXPORT ExternalToolRunTaskHelper : public QObject {
    Q_OBJECT
public:
    ExternalToolRunTaskHelper(ExternalToolRunTask* t);
    ExternalToolRunTaskHelper(QProcess *process, ExternalToolLogParser *logParser, U2OpStatus &os);

    void addOutputListener(ExternalToolListener* listener);

public slots:
    void sl_onReadyToReadLog();
    void sl_onReadyToReadErrLog();

protected:
    virtual void processErrorToLog();

private:
    QMutex logMutex;
    U2OpStatus& os;
    ExternalToolLogParser* logParser;
    QProcess* process;
    QByteArray logData;
    ExternalToolListener* listener;
};

class U2CORE_EXPORT ExternalToolSupportUtils : public QObject {
    Q_OBJECT
public:
    static void removeTmpDir(const QString &absoulutePath, U2OpStatus &os);
    static QString createTmpDir(const QString &domain, U2OpStatus &os);
    /**
     * Creates a new folder: prePath/domain/tmp_dir_name
     */
    static QString createTmpDir(const QString &prePath, const QString &domain, U2OpStatus &os);
    static void appendExistingFile(const QString &path, QStringList &files);
    static bool startExternalProcess(QProcess *process, const QString &program, const QStringList &arguments);
    static ProcessRun prepareProcess(const QString &toolName, const QStringList &arguments, const QString &workingDirectory, const QStringList &additionalPaths, U2OpStatus &os, ExternalToolListener* listener);
    static QString prepareArgumentsForCmdLine(const QStringList &arguments);
    static QStringList splitCmdLineArguments(const QString &execString);
    static QVariantMap getScoresGapDependencyMap();
};


class U2CORE_EXPORT ExternalToolLogParser : public QObject {
    Q_OBJECT
public:
    ExternalToolLogParser(bool writeErrorsToLog = true);
    virtual int getProgress(){ return progress; }
    virtual void parseOutput(const QString& partOfLog);
    virtual void parseErrOutput(const QString& partOfLog);
    QString getLastError() const {return lastError;}

protected:
    virtual void processLine(const QString &line);
    virtual void processErrLine(const QString &line);
    virtual bool isError(const QString &line) const;
    virtual void setLastError(const QString &value);

private:
    /* If any error occurred, this variable will be non-empty */
    QString     lastError;
    /* Percent values in range 0..100, negative if unknown. */
    int         progress;
    /* Last line printed to stdout */
    QString     lastLine;
    /* Last line printed to stderr */
    QString     lastErrLine;
    /* The error will be written to log if true. Default - true */
    bool        writeErrorsToLog;

protected:
    QStringList lastPartOfLog;
};

/**
 * @brief The ExternalToolLogProcessor class
 * Log processor is supposed to do some additional action
 * on every new output line.
 */

class U2CORE_EXPORT ExternalToolLogProcessor {
public:
    virtual ~ExternalToolLogProcessor();

    virtual void processLogMessage(const QString& message) = 0;
};

class U2CORE_EXPORT ExternalToolListener {
public:
    enum LogType {
        ERROR_LOG = 0,
        OUTPUT_LOG = 1,
        PROGRAM_WITH_ARGUMENTS = 2
    };

    ExternalToolListener(ExternalToolLogProcessor *logProcessor = NULL);
    virtual ~ExternalToolListener();

    virtual void addNewLogMessage(const QString& message, int messageType) = 0;

    virtual void setToolName(const QString& toolName);
    void setLogProcessor(ExternalToolLogProcessor *logProcessor);
    const QString &getToolName() const;

protected:
    ExternalToolLogProcessor *logProcessor;

private:
    QString toolName;
};

}//namespace

#endif // _U2_EXTERNAL_TOOL_RUN_TASK_H
