/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "FindPatternMsaWidget.h"

#include <QFlags>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMovie>

#include <U2Algorithm/FindAlgorithmTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Theme.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/FastaFormat.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MSAEditorSequenceArea.h>

#include "FindPatternMsaTask.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_sequence/find_pattern/FindPatternWidget.h"

namespace U2 {

const int FindPatternMsaWidget::DEFAULT_RESULTS_NUM_LIMIT = 100000;
const int FindPatternMsaWidget::DEFAULT_REGEXP_RESULT_LENGTH_LIMIT = 10000;

const int FindPatternMsaWidget::REG_EXP_MIN_RESULT_LEN = 1;
const int FindPatternMsaWidget::REG_EXP_MAX_RESULT_LEN = 1000;
const int FindPatternMsaWidget::REG_EXP_MAX_RESULT_SINGLE_STEP = 20;

class PatternWalker {
public:
    PatternWalker(const QString& patternsString, int cursor = 0)
        : patternsString(patternsString.toLatin1()), cursor(cursor), current(-1) {
    }

    bool hasNext() const {
        return current < patternsString.size() - 1;
    }

    char next() {
        if (!hasNext()) {
            return 0;
        }
        current++;
        return patternsString[current];
    }

    bool isSequenceChar() const {
        CHECK(-1 != current, false);
        CHECK(current < patternsString.size(), false);
        return true;
    }

