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

#include <QTreeWidget>

#include <U2View/MSAEditorSequenceArea.h>

#include "GTGlobals.h"

namespace U2 {

class GTUtilsQueryDesigner {
public:
    static void openQueryDesigner(HI::GUITestOpStatus& os);

    static QTreeWidgetItem* findAlgorithm(HI::GUITestOpStatus& os, const QString& itemName);
    static void addAlgorithm(HI::GUITestOpStatus& os, const QString& algName);
    static QPoint getItemCenter(HI::GUITestOpStatus& os, const QString& itemName);
    static QRect getItemRect(HI::GUITestOpStatus& os, const QString& itemName);

    static int getItemLeft(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemRight(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemTop(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemBottom(HI::GUITestOpStatus& os, const QString& itemName);
};

}  // namespace U2
