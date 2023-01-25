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

#include "TextSettingsDialog.h"

#include <QColorDialog>
#include <QProxyStyle>
#include <QStyleFactory>

#include <U2Gui/HelpButton.h>

#include "../ov_phyltree/TreeViewerUtils.h"

namespace U2 {

TextSettingsDialog::TextSettingsDialog(QWidget* parent, const OptionsMap& settings)
    : BaseSettingsDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929735");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    curColor = qvariant_cast<QColor>(settings[LABEL_COLOR]);

    QStyle* buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(colorButton);
    colorButton->setStyle(buttonStyle);

    updateColorButton();
    QFont font = TreeViewerUtils::getFontFromSettings(settings);
    fontComboBox->setCurrentFont(font.family());
    sizeSpinBox->setValue(font.pointSize());
    boldToolButton->setChecked(font.bold());
    italicToolButton->setChecked(font.italic());
    underlineToolButton->setChecked(font.underline());
    overlineToolButton->setChecked(font.overline());

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
        updatedSettings[LABEL_COLOR] = curColor;
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

    updatedSettings[LABEL_FONT_FAMILY] = curFont.family();
    updatedSettings[LABEL_FONT_SIZE] = curFont.pointSize();
    updatedSettings[LABEL_FONT_BOLD] = curFont.bold();
    updatedSettings[LABEL_FONT_ITALIC] = curFont.italic();
    updatedSettings[LABEL_FONT_UNDERLINE] = curFont.underline();

    QDialog::accept();
}

}  // namespace U2
