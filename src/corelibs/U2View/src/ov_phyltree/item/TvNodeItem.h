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

#ifndef _U2_TV_BUTTON_ITEM_H_
#define _U2_TV_BUTTON_ITEM_H_

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

class U2VIEW_EXPORT TvNodeItem : public QGraphicsEllipseItem {
public:
    TvNodeItem(const QString& nodeName = 0);

    bool isPathToRootSelected() const;

    void toggleCollapsedState();

    void swapSiblings();

    bool isCollapsed();

    void rerootTree(PhyTreeObject* treeObject);

    void updateSettings(const OptionsMap& settings);

    TvBranchItem* getParentBranchItem() const;

    TvBranchItem* getLeftBranchItem() const;

    TvBranchItem* getRightBranchItem() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    TvTextItem* labelItem = nullptr;
    const QString nodeName;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    TreeViewerUI* getTreeViewerUI() const;

    bool isHovered = false;
    bool isNodeShapeVisible = true;
};

}  // namespace U2

#endif
