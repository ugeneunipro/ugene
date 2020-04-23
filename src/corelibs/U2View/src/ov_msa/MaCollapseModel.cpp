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

#include "MaCollapseModel.h"
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// MaCollapsibleGroup
//////////////////////////////////////////////////////////////////////////

MaCollapsibleGroup::MaCollapsibleGroup(int maRow, qint64 maRowId, bool isCollapsed)
        : maRows(QList<int>() << maRow), maRowIds(QList<qint64>() << maRowId), isCollapsed(isCollapsed) {
}

MaCollapsibleGroup::MaCollapsibleGroup(const QList<int>& maRows, const QList<qint64>& maRowIds, bool isCollapsed)
        : maRows(maRows), maRowIds(maRowIds), isCollapsed(isCollapsed) {
}

MaCollapsibleGroup::MaCollapsibleGroup()
        : isCollapsed(true) {
}

bool MaCollapsibleGroup::operator==(const MaCollapsibleGroup& other) const {
    return maRows == other.maRows && isCollapsed == other.isCollapsed;
}

int MaCollapsibleGroup::size() const {
    return maRows.size();
}

//////////////////////////////////////////////////////////////////////////
/// MaCollapseModel
//////////////////////////////////////////////////////////////////////////

MaCollapseModel::MaCollapseModel(QObject* p, const QList<qint64>& allOrderedMaRowIds)
        : QObject(p), hasGroupsWithMultipleItems(false) {
    reset(allOrderedMaRowIds);
}

void MaCollapseModel::update(const QVector<MaCollapsibleGroup>& newGroups) {
    if (newGroups == groups) {
        return; // nothing is changed.
    }
    emit si_aboutToBeToggled();
    groups = newGroups;
    updateIndex();
    emit si_toggled();
}

void MaCollapseModel::updateFromUnitedRows(const QVector<U2Region>& unitedRows, const QList<qint64>& allOrderedMaRowIds) {
    QVector<U2Region> sortedRegions = unitedRows;
    qSort(sortedRegions);
    QVector<MaCollapsibleGroup> newGroups;
    int maIndex = 0;
    foreach (const U2Region region, unitedRows) {
        for (; maIndex < region.startPos; maIndex++) {
            newGroups.append(MaCollapsibleGroup(maIndex, allOrderedMaRowIds[maIndex], true));
        }
        QList<int> maRows;
        QList<qint64> maRowIds;
        for (;maIndex < region.endPos(); maIndex++) {
            maRows << maIndex;
            maRowIds << allOrderedMaRowIds[maIndex];
        }
        newGroups.append(MaCollapsibleGroup(maRows, maRowIds, true));
    }
    int numSequences = allOrderedMaRowIds.size();
    for (; maIndex < numSequences; maIndex++) {
        newGroups.append(MaCollapsibleGroup(maIndex, allOrderedMaRowIds[maIndex], true));
    }
    // Copy collapse info from the current state.
    for (int i = 0, n = qMin(newGroups.size(), groups.size()); i < n; i++) {
        newGroups[i].isCollapsed = groups[i].isCollapsed;
    }
    update(newGroups);
}

void MaCollapseModel::reset(const QList<qint64>& allOrderedMaRowIds, const QSet<int>& expandedGroupIndexes) {
    QVector<MaCollapsibleGroup> newGroups;
    int numSequences  = allOrderedMaRowIds.size();
    for (int maRow = 0; maRow < numSequences; maRow++) {
        bool isCollapsed = !expandedGroupIndexes.contains(maRow); // maRowIndex is the same as groupIndex here.
        newGroups.append(MaCollapsibleGroup(maRow, allOrderedMaRowIds[maRow], isCollapsed));
    }
    update(newGroups);
}

void MaCollapseModel::collapseAll(bool collapse) {
    emit si_aboutToBeToggled();
    for (int i=0; i < groups.size(); i++) {
        groups[i].isCollapsed = collapse;
    }
    updateIndex();
    emit si_toggled();
}

void MaCollapseModel::toggle(int viewRowIndex) {
    int groupIndex = getCollapsibleGroupIndexByViewRowIndex(viewRowIndex);
    CHECK(groupIndex >= 0 && groupIndex <= groups.size(),)
    MaCollapsibleGroup& group = groups[groupIndex];
    toggleGroup(groupIndex, !group.isCollapsed);
}

void MaCollapseModel::toggle(int viewRowIndex, bool isCollapsed) {
    int groupIndex = getCollapsibleGroupIndexByViewRowIndex(viewRowIndex);
    toggleGroup(groupIndex, isCollapsed);
}

void MaCollapseModel::toggleGroup(int groupIndex, bool isCollapsed) {
    CHECK(groupIndex >= 0 && groupIndex <= groups.size(),)
    MaCollapsibleGroup& group = groups[groupIndex];
    if (group.isCollapsed == isCollapsed) {
        return;
    }
    emit si_aboutToBeToggled();
    group.isCollapsed = isCollapsed;
    updateIndex();
    emit si_toggled();
}

