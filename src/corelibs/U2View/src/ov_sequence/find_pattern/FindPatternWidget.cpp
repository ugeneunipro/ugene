/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "FindPatternWidget.h"

#include <QFlags>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMovie>
#include <QPlainTextEdit>

#include <U2Algorithm/FindAlgorithmTask.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileFilters.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Theme.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/FastaFormat.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/DetView.h>

namespace U2 {

class FastaPatternsWalker {
public:
    FastaPatternsWalker(const QString& patternsString, int cursor = 0)
        : patternsString(patternsString.toLatin1()), cursor(cursor), current(-1), comment(false), header(false) {
    }

    bool hasNext() const {
        return current < patternsString.size() - 1;
    }

    char next() {
        if (!hasNext()) {
            return 0;
        }
        current++;
        if (!updateMetaStart()) {
            if (isMetaChars() && '\n' == patternsString[current]) {
                header = false;
                comment = false;
            }
        }
        return patternsString[current];
    }

    bool isSequenceChar() const {
        CHECK(current != -1, false);
        CHECK(current < patternsString.size(), false);
        return !isMetaChars();
    }

    /** moves current place to the previous */
    void removeCurrent() {
        CHECK(current != -1, );
        CHECK(current < patternsString.size(), );
        patternsString.remove(current, 1);
        if (current < cursor) {
            cursor--;
        }
        current--;
    }

    bool isCorrect() const {
        if (!isSequenceChar()) {
            return true;
        }
        QChar c(patternsString[current]);
        return c.isLetter() ? c.isUpper() : c == '\n';
    }

    void setCurrent(char value) {
        CHECK(current != -1, );
        CHECK(current < patternsString.size(), );
        patternsString[current] = value;
    }

    int getCursor() const {
        return cursor;
    }

    QString getString() const {
        return patternsString;
    }

private:
    QByteArray patternsString;
    int cursor;
    int current;
    bool comment;
    bool header;

private:
    bool updateMetaStart() {
        char c = patternsString[current];
        if (FastaFormat::FASTA_COMMENT_START_SYMBOL != c &&
            FastaFormat::FASTA_HEADER_START_SYMBOL != c) {
            return false;
        }
        if (current == 0 || patternsString[current - 1] == '\n') {
            comment = (FastaFormat::FASTA_COMMENT_START_SYMBOL == c);
            header = (FastaFormat::FASTA_HEADER_START_SYMBOL == c);
            return true;
        }
        return false;
    }
    bool isMetaChars() const {
        return header || comment;
    }
};

#define MAX_SUPPORTED_TEXT_EDIT_SEQUENCE_SIZE 10000

/**
 * Safe version of QTextEdit that does not allow insertions of large texts (sequences) into the edit.
 * Both QTextEdit and QPlainTextEdit have very bad performance when long lines of text (sequences) are used
 * and may cause an app freeze.
 */
class FindPatternTextEdit : public QPlainTextEdit {
public:
    FindPatternTextEdit(QPlainTextEdit* originalEdit) {
        // Copy all important fields from the original edit. These fields can be set in QtDesigner.
        setObjectName(originalEdit->objectName());
        setSizePolicy(originalEdit->sizePolicy());
        setMinimumSize(originalEdit->minimumSize());
        setMaximumSize(originalEdit->maximumSize());
        setFocusPolicy(originalEdit->focusPolicy());
        setTabChangesFocus(originalEdit->tabChangesFocus());
        setUndoRedoEnabled(originalEdit->isUndoRedoEnabled());
    }

protected:
    /** Returns true if the text edit can safely grow by extraTextSize characters. */
    bool isSafeToAddExtraTextSize(int extraTextSize) const {
        return toPlainText().size() + extraTextSize <= MAX_SUPPORTED_TEXT_EDIT_SEQUENCE_SIZE;
    }

    bool canInsertFromMimeData(const QMimeData* source) const override {
        return isSafeToAddExtraTextSize(source->text().length()) && QPlainTextEdit::canInsertFromMimeData(source);
    }

    void insertFromMimeData(const QMimeData* source) override {
        if (isSafeToAddExtraTextSize(source->text().length())) {
            QPlainTextEdit::insertFromMimeData(source);
            return;
        }
        QString notificationMessage = FindPatternWidget::tr("The pattern is too long. Use 'Load pattern from file' option.");
        AppContext::getMainWindow()->addNotification(notificationMessage, Warning_Not);
    }
};

FindPatternEventFilter::FindPatternEventFilter(QPlainTextEdit* textEdit)
    : QObject(textEdit) {
    textEdit->installEventFilter(this);
}

bool FindPatternEventFilter::eventFilter(QObject* obj, QEvent* event) {
    const QEvent::Type eventType = event->type();
    if (eventType == QEvent::KeyPress) {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            if (keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
                emit si_shiftEnterPressed();
                return true;
            } else if (!keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
                emit si_enterPressed();
                return true;
            } else {
                keyEvent->setModifiers(keyEvent->modifiers() & ~Qt::ControlModifier);
                return false;
            }
        }
    } else if (eventType == QEvent::Show) {
        auto watched = dynamic_cast<QWidget*>(obj);
        if (nullptr != watched) {
            watched->setFocus();
        }
    }
    return QObject::eventFilter(obj, event);
}

const int FindPatternWidget::DEFAULT_RESULTS_NUM_LIMIT = 100000;
const int FindPatternWidget::DEFAULT_REGEXP_RESULT_LENGTH_LIMIT = 10000;

const int FindPatternWidget::REG_EXP_MIN_RESULT_LEN = 1;
const int FindPatternWidget::REG_EXP_MAX_RESULT_LEN = 1000;
const int FindPatternWidget::REG_EXP_MAX_RESULT_SINGLE_STEP = 20;

FindPatternWidget::FindPatternWidget(AnnotatedDNAView* annotatedDnaView)
    : annotatedDnaView(annotatedDnaView),
      trackedSelection(nullptr),
      currentResultIndex(-1),
      searchTask(nullptr),
      previousMaxResult(-1),
      usePatternNames(false),
      savableWidget(this, GObjectViewUtils::findViewByName(annotatedDnaView->getName())) {
    setupUi(this);

    // Replace the original text edit with the optimized version.
    auto findPatternTextEdit = new FindPatternTextEdit(textPattern);
    verticalLayout->replaceWidget(textPattern, findPatternTextEdit);
    delete textPattern;
    textPattern = findPatternTextEdit;

    progressMovie = new QMovie(":/core/images/progress.gif", QByteArray(), progressLabel);
    progressLabel->setObjectName("progressLabel");
    resultLabel->setObjectName("resultLabel");
    resultLabel->setFixedHeight(progressLabel->height());
    savableWidget.setRegionWidgetIds(QStringList() << editStart->objectName()
                                                   << editEnd->objectName());

    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    progressLabel->setMovie(progressMovie);
    if (activeContext != nullptr) {
        // Initializing the annotation model
        CreateAnnotationModel annotModel;
        annotModel.hideLocation = true;
        annotModel.hideUsePatternNames = false;
        annotModel.useAminoAnnotationTypes = annotatedDnaView->getActiveSequenceContext()->getAlphabet()->isAmino();
        annotModel.sequenceObjectRef = annotatedDnaView->getActiveSequenceContext()->getSequenceObject()->getReference();
        annotModel.sequenceLen = annotatedDnaView->getActiveSequenceContext()->getSequenceLength();

        createAnnotationController = new CreateAnnotationWidgetController(annotModel, this, CreateAnnotationWidgetController::OptionsPanel);
        annotationModelIsPrepared = false;
        connect(createAnnotationController, SIGNAL(si_annotationNamesEdited()), SLOT(sl_onAnnotationNameEdited()));

        setContentsMargins(0, 0, 0, 0);

        annotationsWidget = createAnnotationController->getWidget();
        annotationsWidget->setObjectName("annotationsWidget");

        const DNAAlphabet* alphabet = activeContext->getAlphabet();
        isAminoSequenceSelected = alphabet->isAmino();

        initLayout();
        connectSlots();

        checkState();

        setFocusProxy(textPattern);
        trackedSelection = nullptr;

        auto findPatternEventFilter = new FindPatternEventFilter(textPattern);
        connect(findPatternEventFilter, SIGNAL(si_enterPressed()), SLOT(sl_onEnterPressed()));
        connect(findPatternEventFilter, SIGNAL(si_shiftEnterPressed()), SLOT(sl_onShiftEnterPressed()));
        connect(createAnnotationController, SIGNAL(si_usePatternNamesStateChanged()), SLOT(sl_usePatternNamesCbClicked()));

        sl_onSearchPatternChanged();
    }
    nextPushButton->setDisabled(true);
    prevPushButton->setDisabled(true);
    getAnnotationsPushButton->setDisabled(true);
    showCurrentResultAndStopProgress();
    setUpTabOrder();
    previousMaxResult = boxMaxResult->value();
    U2WidgetStateStorage::restoreWidgetState(savableWidget);
}

