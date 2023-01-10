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

#ifndef _U2_LREGION_SELECTION_H_
#define _U2_LREGION_SELECTION_H_

#include <U2Core/SelectionModel.h>
#include <U2Core/U2Region.h>

namespace U2 {

/** Selection of LRegions. Contains ordered list of non-empty regions. */
class U2CORE_EXPORT LRegionsSelection : public GSelection {
    Q_OBJECT
public:
    explicit LRegionsSelection(const GSelectionType& type, QObject* p = nullptr);

    /** Returns currently selected regions. */
    const QVector<U2Region>& getSelectedRegions() const;

    /**
     * Sets new selection state and emits 'si_selectionChanged'.
     * Dedups call if the new selection is the same as ignored (does nothing in this case).
     */
    void setSelectedRegions(const QVector<U2Region>& newSelection);

    /** Adds region to the selection. Does nothing if the region is empty or the region is already in the selection. */
    void addRegion(const U2Region& r);

    /** Removes region from the selection. Does nothing if there is no such region in the selection. */
    void removeRegion(const U2Region& r);

    /** Sets the selection to the given region. If the region is empty clears the selection. */
    void setRegion(const U2Region& r);

    /** Returns true if there is no selected regions in the selection. */
    bool isEmpty() const override;

    /** Clears current selection. */
    void clear() override;

    /** Returns true if selection contains given point. */
    bool contains(qint64 pos) const;

    static QVector<U2Region> cropSelection(qint64 sequenceLength, const QVector<U2Region>& regions);

signals:
    /** The signal emitted when selection is changed. */
    void si_selectionChanged(LRegionsSelection* thiz, const QVector<U2Region>& added, const QVector<U2Region>& removed);

public:
    QVector<U2Region> regions;
};

}  // namespace U2

#endif
