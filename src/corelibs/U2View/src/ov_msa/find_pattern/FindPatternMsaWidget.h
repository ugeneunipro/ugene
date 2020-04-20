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

#ifndef _U2_FIND_PATTERN_MSA_WIDGET_H_
#define _U2_FIND_PATTERN_MSA_WIDGET_H_

#include <U2Core/U2Region.h>

#include "../MSAEditor.h"
#include "../MSAEditorSequenceArea.h"
#include "FindPatternMsaWidgetSavableTab.h"
#include "ov_msa/view_rendering/MaEditorSelection.h"
#include "ov_sequence/find_pattern/FindPatternTask.h"
#include "ov_sequence/find_pattern/FindPatternWidget.h"
#include "ui_FindPatternMsaForm.h"

namespace U2 {

class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class CreateAnnotationWidgetController;
class DNASequenceSelection;
class Task;
class U2OpStatus;

class FindPatternMsaWidget : public QWidget, private Ui_FindPatternMsaForm
{
    Q_OBJECT
public:
    FindPatternMsaWidget(MSAEditor* msaEditor);

    int getTargetMsaLength() const;

private slots:
    void sl_onAlgorithmChanged(int);
    void sl_onRegionOptionChanged(int);
    void sl_onRegionValueEdited();
    void sl_onSearchPatternChanged();
    void sl_onMaxResultChanged(int);
    void sl_findPatternTaskStateChanged();

    /** A sequence part was added, removed or replaced */
    void sl_onMsaModified();

    void sl_onSelectedRegionChanged(const MaEditorSelection& current, const MaEditorSelection& prev);
    void sl_activateNewSearch(bool forcedSearch = true, bool activatedByOutsideChanges = false);
    void sl_toggleExtendedAlphabet();
    void sl_prevButtonClicked();
    void sl_nextButtonClicked();

    void sl_onEnterPressed();
    void sl_onShiftEnterPressed();
    void sl_collapseModelChanged();

private:
    class ResultIterator {
    public:
        ResultIterator();
        ResultIterator(const QMap<int, QList<U2Region> >& results, MSAEditor* msaEditor);

        U2Region currentResult() const;
        int getGlobalPos() const;
        int getTotalCount() const;
        int getMsaRow() const;
        void goBegin();
        void goEnd();
        void goNextResult();
        void goPrevResult();
        void collapseModelChanged();

    private:
        void initSortedResults();

        //visible index, msa rowid, regions for current msa index
        QMap<int, QList<U2Region> > searchResults;
        QMap<int, QMap<int, QList<U2Region> > > sortedResults;
        MSAEditor* msaEditor;

        int totalResultsCounter;
        int globalPos; //1-based position

        QMap<int, QMap<int, QList<U2Region> > >::const_iterator sortedVisibleRowsIt;
        QMap<int, QList<U2Region> >::const_iterator msaRowsIt;
        QList<U2Region>::const_iterator regionsIt;
    };

    void initLayout();
    void initAlgorithmLayout();
    void initRegionSelection();
    void initResultsLimit();
    void initMaxResultLenContainer();
    void updateLayout();
    void connectSlots();
    int  getMaxError(const QString& pattern) const;
    void showCurrentResult() const;
    bool isSearchPatternsDifferent(const QList<NamePattern>& newPatterns) const;
    void stopCurrentSearchTask();
    void correctSearchInCombo();
    void setUpTabOrder() const;
    QList<NamePattern> updateNamePatterns();
    void showCurrentResultAndStopProgress(const int current, const int total);
    void startProgressAnimation();

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

    void setCorrectPatternsString();
    void setRegionToWholeSequence();

    U2Region getCompleteSearchRegion(bool& regionIsCorrect, qint64 maxLen) const;

    void initFindPatternTask(const QList< QPair<QString, QString> >& patterns);

    /** Checks if there are several patterns in textPattern which are separated by new line symbol,
    parse them out and returns with their names (if they're exist). */
    QList <QPair<QString, QString> > getPatternsFromTextPatternField(U2OpStatus& os) const;

    /** Checks whether the input string is uppercased or not. */

    void changeColorOfMessageText(const QString &colorName);
    QString currentColorOfMessageText() const;

    void updatePatternText(int previousAlgorithm);

    void validateCheckBoxSize(QCheckBox* checkBox, int requiredWidth);

    MSAEditor* msaEditor;
    bool isAmino;
    bool regionIsCorrect;
    int selectedAlgorithm;
    QString patternString;
    QString patternRegExp;

    QList<MessageFlag> messageFlags;

    /** Widgets in the Algorithm group */
    QHBoxLayout* layoutMismatch;
    QVBoxLayout* layoutRegExpLen;
    QHBoxLayout* layoutRegExpInfo;

    QLabel* lblMatch;
    QSpinBox* spinMatch;

    QWidget *useMaxResultLenContainer;
    QCheckBox* boxUseMaxResultLen;
    QSpinBox* boxMaxResultLen;

    static const int DEFAULT_RESULTS_NUM_LIMIT;
    static const int DEFAULT_REGEXP_RESULT_LENGTH_LIMIT;

    static const QString NEW_LINE_SYMBOL;
    static const QString STYLESHEET_COLOR_DEFINITION;
    static const QString STYLESHEET_DEFINITIONS_SEPARATOR;

    static const int REG_EXP_MIN_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_LEN;
    static const int REG_EXP_MAX_RESULT_SINGLE_STEP;

    QMap<int, QList<U2Region> > findPatternResults;
    ResultIterator resultIterator;
    Task *searchTask;
    QString previousPatternString;
    int previousMaxResult;
    QStringList patternList;
    QStringList nameList;
    QMovie *progressMovie;
    bool setSelectionToFirstValuebleResult;

    FindPatternMsaWidgetSavableTab savableWidget;
};

} // namespace U2

#endif // _U2_FIND_PATTERN_WIDGET_H_
