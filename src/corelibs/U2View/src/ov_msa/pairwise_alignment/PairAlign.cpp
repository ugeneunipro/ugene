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

#include "PairAlign.h"

#include <QMessageBox>
#include <QString>
#include <QStringList>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BuiltInDistanceAlgorithms.h>
#include <U2Algorithm/MsaDistanceAlgorithm.h>
#include <U2Algorithm/MsaDistanceAlgorithmRegistry.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MsaObject.h>
#include <U2Core/Theme.h>
#include <U2Core/U2Alphabet.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/AlignmentAlgorithmGUIExtension.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MsaEditor.h>
#include <U2View/MsaEditorSequenceArea.h>

#include "ov_msa/MaEditorSelection.h"

#define BadAlphabetWarning 0
#define DuplicateSequenceWarning 1

inline U2::U2DataId getSequenceIdByRowId(U2::MsaEditor* msa, qint64 rowId, U2::U2OpStatus& os) {
    const U2::MsaRow& row = msa->getMaObject()->getAlignment()->getRowByRowId(rowId, os);
    CHECK_OP(os, {});
    return row->getSequenceId();
}

namespace U2 {

PairAlign::PairAlign(MsaEditor* _msa)
    : msa(_msa), pairwiseAlignmentWidgetsSettings(_msa->getPairwiseAlignmentWidgetsSettings()),
      distanceCalcTask(nullptr), settingsWidget(nullptr),
      showHideSequenceWidget(nullptr), showHideSettingsWidget(nullptr), showHideOutputWidget(nullptr),
      saveController(nullptr), savableTab(this, GObjectViewUtils::findViewByName(_msa->getName())),
      showSequenceWidget(_msa->getPairwiseAlignmentWidgetsSettings()->showSequenceWidget),
      showAlgorithmWidget(_msa->getPairwiseAlignmentWidgetsSettings()->showAlgorithmWidget),
      showOutputWidget(_msa->getPairwiseAlignmentWidgetsSettings()->showOutputWidget),
      firstSequenceSelectionOn(false), secondSequenceSelectionOn(false),
      sequencesChanged(true), sequenceNamesIsOk(false), alphabetIsOk(false) {
    SAFE_POINT(msa != nullptr, "MSA Editor is NULL.", );
    SAFE_POINT(pairwiseAlignmentWidgetsSettings != nullptr, "pairwiseAlignmentWidgetsSettings is NULL.", );

    setupUi(this);

    firstSeqSelectorWC = new SequenceSelectorWidgetController(msa);
    firstSeqSelectorWC->setObjectName("firstSeqSelectorWC");
    secondSeqSelectorWC = new SequenceSelectorWidgetController(msa);
    secondSeqSelectorWC->setObjectName("secondSeqSelectorWC");

    firstSequenceLayout->addWidget(firstSeqSelectorWC);
    secondSequenceLayout->addWidget(secondSeqSelectorWC);

    initLayout();
    initSaveController();
    initParameters();

    U2WidgetStateStorage::restoreWidgetState(savableTab);

    connectSignals();

    checkState();
}

void PairAlign::initLayout() {
    showHideSequenceWidget = new ShowHideSubgroupWidget("PA_SEQUENCES", tr("Sequences"), sequenceContainerWidget, showSequenceWidget);
    showHideSettingsWidget = new ShowHideSubgroupWidget("PA_SETTINGS", tr("Algorithm settings"), settingsContainerWidget, showAlgorithmWidget);
    showHideOutputWidget = new ShowHideSubgroupWidget("PA_OUTPUT", tr("Output settings"), outputContainerWidget, showOutputWidget);

    mainLayout->insertWidget(0, showHideSequenceWidget);
    mainLayout->insertWidget(1, showHideSettingsWidget);
    mainLayout->insertWidget(2, showHideOutputWidget);
}

bool PairAlign::isValidSequenceId(qint64 sequenceId) const {
    return msa->getMaObject()->getRowPosById(sequenceId) >= 0;
}

void PairAlign::initParameters() {
    const MaEditorSelection& selection = msa->getSelection();
    QList<int> selectedViewRowIndexes = selection.getSelectedRowIndexes();
    if (selectedViewRowIndexes.size() == 2) {
        qint64 firstRowId = msa->getRowByViewRowIndex(selectedViewRowIndexes[0])->getRowId();
        firstSeqSelectorWC->setSequenceId(firstRowId);
        qint64 secondRowId = msa->getRowByViewRowIndex(selectedViewRowIndexes[1])->getRowId();
        secondSeqSelectorWC->setSequenceId(secondRowId);
    } else {
        if (isValidSequenceId(pairwiseAlignmentWidgetsSettings->firstSequenceId)) {
            firstSeqSelectorWC->setSequenceId(pairwiseAlignmentWidgetsSettings->firstSequenceId);
        }
        if (isValidSequenceId(pairwiseAlignmentWidgetsSettings->secondSequenceId)) {
            secondSeqSelectorWC->setSequenceId(pairwiseAlignmentWidgetsSettings->secondSequenceId);
        }
    }

    inNewWindowCheckBox->setChecked(pairwiseAlignmentWidgetsSettings->inNewWindow);

    QString outputFileName = pairwiseAlignmentWidgetsSettings->resultFileName;
    if (outputFileName.isEmpty()) {
        saveController->setPath(getDefaultFilePath());  // controller will roll file name here
    } else {
        outputFileLineEdit->setText(outputFileName);
    }
    outputFileLineEdit->setEnabled(inNewWindowCheckBox->isChecked());
    outputFileSelectButton->setEnabled(inNewWindowCheckBox->isChecked());

    canDoAlign = false;

    AlignmentAlgorithmsRegistry* par = AppContext::getAlignmentAlgorithmsRegistry();
    SAFE_POINT(par != nullptr, "AlignmentAlgorithmsRegistry is NULL.", );
    QStringList algList = par->getAvailableAlgorithmIds(PairwiseAlignment);
    algorithmListComboBox->setEnabled(algList.length() > 0);
    CHECK(algList.length() > 0, );
    algorithmListComboBox->addItems(algList);
    if (pairwiseAlignmentWidgetsSettings->algorithmName.isEmpty()) {
        pairwiseAlignmentWidgetsSettings->algorithmName = algList[0];
    } else {
        int index = algorithmListComboBox->findText(pairwiseAlignmentWidgetsSettings->algorithmName);
        if (index != -1) {
            algorithmListComboBox->setCurrentIndex(index);
        } else {
            pairwiseAlignmentWidgetsSettings->algorithmName = algList[0];
        }
    }
    sl_algorithmSelected(pairwiseAlignmentWidgetsSettings->algorithmName);

    lblMessage->setStyleSheet("color: " + Theme::errorColorLabelStr() + ";"
                                                                        "font: bold;"
                                                                        "padding-top: 15px;");
    sl_alignmentChanged();
}

void PairAlign::updateWarningMessage(int type) {
    QString text;
    switch (type) {
        case DuplicateSequenceWarning:
            text = tr("Please select 2 different sequences to align");
            break;
        case BadAlphabetWarning: {
            QString alphabetName = msa->getMaObject()->getAlphabet()->getName();
            text = tr("Pairwise alignment is not available for alignments with \"%1\" alphabet.").arg(alphabetName);
            break;
        }
        default:
            text = tr("Unexpected error");
    }
    lblMessage->setText(text);
}

void PairAlign::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = outputFileSelectButton;
    config.fileNameEdit = outputFileLineEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save file");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::CLUSTAL_ALN;

