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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class InsertRestrictionSiteDialogFiller : public Filler {
public:
    enum DocumentFormat { FASTA,
                          Genbank };
    enum RegionResolvingMode { Resize,
                               Remove,
                               SplitJoin,
                               SplitSeparate };

    struct InsertRestrictionSiteDialogFillerSettings {
        QString enzymeName;
        bool showUnknownSuppliers = false;
        QString enzymeInfo;
        RegionResolvingMode regionResolvingMode = RegionResolvingMode::Resize;
        qint64 insertPosition = 0;
        QString documentLocation;
        DocumentFormat format = FASTA;
        bool saveToNewFile = false;
        bool mergeAnnotations = false;
        GTGlobals::UseMethod useMethod = GTGlobals::UseMouse;
        bool wrongInput = false;
        bool recalculateQuals = false;
        bool cancelIfWrongInput = false;
    };

    InsertRestrictionSiteDialogFiller(const InsertRestrictionSiteDialogFillerSettings& settings);
    void commonScenario();

private:
    InsertRestrictionSiteDialogFillerSettings settings;

    QMap<DocumentFormat, QString> comboBoxItems;
};


}  // namespace U2
