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

#ifndef _U2_TIMER_H_
#define _U2_TIMER_H_

#include <U2Core/global.h>
#include <U2Core/Counter.h>

#include <QDateTime>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <sys/time.h>
//#include <time.h>
#endif


namespace U2 {

class U2CORE_EXPORT GTimer : public QObject {
    Q_OBJECT
public:
    //Returns microseconds since the Unix Epoch (Jan. 1, 1970), UTC
    inline static qint64 currentTimeMicros();

    //suitable for values returned by GTimer::currentTimeMicros() call
    static QDateTime createDateTime(qint64 micros, Qt::TimeSpec spec = Qt::LocalTime);

    static int secsBetween(qint64 startTime, qint64 endTime) {return (int)((endTime-startTime)/(1000*1000));}
    static int millisBetween(qint64 startTime, qint64 endTime) {return (int)((endTime-startTime)/(1000));}
};

#define WIN_USE_PERF_COUNTER

class U2CORE_EXPORT TimeCounter {
public:
    inline TimeCounter(GCounter* c, bool _start = true);
    inline ~TimeCounter();

    inline void start();
    inline void stop();

    inline static qint64 getCounter();

    static bool     enabled;

    static double getCounterScale() {return frequency;}
    static const QString& getCounterSuffix() {return timeSuffix;}

private:
    GCounter*       totalCounter;
    qint64          startTime;
    bool            started;

    static qint64   correction;
    static double   frequency;
    static QString  timeSuffix;

};

#define GTIMER(cvar, tvar, name) \
    static GCounter cvar(name, TimeCounter::getCounterSuffix(), TimeCounter::getCounterScale()); \
    TimeCounter tvar(&cvar, true)


#define WIN_UNIX_EPOCH_DELTA_MSEC qint64(11644473600000000)
qint64 GTimer::currentTimeMicros() {
    qint64 res = 0;
#ifdef Q_OS_WIN
    FILETIME time;
    GetSystemTimeAsFileTime(&time);
    // Convert FILETIME to Unix Epoch time
    res = (qint64)((quint64(time.dwHighDateTime)<<32) | time.dwLowDateTime) / 10;
    res-= WIN_UNIX_EPOCH_DELTA_MSEC;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    res = qint64(tv.tv_sec)*1000*1000 + tv.tv_usec;
#endif
    return res;
}

TimeCounter::TimeCounter(GCounter* c, bool _start) : totalCounter(c), startTime(0) {
    assert(totalCounter!=NULL);
    started = false;
    if (_start) {
        start();
    }
}

TimeCounter::~TimeCounter() {
    if (started) {
        stop();
    }
}

void TimeCounter::start() {
    assert(!started);
    startTime = getCounter();
    started = true;
}

void TimeCounter::stop() {
    assert(started);
    qint64 endTime = getCounter();
    totalCounter->totalCount += endTime - startTime - correction;
    started = false;
}

inline qint64 TimeCounter::getCounter() {
#if defined(Q_OS_WIN) && defined(WIN_USE_PERF_COUNTER)
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return count.QuadPart;
#else
    return GTimer::currentTimeMicros();
#endif
}


} //namespace

#endif
