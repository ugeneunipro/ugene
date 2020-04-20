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

#ifndef _U2_COLLOCATION_SEARCH_ALGORITHM_H_
#define _U2_COLLOCATION_SEARCH_ALGORITHM_H_

#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class CollocationsAlgorithmListener {
public:
    virtual ~CollocationsAlgorithmListener(){};
    virtual void onResult(const U2Region& r) = 0;
};

class CollocationsAlgorithmItem {
public:
    CollocationsAlgorithmItem(){}
    CollocationsAlgorithmItem(const QString& _name) : name(_name){}

    QString         name;
    QVector<U2Region> regions;
};

class CollocationsAlgorithmSettings;

class CollocationsAlgorithm {
public:
    enum SearchType {NormalSearch, PartialSearch};
    static void find(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si,
        CollocationsAlgorithmListener* l, const CollocationsAlgorithmSettings& cfg);
private:
    static void findN(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si,
                    CollocationsAlgorithmListener* l, const U2Region& searchRegion, qint64 distance);
    static void findP(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si,
                    CollocationsAlgorithmListener* l, const U2Region& searchRegion, qint64 distance);

};

class CollocationsAlgorithmSettings {
public:
    CollocationsAlgorithmSettings()
        : distance(-1), st(CollocationsAlgorithm::NormalSearch), includeBoundaries(true), strand(StrandOption_Both) {}
    U2Region searchRegion;
    int distance;
    CollocationsAlgorithm::SearchType st;
    QString resultAnnotationsName;
    bool includeBoundaries;
    StrandOption strand;
};


} //namespace U2
#endif