    /** moves current place to the previous */
    void removeCurrent() {
        CHECK(-1 != current, );
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
        CHECK(-1 != current, );
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
};

#define SEARCH_MODE_SEQUENCES_INDEX 0
#define SEARCH_MODE_NAMES_INDEX 1

/** Last used search mode. Stored per session only. */
static int isSearchInNamesModeByDefault = false;

FindPatternMsaWidget::FindPatternMsaWidget(MSAEditor* msaEditor, TriState isSearchInNamesModeTriState)
    : msaEditor(msaEditor),
      currentResultIndex(-1),
      searchTask(nullptr),
      previousMaxResult(-1),
      setSelectionToTheFirstResult(true),
      savableWidget(this, GObjectViewUtils::findViewByName(msaEditor->getName())),
      algorithmSubgroup(nullptr),
      searchInSubgroup(nullptr),
      otherSettingsSubgroup(nullptr) {
    setupUi(this);
    setObjectName("FindPatternMsaWidget");
    if (isSearchInNamesModeTriState == TriState_Unknown) {  // Re-use the last state
        isSearchInNamesMode = isSearchInNamesModeByDefault;
    } else {
        isSearchInNamesMode = isSearchInNamesModeTriState == TriState_Yes;
    }

    progressMovie = new QMovie(":/core/images/progress.gif", QByteArray(), progressLabel);
    progressLabel->setObjectName("progressLabel");
    resultLabel->setObjectName("resultLabel");
    resultLabel->setFixedHeight(progressLabel->height());
    savableWidget.setRegionWidgetIds(QStringList() << editStart->objectName()
                                                   << editEnd->objectName());
    progressLabel->setMovie(progressMovie);

    setContentsMargins(0, 0, 0, 0);

    initLayout();
    connectSlots();
    updateActions();

    checkStateAndUpdateStatus();
    setFocusProxy(textPattern);

    auto findPatternEventFilter = new FindPatternEventFilter(textPattern);
    connect(findPatternEventFilter, SIGNAL(si_enterPressed()), SLOT(sl_onEnterPressed()));
    connect(findPatternEventFilter, SIGNAL(si_shiftEnterPressed()), SLOT(sl_onShiftEnterPressed()));

    sl_onSearchPatternChanged();

    showCurrentResultAndStopProgress();
    setUpTabOrder();
    previousMaxResult = boxMaxResult->value();
    U2WidgetStateStorage::restoreWidgetState(savableWidget);
}

int FindPatternMsaWidget::getTargetMsaLength() const {
    return msaEditor->getAlignmentLen();
}

void FindPatternMsaWidget::setSearchInNamesMode(bool flag) {
    CHECK(isSearchInNamesMode != flag, )
    int indexToActivate = flag ? SEARCH_MODE_NAMES_INDEX : SEARCH_MODE_SEQUENCES_INDEX;
    searchContextComboBox->setCurrentIndex(indexToActivate);  // triggers a signal.
}

void FindPatternMsaWidget::showCurrentResultAndStopProgress() {
    progressMovie->stop();
    progressLabel->hide();
    resultLabel->show();
    updateCurrentResultLabel();
}

void FindPatternMsaWidget::initLayout() {
    lblErrorMessage->setStyleSheet("font: bold;");
    lblErrorMessage->setText("");
    initAlgorithmLayout();
    initRegionSelection();
    initResultsLimit();

    subgroupsLayout->setSpacing(0);

    algorithmSubgroup = new ShowHideSubgroupWidget(QObject::tr("Search algorithm"), QObject::tr("Search algorithm"), widgetAlgorithm, false);
    subgroupsLayout->addWidget(algorithmSubgroup);

    searchInSubgroup = new ShowHideSubgroupWidget(QObject::tr("Search in"), QObject::tr("Search in"), widgetSearchIn, false);
    subgroupsLayout->addWidget(searchInSubgroup);

    otherSettingsSubgroup = new ShowHideSubgroupWidget(QObject::tr("Other settings"), QObject::tr("Other settings"), widgetOther, false);
    subgroupsLayout->addWidget(otherSettingsSubgroup);

    searchContextComboBox->addItem(tr("Sequences"));
    searchContextComboBox->addItem(tr("Sequence Names"));
    if (isSearchInNamesMode) {
        searchContextComboBox->setCurrentIndex(SEARCH_MODE_NAMES_INDEX);
    }

    updateLayout();

    layoutSearchButton->setAlignment(Qt::AlignTop);
    this->layout()->setAlignment(Qt::AlignTop);

    this->layout()->setMargin(0);
}

void FindPatternMsaWidget::initAlgorithmLayout() {
    boxAlgorithm->addItem(tr("Exact"), FindAlgorithmPatternSettings_Exact);
    if (!isAmino()) {
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
    initMaxResultLenContainer();

    selectedAlgorithm = boxAlgorithm->itemData(boxAlgorithm->currentIndex()).toInt();
}

void FindPatternMsaWidget::initRegionSelection() {
    boxRegion->addItem(FindPatternMsaWidget::tr("Whole alignment"), RegionSelectionIndex_WholeSequence);
    boxRegion->addItem(FindPatternMsaWidget::tr("Custom columns region"), RegionSelectionIndex_CustomRegion);
    boxRegion->addItem(FindPatternMsaWidget::tr("Selected columns region"), RegionSelectionIndex_CurrentSelectedRegion);
    setRegionToWholeSequence();

    editStart->setValidator(new QIntValidator(1, msaEditor->getAlignmentLen(), editStart));
    editEnd->setValidator(new QIntValidator(1, msaEditor->getAlignmentLen(), editEnd));

    sl_onRegionOptionChanged(RegionSelectionIndex_WholeSequence);
}

void FindPatternMsaWidget::initResultsLimit() {
    boxMaxResult->setMinimum(1);
    boxMaxResult->setMaximum(INT_MAX);
    boxMaxResult->setValue(DEFAULT_RESULTS_NUM_LIMIT);
    boxMaxResult->setEnabled(true);
}

void FindPatternMsaWidget::initMaxResultLenContainer() {
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
    connect(boxUseMaxResultLen, SIGNAL(toggled(bool)), SLOT(sl_validateStateAndStartNewSearch()));
    connect(boxMaxResultLen, SIGNAL(valueChanged(int)), SLOT(sl_validateStateAndStartNewSearch()));

    layoutRegExpLen->addLayout(layoutUseMaxResultLen);
    layoutRegExpLen->addWidget(boxMaxResultLen);
    layoutAlgorithmSettings->addWidget(useMaxResultLenContainer);

    connect(msaEditor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_collapseModelChanged()));
}

void FindPatternMsaWidget::connectSlots() {
    connect(boxAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(sl_onAlgorithmChanged(int)));
    connect(boxRegion, SIGNAL(currentIndexChanged(int)), SLOT(sl_onRegionOptionChanged(int)));
    connect(textPattern, SIGNAL(textChanged()), SLOT(sl_onSearchPatternChanged()));
    connect(editStart, SIGNAL(textChanged(QString)), SLOT(sl_onRegionValueEdited()));
    connect(editEnd, SIGNAL(textChanged(QString)), SLOT(sl_onRegionValueEdited()));
    connect(boxMaxResult, SIGNAL(valueChanged(int)), SLOT(sl_onMaxResultChanged(int)));
    connect(removeOverlapsBox, SIGNAL(stateChanged(int)), SLOT(sl_validateStateAndStartNewSearch()));
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    connect(msaObject, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), this, SLOT(sl_onMsaModified()));
    connect(msaObject, SIGNAL(si_alphabetChanged(const MaModificationInfo&, const DNAAlphabet*)), this, SLOT(sl_onMsaModified()));
    connect(msaObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_msaStateChanged()));
    connect(prevPushButton, SIGNAL(clicked()), SLOT(sl_prevButtonClicked()));
    connect(nextPushButton, SIGNAL(clicked()), SLOT(sl_nextButtonClicked()));
    connect(groupResultsButton, SIGNAL(clicked()), SLOT(sl_groupResultsButtonClicked()));
    connect(spinMatch, SIGNAL(valueChanged(int)), SLOT(sl_validateStateAndStartNewSearch()));

    connect(msaEditor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_onSelectedRegionChanged(const MaEditorSelection&, const MaEditorSelection&)));

    connect(searchContextComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_searchModeChanged()));
}

void FindPatternMsaWidget::sl_onAlgorithmChanged(int index) {
    int previousAlgorithm = selectedAlgorithm;
    selectedAlgorithm = boxAlgorithm->itemData(index).toInt();
    updatePatternText(previousAlgorithm);
    updateLayout();
    sl_validateStateAndStartNewSearch();
}