    saveController = new SaveDocumentController(config, formats, this);
    saveController->setPath(getDefaultFilePath());  // controller will roll file name here
}

QString PairAlign::getDefaultFilePath() {
    return GUrlUtils::getDefaultDataPath() + "/" + PairwiseAlignmentTaskSettings::DEFAULT_RESULT_FILE_NAME;
}

void PairAlign::connectSignals() {
    connect(showHideSequenceWidget, SIGNAL(si_subgroupStateChanged(QString)), SLOT(sl_subwidgetStateChanged(QString)));
    connect(showHideSettingsWidget, SIGNAL(si_subgroupStateChanged(QString)), SLOT(sl_subwidgetStateChanged(QString)));
    connect(showHideOutputWidget, SIGNAL(si_subgroupStateChanged(QString)), SLOT(sl_subwidgetStateChanged(QString)));
    connect(algorithmListComboBox, SIGNAL(currentIndexChanged(QString)), SLOT(sl_algorithmSelected(QString)));
    connect(inNewWindowCheckBox, SIGNAL(clicked(bool)), SLOT(sl_inNewWindowCheckBoxChangeState(bool)));
    connect(alignButton, SIGNAL(clicked()), SLOT(sl_alignButtonPressed()));
    connect(outputFileSelectButton, SIGNAL(clicked()), SLOT(sl_checkState()));
    connect(outputFileLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_outputFileChanged()));

    connect(firstSeqSelectorWC, SIGNAL(si_selectionChanged()), SLOT(sl_selectorTextChanged()));
    connect(secondSeqSelectorWC, SIGNAL(si_selectionChanged()), SLOT(sl_selectorTextChanged()));
    connect(msa->getMaObject(), SIGNAL(si_lockedStateChanged()), SLOT(sl_checkState()));
    connect(msa->getMaObject(), SIGNAL(si_alignmentChanged(const Msa&, const MaModificationInfo&)), SLOT(sl_alignmentChanged()));
}

