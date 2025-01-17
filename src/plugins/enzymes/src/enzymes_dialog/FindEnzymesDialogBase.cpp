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

#include "FindEnzymesDialogBase.h"

#include <QDialogButtonBox>
#include <QMessageBox>

#include <U2Core/L10n.h>

#include <U2Gui/HelpButton.h>

#include "EnzymesSelectorWidget.h"

namespace U2 {


FindEnzymesDialogBase::FindEnzymesDialogBase(QWidget* parent)
    : QDialog(parent) {}

void FindEnzymesDialogBase::accept() {
    CHECK(acceptProtected(), );

    QDialog::accept();
}

void FindEnzymesDialogBase::initTitleAndLayout() {
    setObjectName("FindEnzymesDialog");
    setWindowTitle(tr("Find Restriction Sites"));

    auto layout = new QVBoxLayout(this);
    layout->setSpacing(0);
}

void FindEnzymesDialogBase::initEnzymesSelectorWidget() {
    enzSel = new EnzymesSelectorWidget(this);
    enzSel->setObjectName("enzymesSelectorWidget");
    layout()->addWidget(enzSel);
}

void FindEnzymesDialogBase::saveSettings() {
    enzSel->saveSettings();
}

void FindEnzymesDialogBase::initDialogButtonBox() {
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Ok);
    new HelpButton(this, buttonBox, "65930747");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    layout()->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &FindEnzymesDialogBase::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &FindEnzymesDialogBase::reject);
}

}  // namespace U2
