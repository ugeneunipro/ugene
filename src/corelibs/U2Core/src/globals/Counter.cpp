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

#include "Counter.h"

namespace U2 {

/** A helper class that deletes all onHeap counters when destroyed. */
class GCounterList {
public:
    ~GCounterList() {
        for (int i = 0; i < list.size(); i++) {
            GCounter* counter = list[i];
            if (counter->isOnHeap) {
                list[i] = nullptr;
                delete counter;
            }
        }
    }
    QList<GCounter*> list;
};

static QList<GCounter*>& getGlobalCounterList() {
    // Thread safe initialization of the global counters list.
    static GCounterList counterList;
    return counterList.list;
}

GCounter::GCounter(const QString& name, const QString& suffix, qint64 value, double scale, bool isReportable, bool isOnHeap)
    : name(name), suffix(suffix), value(value), scale(scale), isReportable(isReportable), isOnHeap(isOnHeap) {
    getGlobalCounterList() << this;
}

GCounter::~GCounter() {
    getGlobalCounterList().removeOne(this);
};

GCounter* GCounter::findCounter(const QString& name, const QString& suffix) {
    for (GCounter* counter : qAsConst(getGlobalCounterList())) {
        if (name == counter->name && suffix == counter->suffix) {
            return counter;
        }
    }
    return nullptr;
}

QList<GCounter*> GCounter::getAllCounters() {
    return getGlobalCounterList();
}

void GCounter::increment(const QString& name, const QString& suffix) {
    GCounter* counter = GCounter::findCounter(name, suffix);
    if (counter == nullptr) {
        counter = new GCounter(name, suffix, 0, 1, true, true);
    }
    counter->value++;
}

}  // namespace U2