void PairAlign::sl_checkState() {
    checkState();
}

void PairAlign::sl_alignmentChanged() {
    const DNAAlphabet* dnaAlphabet = msa->getMaObject()->getAlphabet();
    SAFE_POINT(dnaAlphabet != nullptr, "Alignment alphabet is not defined.", );

    pairwiseAlignmentWidgetsSettings->customSettings.insert("alphabet", dnaAlphabet->getId());

    QString curAlgorithmId = pairwiseAlignmentWidgetsSettings->algorithmName;
    AlignmentAlgorithm* alg = getAlgorithmById(curAlgorithmId);
    SAFE_POINT(alg != nullptr, QString("Algorithm %1 not found.").arg(curAlgorithmId), );
    alphabetIsOk = alg->checkAlphabet(dnaAlphabet);

    if (settingsWidget != nullptr) {
        settingsWidget->updateWidget();
    }
    checkState();
}

void PairAlign::checkState() {
    SAFE_POINT((firstSequenceSelectionOn && secondSequenceSelectionOn) == false,
               tr("Either addFirstButton and addSecondButton are pressed. Sequence selection mode works incorrect."), );

    sequenceNamesIsOk = checkSequenceNames();

    outputFileLineEdit->setEnabled(inNewWindowCheckBox->isChecked());
    outputFileSelectButton->setEnabled(inNewWindowCheckBox->isChecked());

    if (sequencesChanged) {
        updatePercentOfSimilarity();
    }

    qint64 firstSequenceId = firstSeqSelectorWC->sequenceId();
    qint64 secondSequenceId = secondSeqSelectorWC->sequenceId();
    bool sameSequenceInBothSelectors = firstSequenceId == secondSequenceId && firstSequenceId != U2MsaRow::INVALID_ROW_ID;
    if (!alphabetIsOk) {
        updateWarningMessage(BadAlphabetWarning);
    } else if (sameSequenceInBothSelectors) {
        updateWarningMessage(DuplicateSequenceWarning);
    }
    lblMessage->setVisible(!alphabetIsOk || sameSequenceInBothSelectors);

    showHideSettingsWidget->setEnabled(alphabetIsOk);
    showHideOutputWidget->setEnabled(alphabetIsOk);

    bool readOnly = msa->getMaObject()->isStateLocked();
    canDoAlign = ((U2MsaRow::INVALID_ROW_ID != firstSequenceId) && (U2MsaRow::INVALID_ROW_ID != secondSequenceId) && (firstSequenceId != secondSequenceId) && sequenceNamesIsOk && alphabetIsOk && (!readOnly || inNewWindowCheckBox->isChecked()));

    alignButton->setEnabled(canDoAlign);

    pairwiseAlignmentWidgetsSettings->firstSequenceId = firstSequenceId;
    pairwiseAlignmentWidgetsSettings->secondSequenceId = secondSequenceId;
    pairwiseAlignmentWidgetsSettings->algorithmName = algorithmListComboBox->currentText();
    pairwiseAlignmentWidgetsSettings->inNewWindow = inNewWindowCheckBox->isChecked();
    pairwiseAlignmentWidgetsSettings->resultFileName = saveController->getSaveFileName();
    pairwiseAlignmentWidgetsSettings->showSequenceWidget = showSequenceWidget;
    pairwiseAlignmentWidgetsSettings->showAlgorithmWidget = showAlgorithmWidget;
    pairwiseAlignmentWidgetsSettings->showOutputWidget = showOutputWidget;
}

