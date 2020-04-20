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

#include "Timer.h"

namespace U2 {


QDateTime GTimer::createDateTime(qint64 micros, Qt::TimeSpec spec) {
    QDateTime res(QDate(1970, 1, 1), QTime(0, 0), Qt::UTC);
    res = res.addMSecs(micros/1000);
    assert(res.isValid());
    return res.toTimeSpec(spec);
}

static double getFrequency() {
    double frequency = 0;
#if defined(Q_OS_WIN) && defined(WIN_USE_PERF_COUNTER)
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    frequency = (double)freq.QuadPart;
#else
    frequency = 1000*1000; //microseconds
#endif
    return frequency;

}

static qint64 getCorrection() {
    GCounter totalCounter("timer correction", "ticks", 1);

    TimeCounter tc(&totalCounter, false);
    tc.start(); tc.stop();
    tc.start(); tc.stop();
    tc.start(); tc.stop();
    tc.start(); tc.stop();

    qint64 correction = totalCounter.totalCount / 4;
    return correction;
}

qint64 TimeCounter::correction = getCorrection();
double TimeCounter::frequency = getFrequency();

bool TimeCounter::enabled = true;
QString  TimeCounter::timeSuffix("seconds");
} //namespace
