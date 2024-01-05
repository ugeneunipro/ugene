#include "KalignPairwiseAlignmentTask.h"

#include <QRegExp>
#include <QString>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Msa.h>
#include <U2Core/MsaImportUtils.h>
#include <U2Core/MsaObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include <U2Lang/SimpleWorkflowTask.h>

namespace U2 {

const QString KalignPairwiseAlignmentTaskSettings::GAP_OPEN_PENALTY_KEY("KALIGN_GAP_OPEN_PENALTY_KEY");
const QString KalignPairwiseAlignmentTaskSettings::GAP_EXTENSION_PENALTY_KEY("KALIGN_GAP_EXTENSION_PENALTY_KEY");
const QString KalignPairwiseAlignmentTaskSettings::TERMINAL_GAP_EXTENSION_PENALTY_KEY("KALIGN_TERMINAL_GAP_EXTENSION_PENALTY_KEY");

AbstractAlignmentTask* KalignPairwiseAlignmentTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings* _settings) const {
    auto pairwiseSettings = dynamic_cast<PairwiseAlignmentTaskSettings*>(_settings);
    SAFE_POINT(pairwiseSettings != nullptr, "Pairwise alignment: incorrect settings", nullptr);
    auto settings = new KalignPairwiseAlignmentTaskSettings(*pairwiseSettings);
    SAFE_POINT(!settings->inNewWindow || !settings->resultFileName.isEmpty(), "Pairwise alignment: incorrect settings, empty output file name", nullptr);
    return new KalignPairwiseAlignmentTask(settings);
}

KalignPairwiseAlignmentTaskSettings::KalignPairwiseAlignmentTaskSettings(const PairwiseAlignmentTaskSettings& s)
    : PairwiseAlignmentTaskSettings(s) {
}

bool KalignPairwiseAlignmentTaskSettings::convertCustomSettings() {
    kalign3Settings.gapOpenPenalty = customSettings.value(GAP_OPEN_PENALTY_KEY, -1).toDouble();
    kalign3Settings.gapExtensionPenalty = customSettings.value(GAP_EXTENSION_PENALTY_KEY, -1).toDouble();
    kalign3Settings.terminalGapExtensionPenalty = customSettings.value(TERMINAL_GAP_EXTENSION_PENALTY_KEY, -1).toDouble();
    PairwiseAlignmentTaskSettings::convertCustomSettings();
    return true;
}

bool KalignPairwiseAlignmentTaskSettings::isValid() const {
    return PairwiseAlignmentTaskSettings::isValid() &&
           kalign3Settings.gapOpenPenalty >= 0 &&
           kalign3Settings.gapExtensionPenalty >= 0 &&
           kalign3Settings.terminalGapExtensionPenalty >= 0;
}

KalignPairwiseAlignmentTask::KalignPairwiseAlignmentTask(KalignPairwiseAlignmentTaskSettings* _settings)
    : PairwiseAlignmentTask(TaskFlag_NoRun),
      settings(_settings) {
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

    ma = Msa(firstName + " vs. " + secondName, alphabet);
    ma->addRow(firstName, first);
    ma->addRow(secondName, second);

    kalignSubTask = new Kalign3SupportTask(ma, GObjectReference(), settings->kalign3Settings);
    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);
    addSubTask(kalignSubTask);
}

KalignPairwiseAlignmentTask::~KalignPairwiseAlignmentTask() {
    delete settings;
}

QList<Task*> KalignPairwiseAlignmentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask->hasError() || subTask->isCanceled()) {
        return res;
    }

    if (subTask == kalignSubTask) {
        if (settings->inNewWindow) {
            TaskStateInfo localStateInfo;
            Project* currentProject = AppContext::getProject();

            DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::CLUSTAL_ALN);

            QString newFileUrl = settings->resultFileName.getURLString();
            makeUniqueUrl(newFileUrl, currentProject);

            Document* alignmentDoc = format->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(newFileUrl), localStateInfo);
            CHECK_OP(localStateInfo, res);

            Msa resultMa = kalignSubTask->resultMA;
            MsaObject* docObject = MsaImportUtils::createMsaObject(alignmentDoc->getDbiRef(), resultMa, localStateInfo);
            CHECK_OP(localStateInfo, res);
            alignmentDoc->addObject(docObject);
            res << new SaveDocumentTask(alignmentDoc, SaveDoc_OpenAfter);
        } else {  // in current window
            U2OpStatus2Log os;
            DbiConnection con(settings->msaRef.dbiRef, os);
            CHECK_OP(os, res);

            QList<U2MsaRow> rows = con.dbi->getMsaDbi()->getRows(settings->msaRef.entityId, os);
            CHECK_OP(os, res);
            U2UseCommonUserModStep userModStep(settings->msaRef, os);
            SAFE_POINT_OP(os, res);
            for (int rowNumber = 0; rowNumber < rows.length(); ++rowNumber) {
                if (rows[rowNumber].sequenceId == settings->firstSequenceRef.entityId) {
                    con.dbi->getMsaDbi()->updateGapModel(settings->msaRef.entityId, rows[rowNumber].rowId, kalignSubTask->resultMA->getRow(0)->getGaps(), os);
                    CHECK_OP(os, res);
                }
                if (rows[rowNumber].sequenceId == settings->secondSequenceRef.entityId) {
                    con.dbi->getMsaDbi()->updateGapModel(settings->msaRef.entityId, rows[rowNumber].rowId, kalignSubTask->resultMA->getRow(1)->getGaps(), os);
                    CHECK_OP(os, res);
                }
            }
        }
    }
    return res;
}

Task::ReportResult KalignPairwiseAlignmentTask::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

void KalignPairwiseAlignmentTask::makeUniqueUrl(QString& givenUrl, const Project* curProject) {
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