void FindPatternMsaWidget::sl_onRegionOptionChanged(int index) {
    if (boxRegion->itemData(index).toInt() == RegionSelectionIndex_WholeSequence) {
        editStart->hide();
        lblStartEndConnection->hide();
        editEnd->hide();
        setRegionToWholeSequence();
        checkStateAndUpdateStatus();
    } else if (boxRegion->itemData(index).toInt() == RegionSelectionIndex_CustomRegion) {
        editStart->show();
        lblStartEndConnection->show();
        editEnd->show();
        editStart->setReadOnly(false);
        editEnd->setReadOnly(false);
        checkStateAndUpdateStatus();
    } else if (boxRegion->itemData(index).toInt() == RegionSelectionIndex_CurrentSelectedRegion) {
        editStart->show();
        lblStartEndConnection->show();
        editEnd->show();
        sl_onSelectedRegionChanged(msaEditor->getSelection(), MaEditorSelection());
    }
}

void FindPatternMsaWidget::sl_onRegionValueEdited() {
    boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_CustomRegion));
    sl_validateStateAndStartNewSearch();
}

void FindPatternMsaWidget::updateActions() {
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    groupResultsButton->setEnabled(!msaObject->isStateLocked());
}

void FindPatternMsaWidget::updateLayout() {
    updateActions();
    algorithmSubgroup->setVisible(!isSearchInNamesMode);
    searchInSubgroup->setVisible(!isSearchInNamesMode);
    otherSettingsSubgroup->setVisible(!isSearchInNamesMode);
    if (isSearchInNamesMode) {  // search in names
        return;
    }
    // Algorithm group
    if (selectedAlgorithm == FindAlgorithmPatternSettings_Exact) {
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        spinMatch->hide();
        lblMatch->hide();
    }
    if (selectedAlgorithm == FindAlgorithmPatternSettings_InsDel) {
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        enableDisableMatchSpin();
        lblMatch->show();
        spinMatch->show();
        QWidget::setTabOrder(boxAlgorithm, spinMatch);
    } else if (selectedAlgorithm == FindAlgorithmPatternSettings_Subst) {
        useMaxResultLenContainer->hide();
        boxMaxResultLen->hide();
        QWidget::setTabOrder(boxAlgorithm, spinMatch);
        enableDisableMatchSpin();
        lblMatch->show();
        spinMatch->show();
    } else if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        useMaxResultLenContainer->show();
        boxMaxResultLen->show();
        spinMatch->hide();
        lblMatch->hide();
        QWidget::setTabOrder(boxAlgorithm, boxUseMaxResultLen);
        QWidget::setTabOrder(boxUseMaxResultLen, boxMaxResultLen);
    }
}

void FindPatternMsaWidget::setMessageFlag(const MessageFlag& messageFlag, bool show, const QString& additionalMsg) {
    if (show) {
        messageFlagMap.insert(messageFlag, additionalMsg);
    } else {
        messageFlagMap.remove(messageFlag);
    }
    updateErrorLabelState();
}

