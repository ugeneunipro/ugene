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

#ifndef _U2_FIND_PATTERN_MSA_WIDGET_H_
#define _U2_FIND_PATTERN_MSA_WIDGET_H_

#include <U2Core/U2Region.h>

#include <U2View/MSAEditor.h>

#include "FindPatternMsaWidgetSavableTab.h"
#include "ov_msa/view_rendering/MaEditorSelection.h"
#include "ov_sequence/find_pattern/FindPatternTask.h"
#include "ov_sequence/find_pattern/FindPatternWidget.h"
#include "ui_FindPatternMsaForm.h"

namespace U2 {
class ShowHideSubgroupWidget;
class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class CreateAnnotationWidgetController;
class DNASequenceSelection;
class Task;
class U2OpStatus;

/** Find algorithm results with view positioning info. */
struct FindPatternWidgetResult {
    FindPatternWidgetResult(qint64 rowId, int viewRowIndex, const U2Region &region);

    /** MA sequence row id. */
    qint64 rowId;

    /** View row index of the result. -1 if the result is not visible: the result is inside of some collapsed group. */
    int viewRowIndex;

    /** Region with gaps. */
    U2Region region;
};

class FindPatternMsaWidget : public QWidget, private Ui_FindPatternMsaForm {
    Q_OBJECT
public:
    /** Creates a new widget. Activates search-in-name mode if isSearchInNamesMode is yes. Uses the last saved state if isSearchInNamesMode is Unknown. */
    FindPatternMsaWidget(MSAEditor *msaEditor, TriState isSearchInNamesModeTriState = TriState_Unknown);

    int getTargetMsaLength() const;

    void setSearchInNamesMode(bool flag);

private slots:
    void sl_onAlgorithmChanged(int);
    void sl_onRegionOptionChanged(int);
    void sl_onRegionValueEdited();
    void sl_onSearchPatternChanged();
    void sl_onMaxResultChanged(int);
    void sl_findPatternTaskStateChanged();
    void sl_searchModeChanged();
    void sl_groupResultsButtonClicked();

    /** A sequence part was added, removed or replaced */
    void sl_onMsaModified();
    void sl_msaStateChanged();

    void sl_onSelectedRegionChanged(const MaEditorSelection &currentSelection, const MaEditorSelection &prev);
    void sl_prevButtonClicked();
    void sl_nextButtonClicked();

    void sl_onEnterPressed();
    void sl_onShiftEnterPressed();
    void sl_collapseModelChanged();

    void sl_validateStateAndStartNewSearch(bool activatedByOutsideChanges = false);

private:
    void updateActions();
    void initLayout();
    void initAlgorithmLayout();
    void initRegionSelection();
    void initResultsLimit();
    void initMaxResultLenContainer();
    void updateLayout();
    void connectSlots();
    bool checkRegion();
    void clearResults();

    /** Returns true if the alignment alphabet is Amino. */
    bool isAmino() const;

    int getMaxError(const QString &pattern) const;

    /** Assigns valid viewRowIndex value to all results & resorts them based on the view position. */
    void resortResultsByViewState();

    /** Returns current visible result equal to the selection. Returns -1 if no such result found. */
    int findCurrentResultIndexFromSelection() const;

    /** Returns next or prev result index using current selection top-left position. */
    int getNextOrPrevResultIndexFromSelection(bool isNext);

    /** Updates label with current result position. */
    void updateCurrentResultLabel();

    /** Selects current search result in the MSA editor . */
    void selectCurrentResult();

    /** Returns true if current MSA editor selection region is equal to the current result. */
    bool isResultSelected() const;

    void stopCurrentSearchTask();
    void correctSearchInCombo();
    void setUpTabOrder() const;
    void showCurrentResultAndStopProgress();
    void startProgressAnimation();

    /**
     * Enables or disables the Search button depending on
     * the Pattern field value (it should be not empty and not too long)
     * and on the validity of the region.
     * Return true if the search can be started.
     */
    bool checkStateAndUpdateStatus();
    bool checkPatternRegion(const QString &pattern);

    /**
     * The "Match" spin is disabled if this is an amino acid sequence or
     * the search pattern is empty. Otherwise it is enabled.
     */
    void enableDisableMatchSpin();

    /** Allows showing of several error messages. */
    void showHideMessage(bool show, MessageFlag messageFlag, const QString &additionalMsg = QString());

    /** Checks pattern alphabet and sets error message if needed. Returns false on error or true if no error found */
    bool verifyPatternAlphabet();
    bool checkAlphabet(const QString &pattern);
    void showTooLongSequenceError();
    void hideAllMessages();

    void setCorrectPatternsString();
    void setRegionToWholeSequence();

    /** Performs in-main thread search in sequence names. */
    void runSearchInSequenceNames(const QStringList &patterns);

    /**
     * Checks current UI state and returns either valid or empty region.
     * Sets 'isRegionIsCorrect' if the region is valid.
     */
    U2Region getSearchRegionFromUi(bool &isRegionIsCorrect) const;

    void startFindPatternInMsaTask(const QStringList &patterns);

    /**
     * Checks if there are several patterns in textPattern which are separated by new line symbol,
     * parses them out and returns with their names (if they're exist).
     */
    QStringList getPatternsFromTextPatternField(U2OpStatus &os) const;

    void updatePatternText(int previousAlgorithm);

    /** Post processes allSearchResults list after search task is finished. */
    void postProcessAllSearchResults();

    MSAEditor *msaEditor;
    int selectedAlgorithm;
    QString patternString;
    QString patternRegExp;

    QList<MessageFlag> messageFlags;

    /** Widgets in the Algorithm group */
    QHBoxLayout *layoutMismatch;
    QVBoxLayout *layoutRegExpLen;

    QLabel *lblMatch;
    QSpinBox *spinMatch;

    QWidget *useMaxResultLenContainer;
    QCheckBox *boxUseMaxResultLen;
    QSpinBox *boxMaxResultLen;

    static const int DEFAULT_RESULTS_NUM_LIMIT;
    static const int DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    static const int REG_EXP_MIN_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_SINGLE_STEP;

    /** Visible only search results. */
    QList<FindPatternWidgetResult> visibleSearchResults;

    /** All search results: both visible & hidden in collapsed groups. */
    QList<FindPatternWidgetResult> allSearchResults;

    /** Index of the currently selected result. */
    int currentResultIndex;

    Task *searchTask;
    int previousMaxResult;
    QStringList currentSearchPatternList;
    QMovie *progressMovie;
    bool setSelectionToTheFirstResult;
    bool isSearchInNamesMode;

    FindPatternMsaWidgetSavableTab savableWidget;

    ShowHideSubgroupWidget *algorithmSubgroup;
    ShowHideSubgroupWidget *searchInSubgroup;
    ShowHideSubgroupWidget *otherSettingsSubgroup;
};

}    // namespace U2

#endif    // _U2_FIND_PATTERN_WIDGET_H_