void FindPatternWidget::showCurrentResultAndStopProgress() const {
    progressMovie->stop();
    progressLabel->hide();
    resultLabel->show();
    updateResultLabelText();
}

void FindPatternWidget::updateResultLabelText() const {
    QString currentResultText = currentResultIndex == -1 ? "-" : QString::number(currentResultIndex + 1);
    resultLabel->setText(tr("Results: %1/%2").arg(currentResultText).arg(QString::number(findPatternResults.size())));
}

void FindPatternWidget::initLayout() {
    lblErrorMessage->setStyleSheet("font: bold;");
    lblErrorMessage->setText("");
    initAlgorithmLayout();
    initStrandSelection();
    initSeqTranslSelection();
    initRegionSelection();
    initResultsLimit();

    subgroupsLayout->setSpacing(0);
    subgroupsLayout->addWidget(new ShowHideSubgroupWidget(QObject::tr("Search algorithm"), QObject::tr("Search algorithm"), widgetAlgorithm, false));
    subgroupsLayout->addWidget(new ShowHideSubgroupWidget(QObject::tr("Search in"), QObject::tr("Search in"), widgetSearchIn, false));
    subgroupsLayout->addWidget(new ShowHideSubgroupWidget(QObject::tr("Other settings"), QObject::tr("Other settings"), widgetOther, false));
    subgroupsLayout->addWidget(annotationsWidget);

    updateLayout();

    layoutSearchButton->setAlignment(Qt::AlignTop);
    this->layout()->setAlignment(Qt::AlignTop);

    this->layout()->setMargin(0);
}

void FindPatternWidget::initAlgorithmLayout() {
    boxAlgorithm->addItem(tr("Exact"), FindAlgorithmPatternSettings_Exact);
    if (!isAminoSequenceSelected) {
        boxAlgorithm->addItem(tr("InsDel"), FindAlgorithmPatternSettings_InsDel);
        boxAlgorithm->addItem(tr("Substitute"), FindAlgorithmPatternSettings_Subst);
    }
    boxAlgorithm->addItem(tr("Regular expression"), FindAlgorithmPatternSettings_RegExp);

    layoutMismatch = new QHBoxLayout();

    lblMatch = new QLabel(tr("Should match"));

    spinMatch = new QSpinBox(this);
    spinMatch->setSuffix("%");  // Percentage value
    spinMatch->setMinimum(30);
    spinMatch->setMaximum(100);
    spinMatch->setSingleStep(1);
    spinMatch->setValue(100);
    spinMatch->setObjectName("spinBoxMatch");
    spinMatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layoutMismatch->addWidget(lblMatch);
    layoutMismatch->addWidget(spinMatch);

    layoutAlgorithmSettings->addLayout(layoutMismatch);

    initUseAmbiguousBasesContainer();
    initMaxResultLenContainer();

    selectedAlgorithm = boxAlgorithm->itemData(boxAlgorithm->currentIndex()).toInt();
}

void FindPatternWidget::initStrandSelection() {
    boxStrand->addItem(tr("Both"), FindAlgorithmStrand_Both);
    boxStrand->addItem(tr("Direct"), FindAlgorithmStrand_Direct);
    boxStrand->addItem(tr("Reverse-coml."), FindAlgorithmStrand_Complement);
}

void FindPatternWidget::initSeqTranslSelection() {
    boxSeqTransl->addItem(tr("Sequence"), SeqTranslIndex_Sequence);
    boxSeqTransl->addItem(tr("Translation"), SeqTranslIndex_Translation);
}

void FindPatternWidget::initRegionSelection() {
    boxRegion->addItem(tr("Whole sequence"), RegionSelectionIndex_WholeSequence);
    boxRegion->addItem(tr("Custom region"), RegionSelectionIndex_CustomRegion);
    boxRegion->addItem(tr("Selected region"), RegionSelectionIndex_CurrentSelectedRegion);

    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    SAFE_POINT(activeContext != nullptr, "Internal error: sequence context is NULL during region selection init.", );

    setRegionToWholeSequence();

    editStart->setValidator(new QIntValidator(1, activeContext->getSequenceLength(), editStart));
    editEnd->setValidator(new QIntValidator(1, activeContext->getSequenceLength(), editEnd));

    trackedSelection = annotatedDnaView->getActiveSequenceContext()->getSequenceSelection();

    sl_onRegionOptionChanged(RegionSelectionIndex_WholeSequence);
}

void FindPatternWidget::initResultsLimit() {
    boxMaxResult->setMinimum(1);
    boxMaxResult->setMaximum(INT_MAX);
    boxMaxResult->setValue(DEFAULT_RESULTS_NUM_LIMIT);
    boxMaxResult->setEnabled(true);
}

void FindPatternWidget::initUseAmbiguousBasesContainer() {
    useAmbiguousBasesContainer = new QWidget();

    QHBoxLayout* useAmbiguousBasesLayout = new QHBoxLayout();
    useAmbiguousBasesLayout->setContentsMargins(0, 0, 0, 0);
    useAmbiguousBasesLayout->setSpacing(10);
    useAmbiguousBasesLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    useAmbiguousBasesContainer->setLayout(useAmbiguousBasesLayout);

    useAmbiguousBasesBox = new QCheckBox();
    useAmbiguousBasesBox->setObjectName("useAmbiguousBasesBox");
    QLabel* useAmbiguousBasesLabel = new QLabel(tr("Search with ambiguous bases"));
    useAmbiguousBasesLabel->setWordWrap(true);

    useAmbiguousBasesLayout->addWidget(useAmbiguousBasesBox, 0);
    useAmbiguousBasesLayout->addWidget(useAmbiguousBasesLabel, 1);
    layoutAlgorithmSettings->addWidget(useAmbiguousBasesContainer);
}

