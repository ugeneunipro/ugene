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

#ifndef _U2_GRAPHICS_BRANCH_ITEM_H_
#define _U2_GRAPHICS_BRANCH_ITEM_H_

#include <QAbstractGraphicsShapeItem>

#include <U2Core/global.h>

#include "TreeSettings.h"

namespace U2 {

class PhyNode;
class GraphicsButtonItem;
class GraphicsRectangularBranchItem;

class U2VIEW_EXPORT GraphicsBranchItem : public QAbstractGraphicsShapeItem {
public:
    enum Direction { up,
                     down };

    static const int TextSpace;
    static const int SelectedPenWidth;

    /** Maximum distance (count) from this branch to the end (leaf) of the tree. */
    int maxStepsToLeaf = 0;

    /** Delta between parent's branch 'maxStepsToLeaf' and this branch 'maxStepsToLeaf'. */
    int maxStepsToLeafParentDelta = 1;

    /** Corresponding rectangular branch item for the branch. Set only for circular & unrooted branch items. */
    GraphicsRectangularBranchItem* correspondingRectangularBranchItem = nullptr;

private:
    void initText(qreal d);

    GraphicsButtonItem* buttonItem = nullptr;

    QGraphicsEllipseItem* nameItemSelection = nullptr;

protected:
    QGraphicsSimpleTextItem* distanceText = nullptr;
    QGraphicsSimpleTextItem* nameText = nullptr;
    qreal width = 0;
    qreal dist = 0;
    bool collapsed = false;

    OptionsMap settings;

    GraphicsBranchItem(const QString& name);
    GraphicsBranchItem(qreal d, bool withButton = true, double nodeValue = -1.0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    virtual void setLabelPositions();

public:
    GraphicsBranchItem(bool withButton = true, double nodeValue = -1.0);

    GraphicsButtonItem* getButton() const {
        return buttonItem;
    }
    qreal getNodeLabel() const;
    QGraphicsSimpleTextItem* getDistanceText() const {
        return distanceText;
    }
    QGraphicsSimpleTextItem* getNameText() const {
        return nameText;
    }
    qreal getWidth() const {
        return width;
    }
    qreal getDist() const {
        return dist;
    }
    void setDistanceText(const QString& text);
    void setWidthW(qreal w) {
        width = w;
    }
    void setWidth(qreal w);
    void setDist(qreal d) {
        dist = d;
    }
    virtual void collapse();
    void setSelectedRecurs(bool sel, bool selectChilds);
    void setSelected(bool sel);
    bool isCollapsed() const;

    void updateSettings(const OptionsMap& settings);
    void updateChildSettings(const OptionsMap& settings);
    /** Update current property with given one */
    void updateTextProperty(TreeViewOption property, const QVariant& propertyVal);

    const OptionsMap& getSettings() const;

    const QList<QGraphicsItem*> getChildItems() const {
        return childItems();
    }

    QGraphicsItem* getParentItem() const {
        return parentItem();
    }

    void initDistanceText(const QString& text = QString());

    QRectF visibleChildrenBoundingRect(const QTransform& viewTransform) const;
};

}  // namespace U2

#endif
