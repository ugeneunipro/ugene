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

#include "Counter.h"

namespace U2 {

QList<GCounter*>& GCounter::getCounters() {
    static GCounterList counters;
    return counters.list;
}

GCounter::GCounter(const QString& _name, const QString& s, double scale) : name(_name), suffix(s), totalCount(0), counterScale(scale), destroyMe(false) {
    assert(counterScale > 0);
    getCounters().append(this);
}

GCounter::~GCounter() {
    getCounters().removeOne(this);
}

GCounter *GCounter::getCounter(const QString &name, const QString &suffix) {
    foreach (GCounter *counter, getCounters()) {
        if (name == counter->name && suffix == counter->suffix) {
            return counter;
        }
    }
    return NULL;
}

GReportableCounter::GReportableCounter(const QString& name, const QString& suffix, double scale /* = 1 */) :
GCounter(name, suffix, scale) {
}

GCounterList::~GCounterList() {
    for (int i = 0; i < list.size(); i++) {
        if (list[i]->destroyMe) {
            GCounter *counter = list[i];
            list[i] = NULL;
            delete counter;
        }
    }
}

} //namespace
