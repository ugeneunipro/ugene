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

enum TreeLayoutType {
    RECTANGULAR_LAYOUT,
    CIRCULAR_LAYOUT,
    UNROOTED_LAYOUT
};

enum TreeViewOption {
    /** How branch depth (length) is computed: Default (fit-depth), Cladogram (equal length) or Phylogram (length is proportional to distance). */
    BRANCH_DEPTH_SCALE_MODE,

    TREE_LAYOUT_TYPE,

    /*
     * Affects breadth of the tree:
     * 100% is default,
     * values < 100% make breadth smaller (a narrower tree),
     * values > 100% make breadth larger (a wider tree)
     *
     * Available only for in the rectangular layout.
     */
    BREADTH_SCALE_ADJUSTMENT_PERCENT,

    /** Curvature of branches in Rectangular layout mode. 0 -> no curvature. 100 -> maximum possible curvature. */
    BRANCH_CURVATURE,

    LABEL_COLOR,
    LABEL_FONT_FAMILY,
    LABEL_FONT_SIZE,
    LABEL_FONT_BOLD,
    LABEL_FONT_ITALIC,
    LABEL_FONT_UNDERLINE,

    BRANCH_COLOR,
    BRANCH_THICKNESS,

    /**
     * If true (default): nodes shapes are visible (circles).
     * If false: the shape is visible only for selected nodes or on hover over the node area.
     */
    SHOW_NODE_SHAPE,
    /** Same as SHOW_NODE_SHAPE but for tips (leafs). */
    SHOW_TIP_SHAPE,
    NODE_COLOR,
    NODE_RADIUS,

    /** Shows/hides branch distance labels. */
    SHOW_BRANCH_DISTANCE_LABELS,

    /** Shows/hides inner node labels: names/heights (heights are not implemented yet). */
    SHOW_INNER_NODE_LABELS,

    /** Shows/hides leaf node labels (sequence/species names). */
    SHOW_LEAF_NODE_LABELS,

    /** Aligns leaf node labels (sequence/species names). */
    ALIGN_LEAF_NODE_LABELS,

    /** Distance-units length of the legend line. */
    SCALEBAR_RANGE,

    SCALEBAR_FONT_SIZE,
    SCALEBAR_LINE_WIDTH,

    OPTION_ENUM_END
};

}  // namespace U2
