/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <QDialog>
#include <QPointer>

namespace U2 {

class EnzymesSelectorWidget;

/**
 * @brief This class describes a base dialog for restriction enzymes.
 * It can draw restriction enzymet tree view (and corresponding widgets e.g. filters)
 * and dialog button box (OK, Cancel and Help).
 * This class is pure virtual.
 */
class FindEnzymesDialogBase : public QDialog {
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param parent parent widget. Could be nullptr.
     */
    FindEnzymesDialogBase(QWidget* parent);

    /**
     * @brief This function is called when a user clicked OK in the dialog.
     */
    void accept() override;

protected:
    virtual bool acceptProtected() = 0;

    void initTitleAndLayout();
    void initEnzymesSelectorWidget();
    void initDialogButtonBox();

    virtual void saveSettings();

    EnzymesSelectorWidget* enzSel = nullptr;
};


}  // namespace U2