void FindPatternWidget::initMaxResultLenContainer() {
    useMaxResultLenContainer = new QWidget();

    layoutRegExpLen = new QVBoxLayout();
    layoutRegExpLen->setContentsMargins(0, 0, 0, 0);
    layoutRegExpLen->setSpacing(3);
    layoutRegExpLen->setSizeConstraint(QLayout::SetMinAndMaxSize);
    useMaxResultLenContainer->setLayout(layoutRegExpLen);

    QHBoxLayout* layoutUseMaxResultLen = new QHBoxLayout();
    layoutUseMaxResultLen->setSpacing(10);
    layoutUseMaxResultLen->setSizeConstraint(QLayout::SetMinAndMaxSize);

    boxUseMaxResultLen = new QCheckBox();
    boxUseMaxResultLen->setObjectName("boxUseMaxResultLen");
    QLabel* labelUseMaxResultLen = new QLabel(tr("Results no longer than:"));
    labelUseMaxResultLen->setWordWrap(true);
    layoutUseMaxResultLen->addWidget(boxUseMaxResultLen, 0);
    layoutUseMaxResultLen->addWidget(labelUseMaxResultLen, 1);

    boxMaxResultLen = new QSpinBox();
    boxMaxResultLen->setObjectName("boxMaxResultLen");
    boxMaxResultLen->setMinimum(REG_EXP_MIN_RESULT_LEN);
    boxMaxResultLen->setMaximum(REG_EXP_MAX_RESULT_LEN);
    boxMaxResultLen->setSingleStep(REG_EXP_MAX_RESULT_SINGLE_STEP);
    boxMaxResultLen->setValue(REG_EXP_MAX_RESULT_LEN);
    boxMaxResultLen->setEnabled(false);
    connect(boxUseMaxResultLen, SIGNAL(toggled(bool)), boxMaxResultLen, SLOT(setEnabled(bool)));
    connect(boxUseMaxResultLen, SIGNAL(toggled(bool)), SLOT(sl_activateNewSearch()));
    connect(boxMaxResultLen, SIGNAL(valueChanged(int)), SLOT(sl_activateNewSearch()));

    layoutRegExpLen->addLayout(layoutUseMaxResultLen);
    layoutRegExpLen->addWidget(boxMaxResultLen);
    layoutAlgorithmSettings->addWidget(useMaxResultLenContainer);
}

void FindPatternWidget::connectSlots() {
    connect(boxAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(sl_onAlgorithmChanged(int)));
    connect(boxRegion, SIGNAL(currentIndexChanged(int)), SLOT(sl_onRegionOptionChanged(int)));
    connect(textPattern, SIGNAL(textChanged()), SLOT(sl_onSearchPatternChanged()));
    connect(editStart, SIGNAL(textEdited(QString)), SLOT(sl_onSearchRegionIsChangedByUser()));
    connect(editEnd, SIGNAL(textEdited(QString)), SLOT(sl_onSearchRegionIsChangedByUser()));
    connect(boxSeqTransl, SIGNAL(currentIndexChanged(int)), SLOT(sl_onSequenceTranslationChanged(int)));
    connect(boxMaxResult, SIGNAL(valueChanged(int)), SLOT(sl_onMaxResultChanged(int)));

    connect(boxStrand, SIGNAL(currentIndexChanged(int)), SLOT(sl_activateNewSearch()));
    connect(boxSeqTransl, SIGNAL(currentIndexChanged(int)), SLOT(sl_activateNewSearch()));

    connect(removeOverlapsBox, SIGNAL(stateChanged(int)), SLOT(sl_activateNewSearch()));

    // A sequence has been activated in the Sequence View
    connect(annotatedDnaView, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget*, ADVSequenceWidget*)), this, SLOT(sl_onActiveSequenceChanged()));

    // A sequence has been modified (a subsequence added, removed, etc.)
    connect(annotatedDnaView->getActiveSequenceContext()->getSequenceObject(), SIGNAL(si_sequenceChanged()), this, SLOT(sl_onSequenceModified()));

    connect(loadFromFileToolButton, SIGNAL(clicked()), SLOT(sl_onFileSelectorClicked()));
    connect(usePatternFromFileRadioButton, SIGNAL(toggled(bool)), SLOT(sl_onFileSelectorToggled(bool)));

    usePatternFromTextEditRadioButton->setChecked(true);
    updatePatternSourceControlsUiState();

    connect(filePathLineEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_activateNewSearch()));

    connect(getAnnotationsPushButton, SIGNAL(clicked()), SLOT(sl_getAnnotationsButtonClicked()));
    connect(prevPushButton, SIGNAL(clicked()), SLOT(sl_prevButtonClicked()));
    connect(nextPushButton, SIGNAL(clicked()), SLOT(sl_nextButtonClicked()));

    connect(useAmbiguousBasesBox, SIGNAL(toggled(bool)), SLOT(sl_toggleExtendedAlphabet()));
    connect(spinMatch, SIGNAL(valueChanged(int)), SLOT(sl_activateNewSearch()));
}

void FindPatternWidget::updatePatternSourceControlsUiState() {
    bool isPatternFromText = this->usePatternFromTextEditRadioButton->isChecked();
    textPattern->setEnabled(isPatternFromText);
    filePathLineEdit->setEnabled(!isPatternFromText);
    loadFromFileToolButton->setEnabled(!isPatternFromText);
}

void FindPatternWidget::sl_onAlgorithmChanged(int index) {
    int previousAlgorithm = selectedAlgorithm;
    selectedAlgorithm = boxAlgorithm->itemData(index).toInt();
    updatePatternText(previousAlgorithm);
    updateLayout();
    bool noValidationErrors = verifyPatternAlphabet();
    if (noValidationErrors) {
        sl_activateNewSearch(true);
    }
}

void FindPatternWidget::sl_onRegionOptionChanged(int index) {
    stopTrackingFocusedSequenceSelection();
    int optionIndex = boxRegion->itemData(index).toInt();
    if (optionIndex == RegionSelectionIndex_WholeSequence) {
        editStart->hide();
        lblStartEndConnection->hide();
        editEnd->hide();
        regionIsCorrect = true;
        checkState();
        setRegionToWholeSequence();
    } else if (optionIndex == RegionSelectionIndex_CustomRegion) {
        editStart->show();
        lblStartEndConnection->show();
        editEnd->show();
        editStart->setReadOnly(false);
        editEnd->setReadOnly(false);
        checkState();
    } else if (optionIndex == RegionSelectionIndex_CurrentSelectedRegion) {
        editStart->show();
        lblStartEndConnection->show();
        editEnd->show();
        startTrackingFocusedSequenceSelection();
    }
    sl_activateNewSearch();
}

void FindPatternWidget::sl_onSearchRegionIsChangedByUser() {
    regionIsCorrect = true;

    // The values are not empty
    if (editStart->text().isEmpty()) {
        GUIUtils::setWidgetWarningStyle(editStart, true);
        regionIsCorrect = false;
    } else if (editEnd->text().isEmpty()) {
        GUIUtils::setWidgetWarningStyle(editEnd, true);
        regionIsCorrect = false;
    } else {
        bool ok = false;
        qint64 value1 = editStart->text().toLongLong(&ok);
        if (!ok || (value1 < 1)) {
            GUIUtils::setWidgetWarningStyle(editStart, true);
            regionIsCorrect = false;
        }
        int value2 = editEnd->text().toLongLong(&ok);
        if (!ok || value2 < 1) {
            GUIUtils::setWidgetWarningStyle(editEnd, true);
            regionIsCorrect = false;
        }
    }

    if (regionIsCorrect) {
        GUIUtils::setWidgetWarningStyle(editStart, false);
        GUIUtils::setWidgetWarningStyle(editEnd, false);
    }

    boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_CustomRegion));

    checkState();
    if (regionIsCorrect) {
        sl_activateNewSearch();
    }
}

