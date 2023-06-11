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

#include <GTGlobals.h>

namespace U2 {

class GTUtilsOptionPanelPhyTree {
public:
    /** Opens options panel in tree viewer (today we have only 1 tab . */
    static QWidget* openTab();

    /** Returns tree-options widget. */
    static QWidget* getOptionsPanelWidget();

    /** Returns currently shown font size. */
    static int getFontSize();

    /** Sets new font size. */
    static void setFontSize(int fontSize);

    /** Changes tree layout to the given value. */
    static void changeTreeLayout(const QString& layoutName);

    /** Checks current tree layout. */
    static void checkBranchDepthScaleMode(const QString& mode);

    /** Changes tree type to the given value. */
    static void changeBranchDepthScaleMode(const QString& mode);
};

}  // namespace U2