void PairAlign::updatePercentOfSimilarity() {
    similarityValueLabel->setText(tr("Not defined"));
    similarityWidget->setVisible(false);
    sequencesChanged = false;
    if (!sequenceNamesIsOk) {
        return;
    }

    MsaDistanceAlgorithmRegistry* distanceReg = AppContext::getMSADistanceAlgorithmRegistry();
    SAFE_POINT(distanceReg != nullptr, "MSADistanceAlgorithmRegistry is NULL.", );
    MsaDistanceAlgorithmFactory* distanceFactory = distanceReg->getAlgorithmFactory(BuiltInDistanceAlgorithms::SIMILARITY_ALGO);
    SAFE_POINT(distanceFactory != nullptr, QString("%1 algorithm factory not found.").arg(BuiltInDistanceAlgorithms::SIMILARITY_ALGO), );

    U2OpStatusImpl os;
    Msa ma;
    const Msa currentAlignment = msa->getMaObject()->getAlignment();
    ma->addRow(firstSeqSelectorWC->text(), currentAlignment->getRowByRowId(firstSeqSelectorWC->sequenceId(), os)->getData(), -1);
    ma->addRow(secondSeqSelectorWC->text(), currentAlignment->getRowByRowId(secondSeqSelectorWC->sequenceId(), os)->getData(), -1);
    distanceCalcTask = distanceFactory->createAlgorithm(ma);
    distanceCalcTask->setExcludeGaps(true);
    connect(distanceCalcTask, SIGNAL(si_stateChanged()), SLOT(sl_distanceCalculated()));
    AppContext::getTaskScheduler()->registerTopLevelTask(distanceCalcTask);
}

bool PairAlign::checkSequenceNames() {
    QList<qint64> rowIds = msa->getMaObject()->getAlignment()->getRowsIds();
    return (rowIds.contains(firstSeqSelectorWC->sequenceId()) && rowIds.contains(secondSeqSelectorWC->sequenceId()));
}

AlignmentAlgorithm* PairAlign::getAlgorithmById(const QString& algorithmId) {
    AlignmentAlgorithmsRegistry* par = AppContext::getAlignmentAlgorithmsRegistry();
    SAFE_POINT(par != nullptr, "AlignmentAlgorithmsRegistry is NULL.", nullptr);
    return par->getAlgorithm(algorithmId);
}

void PairAlign::sl_algorithmSelected(const QString& algorithmName) {
    if (settingsWidget != nullptr) {
        delete settingsWidget;
        settingsWidget = nullptr;
    }

    AlignmentAlgorithm* alg = getAlgorithmById(algorithmName);
    SAFE_POINT(alg != nullptr, QString("Algorithm %1 not found.").arg(algorithmName), );
    QString firstAlgorithmRealization = alg->getRealizationsList().first();
    alphabetIsOk = alg->checkAlphabet(msa->getMaObject()->getAlphabet());

    AlignmentAlgorithmGUIExtensionFactory* algGUIFactory = alg->getGUIExtFactory(firstAlgorithmRealization);
    SAFE_POINT(algGUIFactory != nullptr, QString("Algorithm %1 GUI factory not found.").arg(firstAlgorithmRealization), );
    settingsWidget = algGUIFactory->createMainWidget(this, &pairwiseAlignmentWidgetsSettings->customSettings);
    connect(msa, &QObject::destroyed, settingsWidget, &AlignmentAlgorithmMainWidget::sl_externSettingsInvalidate);
    settingsContainerWidgetLayout->addWidget(settingsWidget);

    checkState();
}

void PairAlign::sl_subwidgetStateChanged(const QString& id) {
    if (id == "PA_SEQUENCES") {
        showSequenceWidget = showHideSequenceWidget->isSubgroupOpened();
    }
    if (id == "PA_SETTINGS") {
        showAlgorithmWidget = showHideSettingsWidget->isSubgroupOpened();
    }
    if (id == "PA_OUTPUT") {
        showOutputWidget = showHideOutputWidget->isSubgroupOpened();
    }
    checkState();
}

void PairAlign::sl_inNewWindowCheckBoxChangeState(bool newState) {
    Q_UNUSED(newState);
    checkState();
}

