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

#include <QColorDialog>
#include <QProxyStyle>
#include <QStyleFactory>

#include <U2Gui/HelpButton.h>

#include "TextSettingsDialog.h"

namespace U2 {

TextSettingsDialog::TextSettingsDialog(QWidget *parent, const OptionsMap& settings)
: BaseSettingsDialog(parent) {

    setupUi(this);
    new HelpButton(this, buttonBox, "24748925");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    curColor = qvariant_cast<QColor>(settings[LABEL_COLOR]);

    QStyle *buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(colorButton);
    colorButton->setStyle(buttonStyle);

    updateColorButton();
    QFont curFont = qvariant_cast<QFont>(settings[LABEL_FONT]);
    fontComboBox->setCurrentFont(curFont);
    sizeSpinBox->setValue(curFont.pointSize());

    boldToolButton->setChecked(curFont.bold());
    italicToolButton->setChecked(curFont.italic());
    underlineToolButton->setChecked(curFont.underline());
    overlineToolButton->setChecked(curFont.overline());

    overlineToolButton->setVisible(false);

    connect(colorButton, SIGNAL(clicked()), SLOT(sl_colorButton()));
}

void TextSettingsDialog::updateColorButton() {
    QPalette palette = colorButton->palette();
    palette.setColor(colorButton->backgroundRole(), curColor);
    colorButton->setPalette(palette);
}

void TextSettingsDialog::sl_colorButton() {
    curColor = QColorDialog::getColor(curColor, this);
    if (curColor.isValid()) {
        changedSettings[LABEL_COLOR] = curColor;
        updateColorButton();
    }
}

void TextSettingsDialog::accept() {
    QFont curFont = fontComboBox->currentFont();
    curFont.setPointSize(sizeSpinBox->value());

    curFont.setBold(boldToolButton->isChecked());
    curFont.setItalic(italicToolButton->isChecked());
    curFont.setUnderline(underlineToolButton->isChecked());
    curFont.setOverline(overlineToolButton->isChecked());

    changedSettings[LABEL_FONT] = curFont;

    QDialog::accept();
}

} //namespace
