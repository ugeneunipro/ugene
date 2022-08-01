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

#ifndef _U2_BLAST_RUN_COMMON_DIALOG_H
#define _U2_BLAST_RUN_COMMON_DIALOG_H

#include <QDialog>

#include <U2Core/DNASequenceObject.h>

#include <U2Gui/CreateAnnotationWidgetController.h>

#include "BlastDBSelectorWidgetController.h"
#include "BlastTaskSettings.h"

#include <ui_BlastLocalSearchDialog.h>

namespace U2 {

class BlastRunCommonDialog : public QDialog, public Ui_BlastLocalSearchDialog {
    Q_OBJECT
public:
    BlastRunCommonDialog(QWidget* parent, const DNAAlphabet* alphabet);

    const BlastTaskSettings& getSettings() const;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* restoreButton;

protected slots:
    virtual void sl_runQuery() = 0;
    virtual void sl_lineEditChanged() = 0;

    void sl_onProgramNameChange(int i);
    void sl_onMatchScoresChanged(int i);
    void sl_onMatrixChanged(int i);
    void sl_megablastChecked();
    void sl_restoreDefault();
    void sl_onCompStatsChanged();

protected:
    /**
     * Keeps only programs compatible with the give sequence alphabet.
     * If the alphabet 'nullptr' keeps all options.
     */
    void updateAvailableProgramsList(const DNAAlphabet* alphabet);

    bool checkSelectedToolPath() const;

    void getSettings(BlastTaskSettings& settingsSnapshot);
    void enableStrandBox(bool enable);

    BlastTaskSettings settings;
    bool needRestoreDefault = false;
    CreateAnnotationWidgetController* ca_c = nullptr;
    BlastDBSelectorWidgetController* dbSelector = nullptr;

private:
    void setupCompositionBasedStatistics();

    QStringList activeToolList;
};
}  // namespace U2
#endif  // _U2_BLAST_RUN_COMMON_DIALOG_H
