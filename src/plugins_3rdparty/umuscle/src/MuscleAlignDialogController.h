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

#ifndef _U2_MUSCLE_ALIGN_DIALOG_CONTROLLER_H_
#define _U2_MUSCLE_ALIGN_DIALOG_CONTROLLER_H_

#include <QDialog>

#include <U2Core/GAutoDeleteList.h>
#include <ui_MuscleAlignDialog.h>
#include <U2Gui/DialogUtils.h>

#include "MuscleTask.h"

namespace U2 {

class SaveDocumentController;

class MuscleAlignPreset {
public:
    virtual ~MuscleAlignPreset(){}
    virtual void apply(MuscleTaskSettings& ts) const = 0;

    QString name;
    QString desc;
};

class DefaultModePreset : public MuscleAlignPreset {
public:
    DefaultModePreset();
    virtual void apply(MuscleTaskSettings& ts) const {
        //no options
        ts.reset();
    }
};

class LargeModePreset : public MuscleAlignPreset {
public:
    LargeModePreset();

    virtual void apply(MuscleTaskSettings& ts) const {
        ts.reset();
        ts.maxIterations = 2;
    }
};

class RefineModePreset : public MuscleAlignPreset {
public:
    RefineModePreset();
    virtual void apply(MuscleTaskSettings& ts) const {
        ts.reset();
        ts.op = MuscleTaskOp_Refine;
    }
};


class MuscleAlignDialogController : public QDialog, public Ui_MuscleAlignmentDialog {
    Q_OBJECT

public:
    MuscleAlignDialogController(QWidget* w, const MultipleSequenceAlignment& ma, MuscleTaskSettings& settings);
    bool translateToAmino();
    QString getTranslationId();    
public slots:
    void accept();

private slots:
    void sl_onPresetChanged(int current);
    
private:
    void initPresets();

    MultipleSequenceAlignment                          ma;
    MuscleTaskSettings&                 settings;
    GAutoDeleteList<MuscleAlignPreset>  presets;

};

class MuscleAlignWithExtFileSpecifyDialogController : public QDialog, public Ui_MuscleAlignmentDialog {
    Q_OBJECT

public:
    MuscleAlignWithExtFileSpecifyDialogController(QWidget* w, MuscleTaskSettings& settings);
    
public slots:
    void accept();

private slots:
    void sl_onPresetChanged(int current);
    void sl_inputPathButtonClicked();

private:
    void initPresets();
    void initSaveController();

    MuscleTaskSettings&                 settings;
    GAutoDeleteList<MuscleAlignPreset>  presets;
    SaveDocumentController *            saveController;
};


}//namespace
#endif
