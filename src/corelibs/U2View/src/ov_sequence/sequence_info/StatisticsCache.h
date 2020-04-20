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

#ifndef _U2_STATISTICS_CACHE_H_
#define _U2_STATISTICS_CACHE_H_

#include <U2Core/U2Region.h>

namespace U2 {

class StatisticsCacheBase : public QObject {
    Q_OBJECT
public slots:
    virtual void sl_invalidate() = 0;
};

template<class T>
class StatisticsCache : public StatisticsCacheBase{
public:
    StatisticsCache();

    const T &getStatistics() const;
    void setStatistics(const T &statistics, const QVector<U2Region>& regions);

    bool isValid(const QVector<U2Region>& regionsToMatch) const;

    void sl_invalidate();

private:
    T statistics;
    QVector<U2Region> regions;
    bool valid;
};

template<class T>
StatisticsCache<T>::StatisticsCache()
    : valid(false)
{

}

template<class T>
const T &StatisticsCache<T>::getStatistics() const {
    return statistics;
}

template<class T>
void StatisticsCache<T>::setStatistics(const T &newStatistics, const QVector<U2Region>& newRegions) {
    statistics = newStatistics;
    regions = newRegions;
    valid = true;
}

template<class T>
bool StatisticsCache<T>::isValid(const QVector<U2Region>& regionsToMatch) const {
    return regions == regionsToMatch && valid;
}

template<class T>
void StatisticsCache<T>::sl_invalidate() {
    valid = false;
}

}

#endif // _U2_STATISTICS_CACHE_H_