void FindPatternWidget::sl_onActiveSequenceChanged() {
    stopTrackingFocusedSequenceSelection();

    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    if (activeContext == nullptr) {
        return;
    }
    const DNAAlphabet* alphabet = activeContext->getAlphabet();
    isAminoSequenceSelected = alphabet->isAmino();
    updateLayout();

    // Update region
    setRegionToWholeSequence();
    GUIUtils::setWidgetWarningStyle(editStart, false);
    GUIUtils::setWidgetWarningStyle(editEnd, false);

    // Update available annotations table objects, etc.
    updateAnnotationsWidget();

    if (isSearchInSelectionMode()) {
        startTrackingFocusedSequenceSelection();
    }
}

void FindPatternWidget::updateLayout() {
    // Algorithm group
    if (selectedAlgorithm == FindAlgorithmPatternSettings_Exact) {
        useAmbiguousBasesBox->setChecked(false);
        useAmbiguousBasesContainer->hide();
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        spinMatch->hide();
        lblMatch->hide();
    }
    if (selectedAlgorithm == FindAlgorithmPatternSettings_InsDel) {
        useAmbiguousBasesBox->setChecked(false);
        useAmbiguousBasesContainer->hide();
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        enableDisableMatchSpin();
        lblMatch->show();
        spinMatch->show();
        QWidget::setTabOrder(boxAlgorithm, spinMatch);
        QWidget::setTabOrder(spinMatch, boxStrand);
    } else if (selectedAlgorithm == FindAlgorithmPatternSettings_Subst) {
        useAmbiguousBasesContainer->show();
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        QWidget::setTabOrder(boxAlgorithm, spinMatch);
        QWidget::setTabOrder(spinMatch, useAmbiguousBasesBox);
        enableDisableMatchSpin();
        lblMatch->show();
        spinMatch->show();
        QWidget::setTabOrder(useAmbiguousBasesBox, boxStrand);
    } else if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        useAmbiguousBasesBox->setChecked(false);
        useAmbiguousBasesContainer->hide();
        useMaxResultLenContainer->show();
        boxMaxResultLen->show();
        spinMatch->hide();
        lblMatch->hide();
        QWidget::setTabOrder(boxAlgorithm, boxUseMaxResultLen);
        QWidget::setTabOrder(boxUseMaxResultLen, boxMaxResultLen);
    }

    // "Search in" group
    if (isAminoSequenceSelected) {
        lblStrand->hide();
        boxStrand->hide();
        lblSeqTransl->hide();
        boxSeqTransl->hide();
    } else {
        lblStrand->show();
        boxStrand->show();
        lblSeqTransl->show();
        boxSeqTransl->show();
    }
}

void FindPatternWidget::setMessageFlag(const MessageFlag& messageFlag, bool show, const QString& additionalMsg) {
    if (show) {
        messageFlagMap.insert(messageFlag, additionalMsg);
    } else {
        messageFlagMap.remove(messageFlag);
    }

    if (messageFlag != PleaseInputAtLeastOneSearchPatternTip) {
        // Show PleaseInputAtLeastOneSearchPatternTip only if there are no other messages.
        bool hasNoFlagsOrOnlyTheTip = messageFlagMap.isEmpty() ||
                                      (messageFlagMap.size() == 1 && messageFlagMap.contains(PleaseInputAtLeastOneSearchPatternTip));
        bool isShowEmptyPatternHintOn = hasNoFlagsOrOnlyTheTip && textPattern->toPlainText().trimmed().isEmpty();
        setMessageFlag(PleaseInputAtLeastOneSearchPatternTip, isShowEmptyPatternHintOn);
    }
    updateErrorLabelState();
}

void FindPatternWidget::updateErrorLabelState() {
    QString errorHtml = buildErrorLabelHtml();
    lblErrorMessage->setText(errorHtml);
    bool hasNoPatternErrors = messageFlagMap.isEmpty() ||
                              messageFlagMap.contains(SequenceIsTooBig) ||
                              (messageFlagMap.size() == 1 && messageFlagMap.contains(PleaseInputAtLeastOneSearchPatternTip));
    if (hasNoPatternErrors) {
        GUIUtils::setWidgetWarningStyle(textPattern, false);
    }
}

