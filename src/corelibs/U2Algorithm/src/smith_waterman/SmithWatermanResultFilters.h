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

#ifndef _U2_SMITH_WATERMAN_RESULT_FILTERS_H_
#define _U2_SMITH_WATERMAN_RESULT_FILTERS_H_

#include <QList>
#include <U2Core/U2Region.h>
#include <U2Algorithm/SmithWatermanResult.h>


namespace U2 {

class U2ALGORITHM_EXPORT SmithWatermanResultFilter {
public:
    virtual bool applyFilter(QList<SmithWatermanResult>* lst) = 0;
    virtual SmithWatermanResultFilter* clone() const = 0;
    virtual ~SmithWatermanResultFilter() {};

    virtual QString getId() const = 0;
};

class U2ALGORITHM_EXPORT SWRF_EmptyFilter: public SmithWatermanResultFilter {
private:
    static const QString ID;

public:
    virtual bool applyFilter(QList<SmithWatermanResult>* lst);
    virtual SmithWatermanResultFilter* clone() const;

    virtual QString getId() const;

private:
    bool needErase(const SmithWatermanResult& currItem,
                   const SmithWatermanResult& someItem) const;
};

class U2ALGORITHM_EXPORT SWRF_WithoutIntersect: public SmithWatermanResultFilter {
private:
    static const QString ID;

public:
    virtual bool applyFilter(QList<SmithWatermanResult>* lst);
    virtual SmithWatermanResultFilter* clone() const;

    virtual QString getId() const;

private:
    bool needErase(const SmithWatermanResult& currItem,
                   const SmithWatermanResult& someItem) const;

};

} // namesapce

#endif
