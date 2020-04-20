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

#include <U2Gui/HelpButton.h>

#include "CommonImportOptionsDialog.h"
#include "ImportOptionsWidget.h"

namespace U2 {

CommonImportOptionsDialog::CommonImportOptionsDialog(const QString& baseFolder, const ImportToDatabaseOptions& options, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749086");
    init(baseFolder, options);
}

QString CommonImportOptionsDialog::getBaseFolder() const {
    return optionsWidget->getFolder();
}

ImportToDatabaseOptions CommonImportOptionsDialog::getOptions() const {
    return optionsWidget->getOptions();
}

void CommonImportOptionsDialog::init(const QString& baseFolder, const ImportToDatabaseOptions& options) {
    optionsWidget->init(baseFolder, options);
}

}   // namespace U2