void FindPatternMsaWidget::updateErrorLabelState() {
    QString text = "";
    QList<MessageFlag> messageFlags = messageFlagMap.keys();

    for (const MessageFlag& flag : qAsConst(messageFlags)) {
        QString customErrorMessage = messageFlagMap[flag];
        text += text.isEmpty() ? "" : "<br>";
        switch (flag) {
            case PatternAlphabetDoNotMatch: {
                QString message = tr("Warning: input value contains characters that"
                                     " do not match the active alphabet!");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::warningColorLabelHtmlStr()).arg(message);
                GUIUtils::setWidgetWarning(textPattern, true);
                break;
            }
            case PatternsWithBadAlphabetInFile: {
                QString message = tr("Warning: file contains patterns that"
                                     " do not match the active alphabet! Those patterns were ignored ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::warningColorLabelHtmlStr()).arg(message);
                break;
            }
            case PatternsWithBadRegionInFile: {
                QString message = tr("Warning: file contains patterns that"
                                     " longer than the search region! Those patterns were ignored. Please input a shorter value or select another region! ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::warningColorLabelHtmlStr()).arg(message);
                break;
            }
            case PleaseInputAtLeastOneSearchPatternTip: {
                QString message = isSearchInNamesMode ? tr("Info: please input at least one pattern to search in the sequence names.") : tr("Info: please input at least one sequence pattern to search for.");

                message += " " + tr("Use %1 to input multiple patterns").arg(isOsMac() ? "Cmd+Enter" : "Ctrl+Enter");
                text = QString("<b><font color=%1>%2</font><br></br></b>").arg(Theme::infoColorLabelHtmlStr()).arg(message);
                break;
            }
            case AnnotationNotValidName: {
                QString message = tr("Warning: annotation name or annotation group name are invalid. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(message);
                if (!customErrorMessage.isEmpty()) {
                    text += tr("<b><font color=%1>%2</font></b>").arg(Theme::errorColorLabelHtmlStr()).arg(tr("Reason: "));
                    text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(customErrorMessage);
                }
                QString msg = tr(" Please input valid annotation names. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(msg);
                break;
            }
            case AnnotationNotValidFastaParsedName: {
                QString message = tr("Warning: annotation names are invalid. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(message);
                if (!customErrorMessage.isEmpty()) {
                    text += tr("<b><font color=%1>%2</font></b>").arg(Theme::errorColorLabelHtmlStr()).arg(tr("Reason: "));
                    text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(customErrorMessage);
                }
                QString msg = tr(" It will be automatically changed to acceptable name if 'Get annotations' button is pressed. ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(msg);
                break;
            }
            case NoPatternToSearch: {
                QString message = tr("Warning: there is no pattern to search. ");
                text += tr("<b><font color=%1>%2</font></b>").arg(Theme::errorColorLabelHtmlStr()).arg(message);
                QString msg = tr(" Please input a valid pattern ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(msg);
                break;
            }
            case SearchRegionIncorrect: {
                SAFE_POINT(!customErrorMessage.isEmpty(), "InvalidSearchRegion must provide a valid error message.", );
                text += tr("<b><font color=%1>%2</font></b>").arg(Theme::errorColorLabelHtmlStr()).arg(customErrorMessage);
                break;
            }
            case PatternWrongRegExp: {
                QString message = tr("Warning: the input regular expression is invalid! ");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(message);
                GUIUtils::setWidgetWarning(textPattern, true);
                break;
            }
            case SequenceIsTooBig: {
                text.clear();  // the search is blocked at all -- any other messages are meaningless
                QString message = tr("Warning: current sequence is too long to search in.");
                text += tr("<b><font color=%1>%2</font><br></br></b>").arg(Theme::errorColorLabelHtmlStr()).arg(message);
                break;
            }
            default:
                FAIL("Unexpected value of the error flag in show/hide error message for pattern!", );
        }
    }
    lblErrorMessage->setText(text);
    bool hasNoErrors = messageFlags.isEmpty() || (messageFlags.size() == 1 && messageFlags.contains(PleaseInputAtLeastOneSearchPatternTip));
    if (hasNoErrors) {
        GUIUtils::setWidgetWarning(textPattern, false);
    }
}

void FindPatternMsaWidget::sl_onSearchPatternChanged() {
    sl_validateStateAndStartNewSearch();
}

void FindPatternMsaWidget::sl_validateStateAndStartNewSearch(bool activatedByOutsideChanges) {
    stopCurrentSearchTask();
    clearResults();

    setCorrectPatternsString();
    if (searchTask != nullptr) {
        // A new search task was started as the result of the recursive
        // 'setCorrectPatternsString' method call.
        return;
    }
    enableDisableMatchSpin();
    checkStateAndUpdateStatus();
    if (!messageFlagMap.isEmpty()) {
        return;
    }
    setSelectionToTheFirstResult = !activatedByOutsideChanges;
    U2OpStatusImpl os;
    QStringList newPatterns = getPatternsFromTextPatternField(os);
    CHECK_OP(os, )
    currentResultIndex = -1;
    if (isSearchInNamesMode) {
        runSearchInSequenceNames(newPatterns);
    } else {
        startFindPatternInMsaTask(newPatterns);
    }
}

void FindPatternMsaWidget::clearResults() {
    visibleSearchResults.clear();
    allSearchResults.clear();
    nextPushButton->setDisabled(true);
    prevPushButton->setDisabled(true);
}

void FindPatternMsaWidget::sl_onMaxResultChanged(int newMaxResult) {
    int resultsSize = visibleSearchResults.size();
    bool limitResult = !visibleSearchResults.isEmpty() && newMaxResult < resultsSize;
    bool widenResult = newMaxResult > previousMaxResult && resultsSize == previousMaxResult;
    bool prevSearchIsNotComplete = visibleSearchResults.isEmpty() && searchTask != nullptr;
    if (limitResult || widenResult || prevSearchIsNotComplete) {
        sl_validateStateAndStartNewSearch();
    }
}

void FindPatternMsaWidget::setCorrectPatternsString() {
    if (isSearchInNamesMode) {
        return;
    }
    QTextCursor cursorInTextEdit = textPattern->textCursor();

    if (selectedAlgorithm != FindAlgorithmPatternSettings_RegExp) {
        PatternWalker walker(textPattern->toPlainText(), cursorInTextEdit.position());
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

        if (textPattern->toPlainText() != walker.getString()) {
            textPattern->setText(walker.getString());
            cursorInTextEdit.setPosition(walker.getCursor());
            textPattern->setTextCursor(cursorInTextEdit);
        }
    }
}

void FindPatternMsaWidget::setRegionToWholeSequence() {
    editStart->setText(QString::number(1));
    editEnd->setText(QString::number(msaEditor->getAlignmentLen()));
    boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_WholeSequence));
}

bool FindPatternMsaWidget::verifyPatternAlphabet() {
    bool alphabetIsOk = checkAlphabet(textPattern->toPlainText().remove("\n"));
    setMessageFlag(PatternAlphabetDoNotMatch, !alphabetIsOk);

    bool result = alphabetIsOk;

    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        QString reText = textPattern->toPlainText();

        // Check that all symbols are ascii
        if (reText.contains(QRegularExpression(QStringLiteral("[^\\x{0000}-\\x{007F}]")))) {
            setMessageFlag(PatternWrongRegExp, true);
            result = false;
        } else {
            QRegExp regExp(reText.toUtf8());
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

void FindPatternMsaWidget::sl_onMsaModified() {
    setRegionToWholeSequence();
    sl_validateStateAndStartNewSearch(true);
}

void FindPatternMsaWidget::hideAllMessages() {
    messageFlagMap.clear();
    updateErrorLabelState();
}

void FindPatternMsaWidget::showTooLongSequenceError() {
    hideAllMessages();
    setMessageFlag(SequenceIsTooBig, true);
}

void FindPatternMsaWidget::checkStateAndUpdateStatus() {
    hideAllMessages();
    updateCurrentResultLabel();

    GUIUtils::setWidgetWarning(textPattern, false);

    QString searchRegionError = checkSearchRegion();
    GUIUtils::setWidgetWarning(editStart, !searchRegionError.isEmpty());
    GUIUtils::setWidgetWarning(editEnd, !searchRegionError.isEmpty());
    GUIUtils::setWidgetWarning(textPattern, !searchRegionError.isEmpty() && !patternString.isEmpty());
    setMessageFlag(SearchRegionIncorrect, !searchRegionError.isEmpty(), searchRegionError);

    // Make extra checks for search-in-sequence mode.
    if (!isSearchInNamesMode) {
        verifyPatternAlphabet();
    }

    // If everything is OK and search pattern is empty: show empty pattern tip.
    if (textPattern->toPlainText().isEmpty()) {
        setMessageFlag(PleaseInputAtLeastOneSearchPatternTip, true);
    }
}

void FindPatternMsaWidget::enableDisableMatchSpin() {
    spinMatch->setDisabled(textPattern->toPlainText().isEmpty() || isAmino());
}

U2Region FindPatternMsaWidget::getSearchRegion() const {
    qint64 alignmentLength = msaEditor->getAlignmentLen();
    if (boxRegion->itemData(boxRegion->currentIndex()).toInt() == RegionSelectionIndex_WholeSequence) {
        return U2Region(0, alignmentLength);
    }
    bool startIsOk = false;
    bool endIsOk = false;
    qint64 start = editStart->text().toLongLong(&startIsOk) - 1;
    qint64 end = editEnd->text().toLongLong(&endIsOk) - 1;
    U2Region resultRegion(start, end - start + 1);
    CHECK(startIsOk && endIsOk && resultRegion.length > 0 && U2Region(0, alignmentLength).contains(resultRegion), {});
    return resultRegion;
}

int FindPatternMsaWidget::getMaxError(const QString& pattern) const {
    if (selectedAlgorithm == FindAlgorithmPatternSettings_Exact) {
        return 0;
    }
    return int((float)(1 - float(spinMatch->value()) / 100) * pattern.length());
}

QStringList FindPatternMsaWidget::getPatternsFromTextPatternField(U2OpStatus& os) const {
    QString inputText = textPattern->toPlainText();
    QList<NamePattern> nameList = FastaFormat::getSequencesAndNamesFromUserInput(inputText, os);
    if (!nameList.isEmpty()) {
        QStringList result;
        foreach (const NamePattern& namePattern, nameList) {
            result << namePattern.second;
        }
        return result;
    }
    return inputText.split(QRegExp("\n"), QString::SkipEmptyParts);
}

#define FIND_PATTER_LAST_DIR "Find_msa_pattern_last_dir"

void FindPatternMsaWidget::startFindPatternInMsaTask(const QStringList& patterns) {
    currentSearchPatternList = patterns;
    CHECK(!patterns.isEmpty(), );

    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        QRegExp regExp(textPattern->toPlainText());
        CHECK(regExp.isValid(), );
    }

    FindPatternMsaSettings settings;
    foreach (const QString& pattern, patterns) {
        settings.patterns << NamePattern("", pattern);
    }

    settings.msaObj = msaEditor->getMaObject();
    U2OpStatusImpl os;
    CHECK_OP_EXT(os, showTooLongSequenceError(), );  // suppose that if the sequence cannot be fetched from the DB, UGENE ran out of memory

    // Limit results number to the specified value
    settings.findSettings.maxResult2Find = boxMaxResult->value();
    previousMaxResult = settings.findSettings.maxResult2Find;

    // Region
    U2Region region = getSearchRegion();
    SAFE_POINT(!region.isEmpty(), "Internal error: incorrect search region has been supplied."
                                  " Skipping the pattern search.", );
    settings.findSettings.searchRegion = region;

    // Algorithm settings
    settings.findSettings.patternSettings = static_cast<FindAlgorithmPatternSettings>(selectedAlgorithm);

    settings.findSettings.maxErr = 0;

    settings.findSettings.maxRegExpResultLength = boxUseMaxResultLen->isChecked() ? boxMaxResultLen->value() : DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    // Creating and registering the task
    settings.removeOverlaps = removeOverlapsBox->isChecked();
    settings.findSettings.maxResult2Find = boxMaxResult->value();
    settings.matchValue = spinMatch->value();

    SAFE_POINT(searchTask == nullptr, "Search task is not nullptr", );
    groupResultsButton->setDisabled(true);

    searchTask = new FindPatternMsaTask(settings);
    connect(searchTask, SIGNAL(si_stateChanged()), SLOT(sl_findPatternTaskStateChanged()));
    startProgressAnimation();
    TaskWatchdog::trackResourceExistence(msaEditor->getMaObject(), searchTask);
    AppContext::getTaskScheduler()->registerTopLevelTask(searchTask);

    // Switch from the 'selected region' to 'custom region' because search task results will trigger selection updates.
    int boxRegionData = boxRegion->itemData(boxRegion->currentIndex()).toInt();
    if (boxRegionData == RegionSelectionIndex_CurrentSelectedRegion) {
        boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_CustomRegion));
    }
}

void FindPatternMsaWidget::sl_searchModeChanged() {
    isSearchInNamesMode = searchContextComboBox->currentIndex() == SEARCH_MODE_NAMES_INDEX;
    isSearchInNamesModeByDefault = isSearchInNamesMode;
    clearResults();
    updateLayout();
    sl_validateStateAndStartNewSearch();
}

void FindPatternMsaWidget::sl_findPatternTaskStateChanged() {
    FindPatternMsaTask* findTask = static_cast<FindPatternMsaTask*>(sender());
    CHECK(findTask != nullptr, );
    if (findTask != searchTask) {
        return;
    }
    if (!findTask->isFinished() && !findTask->isCanceled() && !findTask->hasError()) {
        return;
    }
    allSearchResults.clear();
    const QList<FindPatternInMsaResult>& findTaskResultList = findTask->getResults();
    for (int i = 0; i < findTaskResultList.size(); i++) {
        const FindPatternInMsaResult& findTaskResult = findTaskResultList[i];
        for (int j = 0; j < findTaskResult.regions.length(); j++) {
            allSearchResults << FindPatternWidgetResult(findTaskResult.rowId, -1, findTaskResult.regions[j]);
        }
    }
    postProcessAllSearchResults();
    searchTask = nullptr;
}

void FindPatternMsaWidget::postProcessAllSearchResults() {
    visibleSearchResults.clear();
    resortResultsByViewState();
    showCurrentResultAndStopProgress();
    bool hasVisibleResults = !visibleSearchResults.isEmpty();
    nextPushButton->setEnabled(hasVisibleResults);
    prevPushButton->setEnabled(hasVisibleResults);
    groupResultsButton->setEnabled(hasVisibleResults && !msaEditor->getMaObject()->isStateLocked());
    if (hasVisibleResults) {
        correctSearchInCombo();
        if (setSelectionToTheFirstResult) {
            currentResultIndex = 0;
            selectCurrentResult();
        }
    }
}

bool FindPatternMsaWidget::checkAlphabet(const QString& pattern) {
    const DNAAlphabet* alphabet = msaEditor->getMaObject()->getAlphabet();
    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        return true;
    }
    return TextUtils::fits(alphabet->getMap(), pattern.toLocal8Bit().data(), pattern.size());
}

QString FindPatternMsaWidget::checkSearchRegion() const {
    CHECK(!isSearchInNamesMode, "");

    U2Region region = getSearchRegion();
    CHECK(!region.isEmpty(), tr("Warning: Invalid search region."));

    CHECK(selectedAlgorithm != FindAlgorithmPatternSettings_RegExp, "");

    QStringList patternLines = textPattern->toPlainText().split("\n", QString::SkipEmptyParts);
    CHECK(!patternLines.isEmpty(), "");

    int minPatternLength = INT_MAX;
    for (const QString& line : qAsConst(patternLines)) {
        minPatternLength = qMin(minPatternLength, line.length());
    }
    if (region.length < minPatternLength) {
        return tr("Warning: Pattern is too long.");
    }
    return "";
}

void FindPatternMsaWidget::sl_onSelectedRegionChanged(const MaEditorSelection& currentSelection, const MaEditorSelection& prev) {
    Q_UNUSED(prev);
    int boxRegionData = boxRegion->itemData(boxRegion->currentIndex()).toInt();
    bool isSearchInSelectionOn = boxRegionData == RegionSelectionIndex_CurrentSelectedRegion;
    if (isSearchInSelectionOn && findCurrentResultIndexFromSelection() == -1) {
        currentResultIndex = -1;
        if (currentSelection.isEmpty()) {
            editStart->setText(QString::number(1));
            editEnd->setText(QString::number(msaEditor->getAlignmentLen()));
        } else {
            QRect selectionRect = currentSelection.toRect();
            U2Region firstRegion = U2Region(selectionRect.topLeft().rx(), selectionRect.width());
            editStart->setText(QString::number(firstRegion.startPos + 1));
            editEnd->setText(QString::number(firstRegion.endPos()));
        }
        // Select the option again. Reason: setting text makes RegionSelectionIndex_CurrentRegion activated
        boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_CurrentSelectedRegion));
    } else if (!isResultSelected()) {
        // reset result position on non-compatible selection change.
        currentResultIndex = -1;
    }
    checkStateAndUpdateStatus();
}

