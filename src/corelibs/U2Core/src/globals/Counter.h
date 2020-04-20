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

#ifndef _U2_COUNTER_H_
#define _U2_COUNTER_H_

#include <QList>
#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT GCounter : public QObject {
    Q_OBJECT
public:
    GCounter(const QString& name, const QString& suffix, double scale = 1);
    virtual ~GCounter();

    static const QList<GCounter*>& allCounters() {return getCounters();}
    static GCounter *getCounter(const QString &name, const QString &suffix);

    QString name;
    QString suffix;
    qint64  totalCount;
    double  counterScale;
    bool    destroyMe; //true if counter should be deleted by counter list

    double scaledTotal() const {return totalCount / counterScale;}

protected:

    static QList<GCounter*>& getCounters();
};

class GCounterList {
public:
    ~GCounterList();

    QList<GCounter *> list;
};


//Marks that counter will be reported by Shtirlitz
//TODO: implement GPerformanceCounter for plugin_perf_monitor?
class U2CORE_EXPORT GReportableCounter : public GCounter {
    Q_OBJECT
public:
    GReportableCounter(const QString& name, const QString& suffix, double scale = 1);
};


class U2CORE_EXPORT SimpleEventCounter {
public:
    SimpleEventCounter(GCounter* tc) : totalCounter(tc), eventCount(1){ assert(tc!=NULL);}
    virtual ~SimpleEventCounter() {totalCounter->totalCount+=eventCount;}
private:
    GCounter*   totalCounter;
    qint64      eventCount;
};

#define GCOUNTER(cvar, tvar, name) \
    static GReportableCounter cvar(name, "", 1); \
    SimpleEventCounter tvar(&cvar)

#define GRUNTIME_NAMED_COUNTER(cvar, tvar, name, suffix) \
    GCounter *cvar = GCounter::getCounter(name, suffix); \
    if (NULL == cvar) { \
        cvar = new GReportableCounter(name, suffix, 1); \
        cvar->destroyMe = true; \
            } \
    SimpleEventCounter tvar(cvar)

#define GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, condition, name, suffix) \
    if(condition) {\
        GRUNTIME_NAMED_COUNTER(cvar, tvar, name, suffix); \
    }

} //namespace

#endif