int MaCollapseModel::getMaRowIndexByViewRowIndex(int viewRowIndex) const {
    return maRowByViewRow.value(viewRowIndex, -1);
}

QList<int> MaCollapseModel::getMaRowIndexesByViewRowIndexes(const U2Region& viewRowIndexesRegion, bool includeChildRowsForCollapsedGroups) {
    QList<int> maRows;
    QSet<int> visitedRows;
    for (int viewRow = viewRowIndexesRegion.startPos, n = viewRowIndexesRegion.endPos(); viewRow < n; viewRow++) {
        int maRow = getMaRowIndexByViewRowIndex(viewRow);
        if (maRow >= 0 && !visitedRows.contains(maRow)) {
            maRows << maRow;
            visitedRows.insert(maRow);
        }
        if (includeChildRowsForCollapsedGroups) {
            const MaCollapsibleGroup* group = getCollapsibleGroupByViewRow(viewRow);
            bool isGroupHeader = group->maRows.first() == maRow;
            if (isGroupHeader && group->isCollapsed) {
                for (int i = 1; i < group->maRows.length(); i++) {
                    int childMaRow = group->maRows[i];
                    if (!visitedRows.contains(childMaRow)) {
                        maRows << childMaRow;
                        visitedRows.insert(childMaRow);
                    }
                }
            }
        }
    }
    return maRows;
}

QList<int> MaCollapseModel::getMaRowsIndexesWithViewRowIndexes() const {
    QList<int> maRows;
    for (int viewRow = 0, n = getViewRowCount(); viewRow < n ; viewRow++) {
        int maRow = getMaRowIndexByViewRowIndex(viewRow);
        if (maRow >= 0) {
            maRows << maRow;
        }
    }
    return maRows;
}

int MaCollapseModel::getViewRowIndexByMaRowIndex(int maRowIndex, bool failIfNotVisible) const {
    int viewRowIndex = viewRowByMaRow.value(maRowIndex, -1);
    if (viewRowIndex >= 0) {
        return viewRowIndex;
    }
    if (failIfNotVisible) {
        return -1;
    }
    int groupIndex = groupByMaRow.value(maRowIndex, -1);
    if (groupIndex == -1) {
        return -1;
    }
    const MaCollapsibleGroup& group = groups[groupIndex];
    int firstMaInGroup = group.maRows[0];
    return viewRowByMaRow.value(firstMaInGroup, -1);
}

int MaCollapseModel::getViewRowIndexByMaRowId(qint64 maRowId) const {
    return viewRowByMaRowId.value(maRowId, -1);
}

bool MaCollapseModel::isGroupWithMaRowIndexCollapsed(int maRowIndex) const {
    int viewRowIndex = getViewRowIndexByMaRowIndex(maRowIndex);
    int groupIndex = getCollapsibleGroupIndexByViewRowIndex(viewRowIndex);
    const MaCollapsibleGroup* group = getCollapsibleGroup(groupIndex);
    return group != NULL && group->isCollapsed;
}

int MaCollapseModel::getCollapsibleGroupIndexByViewRowIndex(int viewRowIndex) const {
    int maIndex = maRowByViewRow.value(viewRowIndex, -1);
    return groupByMaRow.value(maIndex, -1);
}

const MaCollapsibleGroup* MaCollapseModel::getCollapsibleGroup(int collapsibleGroupIndex) const {
    if (collapsibleGroupIndex < 0 || collapsibleGroupIndex >= groups.length()) {
        return NULL;
    }
    return &groups.constData()[collapsibleGroupIndex];
}

const MaCollapsibleGroup* MaCollapseModel::getCollapsibleGroupByViewRow(int viewRowIndex) const {
    return getCollapsibleGroup(getCollapsibleGroupIndexByViewRowIndex(viewRowIndex));
}
const MaCollapsibleGroup* MaCollapseModel::getCollapsibleGroupByMaRow(int maRowIndex) const {
    return getCollapsibleGroupByViewRow(getViewRowIndexByMaRowIndex(maRowIndex));
}

int MaCollapseModel::getViewRowCount() const {
    return viewRowByMaRow.size();
}

void MaCollapseModel::updateIndex() {
    viewRowByMaRow.clear();
    viewRowByMaRowId.clear();
    maRowByViewRow.clear();
    groupByMaRow.clear();
    hasGroupsWithMultipleItems = false;
    int viewRow = 0;
    for (int groupIndex = 0; groupIndex < groups.size(); groupIndex++) {
        const MaCollapsibleGroup& group = groups[groupIndex];
        hasGroupsWithMultipleItems = hasGroupsWithMultipleItems || group.maRows.size() > 1;
        for (int i = 0; i < group.maRows.size(); i++) {
            int maRow = group.maRows[i];
            if (i == 0 || !group.isCollapsed) {
                qint64 maRowId = group.maRowIds[i];
                viewRowByMaRowId.insert(maRowId, viewRow);
                viewRowByMaRow.insert(maRow, viewRow);
                maRowByViewRow.insert(viewRow, maRow);
                viewRow++;
            }
            groupByMaRow.insert(maRow, groupIndex);
        }
    }
}

} // namespace U2