void PairAlign::sl_alignButtonPressed() {
    firstSequenceSelectionOn = false;
    secondSequenceSelectionOn = false;
    checkState();
    SAFE_POINT(canDoAlign, "Invalid state of PairAlign options panel widget. startAlignButton is not disabled.", );

    U2OpStatus2Log os;
    U2EntityRef msaRef = msa->getMaObject()->getEntityRef();
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2DataId firstSeqId = getSequenceIdByRowId(msa, pairwiseAlignmentWidgetsSettings->firstSequenceId, os);
    CHECK_OP(os, );
    U2EntityRef firstSequenceRef = U2EntityRef(msaRef.dbiRef, firstSeqId);

    U2DataId secondSeqId = getSequenceIdByRowId(msa, pairwiseAlignmentWidgetsSettings->secondSequenceId, os);
    CHECK_OP(os, );
    U2EntityRef secondSequenceRef = U2EntityRef(msaRef.dbiRef, secondSeqId);

    PairwiseAlignmentTaskSettings settings;
    settings.algorithmId = algorithmListComboBox->currentText();

    if (!saveController->getSaveFileName().isEmpty()) {
        settings.resultFileName = GUrl(saveController->getSaveFileName());
    } else {
        settings.resultFileName = GUrl(AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath() +
                                       "/" + PairwiseAlignmentTaskSettings::DEFAULT_RESULT_FILE_NAME);
    }
    GUrlUtils::validateLocalFileUrl(settings.resultFileName, os);
    if (os.hasError()) {
        QMessageBox::warning(this, tr("Error"), tr("Please, change the output file.") + "\n" + os.getError());
        outputFileLineEdit->setFocus(Qt::MouseFocusReason);
        return;
    }

    settings.inNewWindow = inNewWindowCheckBox->isChecked();
    settings.msaRef = msaRef;
    settings.alphabet = U2AlphabetId(msa->getMaObject()->getAlphabet()->getId());
    settings.firstSequenceRef = firstSequenceRef;
    settings.secondSequenceRef = secondSequenceRef;
    settingsWidget->getAlignmentAlgorithmCustomSettings(true);
    settings.appendCustomSettings(pairwiseAlignmentWidgetsSettings->customSettings);
    settings.convertCustomSettings();

    if (!pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask.isNull()) {
        disconnect(this, SLOT(sl_alignComplete()));
        pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask->cancel();
        pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask.clear();
    }

    AlignmentAlgorithmsRegistry* par = AppContext::getAlignmentAlgorithmsRegistry();
    SAFE_POINT(par != nullptr, "AlignmentAlgorithmsRegistry is NULL.", );
    AbstractAlignmentTaskFactory* factory = par->getAlgorithm(settings.algorithmId)->getFactory(settings.realizationName);
    SAFE_POINT(factory != nullptr, QString("Task factory for algorithm %1, realization %2 not found.").arg(settings.algorithmId, settings.realizationName), );

    auto task = qobject_cast<PairwiseAlignmentTask*>(factory->getTaskInstance(&settings));
    SAFE_POINT(task != nullptr, "Task is null!", );
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_alignComplete()));
    pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask = task;
    AppContext::getTaskScheduler()->registerTopLevelTask(task);

    con.close(os);
    checkState();
}

void PairAlign::sl_outputFileChanged() {
    checkState();
}

void PairAlign::sl_distanceCalculated() {
    if (distanceCalcTask == nullptr) {
        return;
    }
    if (distanceCalcTask->isFinished()) {
        const MsaDistanceMatrix& distanceMatrix = distanceCalcTask->getMatrix();
        similarityValueLabel->setText(QString::number(distanceMatrix.getSimilarity(0, 1, true)) + "%");
        similarityWidget->setVisible(true);
        distanceCalcTask = nullptr;
    }
}

void PairAlign::sl_alignComplete() {
    CHECK(pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask == sender(), );
    SAFE_POINT(!pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask.isNull(), "Can't process an unexpected align task", );
    if (pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask->isFinished()) {
        if (!inNewWindowCheckBox->isChecked()) {
            MaModificationInfo mi;
            mi.rowListChanged = false;
            mi.modifiedRowIds.append(pairwiseAlignmentWidgetsSettings->firstSequenceId);
            mi.modifiedRowIds.append(pairwiseAlignmentWidgetsSettings->secondSequenceId);
            msa->getMaObject()->updateCachedMultipleAlignment(mi);
        }
        pairwiseAlignmentWidgetsSettings->pairwiseAlignmentTask.clear();
    }
    checkState();
}

void PairAlign::sl_selectorTextChanged() {
    sequencesChanged = true;
    checkState();
}

}  // namespace U2