QString FindPatternWidget::buildErrorLabelHtml() const {
    QString errorColor = Theme::errorColorLabelHtmlStr();
    QString warningColor = Theme::warningColorLabelHtmlStr();
    QString infoColor = Theme::infoColorLabelHtmlStr();

    if (messageFlagMap.contains(SequenceIsTooBig)) {
        // The search is not possible - any other messages are meaningless.
        QString message = tr("Warning: current sequence is too long to search in.");
        return tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(message);
    }
    QString text = "";
    QList<MessageFlag> errorFlags = messageFlagMap.keys();
    for (const MessageFlag& flag : qAsConst(errorFlags)) {
        QString customErrorMessage = messageFlagMap[flag];
        text += text.isEmpty() ? "" : "<br>";
        switch (flag) {
            case SearchRegionIncorrect: {
                SAFE_POINT(!customErrorMessage.isEmpty(), "InvalidSearchRegion must provide a valid error message.", "");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(customErrorMessage);
                break;
            }
            case PatternAlphabetDoNotMatch: {
                QString message = tr("Warning: input value contains characters that do not match the active alphabet!");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(warningColor).arg(message);
                GUIUtils::setWidgetWarningStyle(textPattern, true);
                break;
            }
            case PatternsWithBadAlphabetInFile: {
                QString message = tr("Warning: file contains patterns that do not match the active alphabet!"
                                     " Those patterns were ignored ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(warningColor).arg(message);
                break;
            }
            case PatternsWithBadRegionInFile: {
                QString message = tr("Warning: file contains patterns that"
                                     " longer than the search region! Those patterns were ignored."
                                     " Please input a shorter value or select another region! ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(warningColor).arg(message);
                break;
            }
            case PleaseInputAtLeastOneSearchPatternTip: {
                QString lineBreakShortcut = isOsMac() ? "Cmd+Enter" : "Ctrl+Enter";
                QString message = tr("Info: please input at least one sequence pattern to search for."
                                     " Use %1 to input multiple patterns. Alternatively, load patterns from a FASTA file.")
                                      .arg(lineBreakShortcut);
                text = tr("<b><font color=%1>%2</font><br></br></b>").arg(infoColor).arg(message);
                break;
            }
            case InvalidAnnotationName: {
                QString message = tr("Error: annotation name or annotation group name is invalid. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(message);
                if (!customErrorMessage.isEmpty()) {
                    text += tr("<b><font color=%1>%2</font></b>").arg(errorColor).arg(tr("Reason: "));
                    text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(customErrorMessage);
                }
                const QString msg = tr(" Please input valid annotation names. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(msg);
                break;
            }
            case UnsetAnnotationName_UseCustomPatternAndNoInputFasta: {
                text += tr("<b><font color=%1>%2</font><br></br></b>")
                            .arg(infoColor)
                            .arg(tr("Info: annotation name is not set."));
                text += tr("<br><b><font color=%1>%2</font><br></br></b>")
                            .arg(infoColor)
                            .arg(tr("Annotation names will be automatically composed using search pattern properties."));
                break;
            }
            case NoPatternToSearch: {
                QString message = tr("Error: there is no pattern to search. ");
                text += tr("<b><font color=%1>%2</font></b>").arg(errorColor).arg(message);
                const QString msg = tr(" Please input a valid pattern or choose a file with patterns ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(msg);
                break;
            }
            case PatternWrongRegExp: {
                QString message = tr("Warning: the input regular expression is invalid! ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(errorColor).arg(message);
                GUIUtils::setWidgetWarningStyle(textPattern, true);
                break;
            }
            default:
                FAIL("Unexpected value of the error flag in show/hide error message for pattern!", "");
        }
    }
    return text;
}

void FindPatternWidget::sl_onSearchPatternChanged() {
    static QString cachedTextPatterns = "";
    QString currentTextPatterns = textPattern->toPlainText();
    if (currentTextPatterns == cachedTextPatterns) {
        return;
    }
    cachedTextPatterns = currentTextPatterns;
    setCorrectPatternsString();

    checkState();

    enableDisableMatchSpin();

    // Show a warning if the pattern alphabet doesn't match,
    // but do not block the "Search" button
    bool noValidationErrors = verifyPatternAlphabet();
    if (noValidationErrors && cachedTextPatterns != previousPatternString) {
        previousPatternString = cachedTextPatterns;
        sl_activateNewSearch(false);
    }
}

void FindPatternWidget::sl_onMaxResultChanged(int newMaxResult) {
    bool limitResult = !findPatternResults.isEmpty() && newMaxResult < findPatternResults.size();
    bool widenResult = newMaxResult > previousMaxResult && findPatternResults.size() == previousMaxResult;
    bool prevSearchIsNotComplete = findPatternResults.isEmpty() && searchTask != nullptr;
    if (limitResult || widenResult || prevSearchIsNotComplete) {
        sl_activateNewSearch();
    }
}

void FindPatternWidget::setCorrectPatternsString() {
    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        return;
    }
    QTextCursor cursorInTextEdit = textPattern->textCursor();
    QString textPatternByUser = textPattern->toPlainText();
    FastaPatternsWalker walker(textPatternByUser, cursorInTextEdit.position());
    // Delete all non-alphabet symbols.
    while (walker.hasNext()) {
        QChar character(walker.next());
        if (walker.isCorrect()) {
            continue;
        }
        if (character.isLetter()) {
            if (!character.isUpper()) {
                walker.setCurrent(character.toUpper().toLatin1());
            }
        } else {
            if (character != '\n') {
                walker.removeCurrent();
            }
        }
    }
    QString textPatternByWalker = walker.getString();
    if (textPatternByUser != textPatternByWalker) {
        textPattern->setPlainText(textPatternByWalker);
        cursorInTextEdit.setPosition(walker.getCursor());
        textPattern->setTextCursor(cursorInTextEdit);
    }
    updateNamePatterns();
}

void FindPatternWidget::setRegionToWholeSequence() {
    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    SAFE_POINT(activeContext != nullptr, "Internal error: sequence context is NULL during setting region to whole sequence.", );

    editStart->setText(QString::number(1));
    editEnd->setText(QString::number(activeContext->getSequenceLength()));
    regionIsCorrect = true;
    boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_WholeSequence));
}

bool FindPatternWidget::verifyPatternAlphabet() {
    U2OpStatusImpl os;

    QStringList patternNoNames;
    QList<NamePattern> patternsWithNames = getPatternsFromTextPatternField(os);
    for (const NamePattern& name_pattern : qAsConst(patternsWithNames)) {
        patternNoNames.append(name_pattern.second);
    }

    QString patterns = patternNoNames.join("");

    bool alphabetIsOk = checkAlphabet(patterns);
    setMessageFlag(PatternAlphabetDoNotMatch, !alphabetIsOk);

    bool result = alphabetIsOk;

    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        QString pattern = textPattern->toPlainText();

        // Check that all symbols are ascii
        if (pattern.contains(QRegularExpression(QStringLiteral("[^\\x{0000}-\\x{007F}]")))) {
            setMessageFlag(PatternWrongRegExp, true);
            result = false;
        } else {
            QRegExp regExp(pattern.toUtf8());
            if (regExp.isValid()) {
                setMessageFlag(PatternWrongRegExp, false);
            } else {
                setMessageFlag(PatternWrongRegExp, true);
                result = false;
            }
        }
    } else {
        setMessageFlag(PatternWrongRegExp, false);
    }
    return result;
}

void FindPatternWidget::sl_onSequenceTranslationChanged(int /* index */) {
    verifyPatternAlphabet();
}

void FindPatternWidget::sl_onSequenceModified() {
    setRegionToWholeSequence();
    checkState();
    verifyPatternAlphabet();
}

void FindPatternWidget::checkState() {
    // validate annotation name
    QString v = createAnnotationController->validate();
    if (!v.isEmpty()) {
        setMessageFlag(InvalidAnnotationName, true, v);
        createAnnotationController->setFocusToNameEdit();
        getAnnotationsPushButton->setDisabled(true);
        return;
    }
    if (usePatternNames && !usePatternFromFileRadioButton->isChecked()) {
        for (const QString& name : qAsConst(nameList)) {
            if (!Annotation::isValidAnnotationName(name)) {
                setMessageFlag(UnsetAnnotationName_UseCustomPatternAndNoInputFasta, true);
                return;
            }
        }
        setMessageFlag(UnsetAnnotationName_UseCustomPatternAndNoInputFasta, false);
    }

    getAnnotationsPushButton->setEnabled(!findPatternResults.isEmpty());

    QString pattern = textPattern->toPlainText();

    // Disable the "Search" button if the pattern is empty and pattern is not loaded from a file.
    if (pattern.isEmpty() && !usePatternFromFileRadioButton->isChecked()) {
        setMessageFlag(NoPatternToSearch, false);
        GUIUtils::setWidgetWarningStyle(textPattern, false);
        return;
    }

    // Check region.
    QString regionErrorMessage = checkSearchRegion();
    if (!regionErrorMessage.isEmpty()) {
        GUIUtils::setWidgetWarningStyle(textPattern, true);
        setMessageFlag(SearchRegionIncorrect, true, regionErrorMessage);
        return;
    }
    if (usePatternFromFileRadioButton->isChecked()) {
        setMessageFlag(PatternAlphabetDoNotMatch, false);
    }
    setMessageFlag(UnsetAnnotationName_UseCustomPatternAndNoInputFasta, false);
    setMessageFlag(InvalidAnnotationName, false);
    setMessageFlag(PatternsWithBadRegionInFile, false);
    setMessageFlag(PatternsWithBadAlphabetInFile, false);
    setMessageFlag(NoPatternToSearch, false);
    setMessageFlag(SearchRegionIncorrect, false);
    setMessageFlag(SequenceIsTooBig, false);
    GUIUtils::setWidgetWarningStyle(textPattern, false);
}

void FindPatternWidget::enableDisableMatchSpin() {
    spinMatch->setEnabled(!((textPattern->toPlainText().isEmpty() && !usePatternFromFileRadioButton->isChecked()) || isAminoSequenceSelected));
}

U2Region FindPatternWidget::getSearchRegion() const {
    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    qint64 sequenceLength = activeContext->getSequenceLength();
    if (boxRegion->itemData(boxRegion->currentIndex()).toInt() == RegionSelectionIndex_WholeSequence) {
        return U2Region(0, sequenceLength);
    }
    bool startIsOk = false;
    bool endIsOk = false;
    qint64 start = editStart->text().toLongLong(&startIsOk) - 1;
    qint64 end = editEnd->text().toLongLong(&endIsOk) - 1;
    U2Region resultRegion(start, end - start + 1);
    CHECK(startIsOk && endIsOk && resultRegion.length > 0 && U2Region(0, sequenceLength).contains(resultRegion), {});
    return resultRegion;
}

QList<QPair<QString, QString>> FindPatternWidget::getPatternsFromTextPatternField(U2OpStatus& os) const {
    QString inputText = textPattern->toPlainText().toLocal8Bit();
    QList<NamePattern> result = FastaFormat::getSequencesAndNamesFromUserInput(inputText, os);

    if (result.isEmpty()) {
        QStringList patterns = inputText.split(QRegExp("\n"), QString::SkipEmptyParts);
        for (const QString& pattern : qAsConst(patterns)) {
            result.append(qMakePair(QString(""), pattern));
        }
    }

    if (!usePatternNames) {
        createAnnotationController->validate();
        const CreateAnnotationModel& model = createAnnotationController->getModel();
        for (int i = 0; i < result.size(); i++) {
            result[i].first = model.data->name;
        }
    }

    return result;
}

void FindPatternWidget::updateAnnotationsWidget() {
    // Updating the annotations widget
    SAFE_POINT(annotatedDnaView->getActiveSequenceContext() != nullptr,
               "There is no sequence in focus to update the annotations widget on the 'Search in Sequence' tab.", );
    CreateAnnotationModel annotationModel = createAnnotationController->getModel();

    annotationModel.newDocUrl.clear();
    annotationModel.hideLocation = true;
    annotationModel.sequenceObjectRef = annotatedDnaView->getActiveSequenceContext()->getSequenceObject()->getReference();
    annotationModel.sequenceLen = annotatedDnaView->getActiveSequenceContext()->getSequenceLength();
    createAnnotationController->updateWidgetForAnnotationModel(annotationModel);
}

#define FIND_PATTER_LAST_DIR "Find_pattern_last_dir"

void FindPatternWidget::sl_onFileSelectorClicked() {
    LastUsedDirHelper lod(FIND_PATTER_LAST_DIR);

    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::SEQUENCE});
    lod.url = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select file to open..."), lod.dir, filter);
    if (!lod.url.isEmpty())
        filePathLineEdit->setText(lod.url);
}

