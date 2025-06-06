/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "DNAExportPluginTests.h"

#include <QTemporaryFile>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MsaObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ExportTasks.h>

#include "ImportQualityScoresTask.h"

namespace U2 {

#define URL_ATTR "url"
#define SEQLIST_ATTR "seq-list"
#define NUCL_ALIGN_URL_ATTR "nucleic-url"
#define EXP_ALIGN_URL_ATTR "exp-url"
#define EXTRACT_ROWS_ATTR "rows"
#define TRANS_TABLE_ATTR "trans-table"
#define INCLUDE_GAPS "include-gaps"
#define UNKNOWN_AMINO_2_GAP "unknown-amino-to-gap"
#define TRANSLATION_FRAME "translation-frame"

void GTest_ImportPhredQualityScoresTask::init(XMLTestFormat*, const QDomElement& el) {
    QString buf = el.attribute(SEQLIST_ATTR);
    if (buf.isEmpty()) {
        failMissingValue(SEQLIST_ATTR);
        return;
    }

    seqNameList = buf.split(",");
    if (seqNameList.count() == 0) {
        stateInfo.setError("No sequences are found.");
        return;
    }

    fileName = el.attribute(URL_ATTR);
    if (fileName.isEmpty()) {
        failMissingValue(URL_ATTR);
    }

    fileName = env->getVar("COMMON_DATA_DIR") + "/" + fileName;
}

void GTest_ImportPhredQualityScoresTask::prepare() {
    foreach (const QString& seqName, seqNameList) {
        GObject* obj = getContext<GObject>(this, seqName);
        if (obj == nullptr) {
            stateInfo.setError(QString("wrong sequence name: %1").arg(seqName));
            return;
        }

        auto mySequence = qobject_cast<U2SequenceObject*>(obj);
        if (mySequence == nullptr) {
            stateInfo.setError(QString("Can't cast to sequence from: %1").arg(obj->getGObjectName()));
            return;
        }

        seqList.append(mySequence);
    }

    ImportQualityScoresConfig cfg;
    cfg.createNewDocument = false;
    cfg.fileName = fileName;
    cfg.type = DNAQualityType_Sanger;

    Task* importTask = new ImportPhredQualityScoresTask(seqList, cfg);

    addSubTask(importTask);
}

void GTest_ExportNucleicToAminoAlignmentTask::init(XMLTestFormat*, const QDomElement& el) {
    QString buf;

    buf = el.attribute(NUCL_ALIGN_URL_ATTR);
    if (buf.isEmpty()) {
        failMissingValue(NUCL_ALIGN_URL_ATTR);
        return;
    }
    inputFile = buf;

    QTemporaryFile qtf(env->getVar("TEMP_DATA_DIR") + "/XXXXXX.aln");
    if (qtf.open()) {
        outputFileName = qtf.fileName();
    } else {
        stateInfo.setError(GTest::tr("Unable to create temporary file"));
    }

    buf = el.attribute(EXP_ALIGN_URL_ATTR);
    if (buf.isEmpty()) {
        failMissingValue(EXP_ALIGN_URL_ATTR);
        return;
    }
    expectedOutputFile = buf;

    buf = el.attribute(TRANS_TABLE_ATTR);
    if (buf.isEmpty()) {
        failMissingValue(TRANS_TABLE_ATTR);
        return;
    }
    bool ok;
    ok = false;
    transTable = buf.toInt(&ok);
    if (!ok || transTable < 0) {
        stateInfo.setError(GTest::tr("Invalid translation table num: %1").arg(buf));
        return;
    }

    selectedRows = U2Region();
    buf = el.attribute(EXTRACT_ROWS_ATTR);
    if (!buf.isEmpty()) {
        QStringList sl = buf.split(",");
        if (sl.size() != 2) {
            assert(0);
            return;
        }

        ok = false;
        int base = sl[0].toInt(&ok);
        if (!ok || base < 0) {
            stateInfo.setError(GTest::tr("Invalid base : %1").arg(sl[0]));
            return;
        }
        ok = false;
        int len = sl[1].toInt(&ok);
        if (!ok || len < 0) {
            stateInfo.setError(GTest::tr("Invalid base : %1").arg(sl[1]));
            return;
        }
        selectedRows = U2Region(base, len);
    }

    buf = el.attribute(INCLUDE_GAPS);
    if (!buf.isEmpty() && buf == "true") {
        includeGaps = true;
    }

    buf = el.attribute(UNKNOWN_AMINO_2_GAP);
    if (!buf.isEmpty() && buf == "true") {
        convertUnknownAmino2Gap = true;
    }

    buf = el.attribute(TRANSLATION_FRAME);
    if (!buf.isEmpty()) {
        ok = false;
        int frame = buf.toInt(&ok);
        if (!ok || frame == 0 || frame > 3 || frame < -3) {
            stateInfo.setError(GTest::tr("Invalid translation frame : %1").arg(frame));
            return;
        }
        translationFrame = frame;
    }
}

void GTest_ExportNucleicToAminoAlignmentTask::prepare() {
    if (hasError()) {
        return;
    }
    Document* doc = getContext<Document>(this, inputFile);
    if (doc == nullptr) {
        stateInfo.setError(GTest::tr(" context not found %1").arg(inputFile));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(GTest::tr(" container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }
    auto alObj = qobject_cast<MsaObject*>(list.first());
    srcAl = alObj->getAlignment()->getCopy();

    QString translationId = DNATranslationID(0);
    translationId.replace("0", QString("%1").arg(transTable));
    const DNATranslation* translation = AppContext::getDNATranslationRegistry()->lookupTranslation(translationId);

    bool reverseComplement = translationFrame < 0;
    int offset = qAbs(translationFrame) - 1;
    QList<qint64> rowIds;
    if (!selectedRows.isEmpty()) {
        rowIds = srcAl->getRowsIds();
        CHECK_EXT(U2Region(0, rowIds.size()).contains(selectedRows), stateInfo.setError("Invalid row range"), );
        rowIds = rowIds.mid(selectedRows.startPos, selectedRows.length);
    }
    U2Region columnRegion(0, srcAl->getLength());
    exportTask = new ExportMSA2MSATask(srcAl,
                                       rowIds,
                                       columnRegion,
                                       outputFileName,
                                       translation,
                                       BaseDocumentFormats::CLUSTAL_ALN,
                                       !includeGaps,
                                       convertUnknownAmino2Gap,
                                       reverseComplement,
                                       offset);
    addSubTask(exportTask);
}

QList<Task*> GTest_ExportNucleicToAminoAlignmentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || subTask->hasError() || isCanceled()) {
        return res;
    }

    if (subTask == exportTask) {
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(outputFileName));
        resultLoadTask = new LoadDocumentTask(BaseDocumentFormats::CLUSTAL_ALN, outputFileName, iof);
        res << resultLoadTask;
    } else if (subTask == resultLoadTask) {
        Document* resdoc = resultLoadTask->getDocument();

        if (resdoc == nullptr) {
            stateInfo.setError(GTest::tr("context  not found %1").arg(outputFileName));
            return res;
        }

        QList<GObject*> reslist = resdoc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (reslist.size() == 0) {
            stateInfo.setError(GTest::tr("container  of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
            return res;
        }
        auto resAlign = qobject_cast<MsaObject*>(reslist.first());
        resAl = resAlign->getAlignment()->getCopy();
    }
    return res;
}

Task::ReportResult GTest_ExportNucleicToAminoAlignmentTask::report() {
    propagateSubtaskError();
    if (hasError()) {
        return ReportResult_Finished;
    }

    Document* expdoc = getContext<Document>(this, expectedOutputFile);
    if (expdoc == nullptr) {
        stateInfo.setError(GTest::tr("context not  found %1").arg(expectedOutputFile));
        return ReportResult_Finished;
    }

    QList<GObject*> explist = expdoc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (explist.size() == 0) {
        stateInfo.setError(GTest::tr("container of  object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return ReportResult_Finished;
    }
    auto expAlign = qobject_cast<MsaObject*>(explist.first());
    const Msa expAl = expAlign->getAlignment();

    if (resAl->getLength() != expAl->getLength()) {
        stateInfo.setError(GTest::tr("Unexpected alignment length %1, expected %2").arg(resAl->getLength()).arg(expAl->getLength()));
        return ReportResult_Finished;
    }

    if (resAl->getRowCount() != expAl->getRowCount()) {
        stateInfo.setError(GTest::tr("Unexpected alignment size %1, expected %2").arg(resAl->getRowCount()).arg(expAl->getRowCount()));
        return ReportResult_Finished;
    }

    QStringList resNames = resAl->getRowNames();
    QStringList expNames = expAl->getRowNames();

    for (int i = 0; i < resAl->getRowCount(); i++) {
        if (resNames[i] != expNames[i]) {
            stateInfo.setError(GTest::tr("Invalid name for row %1: %2, expected %3").arg(i + 1).arg(resNames[i]).arg(expNames[i]));
            return ReportResult_Finished;
        }
        for (int j = 0; j < resAl->getLength(); j++) {
            if (resAl->charAt(i, j) != expAl->charAt(i, j)) {
                stateInfo.setError(GTest::tr("Invalid char at row %1 column %2: %3, expected %4").arg(i + 1).arg(j + 1).arg(resAl->charAt(i, j)).arg(expAl->charAt(i, j)));
                return ReportResult_Finished;
            }
        }
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory*> DNAExportPluginTests::createTestFactories() {
    QList<XMLTestFactory*> factories;
    factories.append(GTest_ImportPhredQualityScoresTask::createFactory());
    factories.append(GTest_ExportNucleicToAminoAlignmentTask::createFactory());
    return factories;
}

}  // namespace U2
