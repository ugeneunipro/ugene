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

#ifndef _U2_STRUCTURAL_ALIGNMENT_DIALOG_H_
#define _U2_STRUCTURAL_ALIGNMENT_DIALOG_H_

#include "ui_StructuralAlignmentDialog.h"

namespace U2 {

class StructuralAlignmentTask;
class BioStruct3DObject;
class BioStruct3DSubsetEditor;

class StructuralAlignmentDialog : public QDialog, public Ui_StructuralAlignmentDialog
{
    Q_OBJECT

public:
    StructuralAlignmentDialog(const BioStruct3DObject *fixedRef = 0, int fixedRefModel = -1, QWidget *parent = 0);

    /** Check if algorithm available and ::exec() */
    int execIfAlgorithmAvailable();

    /** @returns set up StructuralAlignmentTask */
    StructuralAlignmentTask* getTask() { return task; }

public slots:
    virtual void accept();

private:
    StructuralAlignmentTask *task;
    BioStruct3DSubsetEditor *ref, *mob;

};

}   // namespace U2

#endif  // #ifndef _U2_STRUCTURAL_ALIGNMENT_DIALOG_H_
