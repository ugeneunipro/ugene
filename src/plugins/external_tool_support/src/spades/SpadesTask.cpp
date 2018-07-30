
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

#include <QDir>
#include <QTextStream>
#include <QFileInfo>

#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/AppResources.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/FileAndDirectoryUtils.h>

#include "SpadesSupport.h"
#include "SpadesTask.h"
#include "SpadesWorker.h"

namespace U2 {
// SpadesTask

const QString SpadesTask::OPTION_DATASET_TYPE = "dataset-type";
const QString SpadesTask::OPTION_RUNNING_MODE = "running-mode";
const QString SpadesTask::OPTION_K_MER = "k-mer";
const QString SpadesTask::OPTION_INPUT_DATA = "input-data";
const QString SpadesTask::OPTION_THREADS = "threads";
const QString SpadesTask::OPTION_MEMLIMIT = "memlimit";
const QString SpadesTask::YAML_FILE_NAME = "datasets.yaml";
const QString SpadesTask::CONTIGS_NAME = "contigs.fasta";
const QString SpadesTask::SCAFFOLDS_NAME = "scaffolds.fasta";

SpadesTask::SpadesTask(const  GenomeAssemblyTaskSettings &settings):
    GenomeAssemblyTask(settings, TaskFlags_NR_FOSCOE)
{
    GCOUNTER(cvar, tvar, "SpadesTask");
}

void SpadesTask::prepare() {
    const QDir outDir = QFileInfo(settings.outDir.getURLString()).absoluteDir();
    if (!outDir.exists()) {
        stateInfo.setError(tr("Folder does not exist: ") + outDir.absolutePath());
        return ;
    }
    writeYamlReads();
    if(hasError()){
       return;
    }

    QStringList arguments;

    if (settings.getCustomValue(SpadesTask::OPTION_DATASET_TYPE, LocalWorkflow::SpadesWorker::DATASET_TYPE_STANDARD_ISOLATE).toString() == LocalWorkflow::SpadesWorker::DATASET_TYPE_MDA_SINGLE_CELL) {
        arguments.append("--sc");
    }

    QString runningMode = settings.getCustomValue(SpadesTask::OPTION_RUNNING_MODE, LocalWorkflow::SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_AND_ASSEMBLY).toString();
    if (runningMode == LocalWorkflow::SpadesWorker::RUNNING_MODE_ASSEMBLY_ONLY) {
        arguments.append("--only-assembler");
    } else if (runningMode == LocalWorkflow::SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_ONLY) {
        arguments.append("--only-error-correction");
    }

    QVariantMap inputDataDialogSettings = settings.getCustomValue(SpadesTask::OPTION_INPUT_DATA, QVariantMap()).toMap();
    QString sequencingPlatform = inputDataDialogSettings.value(LocalWorkflow::SpadesWorkerFactory::SEQUENCING_PLATFORM_ID, QString()).toString();
    if (sequencingPlatform == PLATFORM_IONTORRENT) {
        arguments.append(sequencingPlatform);
    }

    arguments.append("--dataset");
    arguments.append(settings.outDir.getURLString() + QDir::separator() + SpadesTask::YAML_FILE_NAME);

    arguments.append("-t");
    arguments.append(settings.getCustomValue(SpadesTask::OPTION_THREADS, "16").toString());

    arguments.append("-m");
    arguments.append(settings.getCustomValue(SpadesTask::OPTION_MEMLIMIT, "250").toString());

    QString k = settings.getCustomValue(SpadesTask::OPTION_K_MER, LocalWorkflow::SpadesWorker::K_MER_AUTO).toString();
    if (k != LocalWorkflow::SpadesWorker::K_MER_AUTO) {
        arguments.append("-k");
        arguments.append(k);
    }

    arguments.append("-o");
    arguments.append(settings.outDir.getURLString());

    //it uses system call gzip. it might not be installed
    arguments.append("--disable-gzip-output");

    assemblyTask = new ExternalToolRunTask(ET_SPADES, arguments, new SpadesLogParser(), settings.outDir.getURLString());
    if (!settings.listeners.isEmpty()) {
        assemblyTask->addOutputListener(settings.listeners.first());
    }
    addSubTask(assemblyTask);
}

Task::ReportResult SpadesTask::report() {
    CHECK(!hasError(), ReportResult_Finished);
    CHECK(!isCanceled(), ReportResult_Finished);

    QString res = settings.outDir.getURLString() + "/" + SpadesTask::SCAFFOLDS_NAME;
    if(!FileAndDirectoryUtils::isFileEmpty(res)){
        resultUrl = res;
    }else{
        stateInfo.setError(tr("File %1 has not been found in output folder %2").arg(SpadesTask::SCAFFOLDS_NAME).arg(settings.outDir.getURLString()));
    }

    QString contigs = settings.outDir.getURLString() + "/" + SpadesTask::CONTIGS_NAME;
    if (!FileAndDirectoryUtils::isFileEmpty(res)) {
        contigsUrl = contigs;
    } else {
        stateInfo.setError(tr("File %1 has not been found in output folder %2").arg(SpadesTask::CONTIGS_NAME).arg(settings.outDir.getURLString()));
    }


    return ReportResult_Finished;
}

QString SpadesTask::getScaffoldsUrl() const {
    return resultUrl;
}

QString SpadesTask::getContigsUrl() const {
    return contigsUrl;
}

QList<Task *> SpadesTask::onSubTaskFinished(Task * /*subTask*/) {
    QList<Task *> result;
    return result;
}

void SpadesTask::writeYamlReads(){
    QFile yaml (settings.outDir.getURLString() + QDir::separator() + YAML_FILE_NAME);
    if(!yaml.open(QFile::WriteOnly)){
        stateInfo.setError(QString("Cannot open write settings file %1").arg(settings.outDir.getURLString() + QDir::separator() + YAML_FILE_NAME));
        return;
    }
    QString res = "";
    res.append("[\n");
    foreach (const AssemblyReads& r , settings.reads){
        res.append("{\n");
        if (LocalWorkflow::SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST.contains(r.libName)) {
            res.append(QString("orientation: \"%1\",\n").arg(r.orientation));
        }
        res.append(QString("type: \"%1\",\n").arg(r.libName));
        if(!GenomeAssemblyUtils::hasRightReads(r.libName)) {
            res.append(QString("%1: [\n").arg(r.readType));

            foreach(const GUrl& url, r.left) {
                res.append(QString("\"%1\",\n").arg(url.getURLString()));
            }
            res.append("]\n");
        }else{
            res.append("left reads: [\n");
            foreach(const GUrl& url, r.left) {
                res.append(QString("\"%1\",\n").arg(url.getURLString()));
            }
            res.append("],\n");
            res.append("right reads: [\n");
            foreach(const GUrl& url, r.right) {
                res.append(QString("\"%1\",\n").arg(url.getURLString()));
            }
            res.append("],\n");
        }
        res.append("},\n");

    }
    res.append("]\n");

    QTextStream outStream(&yaml);
    outStream << res;
}

// SpadesTaskFactory

GenomeAssemblyTask *SpadesTaskFactory::createTaskInstance(const GenomeAssemblyTaskSettings &settings) {
    return new SpadesTask(settings);
}

SpadesLogParser::SpadesLogParser():ExternalToolLogParser(){

}

void SpadesLogParser::parseOutput(const QString &partOfLog){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastLine+lastPartOfLog.first();
    lastLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains("== Error == ")
            || buf.contains(" ERROR ")){
                coreLog.error("Spades: " + buf);
                setLastError(buf);
        }else if (buf.contains("== Warning == ")
                  || buf.contains(" WARN ")){
            algoLog.info(buf);
        }else {
            ioLog.trace(buf);
        }
    }
}

void SpadesLogParser::parseErrOutput(const QString &partOfLog){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains("== Error == ")
            || buf.contains(" ERROR ")){
                coreLog.error("Spades: " + buf);
                setLastError(buf);
        }else if (buf.contains("== Warning == ")
                  || buf.contains(" WARN ")){
            algoLog.info(buf);
        }else {
            algoLog.trace(buf);
        }
    }
}

} // namespace U2