void FindPatternMsaWidget::updatePatternText(int previousAlgorithm) {
    // Save a previous state.
    if (previousAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        patternRegExp = textPattern->toPlainText();
    } else {
        patternString = textPattern->toPlainText();
    }

    // Set a new state.
    if (selectedAlgorithm == FindAlgorithmPatternSettings_RegExp) {
        textPattern->setText(patternRegExp);
    } else {
        textPattern->setText(patternString);
    }
    setCorrectPatternsString();
}

void FindPatternMsaWidget::runSearchInSequenceNames(const QStringList& patterns) {
    currentSearchPatternList = patterns;

    const MultipleAlignment& multipleAlignment = msaEditor->getMaObject()->getMultipleAlignment();
    U2Region wholeRowRegion(0, msaEditor->getAlignmentLen());
    QSet<int> resultRowIndexSet;
    foreach (const QString& pattern, currentSearchPatternList) {
        if (pattern.isEmpty()) {
            continue;
        }
        for (int i = 0, n = multipleAlignment->getRowCount(); i < n; i++) {
            const MultipleAlignmentRow& row = multipleAlignment->getRow(i);
            if (row->getName().contains(pattern, Qt::CaseInsensitive)) {
                resultRowIndexSet << i;
            }
        }
    }
    foreach (int rowIndex, resultRowIndexSet) {
        const MultipleAlignmentRow& row = multipleAlignment->getRow(rowIndex);
        allSearchResults << FindPatternWidgetResult(row->getRowId(), -1, wholeRowRegion);
    }
    postProcessAllSearchResults();
}

