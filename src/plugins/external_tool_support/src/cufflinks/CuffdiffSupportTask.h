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

#ifndef _U2_CUFFDIFFSUPPORTTASK_H_
#define _U2_CUFFDIFFSUPPORTTASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include <U2Formats/ConvertAssemblyToSamTask.h>

#include <U2Lang/DbiDataHandler.h>

namespace U2 {

class AnnotationTableObject;

struct CuffdiffSettings {
    CuffdiffSettings();
    void cleanup();

    enum HitsNorm {
        Total = 0,
        Compatible = 1
    };
    enum Library {
        StandardIllumina = 0,
        dUTP_NSR_NNSR = 1,
        Ligation_StandardSOLiD = 2
    };
    bool timeSeriesAnalysis;
    bool upperQuartileNorm;
    HitsNorm hitsNorm;
    QString fragBiasCorrect;
    bool multiReadCorrect;
    Library libraryType;
    QString maskFile;
    int minAlignmentCount;
    double fdr;
    int maxMleIterations;
    bool emitCountTables;
    QString outDir;

    bool groupBySamples;
    QMap<QString, QStringList> assemblyUrls;
    Workflow::DbiDataStorage *storage;
    QList<Workflow::SharedDbiDataHandler> transcript;
    QString workingDir;
};

class CuffdiffSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
private:
    class LogParser : public ExternalToolLogParser {
    public:
        LogParser();
        void parseErrOutput(const QString &partOfLog);
    };

public:
    CuffdiffSupportTask(const CuffdiffSettings &settings);

    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    ReportResult report();
    QStringList getOutputFiles() const;
    QStringList getSystemOutputFiles() const;

private:
    CuffdiffSettings settings;
    QString workingDir;
    QString transcriptUrl;

    QScopedPointer<Document> transcriptDoc;
    QList<Task*> saveTasks;

    ExternalToolRunTask *diffTask;
    QStringList outputFiles;
    QStringList systemOutputFiles;

    static const QString outSubDirBaseName;
private:
    Task * createTranscriptTask();

    void createTranscriptDoc();
    Task * createCuffdiffTask();
    void addOutFiles();
    void addFile(const QString &fileName, bool openBySystem = false);
    void setupWorkingDir();
};


} // U2

#endif // _U2_CUFFDIFFSUPPORTTASK_H_
