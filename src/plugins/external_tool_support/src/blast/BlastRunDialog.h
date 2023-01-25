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

#ifndef _U2_BLAST_PLUS_SUPPORT_RUN_DIALOG_H
#define _U2_BLAST_PLUS_SUPPORT_RUN_DIALOG_H

#include <QDialog>

#include <U2Core/DNASequenceObject.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>

#include "BlastCommonTask.h"
#include "BlastRunCommonDialog.h"

namespace U2 {

class ADVSequenceObjectContext;
class RegionSelector;

class BlastRunDialog : public BlastRunCommonDialog {
    Q_OBJECT
public:
    BlastRunDialog(ADVSequenceObjectContext* seqCtx, QWidget* parent);

    U2Region getSelectedRegion() const;

protected slots:
    void sl_runQuery() override;
    void sl_lineEditChanged() override;

private:
    U2SequenceObject* sequenceObject = nullptr;
    ADVSequenceObjectContext* seqCtx = nullptr;
    RegionSelector* regionSelector = nullptr;
};

class BlastWithExtFileRunDialog : public BlastRunCommonDialog {
    Q_OBJECT
public:
    BlastWithExtFileRunDialog(QWidget* parent);
    ~BlastWithExtFileRunDialog();
    const QList<BlastTaskSettings>& getSettingsList() const;

protected slots:
    void sl_runQuery() override;
    void sl_lineEditChanged() override;

private slots:
    void sl_cancel();

    void sl_inputFileLineEditChanged(const QString& str);
    void sl_inputFileOpened();

private:
    void tryApplyDoc(Document* doc);
    void onFormatError();
    void loadDoc(const QString& url);

    FileLineEdit* inputFileLineEdit = nullptr;
    bool wasNoOpenProject = false;

    QList<BlastTaskSettings> settingsList;
    QList<GObjectReference> sequencesRefList;
    bool hasValidInput = false;
};
}  // namespace U2
#endif  // _U2_BLAST_PLUS_SUPPORT_RUN_DIALOG_H
