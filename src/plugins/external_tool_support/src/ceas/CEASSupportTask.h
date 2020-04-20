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

#ifndef _U2_CEAS_SUPPORT_TASK_
#define _U2_CEAS_SUPPORT_TASK_

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>

#include "CEASSettings.h"

namespace U2 {

class AnnotationTableObject;
class Document;
class SaveDocumentTask;

class CEASTaskSettings {
public:
    CEASTaskSettings();
    CEASTaskSettings(const CEASSettings &ceas, Workflow::DbiDataStorage *storage, const QList<Workflow::SharedDbiDataHandler> &bedData, const QString &wigData);

    CEASSettings & getCeasSettings();
    const CEASSettings & getCeasSettings() const;

    Workflow::DbiDataStorage *getStorage() const;
    const QList<Workflow::SharedDbiDataHandler> &getBedData() const;

    const QString & getWigData() const;

    static const QString PDF_FORMAT;
    static const QString PNG_FORMAT;

private:
    CEASSettings ceas;
    Workflow::DbiDataStorage *storage;
    QList<Workflow::SharedDbiDataHandler> bedData;
    QString wigData;

    QString outFilePath;
    QString outFileFormat;
};

class CEASSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    CEASSupportTask(const CEASTaskSettings &settings);
    virtual ~CEASSupportTask();

    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    virtual void run();
    virtual void cleanup();

    const CEASTaskSettings & getSettings() const;

private:
    CEASTaskSettings settings;
    QString workingDir;

    Document *bedDoc;

    SaveDocumentTask *bedTask;
    SaveDocumentTask *wigTask;
    ExternalToolRunTask *etTask;

    int activeSubtasks;

    static const QString BASE_DIR_NAME;

private:
    void createBedDoc();
    bool canStartETTask() const;
    bool copyFile(const QString &src, const QString &dst);
    Task* createETTask();
};

class CEASLogParser : public ExternalToolLogParser {
public:
    CEASLogParser();

    int getProgress();
    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);

private:
    QString lastErrLine;
    int     progress;
};


} // U2

#endif // _U2_CEAS_SUPPORT_TASK_
