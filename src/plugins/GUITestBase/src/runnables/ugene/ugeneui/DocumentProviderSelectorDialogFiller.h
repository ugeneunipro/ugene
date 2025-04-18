/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

class DocumentProviderSelectorDialogFiller : public Filler {
public:
    enum OpenWith { AlignmentEditor,
                    AssemblyBrowser };
    DocumentProviderSelectorDialogFiller(OpenWith _openWith)
        : Filler("DocumentProviderSelectorDialog"),
          openWith(_openWith) {
    }
    void commonScenario() override;

private:
    OpenWith openWith;
    static const QMap<OpenWith, QString> openWithMap;
    static QMap<OpenWith, QString> initMap();
};
}  // namespace U2
