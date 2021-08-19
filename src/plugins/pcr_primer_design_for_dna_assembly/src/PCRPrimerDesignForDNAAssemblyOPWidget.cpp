/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "PCRPrimerDesignForDNAAssemblyOPWidget.h"
#include "tasks/ExtractPrimerTask.h"
#include "tasks/PCRPrimerDesignForDNAAssemblyTask.h"

#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/PrimerLineEdit.h>
#include <U2Gui/U2FileDialog.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include <QVBoxLayout>

namespace {
// When a non-nucleotide sequence is selected, the widget should be disabled. But the groupbox titles continue to be
// black as if they enabled. This code makes them gray.
void makeGroupboxTittleGrayIfDisable(QGroupBox *const gb) {
    QPalette palette;
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QApplication::palette().color(QPalette::Disabled,
        QPalette::WindowText));
    gb->setPalette(palette);
}
}    // namespace

namespace U2 {

const QString PCRPrimerDesignForDNAAssemblyOPWidget::USER_PRIMERS_SHOW_HIDE_ID = "user-primers-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID = "parameters-of-priming-sequences-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID = "parameters-2-exclude-in-whole-primers-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID = "select-areas-for-priming-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID = "open-backbone-sequence-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::GENERATE_SEQUENCE_SHOW_HIDE_ID = "generate-sequence-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID = "other-sequences-in-pcr-reaction-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::PCR_TABLE_OBJECT_NAME = QObject::tr("PCR Primer Design for DNA assembly");

PCRPrimerDesignForDNAAssemblyOPWidget::PCRPrimerDesignForDNAAssemblyOPWidget(AnnotatedDNAView* _annDnaView)
    : QWidget(nullptr),
      annDnaView(_annDnaView),
      savableWidget(this, GObjectViewUtils::findViewByName(annDnaView->getName())) {
    setupUi(this);
    parametersMinMaxSpinBoxes = { { sbMinRequireGibbs, sbMaxRequireGibbs },
                                  { spMinRequireMeltingTeml, spMaxRequireMeltingTeml },
                                  { spMinRequireOverlapLength, spMaxRequireOverlapLength },
                                  { sbLeftAreaStart, sbLeftAreaEnd },
                                  { sbRightAreaStart, sbRightAreaEnd } };

    userPrimersShowHideGroup->init(USER_PRIMERS_SHOW_HIDE_ID, tr("User primers"), wgtUserPrimers, true);
    parametersOfPrimingSequencesShowHideGroup->init(PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID,
                                                    tr("Parameters of priming sequences"),
                                                    wgtParametersOfPrimingSequences,
                                                    true);
    parameters2ExcludeInWholePrimersShowHideGroup->init(PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID,
                                                        tr("Parameters to exclude in whole primers"),
                                                        wgtParameters2ExcludeInWholePrimers,
                                                        true);
    areasForPrimingShowHideGroup->init(SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID,
                                       tr("Select areas for priming search"),
                                       wgtAreasForPriming,
                                       true);
    openBackboneSequenceShowHideGroup->init(OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID,
                                            tr("Open the backbone sequence"),
                                            wgtOpenBackboneSequence,
                                            true);
    generateSequenceShowHideGroup->init(GENERATE_SEQUENCE_SHOW_HIDE_ID,
                                        tr("Generate a sequence"),
                                        wgtGenerateSequence,
                                        true);
    otherSequencesInPCRShowHideGroup->init(OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID,
                                           tr("Other sequences in PCR reaction"),
                                           wgtOtherSequencesInPcr,
                                           true);

    auto seqLength = annDnaView->getActiveSequenceContext()->getSequenceLength();

    // Default values, have been provided by the customer
    // Left area: from "1" to "10% of sequence length"
    // Right area: from "50% of sequence length" to "60% of sequence length"
    sbLeftAreaStart->setValue(1);
    sbLeftAreaEnd->setValue(seqLength / 10);
    sbRightAreaStart->setValue(seqLength / 10 * 5);
    sbRightAreaEnd->setValue(seqLength / 10 * 6);

    makeGroupboxTittleGrayIfDisable(groupBox);
    makeGroupboxTittleGrayIfDisable(groupBox_2);
    makeWarningInvisibleIfDna();

    connect(pbStart, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_start);
    connect(tbLeftAreaSelectManually, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually);
    connect(tbRightAreaSelectManually, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually);
    U2WidgetStateStorage::restoreWidgetState(savableWidget);

    sl_updateParametersRanges();
    const auto& parametersMinMaxSpinBoxesKeys = parametersMinMaxSpinBoxes.keys();
    for (auto minSb : qAsConst(parametersMinMaxSpinBoxesKeys)) {
        connect(minSb, &QSpinBox::editingFinished, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_minValueChanged);
        auto maxSb = parametersMinMaxSpinBoxes.value(minSb);
        SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

        connect(maxSb, &QSpinBox::editingFinished, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_maxValueChanged);
    }
    connect(tbLoadBackbone, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadBackbone);
    connect(tbsaveRandomSequences, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_saveRandomSequences);
    connect(tbLoadOtherSequencesInPcr, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadOtherSequenceInPcr);
    connect(annDnaView, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget *, ADVSequenceWidget *)), SLOT(sl_activeSequenceChanged()));
    connect(annDnaView, &AnnotatedDNAView::si_sequenceModified, this,
        &PCRPrimerDesignForDNAAssemblyOPWidget::sl_sequenceModified);
    connect(annDnaView->getActiveSequenceContext()->getSequenceObject(), &U2SequenceObject::si_sequenceChanged, this,
        &PCRPrimerDesignForDNAAssemblyOPWidget::sl_sequenceModified);
    connect(productsTable, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(sl_extractProduct()));
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_activeSequenceChanged() {
    makeWarningInvisibleIfDna();
    sl_sequenceModified();

    // Release "Select manually" buttons.
    tbLeftAreaSelectManually->setChecked(false);
    tbRightAreaSelectManually->setChecked(false);
    smButton = nullptr;
    sbStartRegion = nullptr;
    sbEndRegion = nullptr;
    disconnect(updateRegionConnection);
    updateRegionConnection = QMetaObject::Connection();
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_sequenceModified() {
    const ADVSequenceObjectContext *sequenceContext = annDnaView->getActiveSequenceContext();
    CHECK(sequenceContext != nullptr, )
    const qint64 seqLength = sequenceContext->getSequenceLength();

    // End spinbox maximum value is the previous sequence length. If it has changed, need to update spinboxes.
    if (seqLength != sbLeftAreaEnd->maximum()) {
        int start = sbLeftAreaStart->value() - 1;
        int len = sbLeftAreaEnd->value() - start;
        setRegion(sbLeftAreaStart, {start, len});

        start = sbRightAreaStart->value() - 1;
        len = sbRightAreaEnd->value() - start;
        setRegion(sbRightAreaStart, {start, len});
    }
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_start() {
    PCRPrimerDesignForDNAAssemblyTaskSettings settings;
    settings.forwardUserPrimer = leForwardPrimer->text();
    settings.reverseUserPrimer = leReversePrimer->text();

    settings.gibbsFreeEnergy.minValue = sbMinRequireGibbs->value();
    settings.gibbsFreeEnergy.maxValue = sbMaxRequireGibbs->value();
    settings.meltingPoint.minValue = spMinRequireMeltingTeml->value();
    settings.meltingPoint.maxValue = spMaxRequireMeltingTeml->value();
    settings.overlapLength.minValue = spMinRequireOverlapLength->value();
    settings.overlapLength.maxValue = spMaxRequireOverlapLength->value();

    settings.gibbsFreeEnergyExclude = sbExcludeGibbs->value();
    settings.meltingPointExclude = spExcludeMeltingTeml->value();
    settings.complementLengthExclude = spExcludeComplementLength->value();

    if (backbone5->isChecked()) {
        settings.insertTo = PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone5;
    } else {
        settings.insertTo = PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone3;
    }
    settings.bachbone5Length = sbBackbone5Length->value();
    settings.bachbone3Length = sbBackbone3Length->value();

    settings.leftArea.startPos = (int)(sbLeftAreaStart->value()) - 1;
    settings.leftArea.length = (int)sbLeftAreaEnd->value() - (int)sbLeftAreaStart->value();
    settings.rightArea.startPos = (int)(sbRightAreaStart->value()) - 1;
    settings.rightArea.length = (int)sbRightAreaEnd->value() - (int)sbRightAreaStart->value();

    settings.backboneSequenceUrl = leBackboneFilePath->text();

    settings.generateSequenceUrl = leRandomSequencesFilePath->text();

    settings.otherSequencesInPcrUrl = leOtherSequencesInPcrFilePath->text();

    auto activeSequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(activeSequenceContext != nullptr, L10N::nullPointerError("ADVSequenceObjectContext"), );

    U2SequenceObject* sequenceObject = activeSequenceContext->getSequenceObject();
    SAFE_POINT(NULL != sequenceObject, L10N::nullPointerError("Sequence Object"), );

    U2OpStatus2Log os;
    auto sequence = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );

    pcrTask = new PCRPrimerDesignForDNAAssemblyTask(settings, sequence);
    auto ts = AppContext::getTaskScheduler();
    SAFE_POINT(ts != nullptr, L10N::nullPointerError("TaskScheduler"), );
    connect(pcrTask, SIGNAL(si_stateChanged()), SLOT(sl_onFindTaskFinished()));

    ts->registerTopLevelTask(pcrTask);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually() {
    auto currentSMButton = qobject_cast<QToolButton*>(sender());
    SAFE_POINT(currentSMButton != nullptr, L10N::nullPointerError("QAbstractButton"), );

    if (smButton != nullptr) {
        bool disableState = true;
        if (smButton->isChecked()) {
            smButton->setChecked(false);
            disableState = false;
        }
        smButton = nullptr;
        sbStartRegion = nullptr;
        sbEndRegion = nullptr;
        disconnect(updateRegionConnection);
        updateRegionConnection = QMetaObject::Connection();

        if (disableState) {
            return;
        }
    }

    smButton = currentSMButton;
    if (smButton == tbLeftAreaSelectManually) {
        sbStartRegion = sbLeftAreaStart;
        sbEndRegion = sbLeftAreaEnd;
    } else {
        sbStartRegion = sbRightAreaStart;
        sbEndRegion = sbRightAreaEnd;
    }
    auto sequenceSelection = annDnaView->getActiveSequenceContext()->getSequenceSelection();
    int startRegionValue = sbStartRegion->value();
    int endRegionValue = sbEndRegion->value();
    sequenceSelection->setSelectedRegions({ U2Region(startRegionValue, endRegionValue - startRegionValue) });
    updateRegionConnection = connect(sequenceSelection, &DNASequenceSelection::si_onSelectionChanged, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateRegion);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateRegion() {
    const auto& selectedRegions = qobject_cast<DNASequenceSelection*>(sender())->getSelectedRegions();
    CHECK(!selectedRegions.isEmpty(), );

    if (selectedRegions.size() > 1) {
        coreLog.details(tr("Multiple regions selection, the only first is set as primer search area."));
    }
    const auto& region = selectedRegions.first();
    SAFE_POINT(sbStartRegion != nullptr, L10N::nullPointerError("QSpinBox"), );

    setRegion(sbStartRegion, region);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateParametersRanges() {
    auto seqLength = annDnaView->getActiveSequenceContext()->getSequenceLength();
    const auto& parametersMinMaxSpinBoxesKeys = parametersMinMaxSpinBoxes.keys();
    for (const auto& minSb : qAsConst(parametersMinMaxSpinBoxesKeys)) {
        auto maxSb = parametersMinMaxSpinBoxes.value(minSb);
        SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

        minSb->setMaximum(maxSb->value() - 1);
        maxSb->setMinimum(minSb->value() + 1);
        maxSb->setMaximum(seqLength);
    }
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_minValueChanged() {
    auto minSb = qobject_cast<QSpinBox*>(sender());
    SAFE_POINT(minSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    auto maxSb = parametersMinMaxSpinBoxes.value(minSb);
    SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    maxSb->setMinimum(minSb->value() + 1);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_maxValueChanged() {
    auto maxSb = qobject_cast<QSpinBox*>(sender());
    SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    auto minSb = parametersMinMaxSpinBoxes.key(maxSb);
    SAFE_POINT(minSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    minSb->setMaximum(maxSb->value() - 1);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadBackbone() {
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    QString selectedFilter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getOpenFileName(nullptr, tr("Select a backbone sequence file"), lod, filter, &selectedFilter);
    CHECK(!file.isEmpty(), );

    leBackboneFilePath->setText(file);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_saveRandomSequences() {
    QString filter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getSaveFileName(nullptr, tr("Save a random sequences file"), lod.dir, filter);
    CHECK(!file.isEmpty(), );

    leRandomSequencesFilePath->setText(file);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadOtherSequenceInPcr() {
    const QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    QString selectedFilter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getOpenFileName(nullptr, tr("Select an \"Other sequences in PCR reaction\" file"), lod, filter, &selectedFilter);
    CHECK(!file.isEmpty(), );

    leOtherSequencesInPcrFilePath->setText(file);

}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_onFindTaskFinished() {
    CHECK(sender() == pcrTask, );
    SAFE_POINT(nullptr != pcrTask, L10N::nullPointerError("InSilicoPcrTask"), );
    setEnabled(true);
    if (pcrTask->isCanceled() || pcrTask->hasError()) {
        disconnect(pcrTask, SIGNAL(si_stateChanged()));
        pcrTask = nullptr;
        return;
    }
    CHECK(pcrTask->isFinished(), );
    createResultAnnotations();
    productsTable->setCurrentProducts(pcrTask->getResults(), annDnaView);
    backboneSequence = pcrTask->getBackboneSequence();
    lastRunSettings = pcrTask->getSettings();
    pcrTask = nullptr;
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_extractProduct() {
    ADVSequenceObjectContext *sequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(nullptr != sequenceContext, L10N::nullPointerError("Sequence Context"), );
    U2SequenceObject *sequenceObject = sequenceContext->getSequenceObject();
    SAFE_POINT(nullptr != sequenceObject, L10N::nullPointerError("Sequence Object"), );
    ExtractPrimerTaskSettings settings;
    settings.sequenceRef = sequenceContext->getSequenceRef();
    GUrl sequenceURL = sequenceContext->getSequenceObject()->getDocument()->getURL();
    if (sequenceURL.getType() == GUrl_File) {
        settings.originalSequenceFileName = QFileInfo(sequenceURL.getURLString()).baseName();
    } else {
        settings.originalSequenceFileName = sequenceContext->getSequenceObject()->getSequenceName();
    }
    settings.direction = lastRunSettings.insertTo;
    settings.backboneSequence = backboneSequence;
    Annotation *selectedAnnotation = productsTable->getSelectedAnnotation();
    if (selectedAnnotation != nullptr) {
        CHECK(selectedAnnotation->getRegions().size() == 1, );
        settings.fragmentLocation = selectedAnnotation->getRegions().first();
        settings.fragmentName = selectedAnnotation->getName();
        AppContext::getTaskScheduler()->registerTopLevelTask(new ExtractPrimerAndOpenDocumentTask(settings));
    }
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_annotationCreationTaskFinished() {
    CreateAnnotationsTask *t = qobject_cast<CreateAnnotationsTask *>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()) {
        return;
    }
    productsTable->setAnnotationGroup(t->getResultAnnotations().at(0)->getGroup());
}

void PCRPrimerDesignForDNAAssemblyOPWidget::createResultAnnotations() {
    QList<SharedAnnotationData> annotations;
    int index = 0;
    auto results = pcrTask->getResults();
    bool emptyResults = true;
    for (const U2Region &region : qAsConst(results)) {
        if (region != U2Region()) {
            SharedAnnotationData data(new AnnotationData());
            data->setStrand(index % 2 == 0 ? U2Strand(U2Strand::Direct) : U2Strand(U2Strand::Complementary));
            data->name = PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME.at(index);
            data->location->regions.append(region);
            annotations.append(data);
            emptyResults = false;
        }
        index++;
    }
    if (emptyResults) {
        return;
    }

    U2OpStatusImpl os;
    const U2DbiRef localDbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os, );
    AnnotationTableObject *resultsTableObject = new AnnotationTableObject(PCR_TABLE_OBJECT_NAME, localDbiRef);
    QString newDocUrl = GUrlUtils::rollFileName(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/PCRPrimers.gb", "_");
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    Document *d = df->createNewLoadedDocument(iof, newDocUrl, os);
    CHECK_OP(os, );
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os, );
    resultsTableObject->addObjectRelation(GObjectRelation(annDnaView->getActiveSequenceContext()->getSequenceGObject(), ObjectRole_Sequence));
    d->addObject(resultsTableObject);
    AppContext::getProject()->addDocument(d);
    annDnaView->tryAddObject(resultsTableObject);
    Task *createAnnotationsTask = new CreateAnnotationsTask(resultsTableObject, annotations, "Primers");
    connect(createAnnotationsTask, SIGNAL(si_stateChanged()), SLOT(sl_annotationCreationTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(createAnnotationsTask);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::makeWarningInvisibleIfDna() {
    ADVSequenceObjectContext *sequenceContext = annDnaView->getActiveSequenceContext();
    CHECK(sequenceContext != nullptr, )
    const DNAAlphabet *alphabet = sequenceContext->getAlphabet();
    SAFE_POINT(alphabet != nullptr, L10N::nullPointerError("Alphabet"), )

    bool isDna = alphabet->isDNA();
    runPcrPrimerDesignWidget->setEnabled(isDna);
    alphabetWarningLabel->setVisible(!isDna);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::setRegion(QSpinBox *start, U2Region region) {
    QSpinBox *end = parametersMinMaxSpinBoxes[start];
    const ADVSequenceObjectContext *sequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(start != nullptr && end != nullptr, L10N::nullPointerError("QSpinBox"), )
    CHECK(sequenceContext != nullptr, )

    const qint64 seqLength = sequenceContext->getSequenceLength();
    start->setRange(1, seqLength);
    end->setRange(1, seqLength);

    start->setValue(region.startPos + 1);
    end->setValue(region.endPos());
    sl_updateParametersRanges();
}
}
