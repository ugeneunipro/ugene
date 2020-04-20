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

#ifndef _U2_SPIDEY_SUPPORT_TASK_H_
#define _U2_SPIDEY_SUPPORT_TASK_H_

#include <QFile>
#include <QStringList>

#include <U2Formats/StreamSequenceReader.h>
#include <U2Formats/StreamSequenceWriter.h>
#include <U2Algorithm/SplicedAlignmentTask.h>


#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Gui/CreateAnnotationWidgetController.h>


namespace U2 {

class PrepareInputForSpideyTask : public Task {
    Q_OBJECT
public:
                                PrepareInputForSpideyTask(const U2SequenceObject *dna,
                                    const U2SequenceObject *mRna,
                                    const QString &outputDirPath);
    void                        run();
    const QStringList &         getArgumentsList() { return argumentList; }
    const QString &             getResultPath() { return resultPath; }
private:
    const U2SequenceObject *    dnaObj;
    const U2SequenceObject *    mRnaObj;
    QStringList                 argumentList;
    StreamSequenceReader        seqReader;
    QString                     outputDir;
    QString                     resultPath;
};

class SpideyAlignmentTask : public SplicedAlignmentTask {
    Q_OBJECT
public:
                                SpideyAlignmentTask(const SplicedAlignmentTaskConfig &config, const QString &annDescription);
    void                        prepare();
    QList<SharedAnnotationData> getAlignmentResult() { return resultAnnotations; }
    Task::ReportResult          report();
    QList<Task *>               onSubTaskFinished(Task* subTask);

private:
    QList<SharedAnnotationData> resultAnnotations;
    QString                     tmpDirUrl;
    QString                     tmpOutputUrl;
    const QString               annDescription;
    PrepareInputForSpideyTask*  prepareDataForSpideyTask;
    ExternalToolRunTask*        spideyTask;
};

class SpideyAlignmentTaskFactory : public SplicedAlignmentTaskFactory {
    virtual SplicedAlignmentTask * createTaskInstance(const SplicedAlignmentTaskConfig &config) {
        return new SpideyAlignmentTask(config, "");
    }
};

class SpideyLogParser : public ExternalToolLogParser {
public:
            SpideyLogParser();
    int     getProgress();
};


class SpideySupportTask : public Task {
    Q_OBJECT
public:
                                SpideySupportTask(const SplicedAlignmentTaskConfig &cfg,
                                    AnnotationTableObject *aobj, const QString &annDescription);
    void                        prepare();
    QList<Task *>               onSubTaskFinished(Task *subTask);
private:
    SpideyAlignmentTask *       spideyAlignmentTask;
    AnnotationTableObject *     aObj;
};

}//namespace

#endif // _U2_SPIDEY_SUPPORT_TASK_H_
