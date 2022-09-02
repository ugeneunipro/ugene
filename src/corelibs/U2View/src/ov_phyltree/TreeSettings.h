/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_TREE_SETTINGS_H_
#define _U2_TREE_SETTINGS_H_

#include <QDialog>
#include <QFontInfo>
#include <QMap>
#include <QVariant>

namespace U2 {

enum TreeType {
    DEFAULT,
    PHYLOGRAM,
    CLADOGRAM
};

enum TreeLayout {
    RECTANGULAR_LAYOUT,
    CIRCULAR_LAYOUT,
    UNROOTED_LAYOUT
};

enum TreeViewOption {
    BRANCHES_TRANSFORMATION_TYPE,
    TREE_LAYOUT,
    WIDTH_COEF,
    HEIGHT_COEF,

    LABEL_COLOR,
    LABEL_FONT_TYPE,
    LABEL_FONT_SIZE,
    LABEL_FONT_BOLD,
    LABEL_FONT_ITALIC,
    LABEL_FONT_UNDERLINE,

    BRANCH_COLOR,
    BRANCH_THICKNESS,

    NODE_COLOR,
    NODE_RADIUS,

    SHOW_LABELS,
    SHOW_DISTANCES,
    SHOW_NODE_LABELS,
    ALIGN_LABELS,

    SCALEBAR_RANGE,
    SCALEBAR_FONT_SIZE,
    SCALEBAR_LINE_WIDTH,

    OPTION_ENUM_END
};

typedef QMap<TreeViewOption, QVariant> OptionsMap;
}  // namespace U2
#endif
