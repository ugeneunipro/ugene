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

#ifndef _U2_ASSEMBLY_BROSER_STATE_H_
#define _U2_ASSEMBLY_BROSER_STATE_H_

#include <U2Core/U2Region.h>
#include <U2Core/GObject.h>

#include <QVariant>


namespace U2 {

class AssemblyBrowser;

class U2VIEW_EXPORT AssemblyBrowserState {
public:
    AssemblyBrowserState(){}
    AssemblyBrowserState(const QVariantMap &stateData_) : stateData(stateData_) {}

    bool isValid() const;

    void saveState(const AssemblyBrowser *ab);
    QVariantMap data() { return stateData; }

    // a shortcut for saveState() and data()
    inline static QVariantMap buildStateMap(const AssemblyBrowser *v);

    void restoreState(AssemblyBrowser * ab) const;

    void setGObjectRef(const GObjectReference &ref);
    GObjectReference getGObjectRef() const;

    void setVisibleBasesRegion(const U2Region &r);
    U2Region getVisibleBasesRegion() const;

    void setYOffset(int y);
    int getYOffset() const;

    // TODO: save overview state?
    // TODO: save consensus algo, highlighting mode for reads and consensus areas?
    // TODO: save some local caches to boost reopening?

private:
    QVariantMap stateData;
};

inline QVariantMap AssemblyBrowserState::buildStateMap(const AssemblyBrowser *v) {
    AssemblyBrowserState s;
    s.saveState(v);
    return s.data();
}

} // namespace

#endif
