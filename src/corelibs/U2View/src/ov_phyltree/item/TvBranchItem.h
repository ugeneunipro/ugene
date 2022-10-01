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

#ifndef _U2_TV_BRANCH_ITEM_H_
#define _U2_TV_BRANCH_ITEM_H_

#include <QAbstractGraphicsShapeItem>

#include <U2Core/global.h>

#include "../TreeSettings.h"

namespace U2 {

class TvNodeItem;
class TvTextItem;
class TvRectangularBranchItem;

class U2VIEW_EXPORT TvBranchItem : public QObject, public QAbstractGraphicsShapeItem {
    Q_OBJECT
public:
    /** Side of the branch in the binary tree relative to the root node: left of right. */
    enum class Side {
        Left,
        Right
    };
    TvBranchItem(bool withButton, const Side& side, double nodeValue);

    TvNodeItem* getButtonItem() const;

    TvBranchItem* getChildBranch(const Side& side) const;

    double getNodeLabelValue() const;

    TvTextItem* getDistanceTextItem() const;

    QString getDistanceText() const;

    TvTextItem* getNameTextItem() const;

    double getWidth() const;

    double getDist() const;

    void setDistanceText(const QString& text);

    void setWidthW(double w);

    void setWidth(double newWidth);

    void setDist(double d);

    virtual void toggleCollapsedState();

    /** Recursively selects/unselects current branch item and children. */
    void setSelectedRecursively(bool isSelected);

    bool isCollapsed() const;

    void updateSettings(const OptionsMap& settings);

    void updateChildSettings(const OptionsMap& settings);

    /** Update current property with given one */
    void updateTextProperty(TreeViewOption property, const QVariant& propertyVal);

    const OptionsMap& getSettings() const;

    void initDistanceText(const QString& text = "");

    QRectF visibleChildrenBoundingRect(const QTransform& viewTransform) const;

    /** Spacing between branch line and branch label. */
    static constexpr int TEXT_SPACING = 8;

    /** Width of the selected branch line. */
    static constexpr int SELECTED_PEN_WIDTH_DELTA = 1;

    /** Maximum distance (count) from this branch to the end (leaf) of the tree. */
    int maxStepsToLeaf = 0;

    /** Delta between parent's branch 'maxStepsToLeaf' and this branch 'maxStepsToLeaf'. */
    int maxStepsToLeafParentDelta = 1;

    /** Corresponding rectangular branch item for the branch. Set only for circular & unrooted branch items. */
    TvRectangularBranchItem* correspondingRectangularBranchItem = nullptr;

    /** Returns top level (root) branch item in the tree. */
    TvBranchItem* getRoot();

    /**Returns true if the branch is a root branch of the tree. */
    bool isRoot() const;

    /** Emits si_branchCollapsed signal for the given branch. Can only be called on the root branch. */
    void emitBranchCollapsed(TvBranchItem* branch);

signals:
    void si_branchCollapsed(TvBranchItem* branch);

protected:
    explicit TvBranchItem(const QString& name);

    TvBranchItem(double distance, bool withButton, double nodeValue);

    virtual void setLabelPositions();

    /**
     * Sets up common painter properties based on the current selection/hover state.
     * Called before in the beginning of the paint() method.
     */
    virtual void setUpPainter(QPainter* p);

    void initText(double d);

    TvTextItem* distanceTextItem = nullptr;
    TvTextItem* nameTextItem = nullptr;
    TvNodeItem* nodeItem = nullptr;

    double width = 0;
    /** Distance of the branch (a value from the Newick file or PhyBranch::distance). */
    double distance = 0;
    bool collapsed = false;
    OptionsMap settings;
    Side side = Side::Left;
};

}  // namespace U2

#endif
