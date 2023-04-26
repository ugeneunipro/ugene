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

#include <QAbstractGraphicsShapeItem>

#include <U2Core/global.h>

#include "../TreeSettings.h"

namespace U2 {

class TvNodeItem;
class TvTextItem;
class TvRectangularBranchItem;
class PhyBranch;

class U2VIEW_EXPORT TvBranchItem : public QObject, public QAbstractGraphicsShapeItem {
    Q_OBJECT
public:
    /** Side of the branch in the binary tree relative to the root node: left of right. */
    enum class Side {
        Left,
        Right
    };
    TvBranchItem(TvBranchItem* parentTvBranch, const PhyBranch* branch, const Side& side, const QString& nodeName);

    TvNodeItem* getNodeItem() const;

    TvBranchItem* getChildBranch(const Side& side) const;

    QString getNodeNameFromNodeItem() const;

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

    void updateSettings(const QMap<TreeViewOption, QVariant>& settings);

    const QMap<TreeViewOption, QVariant>& getSettings() const;

    void initDistanceText(const QString& text = "");

    const Side& getSide() const;

    void setSide(const Side& side);

    /** Spacing between branch line and branch label. */
    static constexpr int TEXT_SPACING = 10;

    /** Width of the selected branch line. */
    static constexpr int SELECTED_PEN_WIDTH_DELTA = 1;

    /** Maximum distance (count) from this branch to the end (leaf) of the tree. */
    int maxStepsToLeaf = 0;

    /** Delta between parent's branch 'maxStepsToLeaf' and this branch 'maxStepsToLeaf'. */
    int maxStepsToLeafParentDelta = 1;

    /** Returns top level (root) branch item in the tree. */
    TvBranchItem* getRoot();

    /** Returns true if the branch is a root branch of the tree. */
    bool isRoot() const;

    /** Emits si_branchCollapsed signal for the given branch. Can only be called on the root branch. */
    void emitBranchCollapsed(TvBranchItem* branch);

    /** Returns true if the branch is the last visual branch in the tree. */
    bool isLeaf() const;

    /** Phy-tree related branch. May be null for the root branch item. */
    const PhyBranch* const phyBranch = nullptr;

signals:
    void si_branchCollapsed(TvBranchItem* branch);

protected:
    TvBranchItem(TvBranchItem* parentTvBranch, const PhyBranch* branch, const QString& sequenceName, bool isRoot);

    virtual void updateLabelPositions();

    /**
     * Sets up common painter properties based on the current selection/hover state.
     * Called before in the beginning of the paint() method.
     */
    virtual void setUpPainter(QPainter* p);

    void addDistanceTextItem(double d);

    TvTextItem* distanceTextItem = nullptr;
    TvTextItem* nameTextItem = nullptr;
    TvNodeItem* nodeItem = nullptr;

    double width = 0;
    /** Distance of the branch (a value from the Newick file or PhyBranch::distance). */
    double distance = 0;
    bool collapsed = false;
    QMap<TreeViewOption, QVariant> settings;
    Side side = Side::Left;
};

}  // namespace U2