void FindPatternMsaWidget::sl_prevButtonClicked() {
    int nResults = visibleSearchResults.size();
    CHECK(nResults > 0, );
    if (currentResultIndex == -1 || !isResultSelected()) {
        currentResultIndex = getNextOrPrevResultIndexFromSelection(false);
    } else {
        currentResultIndex = (currentResultIndex - 1 + nResults) % nResults;
    }
    selectCurrentResult();
}

void FindPatternMsaWidget::sl_nextButtonClicked() {
    int nResults = visibleSearchResults.size();
    CHECK(nResults > 0, );
    if (currentResultIndex == -1 || !isResultSelected()) {
        currentResultIndex = getNextOrPrevResultIndexFromSelection(true);
    } else {
        currentResultIndex = (currentResultIndex + 1) % nResults;
    }
    selectCurrentResult();
}

void FindPatternMsaWidget::selectCurrentResult() {
    CHECK(currentResultIndex >= 0 && currentResultIndex < visibleSearchResults.length(), );
    const FindPatternWidgetResult &result = visibleSearchResults[currentResultIndex];
    MaEditorSequenceArea *seqArea = msaEditor->getMaEditorWgt()->getSequenceArea();
    QRect selection(result.region.startPos, result.viewRowIndex, result.region.length, 1);
    seqArea->setSelectionRect(selection);
    seqArea->centerPos(selection.topLeft());
    updateCurrentResultLabel();
}

