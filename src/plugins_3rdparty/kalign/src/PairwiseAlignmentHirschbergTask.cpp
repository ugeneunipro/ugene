#include "PairwiseAlignmentHirschbergTask.h"

#include <QRegExp>
#include <QString>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include <U2Lang/SimpleWorkflowTask.h>

#include "KalignTask.h"

namespace U2 {

const QString PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN("H_gapOpen");
const QString PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD("H_gapExtd");
const QString PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_TERM("H_gapTerm");
const QString PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE("H_bonusScore");
const QString PairwiseAlignmentHirschbergTaskSettings::PA_H_REALIZATION_NAME("H_realizationName");

PairwiseAlignmentHirschbergTaskSettings::PairwiseAlignmentHirschbergTaskSettings(const PairwiseAlignmentTaskSettings &s)
    : PairwiseAlignmentTaskSettings(s),
      gapOpen(0),
      gapExtd(0),
      gapTerm(0),
      bonusScore(0) {
}

PairwiseAlignmentHirschbergTaskSettings::~PairwiseAlignmentHirschbergTaskSettings() {
    // all dynamic objects will be destroyed by the task
}

bool PairwiseAlignmentHirschbergTaskSettings::convertCustomSettings() {
    gapOpen = customSettings.value(PA_H_GAP_OPEN, 217).toInt();
    gapExtd = customSettings.value(PA_H_GAP_EXTD, 39).toInt();
    gapTerm = customSettings.value(PA_H_GAP_TERM, 292).toInt();
    bonusScore = customSettings.value(PA_H_BONUS_SCORE, 283).toInt();

    PairwiseAlignmentTaskSettings::convertCustomSettings();
    return true;
}

PairwiseAlignmentHirschbergTask::PairwiseAlignmentHirschbergTask(PairwiseAlignmentHirschbergTaskSettings *_settings)
    : PairwiseAlignmentTask(_settings, TaskFlag_NoRun),
      settings(_settings),
      kalignSubTask(nullptr),
      workflowKalignSubTask(nullptr),
      alphabet(nullptr) {
    SAFE_POINT(settings != nullptr, "Task settings are not defined.", );
    SAFE_POINT(settings->convertCustomSettings() && settings->isValid(), "Invalid task settings.", );

    U2OpStatus2Log os;
    DbiConnection con(settings->msaRef.dbiRef, os);
    CHECK_OP_EXT(os, setError("Failed to get reference to DB"), );
    U2Sequence sequence = con.dbi->getSequenceDbi()->getSequenceObject(settings->firstSequenceRef.entityId, os);
    CHECK_OP_EXT(os, setError("Failed to get first sequence"), );
    first = con.dbi->getSequenceDbi()->getSequenceData(sequence.id, U2Region(0, sequence.length), os);
    CHECK_OP_EXT(os, setError("Failed to get first sequence data"), );
    QString firstName = sequence.visualName;

    sequence = con.dbi->getSequenceDbi()->getSequenceObject(settings->secondSequenceRef.entityId, os);
    CHECK_OP_EXT(os, setError("Failed to get second sequence"), );
    second = con.dbi->getSequenceDbi()->getSequenceData(sequence.id, U2Region(0, sequence.length), os);
    CHECK_OP_EXT(os, setError("Failed to get first sequence data"), );
    QString secondName = sequence.visualName;
    con.close(os);

    alphabet = U2AlphabetUtils::getById(settings->alphabet);
    SAFE_POINT(alphabet != nullptr, "Alphabet is invalid.", );

    ma = MultipleSequenceAlignment(firstName + " vs. " + secondName, alphabet);
    ma->addRow(firstName, first);
    ma->addRow(secondName, second);

    KalignTaskSettings kalignSettings;
    kalignSettings.gapOpenPenalty = settings->gapOpen;
    kalignSettings.gapExtenstionPenalty = settings->gapExtd;
    kalignSettings.termGapPenalty = settings->gapTerm;
    kalignSettings.secret = settings->bonusScore;

    kalignSubTask = new KalignTask(ma, kalignSettings);
    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);
    addSubTask(kalignSubTask);
}

QList<Task *> PairwiseAlignmentHirschbergTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask->hasError() || subTask->isCanceled()) {
        return res;
    }

    if (subTask == kalignSubTask) {
        if (settings->inNewWindow) {
            TaskStateInfo localStateInfo;
            Project *currentProject = AppContext::getProject();

            DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::CLUSTAL_ALN);

            QString newFileUrl = settings->resultFileName.getURLString();
            changeGivenUrlIfDocumentExists(newFileUrl, currentProject);

            Document *alignmentDoc = format->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(newFileUrl), localStateInfo);
            CHECK_OP(localStateInfo, res);

            MultipleSequenceAlignment resultMa = kalignSubTask->resultMA;

            MultipleSequenceAlignmentObject *docObject = MultipleSequenceAlignmentImporter::createAlignment(alignmentDoc->getDbiRef(), resultMa, localStateInfo);
            CHECK_OP(localStateInfo, res);

            alignmentDoc->addObject(docObject);

            SaveDocFlags flags = SaveDoc_OpenAfter;
            res << new SaveDocumentTask(alignmentDoc, flags);
        } else {  // in current window
            U2OpStatus2Log os;
            DbiConnection con(settings->msaRef.dbiRef, os);
            CHECK_OP(os, res);

            QList<U2MsaRow> rows = con.dbi->getMsaDbi()->getRows(settings->msaRef.entityId, os);
            CHECK_OP(os, res);
            U2UseCommonUserModStep userModStep(settings->msaRef, os);
            Q_UNUSED(userModStep);
            SAFE_POINT_OP(os, res);
            for (int rowNumber = 0; rowNumber < rows.length(); ++rowNumber) {
                if (rows[rowNumber].sequenceId == settings->firstSequenceRef.entityId) {
                    con.dbi->getMsaDbi()->updateGapModel(settings->msaRef.entityId, rows[rowNumber].rowId, kalignSubTask->resultMA->getMsaRow(0)->getGaps(), os);
                    CHECK_OP(os, res);
                }
                if (rows[rowNumber].sequenceId == settings->secondSequenceRef.entityId) {
                    con.dbi->getMsaDbi()->updateGapModel(settings->msaRef.entityId, rows[rowNumber].rowId, kalignSubTask->resultMA->getMsaRow(1)->getGaps(), os);
                    CHECK_OP(os, res);
                }
            }
        }
    }
    return res;
}

Task::ReportResult PairwiseAlignmentHirschbergTask::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

void PairwiseAlignmentHirschbergTask::changeGivenUrlIfDocumentExists(QString &givenUrl, const Project *curProject) {
    if (curProject->findDocumentByURL(GUrl(givenUrl)) != nullptr) {
        for (size_t i = 1;; i++) {
            QString tmpUrl = givenUrl;
            QRegExp dotWithExtensionRegExp("\\.{1,1}[^\\.]*$|^[^\\.]*$");
            dotWithExtensionRegExp.lastIndexIn(tmpUrl);
            tmpUrl.replace(dotWithExtensionRegExp.capturedTexts().last(), "(" + QString::number(i) + ")" + dotWithExtensionRegExp.capturedTexts().last());
            if (curProject->findDocumentByURL(GUrl(tmpUrl)) == nullptr) {
                givenUrl = tmpUrl;
                break;
            }
        }
    }
}

}  // namespace U2
