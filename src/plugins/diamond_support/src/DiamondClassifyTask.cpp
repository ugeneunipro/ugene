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

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>

#include "DiamondClassifyTask.h"
#include "DiamondSupport.h"

namespace U2 {

const QString DiamondClassifyTaskSettings::SENSITIVE_DEFAULT("default");
const QString DiamondClassifyTaskSettings::SENSITIVE_ULTRA("ultra");
const QString DiamondClassifyTaskSettings::SENSITIVE_HIGH("high");
const QString DiamondClassifyTaskSettings::BLOSUM45("BLOSUM45");
const QString DiamondClassifyTaskSettings::BLOSUM50("BLOSUM50");
const QString DiamondClassifyTaskSettings::BLOSUM62("BLOSUM62");
const QString DiamondClassifyTaskSettings::BLOSUM80("BLOSUM80");
const QString DiamondClassifyTaskSettings::BLOSUM90("BLOSUM90");
const QString DiamondClassifyTaskSettings::PAM250("PAM250");
const QString DiamondClassifyTaskSettings::PAM70("PAM70");
const QString DiamondClassifyTaskSettings::PAM30("PAM30");

DiamondClassifyTaskSettings::DiamondClassifyTaskSettings()
    : sensitive(SENSITIVE_DEFAULT),
      topAlignmentsPercentage(10),
      matrix(BLOSUM62),
      max_evalue(0.001),
      block_size(2.0),
      gencode(1),
      frame_shift(0),
      gap_open(-1),
      gap_extend(-1),
      index_chunks(4),
      num_threads(1)
{

}

const QString DiamondClassifyTask::TAXONOMIC_CLASSIFICATION_OUTPUT_FORMAT = "102";  // from the DIAMOND manual

DiamondClassifyTask::DiamondClassifyTask(const DiamondClassifyTaskSettings &settings)
    : ExternalToolSupportTask(tr("Classify sequences with DIAMOND"), TaskFlags_FOSE_COSC),
      settings(settings)
{
    GCOUNTER(cvar, tvar, "DiamondClassifyTask");
    checkSettings();
    CHECK_OP(stateInfo, );
}

const QString &DiamondClassifyTask::getClassificationUrl() const {
    return settings.classificationUrl;
}

const LocalWorkflow::TaxonomyClassificationResult & DiamondClassifyTask::getParsedReport() const {
    return parsedReport;
}

void DiamondClassifyTask::run() {
    QFile reportFile(settings.classificationUrl);
    if (!reportFile.open(QIODevice::ReadOnly)) {
        setError(tr("Cannot open classification report: %1").arg(settings.classificationUrl));
    } else {
        QByteArray line;

        while ((line = reportFile.readLine()).size() != 0) {

            QList<QByteArray> row = line.split('\t');
            if (row.size() == 3) {
                QString objID = row[0];
                QByteArray &assStr = row[1];
                algoLog.trace(QString("Found Diamond classification: %1=%2").arg(objID).arg(QString(assStr)));

                bool ok = true;
                LocalWorkflow::TaxID assID = assStr.toUInt(&ok);
                if (ok) {
                    if (parsedReport.contains(objID)) {
                        QString msg = tr("Duplicate sequence name '%1' have been detected in the classification output.").arg(objID);
                        algoLog.info(msg);
                    } else {
                        parsedReport.insert(objID, assID);
                    }
                    continue;
                }
            }
            setError(tr("Broken Diamond report : %1").arg(settings.classificationUrl));
            break;
        }
        reportFile.close();
    }
}

void DiamondClassifyTask::prepare() {
    ExternalToolRunTask *classifyTask = new ExternalToolRunTask(DiamondSupport::TOOL_ID, getArguments(), new ExternalToolLogParser());
    setListenerForTask(classifyTask);
    addSubTask(classifyTask);
}

void DiamondClassifyTask::checkSettings() {
    SAFE_POINT_EXT(!settings.readsUrl.isEmpty(), setError(tr("Reads URL is empty")), );
    SAFE_POINT_EXT(!settings.databaseUrl.isEmpty(), setError(tr("DIAMOND database URL is empty")), );
    SAFE_POINT_EXT(!settings.classificationUrl.isEmpty(), setError(tr("DIAMOND classification URL is empty")), );
    QString id = DNATranslationID(%1);
    SAFE_POINT_EXT(AppContext::getDNATranslationRegistry()->lookupTranslation(id.arg(settings.gencode)) != NULL,
                   setError(tr("Invalid genetic code: %1").arg(settings.gencode)), );
    // TODO validate matrix value??
}

QStringList DiamondClassifyTask::getArguments() const {
    QStringList arguments;
    arguments << "blastx";
    arguments << "-d" << settings.databaseUrl;
    arguments << "-f" << TAXONOMIC_CLASSIFICATION_OUTPUT_FORMAT;
    arguments << "-q" << settings.readsUrl;
    arguments << "-o" << settings.classificationUrl;

    if (DiamondClassifyTaskSettings::SENSITIVE_HIGH.compare(settings.sensitive, Qt::CaseInsensitive) == 0) {
        arguments << "--sensitive";
    } else if (DiamondClassifyTaskSettings::SENSITIVE_ULTRA.compare(settings.sensitive, Qt::CaseInsensitive) == 0) {
        arguments << "--more-sensitive";
    } else if (DiamondClassifyTaskSettings::SENSITIVE_DEFAULT.compare(settings.sensitive, Qt::CaseInsensitive) != 0) {
        algoLog.error(tr("Unknown sensitivity value: %1, ignored.").arg(settings.sensitive));
    }
    arguments << "--top" << QString::number(settings.topAlignmentsPercentage);
    arguments << "--matrix" << settings.matrix;
    arguments << "-e" << QString::number(settings.max_evalue);
    arguments << "-b" << QString::number(settings.block_size);
    arguments << "-p" << QString::number(settings.num_threads);
    if (settings.gencode > 1) {
        arguments << "--query-gencode" << QString::number(settings.gencode);
    }
    if (settings.frame_shift != 0) {
        arguments << "-F" << QString::number(settings.frame_shift);
    }
    if (settings.gap_open != -1) {
        arguments << "--gapopen" << QString::number(settings.gap_open);
    }
    if (settings.gap_extend != -1) {
        arguments << "--gapextend" << QString::number(settings.gap_extend);
    }
    if (settings.index_chunks != 0) {
        arguments << "--index-chunks" << QString::number(settings.index_chunks);
    }

    return arguments;
}

}   // namespace U2
