/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_RUNNABLES_MESSAGE_BOX_FILLER_H_
#define _HI_GT_RUNNABLES_MESSAGE_BOX_FILLER_H_

#include "utils/GTUtilsDialog.h"
#include <QMessageBox>

namespace HI {

class HI_EXPORT MessageBoxDialogFiller : public Filler {
public:
    MessageBoxDialogFiller(GUITestOpStatus &os, QMessageBox::StandardButton b, const QString &message = "", const QString &objectName = "");
    MessageBoxDialogFiller(GUITestOpStatus &os, const QString &buttonText, const QString &message = "");

    virtual void commonScenario();

protected:
    QMessageBox::StandardButton b;
    QString buttonText;
    QString message;
};

class HI_EXPORT AppCloseMessageBoxDialogFiller : public Filler {
public:
    AppCloseMessageBoxDialogFiller(GUITestOpStatus &os);
    void commonScenario();
};

class HI_EXPORT MessageBoxNoToAllOrNo : public Filler {
public:
    MessageBoxNoToAllOrNo(GUITestOpStatus &os);
    void commonScenario();
};

class HI_EXPORT MessageBoxOpenAnotherProject : public Filler {
public:
    MessageBoxOpenAnotherProject(GUITestOpStatus &os);
    void commonScenario();
};

class HI_EXPORT InputIntFiller : public Filler {
public:
    InputIntFiller(GUITestOpStatus &os, int value);
    void commonScenario();
private:
    int value;
};

}   // namespace

#endif
