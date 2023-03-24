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

#pragma once

#include <QPlainTextEdit>

#include <U2Core/AnnotationData.h>
#include <U2Core/U2Region.h>

#include "FindPatternTask.h"
#include "FindPatternWidgetSavableTab.h"
#include "ui_FindPatternForm.h"

namespace U2 {

class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class CreateAnnotationWidgetController;
class DNASequenceSelection;
class Task;
class U2OpStatus;

enum SeqTranslIndex {
    SeqTranslIndex_Sequence,
    SeqTranslIndex_Translation
};

enum RegionSelectionIndex {
    RegionSelectionIndex_WholeSequence,
    RegionSelectionIndex_CustomRegion,
    RegionSelectionIndex_CurrentSelectedRegion
};

/**
 * Adds extra behavior to the installed QTextEdit:
 *  - emits si_enterPressed and Enter is pressed.
 *  - translates Shift+Enter into 'new-line'.
 *  - auto-focuses the line on QEvent::Show.
 */
class FindPatternEventFilter : public QObject {
    Q_OBJECT
public:
    FindPatternEventFilter(QPlainTextEdit* textEdit);

signals:
    void si_enterPressed();
    void si_shiftEnterPressed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

class FindPatternWidget : public QWidget, private Ui_FindPatternForm {
    Q_OBJECT
public:
    enum MessageFlag {
        PatternAlphabetDoNotMatch,
        PatternsWithBadAlphabetInFile,
        PatternsWithBadRegionInFile,
        PleaseInputAtLeastOneSearchPatternTip,
        InvalidAnnotationName,
        UnsetAnnotationName_UseCustomPatternAndNoInputFasta,
        NoPatternToSearch,
        SearchRegionIncorrect,
        PatternWrongRegExp,
        SequenceIsTooBig
    };

    FindPatternWidget(AnnotatedDNAView*);
    int getTargetSequenceLength() const;

private slots:
    void sl_onAlgorithmChanged(int);
    void sl_onRegionOptionChanged(int);
    void sl_onSearchRegionIsChangedByUser();
    void sl_onSequenceTranslationChanged(int);
    void sl_onSearchPatternChanged();
    void sl_onMaxResultChanged(int);

    void sl_onFileSelectorClicked();
    void sl_onFileSelectorToggled(bool on);
    void sl_loadPatternTaskStateChanged();
    void sl_findPatternTaskStateChanged();

    /** Stops track the old sequence and attaches to the new one. */
    void sl_onActiveSequenceChanged();

    /** A sequence part was added, removed or replaced */
    void sl_onSequenceModified();

    /** Reacts to selection change in CurrentSelectionRegion mode: synchronizes line-edits and starts a new search if needed. */
    void sl_syncSearchRegionWithTrackedSelection();

    void sl_onAnnotationNameEdited();

    void sl_activateNewSearch(bool forcedSearch = true);
    void sl_toggleExtendedAlphabet();
    void sl_getAnnotationsButtonClicked();
    void sl_prevButtonClicked();
    void sl_nextButtonClicked();

    void sl_onEnterPressed();
    void sl_onShiftEnterPressed();
    void sl_usePatternNamesCbClicked();

private:
    void initLayout();
    void initAlgorithmLayout();
    void initStrandSelection();
    void initSeqTranslSelection();
    void initRegionSelection();
    void initResultsLimit();
    void initUseAmbiguousBasesContainer();
    void initMaxResultLenContainer();
    void updateLayout();
    void connectSlots();
    void showCurrentResult() const;
    bool isSearchPatternsDifferent(const QList<NamePattern>& newPatterns) const;
    void stopCurrentSearchTask();
    void setUpTabOrder() const;
    QList<NamePattern> updateNamePatterns();

    /** Stops progress movie and update current & total result labels. */
    void showCurrentResultAndStopProgress() const;

    void startProgressAnimation();
    void updatePatternSourceControlsUiState();

    /** Updates result label text based on the current currentResultIndex and findPatternResults size. */
    void updateResultLabelText() const;

