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

#ifndef _U2_MA_COLLAPSE_MODEL_H_
#define _U2_MA_COLLAPSE_MODEL_H_

#include <QObject>
#include <QVector>
#include <QSet>
#include <QList>
#include <QHash>

#include <U2Core/global.h>

namespace U2 {

class MaCollapsibleGroup {
public:
    /* Creates with 1 MA row inside. */
    MaCollapsibleGroup(int maRow, qint64 maRowId, bool isCollapsed = false);

    /* Creates new collapsible group item that starts with maRowIndex and has numRows inside. */
    MaCollapsibleGroup(const QList<int>& maRows, const QList<qint64>& maRowIds, bool isCollapsed = false);

    /* Creates empty group. This method should not be used directly but is required by the Vector<MaCollapsibleGroup>. */
    MaCollapsibleGroup();

    /* Compares this group with another group. Returns true if groups are equal. */
    bool operator== (const MaCollapsibleGroup &other) const;

    /* Returns number of rows in the group. */
    int size() const;

    /* Ordered list of ma rows in the group. */
    QList<int> maRows;

    /* MA row ids in the group. */
    QList<qint64> maRowIds;

    /* If group is collapsed or not. */
    bool isCollapsed;
};

class MaEditorWgt;
class MaModificationInfo;
class U2Region;

/**
 * Collapse model for the MAEditor.
 * Maps MA rows into View rows and used all the time regardless if "collapsible mode" is enabled or not.
 * In "collapse-disabled mode" each MA row is mapped to a 1 View row and the order of rows is the same.
 * In "collapse-enabled mode" a single View row may contain multiple MA rows and/or the order of View rows
 * may not be the same with MA rows.
 * The "collapse-enabled mode" is used to group MA rows by similarity or to mimic Tree Viewer structure.
 */
class U2VIEW_EXPORT MaCollapseModel : public QObject {
    Q_OBJECT
public:
    MaCollapseModel(QObject *p, const QList<qint64>& allOrderedMaRowIds);

    /* Updates model to the given groups. */
    void update(const QVector<MaCollapsibleGroup>& groups);

    /**
     * Updates collapse model using united rows as input.
     * 'allOrderedMaRowIds' is a list of all ma row ids in the alignment.
     */
    void updateFromUnitedRows(const QVector<U2Region>& unitedRows, const QList<qint64>& allOrderedMaRowIds);

    /*
     * Flattens all collapsible groups: makes every group contain only 1 sequence.
     * 'allOrderedMaRowIds' is a list of all ma row ids in the alignment.
     */
    void reset(const QList<qint64>& allOrderedMaRowIds, const QSet<int>& expandedGroupIndexes = QSet<int>());

    /* Toggle 'isCollapsed' state for the group at the given row. */
    void toggle(int viewRowIndex);

    /* Updates 'isCollapsed' state for the group at the given row. */
    void toggle(int viewRowIndex, bool isCollapsed);

    /* Toggle 'isCollapsed' state for the group. */
    void toggleGroup(int collapsibleGroupIndex, bool isCollapsed);

    /* Collapse all groups in the view. */
    void collapseAll(bool collapse);

    /* Converts view row index to MA row index. */
    int getMaRowIndexByViewRowIndex(int viewRowIndex) const;

    /*
     * Returns list of MA row indexes for the given view row indexes.
     * If 'includeChildRowsForCollapsedGroups' is true adds all MA rows in the group
     * for every viewRow that is a header of the collapsed group.
     */
    QList<int> getMaRowIndexesByViewRowIndexes(const U2Region& viewRowIndexesRegion, bool includeChildRowsForCollapsedGroups = false);

    /* Returns list of all MA row indexes that have valid view row index (not hidden by collapsing). */
    QList<int> getMaRowsIndexesWithViewRowIndexes() const;

    /*
    * Converts MA row index to the view row index.
    * If MA row has no viewRowIndex (is inside of collapsed group) returns -1 if failIfNotVisible
     * is true , otherwise returns groups view row index.
    */
    int getViewRowIndexByMaRowIndex(int maRowIndex, bool failIfNotVisible = false) const;

    /* Returns 'true' if the MA row is inside of some collapsible group and the group is collapsed. */
    bool isGroupWithMaRowIndexCollapsed(int maRowIndex) const;

    /* Returns the collapsible group index with the row or -1, if the row is not in a collapsible group. */
    int getCollapsibleGroupIndexByViewRowIndex(int viewRowIndex) const;

    /* Returns collapsible group by index or NULL if there is no group for the index. */
    const MaCollapsibleGroup* getCollapsibleGroup(int collapsibleGroupIndex) const;

    /* Returns collapsible group by view row index or NULL if there is no group for the index. */
    const MaCollapsibleGroup* getCollapsibleGroupByViewRow(int viewRowIndex) const;

    /* Returns collapsible group by MA row index or NULL if there is no group for the index. */
    const MaCollapsibleGroup* getCollapsibleGroupByMaRow(int maRowIndex) const;

    /* Returns number of view rows. Every collapsible group has at least one (the first) row counted. */
    int getViewRowCount() const;

    /* Returns current set of collapsible groups. */
    int getGroupCount() const {return groups.size();}

    bool hasGroupsWithMultipleRows() const { return hasGroupsWithMultipleItems; }

signals:
    void si_aboutToBeToggled();
    void si_toggled();

private:
    void updateIndex();

    QVector<MaCollapsibleGroup> groups;

    QHash<int, int> viewRowByMaRow;
    QHash<int, int> maRowByViewRow;
    QHash<int, int> groupByMaRow;
    bool hasGroupsWithMultipleItems;
};

} //namespace

#endif