void FindPatternWidget::sl_onFileSelectorToggled(bool on) {
    updatePatternSourceControlsUiState();
    checkState();
    if (!on) {  // if returning to input-pattern mode -> recheck it's content
        verifyPatternAlphabet();
    }
    sl_activateNewSearch(true);
}

void FindPatternWidget::initFindPatternTask(const QList<NamePattern>& patterns) {
    CHECK(!patterns.isEmpty(), );

    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        QRegExp regExp(textPattern->toPlainText());
        CHECK(regExp.isValid(), );
    }
    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    SAFE_POINT(activeContext != nullptr, "Internal error: there is no sequence in focus!", );

    FindAlgorithmTaskSettings settings;
    U2OpStatusImpl os;
    settings.sequence = activeContext->getSequenceObject()->getWholeSequenceData(os);
    CHECK_OP_EXT(os, setMessageFlag(SequenceIsTooBig, true), );  // suppose that if the sequence cannot be fetched from the DB, UGENE ran out of memory
    settings.sequenceAlphabet = activeContext->getSequenceObject()->getAlphabet();
    settings.searchIsCircular = activeContext->getSequenceObject()->isCircular();

    // Strand
    if (isAminoSequenceSelected) {
        settings.strand = FindAlgorithmStrand_Direct;
    } else {
        int strandId = boxStrand->currentIndex();
        switch (strandId) {
            case FindAlgorithmStrand_Both:
                settings.strand = FindAlgorithmStrand_Both;
                break;
            case FindAlgorithmStrand_Direct:
                settings.strand = FindAlgorithmStrand_Direct;
                break;
            case FindAlgorithmStrand_Complement:
                settings.strand = FindAlgorithmStrand_Complement;
                break;
            default:
                FAIL("Unexpected value of the strand parameter!", );
        }
    }

    if (!isAminoSequenceSelected) {
        settings.complementTT = activeContext->getComplementTT();
        if (settings.complementTT == nullptr && settings.strand != FindAlgorithmStrand_Direct) {
            coreLog.error(tr("Unable to search on the reverse-complement strand,"
                             " searching on the direct strand only!"));
            settings.strand = FindAlgorithmStrand_Direct;
        }
    }

    // Sequence/translation
    if (!isAminoSequenceSelected && (SeqTranslIndex_Translation == boxSeqTransl->currentIndex())) {
        settings.proteinTT = activeContext->getAminoTT();
    } else {
        settings.proteinTT = nullptr;
    }

    // Limit results number to the specified value
    settings.maxResult2Find = boxMaxResult->value();
    previousMaxResult = settings.maxResult2Find;

    // Region
    U2Region region = getSearchRegion();
    CHECK(!region.isEmpty(), );
    settings.searchRegion = region;

    // Algorithm settings
    settings.patternSettings = static_cast<FindAlgorithmPatternSettings>(selectedAlgorithm);

    settings.maxErr = 0;

    settings.useAmbiguousBases = useAmbiguousBasesBox->isChecked();
    settings.maxRegExpResultLength = boxUseMaxResultLen->isChecked() ? boxMaxResultLen->value() : DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    // Creating and registering the task
    bool removeOverlaps = removeOverlapsBox->isChecked();

    SAFE_POINT(searchTask == nullptr, "Search task is not NULL", );
    nextPushButton->setDisabled(true);
    prevPushButton->setDisabled(true);

    searchTask = new FindPatternListTask(settings, patterns, removeOverlaps, spinMatch->value());
    connect(searchTask, SIGNAL(si_stateChanged()), SLOT(sl_findPatternTaskStateChanged()));
    startProgressAnimation();
    AppContext::getTaskScheduler()->registerTopLevelTask(searchTask);
}

void FindPatternWidget::sl_loadPatternTaskStateChanged() {
    auto loadTask = qobject_cast<LoadPatternsFileTask*>(sender());
    CHECK(loadTask != nullptr, );
    CHECK(loadTask->isFinished() && !loadTask->isCanceled(), );
    CHECK(!loadTask->hasError(), );

    QList<NamePattern> namesPatterns = loadTask->getNamesPatterns();
    nameList.clear();
    for (int i = 0; i < namesPatterns.size(); i++) {
        nameList << namesPatterns[i].first;
        namesPatterns[i].first = QString::number(i);
    }

    stopCurrentSearchTask();
    initFindPatternTask(namesPatterns);

    annotationModelIsPrepared = false;
    updateAnnotationsWidget();
}

bool compareByRegionStartPos(const SharedAnnotationData& r1, const SharedAnnotationData& r2) {
    return r1->getRegions().first().startPos < r2->getRegions().first().startPos;
}

