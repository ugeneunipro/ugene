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

#ifndef _U2_HMMER_BUILD_DIALOG_H_
#define _U2_HMMER_BUILD_DIALOG_H_

#include <QDialog>

#include <U2Core/MultipleSequenceAlignment.h>

#include "HmmerBuildTask.h"
#include "ui_HmmerBuildDialog.h"

namespace U2 {

class SaveDocumentController;

class UHMM3BuildDialogModel {
public:
    UHMM3BuildDialogModel();

    HmmerBuildSettings buildSettings;
    
    /* one of this is used */
    QString                 inputFile;
    MultipleSequenceAlignment alignment;
    bool                    alignmentUsing;
};

class HmmerBuildDialog : public QDialog, public Ui_HmmerBuildDialog {
    Q_OBJECT
public:
    HmmerBuildDialog(const MultipleSequenceAlignment &ma, QWidget *parent = NULL);
    
    static const QString MA_FILES_DIR_ID;
    static const QString HMM_FILES_DIR_ID;

private slots:
    void sl_maOpenFileButtonClicked();
    void sl_buildButtonClicked();
    void sl_cancelButtonClicked();
    void sl_fastMCRadioButtonChanged(bool checked);
    void sl_wblosumRSWRadioButtonChanged(bool checked);
    void sl_eentESWRadioButtonChanged(bool checked);
    void sl_eclustESWRadioButtonChanged(bool changed);
    void sl_esetESWRadioButtonChanged(bool checked);

private:
    void setModelValues();
    void getModelValues();
    QString checkModel();       // returns error or empty string
    void setSignalsAndSlots();
    void initialize();
    void initSaveController();

    UHMM3BuildDialogModel   model;
    SaveDocumentController *saveController;
};

}   // namespace U2

#endif // _U2_HMMER_BUILD_DIALOG_H_
