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

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

#include <U2Core/global.h>

#include "../TreeSettings.h"

namespace U2 {

class TreeViewerUI;
class PhyTreeObject;
class TvBranchItem;
class TvTextItem;
class PhyNode;

class U2VIEW_EXPORT TvNodeItem : public QGraphicsEllipseItem {
public:
    TvNodeItem(TvBranchItem* parentItem, const QString& nodeName = nullptr);

    /** Returns true if the current node is selected and there is no other selected parent node on top of these node. */
    bool isSelectionRoot() const;

    void toggleCollapsedState() const;

    bool isCollapsed() const;

    PhyNode* getPhyNode() const;

    void updateSettings(const QMap<TreeViewOption, QVariant>& settings);

    TvBranchItem* getParentBranchItem() const;

    TvBranchItem* getLeftBranchItem() const;

    TvBranchItem* getRightBranchItem() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    TvTextItem* labelItem = nullptr;
    const QString nodeName;

    /**
     * TODO: create a default tree viewer settings provider used both by TreeOptionsWidget and items.
     *  Or pass initial settings into the every item constructor.
     */
    bool isShapeVisible = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    TreeViewerUI* getTreeViewerUI() const;

    bool isHovered = false;
};

}  // namespace U2