void FindPatternWidget::sl_findPatternTaskStateChanged() {
    auto findTask = qobject_cast<FindPatternListTask*>(sender());
    CHECK(findTask != nullptr, );
    if (findTask != searchTask) {
        return;
    }
    if (findTask->isFinished() || findTask->isCanceled() || findTask->hasError()) {
        findPatternResults = findTask->getResults();
        if (findPatternResults.isEmpty()) {
            currentResultIndex = -1;
            showCurrentResultAndStopProgress();
            nextPushButton->setDisabled(true);
            prevPushButton->setDisabled(true);
            getAnnotationsPushButton->setDisabled(true);
        } else {
            std::sort(findPatternResults.begin(), findPatternResults.end(), compareByRegionStartPos);
            bool isSearchInSelection = isSearchInSelectionMode();
            // In search in selection mode we do not auto-activate the first search result to avoid concurrent selection update with user.
            currentResultIndex = isSearchInSelection ? -1 : 0;
            showCurrentResultAndStopProgress();
            nextPushButton->setEnabled(true);
            prevPushButton->setEnabled(true);
            getAnnotationsPushButton->setEnabled(true);
            checkState();
            // Activate the first search result.
            if (currentResultIndex >= 0) {
                auto seqWdgt = qobject_cast<ADVSingleSequenceWidget*>(annotatedDnaView->getActiveSequenceWidget());
                if (seqWdgt != nullptr) {
                    if (seqWdgt->getDetView() != nullptr && !seqWdgt->getDetView()->isEditMode()) {
                        showCurrentResult();
                    }
                } else {
                    showCurrentResult();
                }
            }
        }
        disconnect(this, SLOT(sl_loadPatternTaskStateChanged()));
        searchTask = nullptr;
    }
}

bool FindPatternWidget::checkAlphabet(const QString& pattern) {
    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    SAFE_POINT(activeContext != nullptr, "Internal error: there is no sequence in focus on pattern search!", false);

    const DNAAlphabet* alphabet = activeContext->getAlphabet();
    if (!isAminoSequenceSelected && SeqTranslIndex_Translation == boxSeqTransl->currentIndex()) {
        DNATranslation* translation = activeContext->getAminoTT();
        SAFE_POINT(translation != nullptr, "Failed to get translation on pattern search!", false);

        alphabet = translation->getDstAlphabet();
    }
    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        return true;
    }
    bool patternFitsIntoAlphabet = TextUtils::fits(alphabet->getMap(), pattern.toLocal8Bit().data(), pattern.size());
    if (patternFitsIntoAlphabet) {
        return true;
    }
    if (useAmbiguousBasesBox->isChecked() && !alphabet->isExtended()) {
        const DNAAlphabet* extAlphabet = U2AlphabetUtils::getExtendedAlphabet(alphabet);
        if (extAlphabet != nullptr) {
            bool patternFitsIntoExtAlphabet = TextUtils::fits(extAlphabet->getMap(), pattern.toLocal8Bit().data(), pattern.size());
            if (patternFitsIntoExtAlphabet) {
                return true;
            }
        }
    }
    return false;
}

QString FindPatternWidget::checkSearchRegion() const {
    U2Region region = getSearchRegion();
    if (region.isEmpty()) {
        return tr("Warning: Invalid search region.");
    }
    QString pattern = textPattern->toPlainText();
    if (!usePatternFromFileRadioButton->isChecked() && selectedAlgorithm != FindAlgorithmPatternSettings_RegExp && !pattern.isEmpty()) {
        QStringList patterns = pattern.split("\n");
        QString longestPattern = *std::max_element(patterns.begin(),
                                                   patterns.end(),
                                                   [](auto& p1, auto& p2) { return p1.length() < p2.length(); });
        if (longestPattern.length() > region.length) {
            return tr("Warning: Search region is too small.");
        }
    }
    return "";
}

void FindPatternWidget::sl_syncSearchRegionWithTrackedSelection() {
    SAFE_POINT(trackedSelection != nullptr, "No tracked selection is found!", );
    const QVector<U2Region>& selectedRegions = trackedSelection->getSelectedRegions();
    if (isSearchInSelectionMode() && isRegionListInSearchResults(selectedRegions)) {
        return;  // User browses selection results (clicks Prev/Next) -> do not update the search selection range.
    }
    if (!selectedRegions.isEmpty()) {
        U2Region firstReg = selectedRegions.first();
        editStart->setText(QString::number(firstReg.startPos + 1));
        editEnd->setText(QString::number(firstReg.endPos()));

        if (selectedRegions.size() == 2) {
            U2Region secondReg = selectedRegions.last();
            SAFE_POINT(annotatedDnaView->getActiveSequenceContext() != nullptr, tr("Sequence in focus is NULL"), );
            int seqLen = annotatedDnaView->getActiveSequenceContext()->getSequenceLength();
            bool circularSelection = (firstReg.startPos == 0 && secondReg.endPos() == seqLen) || (firstReg.endPos() == seqLen && secondReg.startPos == 0);
            if (circularSelection) {
                if (secondReg.startPos == 0) {
                    editEnd->setText(QString::number(secondReg.endPos()));
                } else {
                    editStart->setText(QString::number(secondReg.startPos + 1));
                }
            }
        }
    } else {
        SAFE_POINT(annotatedDnaView->getActiveSequenceContext() != nullptr, "No sequence in focus, with active search tab in options panel", );
        editStart->setText(QString::number(1));
        editEnd->setText(QString::number(annotatedDnaView->getActiveSequenceContext()->getSequenceLength()));
    }
    regionIsCorrect = true;
    checkState();

    sl_activateNewSearch();
}

void FindPatternWidget::sl_onAnnotationNameEdited() {
    checkState();
}

void FindPatternWidget::updatePatternText(int previousAlgorithm) {
    // Save a previous state.
    if (previousAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        patternRegExp = textPattern->toPlainText();
    } else {
        patternString = textPattern->toPlainText();
    }

    // Set a new state.
    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        textPattern->setPlainText(patternRegExp);
    } else {
        textPattern->setPlainText(patternString);
    }
    setCorrectPatternsString();
}

void FindPatternWidget::sl_toggleExtendedAlphabet() {
    verifyPatternAlphabet();
    sl_activateNewSearch(true);
}

void FindPatternWidget::sl_activateNewSearch(bool forcedSearch) {
    if (usePatternFromFileRadioButton->isChecked()) {
        stopCurrentSearchTask();
        if (filePathLineEdit->text().isEmpty()) {
            return;
        }
        auto loadTask = new LoadPatternsFileTask(filePathLineEdit->text());
        connect(loadTask, SIGNAL(si_stateChanged()), SLOT(sl_loadPatternTaskStateChanged()));
        AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
    } else {
        QList<NamePattern> newPatterns = updateNamePatterns();
        if (isSearchPatternsDifferent(newPatterns) || forcedSearch) {
            patternList.clear();
            for (int i = 0; i < newPatterns.size(); i++) {
                newPatterns[i].first = QString::number(i);
                patternList.append(newPatterns[i].second);
            }
        } else {
            checkState();
            return;
        }

        stopCurrentSearchTask();
        initFindPatternTask(newPatterns);

        annotationModelIsPrepared = false;
    }
}

QList<NamePattern> FindPatternWidget::updateNamePatterns() {
    CHECK(!usePatternFromFileRadioButton->isChecked(), QList<NamePattern>());

    U2OpStatus2Log os;
    QList<NamePattern> newPatterns = getPatternsFromTextPatternField(os);

    nameList.clear();
    for (const NamePattern& newPattern : qAsConst(newPatterns)) {
        nameList.append(newPattern.first);
    }
    return newPatterns;
}

