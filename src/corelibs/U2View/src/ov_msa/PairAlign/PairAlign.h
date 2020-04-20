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

#ifndef _U2_PAIR_ALIGN_H
#define _U2_PAIR_ALIGN_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QToolButton>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>

#include <U2Gui/SuggestCompleter.h>

#include "ui_PairwiseAlignmentOptionsPanelWidget.h"
#include "../MsaOpSavableTab.h"
#include "../SequenceSelectorWidgetController.h"

namespace U2 {

class AlignmentAlgorithm;
class AlignmentAlgorithmMainWidget;
class MaModificationInfo;
class MSADistanceAlgorithm;
class MSAEditor;
class PairwiseAlignmentTaskSettings;
class SaveDocumentController;
class ShowHideSubgroupWidget;

class U2VIEW_EXPORT PairAlign : public QWidget, public Ui_PairwiseAlignmentOptionsPanelWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PairAlign)

public:
    PairAlign(MSAEditor* _msa);

private slots:
    void sl_algorithmSelected(const QString& algorithmName);
    void sl_subwidgetStateChanged(const QString& id);
    void sl_inNewWindowCheckBoxChangeState(bool newState);
    void sl_alignButtonPressed();
    void sl_outputFileChanged();

    void sl_distanceCalculated();
    void sl_alignComplete();
    void sl_selectorTextChanged();
    void sl_checkState();
    void sl_alignmentChanged();

private:
    void initLayout();
    void initParameters();
    void connectSignals();
    void checkState();
    void updatePercentOfSimilarity();
    bool checkSequenceNames();
    AlignmentAlgorithm* getAlgorithmById(const QString& algorithmId);
    /* Updates label with warning message. Types: 0 -> bad alphabet, 1 -> same sequences in selectors. */
    void updateWarningMessage(int warningMessageType);
    void initSaveController();
    /* Checks if the given sequence is in the alignment */
    bool isValidSequenceId(qint64 sequenceId) const;
    static QString getDefaultFilePath();

    MSAEditor* msa;
    PairwiseAlignmentWidgetsSettings * pairwiseAlignmentWidgetsSettings;
    MSADistanceAlgorithm* distanceCalcTask;

    AlignmentAlgorithmMainWidget* settingsWidget;       //created by factories on demand

    ShowHideSubgroupWidget* showHideSequenceWidget;
    ShowHideSubgroupWidget* showHideSettingsWidget;
    ShowHideSubgroupWidget* showHideOutputWidget;

    SequenceSelectorWidgetController *firstSeqSelectorWC;
    SequenceSelectorWidgetController *secondSeqSelectorWC;

    SaveDocumentController *saveController;
    MsaOpSavableTab savableTab;

    bool showSequenceWidget;
    bool showAlgorithmWidget;
    bool showOutputWidget;

    bool firstSequenceSelectionOn;
    bool secondSequenceSelectionOn;

    bool sequencesChanged;
    bool sequenceNamesIsOk;
    bool alphabetIsOk;      //no RNA alphabet allowed
    bool canDoAlign;
};

} // namespace

#endif // PAIR_ALIGN_H
