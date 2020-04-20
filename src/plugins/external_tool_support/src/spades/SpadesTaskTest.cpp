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

#include <QDomElement>
#include <QFile>

#include <U2Core/L10n.h>

#include "SpadesTaskTest.h"
#include "SpadesWorker.h"
#include "utils/OutputCollector.h"

namespace U2 {

const QString GTest_SpadesTaskTest::SEQUENCING_PLATFORM = "platform";

const QString GTest_SpadesTaskTest::PAIRED_END_READS = "pe_reads";
const QString GTest_SpadesTaskTest::PAIRED_END_READS_ORIENTATION = "pe_reads_orientation";
const QString GTest_SpadesTaskTest::PAIRED_END_READS_TYPE = "pe_reads_type";

const QString GTest_SpadesTaskTest::HIGH_QUALITY_MATE_PAIRS = "hq_mp";
const QString GTest_SpadesTaskTest::HIGH_QUALITY_MATE_PAIRS_ORIENTATION = "hq_mp_orientation";
const QString GTest_SpadesTaskTest::HIGH_QUALITY_MATE_PAIRS_TYPE = "hq_mp_type";

const QString GTest_SpadesTaskTest::UNPAIRED_READS = "unpaired_reads";
const QString GTest_SpadesTaskTest::PACBIO_CCS_READS = "pbccs_reads";

const QString GTest_SpadesTaskTest::MATE_PAIRS = "mp";
const QString GTest_SpadesTaskTest::MATE_PAIRS_ORIENTATION = "mp_orientation";
const QString GTest_SpadesTaskTest::MATE_PAIRS_TYPE = "mp_type";

const QString GTest_SpadesTaskTest::PACBIO_CLR_READS = "pbclr_reads";
const QString GTest_SpadesTaskTest::OXFORD_NANOPORE_READS = "onp_reads";
const QString GTest_SpadesTaskTest::SANGER_READS = "sanger_reads";
const QString GTest_SpadesTaskTest::TRUSTED_CONTIGS = "trusted_contigs";
const QString GTest_SpadesTaskTest::UNTRUSTED_CONTIGS = "untrusted_contigs";

const QString GTest_SpadesTaskTest::OUTPUT_DIR = "out";
const QString GTest_SpadesTaskTest::DESIRED_PARAMETERS = "desired_parameters";

void GTest_SpadesTaskTest::init(XMLTestFormat *tf, const QDomElement& el) {
    QVariantMap inputDataSettings;
    QString elementStr = el.attribute(SEQUENCING_PLATFORM);
    if (elementStr == "iontorrent") {
        inputDataSettings.insert(LocalWorkflow::SpadesWorkerFactory::SEQUENCING_PLATFORM_ID, QVariant(PLATFORM_ION_TORRENT));
    }

    elementStr = el.attribute(OUTPUT_DIR);
    if (elementStr.isEmpty()) {
        stateInfo.setError("output_dir_is_empty");
        return;
    }
    taskSettings.outDir = env->getVar("TEMP_DATA_DIR") + "/" + elementStr;

    elementStr = el.attribute(PAIRED_END_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_PAIR_DEFAULT;
        elementStr = el.attribute(PAIRED_END_READS_ORIENTATION);
        if (elementStr.isEmpty()) {
            failMissingValue(PAIRED_END_READS_ORIENTATION);
            return;
        }

        if (elementStr == "fr") {
            reads.orientation = ORIENTATION_FR;
        } else if (elementStr == "rf") {
            reads.orientation = ORIENTATION_RF;
        } else if (elementStr == "ff") {
            reads.orientation = ORIENTATION_FF;
        } else {
            wrongValue(PAIRED_END_READS_ORIENTATION);
            return;
        }

        elementStr = el.attribute(PAIRED_END_READS_TYPE);
        if (elementStr.isEmpty()) {
            failMissingValue(PAIRED_END_READS_TYPE);
            return;
        }

        if (elementStr == "single") {
            reads.readType = TYPE_SINGLE;
        } else if (elementStr == "interlaced") {
            reads.readType = TYPE_INTERLACED;
        } else {
            wrongValue(PAIRED_END_READS_TYPE);
            return;
        }
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(HIGH_QUALITY_MATE_PAIRS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_PAIR_MATE_HQ;
        elementStr = el.attribute(HIGH_QUALITY_MATE_PAIRS_ORIENTATION);
        if (elementStr.isEmpty()) {
            failMissingValue(HIGH_QUALITY_MATE_PAIRS_ORIENTATION);
            return;
        }

        if (elementStr == "fr") {
            reads.orientation = ORIENTATION_FR;
        } else if (elementStr == "rf") {
            reads.orientation = ORIENTATION_RF;
        } else if (elementStr == "ff") {
            reads.orientation = ORIENTATION_FF;
        } else {
            wrongValue(HIGH_QUALITY_MATE_PAIRS_ORIENTATION);
            return;
        }

        elementStr = el.attribute(HIGH_QUALITY_MATE_PAIRS_TYPE);
        if (elementStr.isEmpty()) {
            failMissingValue(HIGH_QUALITY_MATE_PAIRS_TYPE);
            return;
        }

        if (elementStr == "single") {
            reads.readType = TYPE_SINGLE;
        } else if (elementStr == "interlaced") {
            reads.readType = TYPE_INTERLACED;
        } else {
            wrongValue(HIGH_QUALITY_MATE_PAIRS_TYPE);
            return;
        }
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(MATE_PAIRS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_PAIR_MATE;
        elementStr = el.attribute(MATE_PAIRS_ORIENTATION);
        if (elementStr.isEmpty()) {
            failMissingValue(MATE_PAIRS_ORIENTATION);
            return;
        }

        if (elementStr == "fr") {
            reads.orientation = ORIENTATION_FR;
        } else if (elementStr == "rf") {
            reads.orientation = ORIENTATION_RF;
        } else if (elementStr == "ff") {
            reads.orientation = ORIENTATION_FF;
        } else {
            wrongValue(MATE_PAIRS_ORIENTATION);
            return;
        }

        elementStr = el.attribute(MATE_PAIRS_TYPE);
        if (elementStr.isEmpty()) {
            failMissingValue(MATE_PAIRS_TYPE);
            return;
        }

        if (elementStr == "single") {
            reads.readType = TYPE_SINGLE;
        } else if (elementStr == "interlaced") {
            reads.readType = TYPE_INTERLACED;
        } else {
            wrongValue(MATE_PAIRS_TYPE);
            return;
        }
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(UNPAIRED_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_UNPAIRED;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(PACBIO_CCS_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_CSS;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(PACBIO_CLR_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_CLR;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(OXFORD_NANOPORE_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_NANOPORE;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(SANGER_READS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_SANGER;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(TRUSTED_CONTIGS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_TRUSTED;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(UNTRUSTED_CONTIGS);
    if (elementStr == "true") {
        AssemblyReads reads;
        reads.libName = LIB_SINGLE_UNTRUSTED;
        taskSettings.reads.append(reads);
    }

    elementStr = el.attribute(DESIRED_PARAMETERS);
    if (elementStr.isEmpty()) {
        failMissingValue(DESIRED_PARAMETERS);
        return;
    } else {
        desiredParameters = elementStr.split(";");
    }

    //generate read urls
    int counter = 1;
    QList<AssemblyReads>::iterator it(taskSettings.reads.begin());
    for (;it != taskSettings.reads.end(); it++) {
        AssemblyReads &reads = *it;
        if ((reads.libName.contains("mate") || reads.libName.contains("pair")) && reads.readType != TYPE_INTERLACED) {
            reads.left.append(GUrl(QString::number(counter++) + "_left_" + reads.libName + "_read"));
            reads.right.append(GUrl(QString::number(counter++) + "_right_" + reads.libName + "_read"));
        } else {
            reads.left.append(GUrl(QString::number(counter++) + "_" + reads.libName + "_read"));
        }
    }
    taskSettings.setCustomValue(SpadesTask::OPTION_INPUT_DATA, inputDataSettings);
}

void GTest_SpadesTaskTest::prepare() {
    collector = new OutputCollector(false);
    taskSettings.listeners = QList<ExternalToolListener*>() << collector;
    spadesTask = new SpadesTask(taskSettings);
    addSubTask(spadesTask);
}

QList<Task*> GTest_SpadesTaskTest::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask == spadesTask) {
        QString log = collector->getLog();
        delete collector;
        foreach(const QString& el, desiredParameters) {
            if (!log.contains(el)) {
                stateInfo.setError(QString("Desired parameter %1 not found").arg(el));
                return res;
            }
        }
    }
    return res;
}

const QString GTest_CheckYAMLFile::STRINGS_TO_CHECK = "strings_to_check";
const QString GTest_CheckYAMLFile::INPUT_DIR = "input_dir";

void GTest_CheckYAMLFile::init(XMLTestFormat *tf, const QDomElement& el) {
    QVariantMap inputDataSettings;
    QString elementStr = el.attribute(STRINGS_TO_CHECK);
    if (elementStr.isEmpty()) {
        failMissingValue(STRINGS_TO_CHECK);
        return;
    } else {
        desiredStrings = elementStr.split(";");
    }

    elementStr = el.attribute(INPUT_DIR);
    if (elementStr.isEmpty()) {
        failMissingValue(INPUT_DIR);
        return;
    }
    fileToCheck = env->getVar("TEMP_DATA_DIR") + "/" + elementStr + "datasets.yaml";
}

void GTest_CheckYAMLFile::prepare() {
    QFile f(fileToCheck);
    if (!f.open(QIODevice::ReadOnly)) {
        setError(QString("Cannot open file '%1'!").arg(fileToCheck));
        return;
    }

    QStringList fileLines;
    while (!f.atEnd()) {
        QByteArray bytes = f.readLine();
        fileLines.append(bytes);
    }
    f.close();

    foreach(const QString& el, desiredStrings) {
        foreach(const QString& fileLane, fileLines) {
            if (fileLane.contains(el.trimmed())) {
                desiredStrings.removeAll(el);
            }
        }
    }
    if (desiredStrings.size() != 0) {
        setError(QString("Line '%1' not found in yaml file!").arg(desiredStrings.first()));
    }
}

QList<XMLTestFactory*> SpadesTaskTest::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_SpadesTaskTest::createFactory());
    res.append(GTest_CheckYAMLFile::createFactory());

    return res;
}


}