    /**
     * Enables or disables the Search button depending on
     * the Pattern field value (it should be not empty and not too long)
     * and on the validity of the region.
     */
    void checkState();

    /**
     * Checks if currently selected region is valid for the given pattern.
     * Returns error message to display if the region is not correct or an empty string if the region is correct.
     */
    QString checkSearchRegion() const;

    /**
     * The "Match" spin is disabled if this is an amino acid sequence or
     * the search pattern is empty. Otherwise it is enabled.
     */
    void enableDisableMatchSpin();

    /** Toggles error message flag and updates additional error message. Does not trigger re-rendering of the error label. */
    void setMessageFlag(const MessageFlag& messageFlag, bool show, const QString& additionalMsg = QString());

    /** Updates visual error label state based on the curent error flags state. */
    void updateErrorLabelState();

    /** Returns HTML to be rendered by the error label. */
    QString buildErrorLabelHtml() const;

    /** Checks pattern alphabet and sets error message if needed. Returns false on error or true if no error found */
    bool verifyPatternAlphabet();
    bool checkAlphabet(const QString& pattern);

    void setRegionToWholeSequence();

    /** Returns search region parsed from the start/end edits. Returns empty region in case of error. */
    U2Region getSearchRegion() const;

    void initFindPatternTask(const QList<QPair<QString, QString>>& patterns);

    /** Checks if there are several patterns in textPattern which are separated by new line symbol,
    parse them out and returns with their names (if they're exist). */
    QList<QPair<QString, QString>> getPatternsFromTextPatternField(U2OpStatus& os) const;

    void setCorrectPatternsString();

    void updatePatternText(int previousAlgorithm);

    void updateAnnotationsWidget();

    /**
     * Starts tracking selection for the sequence in focus.
     * Stops tracking any old tracked sequence selection.
     * Does not subscribe (sets trackedSelection to nullptr) if there is no focused sequence.
     */
    void startTrackingFocusedSequenceSelection();

    /**
     * Stops tracking focused sequence selection and sets tracked selection to nullptr.
     */
    void stopTrackingFocusedSequenceSelection();

    /** Returns true if the all regions from the list can be found in the current search result. */
    bool isRegionListInSearchResults(const QVector<U2Region>& regionList) const;

    /** Returns true if current search mode is CurrentSelectionRegion. */
    bool isSearchInSelectionMode() const;

    AnnotatedDNAView* annotatedDnaView;
    CreateAnnotationWidgetController* createAnnotationController;
    bool annotationModelIsPrepared;

    bool isAminoSequenceSelected;
    bool regionIsCorrect;
    int selectedAlgorithm;
    QString patternString;
    QString patternRegExp;

    /** Keeps flags of visible messages and optional (may be empty) custom messages as values. */
    QMap<MessageFlag, QString> messageFlagMap;

    /** Widgets in the Algorithm group */
    QHBoxLayout* layoutMismatch;
    QVBoxLayout* layoutRegExpLen;

    QLabel* lblMatch;
    QSpinBox* spinMatch;
    QWidget* useAmbiguousBasesContainer;
    QCheckBox* useAmbiguousBasesBox;

    QWidget* useMaxResultLenContainer;
    QCheckBox* boxUseMaxResultLen;
    QSpinBox* boxMaxResultLen;

    QWidget* annotationsWidget;

    /**
     * Currently tracked selection.
     * Not null only if the current region mode is CurrentSelectionRegion and is equal to selection of the currently focused sequence.
     */
    DNASequenceSelection* trackedSelection;

    static const int DEFAULT_RESULTS_NUM_LIMIT;
    static const int DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    static const int REG_EXP_MIN_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_SINGLE_STEP;

    QList<SharedAnnotationData> findPatternResults;
    /** Index of the currently selected search result. A special value '-1' means that no result is selected. */
    int currentResultIndex;
    Task* searchTask;
    QString previousPatternString;
    int previousMaxResult;
    QStringList patternList;
    QStringList nameList;
    bool usePatternNames;
    QMovie* progressMovie;

    FindPatternWidgetSavableTab savableWidget;
};

}  // namespace U2