void FindPatternWidget::sl_getAnnotationsButtonClicked() {
    if (!annotationModelIsPrepared) {
        bool objectPrepared = createAnnotationController->prepareAnnotationObject();
        SAFE_POINT(objectPrepared, "Cannot create an annotation object. Please check settings", );
        annotationModelIsPrepared = true;
    }
    QString validationError = createAnnotationController->validate();
    SAFE_POINT(validationError.isEmpty(), "Annotation names are invalid", );

    const CreateAnnotationModel& annotationModel = createAnnotationController->getModel();
    QString group = annotationModel.groupName;

    AnnotationTableObject* annotationTableObject = annotationModel.getAnnotationObject();
    SAFE_POINT(annotationTableObject != nullptr, "Invalid annotation table detected!", );
    annotatedDnaView->tryAddObject(annotationTableObject);
    QList<SharedAnnotationData> annotationsToCreate = findPatternResults;

    for (int i = 0; i < findPatternResults.size(); i++) {
        if (usePatternNames) {
            bool ok = false;
            int index = findPatternResults[i]->name.toInt(&ok);
            if (Q_UNLIKELY(!ok)) {
                coreLog.details(tr("Warning: can not get valid pattern name, annotation will be named 'misc_feature'"));
                Q_ASSERT(false);
                if (annotationsToCreate[i]->name.isEmpty()) {
                    annotationsToCreate[i]->name = "misc_feature";
                }
            } else {
                SAFE_POINT(ok, "Failed conversion to integer", );
                SAFE_POINT(nameList.size() > index, "Out of boundaries in names list", );
                SAFE_POINT(index >= 0, "Out of boundaries in names list", );
                QString name = nameList[index];
                annotationsToCreate[i]->name = Annotation::isValidAnnotationName(name) ? name : Annotation::produceValidAnnotationName(name);
            }
        } else {
            annotationsToCreate[i]->name = Annotation::isValidAnnotationName(annotationModel.data->name) ? annotationModel.data->name : Annotation::produceValidAnnotationName(annotationModel.data->name);
        }

        annotationsToCreate[i]->type = annotationModel.data->type;
        U1AnnotationUtils::addDescriptionQualifier(annotationsToCreate[i], annotationModel.description);
    }
    GCOUNTER(cvar, "FindAlgorithmTask");
    if (annotationModel.data->name == annotationModel.groupName && usePatternNames) {
        group.clear();
    }
    createAnnotationController->countDescriptionUsage();
    AppContext::getTaskScheduler()->registerTopLevelTask(new CreateAnnotationsTask(annotationTableObject, {{group, annotationsToCreate}}));

    annotationModelIsPrepared = false;
    updateAnnotationsWidget();
}

void FindPatternWidget::sl_prevButtonClicked() {
    if (currentResultIndex <= 0) {
        currentResultIndex = findPatternResults.size() - 1;
    } else {
        currentResultIndex--;
    }
    showCurrentResult();
}

void FindPatternWidget::sl_nextButtonClicked() {
    if (currentResultIndex == findPatternResults.size() - 1) {
        currentResultIndex = 0;
    } else {
        currentResultIndex++;
    }
    showCurrentResult();
}

void FindPatternWidget::showCurrentResult() const {
    updateResultLabelText();
    CHECK(currentResultIndex < findPatternResults.size(), );
    if (currentResultIndex == -1) {
        return;
    }
    // Activate the current result.
    const SharedAnnotationData& findResult = findPatternResults.at(currentResultIndex);
    ADVSequenceObjectContext* activeContext = annotatedDnaView->getActiveSequenceContext();
    CHECK(activeContext != nullptr, );
    const QVector<U2Region>& regions = findResult->getRegions();
    CHECK(!regions.isEmpty(), );
    activeContext->getSequenceSelection()->setSelectedRegions(regions);
    qint64 centerPos = regions.first().center() + 1;
    annotatedDnaView->sl_onPosChangeRequest(centerPos);
}

void FindPatternWidget::sl_onEnterPressed() {
    if (nextPushButton->isEnabled()) {
        nextPushButton->click();
    }
}

void FindPatternWidget::sl_onShiftEnterPressed() {
    if (prevPushButton->isEnabled()) {
        prevPushButton->click();
    }
}

void FindPatternWidget::sl_usePatternNamesCbClicked() {
    usePatternNames = !usePatternNames;
    createAnnotationController->setEnabledNameEdit(!usePatternNames);
    updateNamePatterns();
    checkState();
}

bool FindPatternWidget::isSearchPatternsDifferent(const QList<NamePattern>& newPatterns) const {
    if (newPatterns.size() != patternList.size()) {
        return true;
    }
    for (const NamePattern& newPattern : qAsConst(newPatterns)) {
        if (!patternList.contains(newPattern.second)) {
            return true;
        }
    }
    return false;
}

void FindPatternWidget::stopCurrentSearchTask() {
    if (searchTask != nullptr) {
        disconnect(this, SLOT(sl_loadPatternTaskStateChanged()));
        if (!searchTask->isCanceled() && searchTask->getState() != Task::State_Finished) {
            searchTask->cancel();
        }
        searchTask = nullptr;
    }
    findPatternResults.clear();
    nextPushButton->setDisabled(true);
    prevPushButton->setDisabled(true);
    getAnnotationsPushButton->setDisabled(true);
    showCurrentResultAndStopProgress();
}

void FindPatternWidget::setUpTabOrder() const {
    auto annotationWidget = qobject_cast<CreateAnnotationWidgetController*>(createAnnotationController);
    SAFE_POINT(annotationWidget != nullptr, "Bad casting to CreateAnnotationWidgetController", );

    QWidget::setTabOrder(nextPushButton, boxAlgorithm);
    QWidget::setTabOrder(boxAlgorithm, boxStrand);
    QWidget::setTabOrder(boxStrand, boxSeqTransl);
    QWidget::setTabOrder(boxSeqTransl, boxRegion);
    QWidget::setTabOrder(boxRegion, editStart);
    QWidget::setTabOrder(editStart, editEnd);
    QWidget::setTabOrder(editEnd, removeOverlapsBox);
    QWidget::setTabOrder(removeOverlapsBox, boxMaxResult);
    QWidget::setTabOrder(boxMaxResult, annotationWidget->getTaborderEntryAndExitPoints().first);
    QWidget::setTabOrder(annotationWidget->getTaborderEntryAndExitPoints().second, getAnnotationsPushButton);
}

int FindPatternWidget::getTargetSequenceLength() const {
    SAFE_POINT(annotatedDnaView->getActiveSequenceContext() != nullptr, "Sequence is NULL", 0);
    return annotatedDnaView->getActiveSequenceContext()->getSequenceLength();
}

void FindPatternWidget::startProgressAnimation() {
    resultLabel->setText(tr("Results:"));
    progressLabel->show();
    progressMovie->start();
}

void FindPatternWidget::startTrackingFocusedSequenceSelection() {
    stopTrackingFocusedSequenceSelection();
    ADVSequenceObjectContext* focusedSequenceContext = annotatedDnaView->getActiveSequenceContext();
    if (focusedSequenceContext != nullptr) {
        trackedSelection = focusedSequenceContext->getSequenceSelection();
        connect(trackedSelection, SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)), this, SLOT(sl_syncSearchRegionWithTrackedSelection()));
        sl_syncSearchRegionWithTrackedSelection();
    }
}

void FindPatternWidget::stopTrackingFocusedSequenceSelection() {
    if (trackedSelection != nullptr) {
        disconnect(trackedSelection, SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)), this, SLOT(sl_syncSearchRegionWithTrackedSelection()));
        trackedSelection = nullptr;
    }
}

bool FindPatternWidget::isRegionListInSearchResults(const QVector<U2Region>& regionList) const {
    for (const U2Region& region : qAsConst(regionList)) {
        bool isFound = false;
        for (const SharedAnnotationData& result : qAsConst(findPatternResults)) {
            if (result->getRegions().contains(region)) {
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            return false;
        }
    }
    return true;
}

bool FindPatternWidget::isSearchInSelectionMode() const {
    return boxRegion->currentData().toInt() == RegionSelectionIndex_CurrentSelectedRegion;
}

}  // namespace U2
