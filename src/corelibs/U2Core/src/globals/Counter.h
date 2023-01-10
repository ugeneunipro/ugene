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

#ifndef _U2_COUNTER_H_
#define _U2_COUNTER_H_

#include <QList>
#include <QObject>

#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT GCounter : public QObject {
    Q_OBJECT
public:
    /**
     * Creates and adds counter to the global counter list.
     * To remove the counter from the global list a counter must be deleted.
     * If the counter is not deleted until application shutdown 'isOnHeap' flag is used to check if the counter must be deleted manually.
     */
    GCounter(const QString& name, const QString& suffix, qint64 value = 0, double scale = 1, bool isReportable = false, bool isOnHeap = false);

    /** Unregisters counter from the global counters list. */
    virtual ~GCounter();

    /** Returns a snapshot (copy) of all registered counters. */
    static QList<GCounter*> getAllCounters();

    /** Returns instance of the currently registered counter or nullptr if no counter with the given name/suffix was found. */
    static GCounter* findCounter(const QString& name, const QString& suffix);

    /** Visual name of the counter. Name + suffix used to uniquely identify the counter. */
    const QString name;

    /**
     * Suffix of the counter or unit. Example: milliseconds, counts, megabytes, algorithm type within some family of algorithms.
     * May be empty. Name + suffix is used to uniquely identify the counter.
     */
    const QString suffix;

    /**
     * Value of the counter.
     * Example: count of invocations, sum of all time periods, used memory size, etc...
     */
    qint64 value;

    /**
     * Scale factor for the counter value. May be used to convert counter values into another domain.
     * Example: convert cpu-clock-count into time-millis.
     */
    const double scale;

    /** Reportable counters are sent to UGENE developers as a part of Shtirlitz service reports. */
    const bool isReportable;

    /** Returns scaled counter value. */
    double getScaledValue() const {
        return value / scale;
    }

    /** If 'true' the counter was allocated on heap and must be manually deleted. */
    const bool isOnHeap;

    /** Increments value of the existing counter or creates a new reportable on-heap counter if no counter with this name+suffix is registered. */
    static void increment(const QString& name, const QString& suffix = "");
};

/** Creates a new reportable counter as function local static variable and increments it's value. */
#define GCOUNTER(cvar, name) \
    static GCounter cvar(name, "", 0, 1, true, false); \
    cvar.value += 1;

}  // namespace U2

#endif
