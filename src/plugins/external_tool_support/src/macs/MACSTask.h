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

#ifndef _U2_MACS_SUPPORT_TASK_
#define _U2_MACS_SUPPORT_TASK_

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include "MACSSettings.h"

namespace U2 {

class AnnotationTableObject;
class Document;
class LoadDocumentTask;
class SaveDocumentTask;

class MACSTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(MACSTask)
public:
    MACSTask(const MACSSettings &settings, const GUrl &treatUrl, const GUrl &conUrl);
    virtual ~MACSTask();

    virtual void prepare();
    virtual QList<Task *> onSubTaskFinished(Task *subTask);
    virtual void run();
    virtual void cleanup();

    const MACSSettings & getSettings();

    // Memory should be released by caller
    QList<AnnotationTableObject *> getPeaks() const;
    QList<AnnotationTableObject *> getPeakSummits() const;

    QString getWiggleUrl();
    QStringList getOutputFiles();

private:
    MACSSettings settings;
    QString workingDir;

    GUrl treatUrl;
    GUrl conUrl;

    Document *peaksDoc;
    Document *summitsDoc;

    LoadDocumentTask *peaksTask;
    LoadDocumentTask *summitsTask;
    ExternalToolRunTask *etTask;

    static const QString BASE_DIR_NAME;
    static const QString BASE_SUBDIR_NAME;
};

class MACSLogParser : public ExternalToolLogParser {
public:
    MACSLogParser();

    int getProgress();
    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);

private:
    QString lastErrLine;
    int     progress;
};

} // U2

#endif // _U2_MACS_SUPPORT_TASK_
