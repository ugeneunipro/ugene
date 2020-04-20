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

#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "SpideySupport.h"
#include "SpideySupportTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
////SpideySupportTask

SpideyAlignmentTask::SpideyAlignmentTask(const SplicedAlignmentTaskConfig &settings, const QString &annDescription)
    : SplicedAlignmentTask("SpideySupportTask", TaskFlags_NR_FOSCOE, settings),
    annDescription(annDescription)
{
    GCOUNTER(cvar, tvar, "SpideySupportTask");
    setMaxParallelSubtasks(1);
    spideyTask = NULL;
    prepareDataForSpideyTask = NULL;
}

void SpideyAlignmentTask::prepare() {
    //Add new subdir for temporary files
    tmpDirUrl = ExternalToolSupportUtils::createTmpDir(SpideySupport::SPIDEY_TMP_DIR, stateInfo);
    CHECK_OP(stateInfo,);

    prepareDataForSpideyTask =
            new PrepareInputForSpideyTask(config.getGenomicSequence(), config.getCDnaSequence(),
                                                             tmpDirUrl);
    addSubTask(prepareDataForSpideyTask);
}

QList<Task *> SpideyAlignmentTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    propagateSubtaskError();

    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == prepareDataForSpideyTask) {
        SAFE_POINT(!prepareDataForSpideyTask->getResultPath().isEmpty(), "Invalid result path!",
            res);

        tmpOutputUrl = prepareDataForSpideyTask->getResultPath();
        const QStringList &arguments = prepareDataForSpideyTask->getArgumentsList();

        spideyTask = new ExternalToolRunTask(SpideySupport::ET_SPIDEY_ID, arguments, new SpideyLogParser());
        spideyTask->setSubtaskProgressWeight(95);
        res.append(spideyTask);
    } else if (subTask == spideyTask) {
        if (!QFile::exists(tmpOutputUrl)) {
            if (AppContext::getExternalToolRegistry()->getById(SpideySupport::ET_SPIDEY_ID)->isValid()) {
                stateInfo.setError(tr("Output file not found"));
            } else {
                ExternalTool *spideyTool = AppContext::getExternalToolRegistry()->getById(SpideySupport::ET_SPIDEY_ID);
                SAFE_POINT(NULL != spideyTool, "Invalid Spidey tool!", res);
                stateInfo.setError(
                    tr("Output file not found. May be %1 tool path '%2' not valid?")
                    .arg(spideyTool->getName()).arg(spideyTool->getPath()));
            }
            return res;
        }

        // parse result

        QFile resultFile(tmpOutputUrl);

        if (!resultFile.open(QFile::ReadOnly)) {
            setError(tr("Failed to open result file %1").arg(tmpOutputUrl));
            return res;
        }

        QTextStream inStream(&resultFile);
        bool strandDirect = true;

        U2Location location;
        location->op = U2LocationOperator_Join;

        while (!inStream.atEnd()) {
            QByteArray buf = inStream.readLine().toLatin1();
            if (buf.startsWith("Strand")) {
                strandDirect = buf.contains("plus");
            }
            if (buf.startsWith("Exon")) {
                // TODO: better to use reg exp here
                int startPos = buf.indexOf(":") + 1;
                int endPos = buf.indexOf("(gen)");
                if (startPos == -1 || endPos == -1) {
                    continue;
                }
                QByteArray loc = buf.mid(startPos, endPos - startPos).trimmed();
                QList<QByteArray> loci = loc.split('-');
                if (loci.size() < 2) {
                    continue;
                }
                int start = QString(loci.at(0)).toInt();
                int finish = QString(loci.at(1)).toInt();

                if (start == finish) {
                    continue;
                }

               location->regions.append(U2Region(start - 1, finish - start + 1));
            }
        }

        if (!location->isEmpty()) {
            SharedAnnotationData data(new AnnotationData);
            data->location = location;
            data->setStrand(U2Strand(strandDirect ? U2Strand::Direct : U2Strand::Complementary));
            data->type = U2FeatureTypes::Exon;
            data->name = "exon";
            U1AnnotationUtils::addDescriptionQualifier(data, annDescription);
            resultAnnotations.append(data);
        }
    }

    return res;
}

Task::ReportResult SpideyAlignmentTask::report() {
    U2OpStatus2Log os;
    ExternalToolSupportUtils::removeTmpDir(tmpDirUrl, os);

    return ReportResult_Finished;
}

//////////////////////////////////////////
////SpideyLogParser

SpideyLogParser::SpideyLogParser() {

}

int SpideyLogParser::getProgress() {
    return 0;
}

//////////////////////////////////////////
////PrepareInput

PrepareInputForSpideyTask::PrepareInputForSpideyTask(const U2SequenceObject *dna,
    const U2SequenceObject *mRna, const QString& outputDirPath)
    : Task("PrepareInputForSpideyTask", TaskFlags_FOSCOE), dnaObj(dna), mRnaObj(mRna),
    outputDir(outputDirPath)
{

}

#define SPIDEY_SUMMARY "spidey_output"
#define DNA_NAME "genomic.fa"
#define MRNA_NAME "mrna.fa"

void PrepareInputForSpideyTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }
    // writing DNA

    const QString dnaPath = outputDir + "/" + DNA_NAME;
    StreamShortReadWriter dnaWriter;
    dnaWriter.init(dnaPath);
    if (!dnaWriter.writeNextSequence(dnaObj)) {
        setError(tr("Failed to write DNA sequence  %1").arg(dnaObj->getSequenceName()));
        return;
    }
    dnaWriter.close();
    argumentList.append("-i");
    argumentList.append(dnaPath);

    //writing mRNA

    const QString mRnaPath = outputDir + "/" + MRNA_NAME;
    StreamShortReadWriter mRnaWriter;
    mRnaWriter.init(mRnaPath);
    if (!mRnaWriter.writeNextSequence(mRnaObj)) {
       setError(tr("Failed to write DNA sequence  %1").arg(mRnaObj->getSequenceName()));
       return;
    }
    mRnaWriter.close();
    argumentList.append("-m");
    argumentList.append(mRnaPath);

    // adding additional arguments

    resultPath = QString("%1/%2").arg(outputDir).arg(SPIDEY_SUMMARY);
    argumentList.append("-p");
    argumentList.append("1");
    argumentList.append("-o");
    argumentList.append(resultPath);
}

//////////////////////////////////////////
////SpideySupportTask

SpideySupportTask::SpideySupportTask(const SplicedAlignmentTaskConfig &cfg,
    AnnotationTableObject *ao, const QString &annDescription)
    : Task("SpideySupportTask", TaskFlags_NR_FOSCOE),
    spideyAlignmentTask(new SpideyAlignmentTask(cfg, annDescription)), aObj(ao)
{

}

void SpideySupportTask::prepare() {
    addSubTask(spideyAlignmentTask);
}

QList<Task *> SpideySupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == spideyAlignmentTask) {
        const QList<SharedAnnotationData> results = spideyAlignmentTask->getAlignmentResult();
        if (results.isEmpty()) {
            setError(tr("Failed to align mRNA to genomic sequence: no alignment is found."));
            DocumentUtils::removeDocumentsContainigGObjectFromProject(aObj);
            aObj = NULL;
        } else {
            aObj->addAnnotations(results);
        }
    }

    return res;
}

} //namespace U2
