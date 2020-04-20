/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include "TreeSettings.h"

namespace U2 {

class PhyNode;
class GraphicsButtonItem;

class GraphicsBranchItem: public QAbstractGraphicsShapeItem {
public:
    enum Direction { up, down };

    static const int TextSpace;
    static const int SelectedPenWidth;

private:
    GraphicsBranchItem* correspondingItem;
    GraphicsButtonItem* buttonItem;
    void initText(qreal d);
    int branchLength;
    QGraphicsEllipseItem*    nameItemSelection;

protected:

    QGraphicsSimpleTextItem* distanceText;
    QGraphicsSimpleTextItem* nameText;
    qreal width;
    qreal dist;
    bool collapsed;
    int lengthCoef;

    OptionsMap settings;

    GraphicsBranchItem(const QString& name);
    GraphicsBranchItem(qreal d, bool withButton = true, double nodeValue = -1.0);
    virtual void paint(QPainter * painter,const QStyleOptionGraphicsItem* option, QWidget* widget = NULL);

    virtual void setLabelPositions();

public:
    GraphicsBranchItem(bool withButton = true, double nodeValue = -1.0);

    GraphicsButtonItem* getButton() const { return buttonItem; }
    qreal getNodeLabel() const;
    QGraphicsSimpleTextItem* getDistanceText() const { return distanceText; }
    QGraphicsSimpleTextItem* getNameText() const { return nameText; }
    qreal getWidth() const { return width; }
    qreal getDist() const { return dist; }
    void setDistanceText(const QString& text);
    void setWidthW(qreal w) { width = w; }
    void setWidth(qreal w);
    void setDist (qreal d) { dist = d; }
    virtual void collapse();
    void setSelectedRecurs(bool sel, bool selectChilds);
    void setSelected(bool sel);
    bool isCollapsed() const;

    void updateSettings(const OptionsMap& settings);
    void updateChildSettings(const OptionsMap& settings);
    void updateTextSettings(const QFont& font, const QColor& color);

    const OptionsMap& getSettings() const;

    GraphicsBranchItem* getCorrespondingItem() const {return correspondingItem;}
    void setCorrespondingItem(GraphicsBranchItem* cItem) {correspondingItem = cItem;}

    const QList<QGraphicsItem*> getChildItems() const {return childItems();}

    void setBranchLength(int newLength) {branchLength = newLength;}
    int getBranchLength() const {return branchLength;}

    QGraphicsItem* getParentItem() const {return parentItem();}

    void setLenghtCoef(int newCoef) {lengthCoef = newCoef;}
    int getLengthCoef() const {return lengthCoef;}

    QRectF visibleChildrenBoundingRect (const QTransform& viewTransform) const;
};

}//namespace;

#endif