void FindPatternMsaWidget::sl_onEnterPressed() {
    nextPushButton->click();
}

void FindPatternMsaWidget::sl_onShiftEnterPressed() {
    prevPushButton->click();
}

void FindPatternMsaWidget::sl_collapseModelChanged() {
    resortResultsByViewState();
    updateCurrentResultLabel();
}

void FindPatternMsaWidget::stopCurrentSearchTask() {
    if (searchTask != nullptr) {
        if (!searchTask->isCanceled() && searchTask->getState() != Task::State_Finished) {
            searchTask->cancel();
        }
        searchTask = nullptr;
    }
    visibleSearchResults.clear();
    nextPushButton->setDisabled(true);
    prevPushButton->setDisabled(true);
    groupResultsButton->setDisabled(true);
    showCurrentResultAndStopProgress();
}

void FindPatternMsaWidget::correctSearchInCombo() {
    if (boxRegion->itemData(boxRegion->currentIndex()).toInt() == RegionSelectionIndex_CurrentSelectedRegion) {
        boxRegion->setCurrentIndex(boxRegion->findData(RegionSelectionIndex_CustomRegion));
    }
}

void FindPatternMsaWidget::setUpTabOrder() const {
    QWidget::setTabOrder(groupResultsButton, prevPushButton);
    QWidget::setTabOrder(prevPushButton, nextPushButton);
    QWidget::setTabOrder(nextPushButton, boxAlgorithm);
    QWidget::setTabOrder(boxRegion, editStart);
    QWidget::setTabOrder(editStart, editEnd);
    QWidget::setTabOrder(editEnd, removeOverlapsBox);
    QWidget::setTabOrder(removeOverlapsBox, boxMaxResult);
}

void FindPatternMsaWidget::startProgressAnimation() {
    resultLabel->setText(tr("Results:"));
    progressLabel->show();
    progressMovie->start();
}

struct SearchResultsComparator {
    bool operator()(const FindPatternWidgetResult& r1, const FindPatternWidgetResult& r2) const {
        return r1.viewRowIndex != r2.viewRowIndex ? r1.viewRowIndex < r2.viewRowIndex : r1.region.startPos < r2.region.startPos;
    }
};

void FindPatternMsaWidget::resortResultsByViewState() {
    MaCollapseModel* collapseModel = msaEditor->getCollapseModel();
    visibleSearchResults.clear();
    for (int i = 0; i < allSearchResults.size(); i++) {
        FindPatternWidgetResult& result = allSearchResults[i];
        result.viewRowIndex = collapseModel->getViewRowIndexByMaRowId(result.rowId);
        if (result.viewRowIndex >= 0) {
            visibleSearchResults << result;
        }
    }
    std::sort(visibleSearchResults.begin(), visibleSearchResults.end(), SearchResultsComparator());
    currentResultIndex = findCurrentResultIndexFromSelection();
}

