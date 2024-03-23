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

#include <U2Formats/AceImporter.h>

#include "ImportWidget.h"
#include "ui_AceImportWidget.h"

namespace U2 {

class SaveDocumentController;

class AceImportWidget : public ImportWidget, public Ui_AceImportWidget {
    Q_OBJECT
public:
    AceImportWidget(const GUrl& url);

    QVariantMap getSettings() const override;

private:
    void initSaveController(const GUrl& url);

    static const QString EXTENSION;
};

}  // namespace U2
