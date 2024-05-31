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

#include <QMessageBox>

#include "utils/GTUtilsDialog.h"

namespace HI {

class HI_EXPORT MessageBoxDialogFiller : public Filler {
public:
    MessageBoxDialogFiller(QMessageBox::StandardButton b, const QString& message = "", const QString& objectName = "");
    MessageBoxDialogFiller(const QString& buttonText, const QString& message = "");

    void commonScenario() override;

protected:
    QMessageBox::StandardButton b;
    QString buttonText;
    QString message;
};

class HI_EXPORT AppCloseMessageBoxDialogFiller : public Filler {
public:
    AppCloseMessageBoxDialogFiller();
    void commonScenario() override;
};

class HI_EXPORT MessageBoxNoToAllOrNo : public Filler {
public:
    MessageBoxNoToAllOrNo();
    void commonScenario() override;
};

class HI_EXPORT MessageBoxOpenAnotherProject : public Filler {
public:
    MessageBoxOpenAnotherProject();
    void commonScenario() override;
};

class HI_EXPORT InputIntFiller : public Filler {
public:
    InputIntFiller(int value);
    void commonScenario() override;

private:
    int value;
};

}  // namespace HI