int FindPatternMsaWidget::findCurrentResultIndexFromSelection() const {
    const MaEditorSelection& selection = msaEditor->getSelection();
    QRect selectionRect = selection.toRect();
    if (visibleSearchResults.isEmpty() || selection.isEmpty() || selectionRect.height() != 1) {
        return -1;
    }
    U2Region selectedXRegion = U2Region::fromXRange(selectionRect);
    for (int i = 0; i < visibleSearchResults.size(); i++) {
        const FindPatternWidgetResult& result = visibleSearchResults[i];
        if (result.viewRowIndex == selectionRect.y() && result.region == selectedXRegion) {
            return i;
        }
    }
    return -1;
}

int FindPatternMsaWidget::getNextOrPrevResultIndexFromSelection(bool isNext) {
    int resultsCount = visibleSearchResults.size();
    CHECK(resultsCount > 0, -1);

    const MaEditorSelection& selection = msaEditor->getSelection();
    CHECK(!selection.isEmpty(), 0);

    int resultIndex = 0;
    for (; resultIndex < resultsCount; resultIndex++) {
        const FindPatternWidgetResult& result = visibleSearchResults[resultIndex];
        QRect selectionRect = selection.toRect();
        bool inTheNextRow = result.viewRowIndex > selectionRect.y();
        bool inTheSameRowAndNext = result.viewRowIndex == selectionRect.y() && result.region.startPos >= selectionRect.x();
        if (inTheNextRow || inTheSameRowAndNext) {
            break;
        }
    }
    if (isNext) {
        return resultIndex == visibleSearchResults.size() ? 0 : resultIndex;
    } else {
        return resultIndex > 0 ? resultIndex - 1 : resultsCount - 1;
    }
}

bool FindPatternMsaWidget::isResultSelected() const {
    const MaEditorSelection& selection = msaEditor->getSelection();
    QRect selectionRect = selection.toRect();
    if (selectionRect.height() != 1 || currentResultIndex < 0 || currentResultIndex >= visibleSearchResults.size()) {
        return false;
    }
    const FindPatternWidgetResult& result = visibleSearchResults[currentResultIndex];
    return selectionRect.y() == result.viewRowIndex && result.region == U2Region(selectionRect.x(), selectionRect.width());
}

void FindPatternMsaWidget::updateCurrentResultLabel() {
    QString currentResultText = visibleSearchResults.isEmpty() || currentResultIndex < 0 ? "-" : QString::number(currentResultIndex + 1);
    if (visibleSearchResults.isEmpty()) {
        resultLabel->setText(tr("No results"));
    } else {
        resultLabel->setText(tr("Results: %1/%2").arg(currentResultText).arg(visibleSearchResults.size()));
    }
}

void FindPatternMsaWidget::sl_groupResultsButtonClicked() {
    CHECK(!allSearchResults.isEmpty(), )
    MultipleSequenceAlignmentObject* maObject = msaEditor->getMaObject();
    CHECK(!maObject->isStateLocked(), );

    // Switch to the Original row order mode.
    msaEditor->getUI()->sl_toggleSequenceRowOrder(false);

    QSet<qint64> resultUidSet;
    for (const FindPatternWidgetResult& result : qAsConst(allSearchResults)) {
        resultUidSet << result.rowId;
    }
    const QList<qint64> allRowIds = msaEditor->getMaRowIds();
    if (resultUidSet.size() >= allRowIds.size()) {
        // Can't re-group anything: every sequence has a result.
        msaEditor->selectRows(0, allRowIds.size());
        return;
    }

    bool isOldGroupedAtStart = true;
    for (int i = 0; i < resultUidSet.size(); i++) {
        if (!resultUidSet.contains(allRowIds[i])) {
            isOldGroupedAtStart = false;
            break;
        }
    }

    // Reorder rows: move search results to the top. Keep the order stable.
    QList<qint64> rowsInTheGroup;
    QList<qint64> rowsOutOfTheGroup;
    for (qint64 rowId : qAsConst(allRowIds)) {
        if (resultUidSet.contains(rowId)) {
            rowsInTheGroup << rowId;
        } else {
            rowsOutOfTheGroup << rowId;
        }
    }
    bool isNewGroupAtStart = !isOldGroupedAtStart;
    QList<qint64> reorderedRowIds = isNewGroupAtStart ? QList<qint64>() << rowsInTheGroup << rowsOutOfTheGroup : QList<qint64>() << rowsOutOfTheGroup << rowsInTheGroup;
    CHECK(!maObject->isStateLocked(), );
    U2OpStatusImpl os;
    maObject->updateRowsOrder(os, reorderedRowIds);
    if (!os.hasError()) {
        if (isNewGroupAtStart) {
            msaEditor->selectRows(0, rowsInTheGroup.size());
        } else {
            msaEditor->selectRows(allRowIds.size() - rowsInTheGroup.size(), rowsInTheGroup.size());
        }
    }
}

bool FindPatternMsaWidget::isAmino() const {
    const DNAAlphabet* alphabet = msaEditor->getMaObject()->getAlphabet();
    return alphabet->isAmino();
}

void FindPatternMsaWidget::sl_msaStateChanged() {
    updateActions();
}

FindPatternWidgetResult::FindPatternWidgetResult(qint64 rowId, int viewRowIndex, const U2Region& region)
    : rowId(rowId), viewRowIndex(viewRowIndex), region(region) {
}

}  // namespace U2
