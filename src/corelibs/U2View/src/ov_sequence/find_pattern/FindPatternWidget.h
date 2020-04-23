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

#ifndef _U2_FIND_PATTERN_WIDGET_H_
#define _U2_FIND_PATTERN_WIDGET_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/U2Region.h>

#include "FindPatternWidgetSavableTab.h"

#include "FindPatternTask.h"
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

enum MessageFlag {
    PatternIsTooLong,
    PatternAlphabetDoNotMatch,
    PatternsWithBadAlphabetInFile,
    PatternsWithBadRegionInFile,
    UseMultiplePatternsTip,
    AnnotationNotValidName,
    AnnotationNotValidFastaParsedName,
    NoPatternToSearch,
    SearchRegionIncorrect,
    PatternWrongRegExp,
    SequenceIsTooBig
};


/**
 * A workaround to listen to enter in the pattern field and
 * make a correct (almost) tab order.
 */
class FindPatternEventFilter : public QObject
{
    Q_OBJECT
public:
    FindPatternEventFilter(QObject* parent);

signals:
    void si_enterPressed();
    void si_shiftEnterPressed();

protected:
    bool eventFilter(QObject* obj, QEvent *event);
};

class FindPatternWidget : public QWidget, private Ui_FindPatternForm
{
    Q_OBJECT
public:
    FindPatternWidget(AnnotatedDNAView*);
    int getTargetSequenceLength() const;

private slots:
    void sl_onAlgorithmChanged(int);
    void sl_onRegionOptionChanged(int);
    void sl_onRegionValueEdited();
    void sl_onSequenceTranslationChanged(int);
    void sl_onSearchPatternChanged();
    void sl_onMaxResultChanged(int);

    void sl_onFileSelectorClicked();
    void sl_onFileSelectorToggled(bool on);
    void sl_loadPatternTaskStateChanged();
    void sl_findPatternTaskStateChanged();

    /** Another sequence has been selected */
    void sl_onFocusChanged(ADVSequenceWidget*, ADVSequenceWidget*);

    /** A sequence part was added, removed or replaced */
    void sl_onSequenceModified();

    void sl_onSelectedRegionChanged();

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
    int getMaxError(const QString& pattern) const;
    void showCurrentResult() const;
    bool isSearchPatternsDifferent(const QList<NamePattern> &newPatterns) const;
    void stopCurrentSearchTask();
    void correctSearchInCombo();
    void setUpTabOrder() const;
    QList<NamePattern> updateNamePatterns();
    void showCurrentResultAndStopProgress(const int current, const int total);
    void startProgressAnimation();
    void updatePatternSourceControlsUiState();
    /**
     * Enables or disables the Search button depending on
     * the Pattern field value (it should be not empty and not too long)
     * and on the validity of the region.
     */
    void checkState();
    bool checkPatternRegion(const QString& pattern);

    /**
     * The "Match" spin is disabled if this is an amino acid sequence or
     * the search pattern is empty. Otherwise it is enabled.
     */
    void enableDisableMatchSpin();

    /** Allows showing of several error messages. */
    void showHideMessage(bool show, MessageFlag messageFlag, const QString& additionalMsg = QString());

    /** Checks pattern alphabet and sets error message if needed. Returns false on error or true if no error found */
    bool verifyPatternAlphabet();
    bool checkAlphabet(const QString& pattern);
    void showTooLongSequenceError();

    void setRegionToWholeSequence();

    U2Region getCompleteSearchRegion(bool& isRegionCorrect, qint64 maxLen) const;

    void initFindPatternTask(const QList< QPair<QString, QString> >& patterns);

    /** Checks if there are several patterns in textPattern which are separated by new line symbol,
    parse them out and returns with their names (if they're exist). */
    QList <QPair<QString, QString> > getPatternsFromTextPatternField(U2OpStatus &os) const;

    void setCorrectPatternsString();

    void updatePatternText(int previousAlgorithm);

    void updateAnnotationsWidget();

    AnnotatedDNAView* annotatedDnaView;
    CreateAnnotationWidgetController* annotController;
    bool annotationModelIsPrepared;

    bool isAminoSequenceSelected;
    bool regionIsCorrect;
    int selectedAlgorithm;
    QString patternString;
    QString patternRegExp;

    QList<MessageFlag> messageFlags;

    /** Widgets in the Algorithm group */
    QHBoxLayout* layoutMismatch;
    QVBoxLayout* layoutRegExpLen;

    QLabel* lblMatch;
    QSpinBox* spinMatch;
    QWidget *useAmbiguousBasesContainer;
    QCheckBox* useAmbiguousBasesBox;

    QWidget *useMaxResultLenContainer;
    QCheckBox* boxUseMaxResultLen;
    QSpinBox* boxMaxResultLen;

    QWidget* annotationsWidget;

    DNASequenceSelection *currentSelection;

    static const int DEFAULT_RESULTS_NUM_LIMIT;
    static const int DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    static const int REG_EXP_MIN_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_SINGLE_STEP;

    QList<SharedAnnotationData> findPatternResults;
    int iterPos;
    Task *searchTask;
    QString previousPatternString;
    int previousMaxResult;
    QStringList patternList;
    QStringList nameList;
    bool usePatternNames;
    QMovie *progressMovie;

    FindPatternWidgetSavableTab savableWidget;
};

} // namespace U2

#endif // _U2_FIND_PATTERN_WIDGET_H_
