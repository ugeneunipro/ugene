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

#include "InsertEnzymeDialog.h"

namespace U2 {

InsertEnzymeDialog::InsertEnzymeDialog(const EditSequencDialogConfig& cfg, QWidget* p)
 : EditSequenceDialogVirtualController(cfg, p) {

    setWindowTitle(tr("Insert Restriction Site"));

    enzymesSelectorWidget = new EnzymesSelectorWidget(nullptr, this);
    addInputDataWidgetToLayout(enzymesSelectorWidget);
}

DNASequence InsertEnzymeDialog::getNewSequence() const {
    return DNASequence();
}

}  // namespace U2