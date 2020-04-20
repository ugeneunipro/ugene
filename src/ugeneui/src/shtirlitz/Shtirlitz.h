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

#ifndef __SHTIRLITZ_H__
#define __SHTIRLITZ_H__

#include <QUuid>
#include <U2Core/Task.h>

namespace U2{

class Shtirlitz;

//The main responsibility of Shtirlitz is to collect and send reports about
//the system where UGENE is installed and about UGENE usage.
class Shtirlitz {
public:
    //Sends all of the reports, if needed. 
    static QList<Task*> wakeup();
    //Sends custom reports, created by somebody other than Shtirlitz.
    static Task * sendCustomReport( const QString & customReport );
    //Saves to settings gathered during the current launch info
    static void saveGatheredInfo();
private:
    //Creates and sends counters info (statistics about UGENE main tasks usage)
    static Task * sendCountersReport();
    //Creates and sends system info: OS, hardware platform, etc.
    static Task * sendSystemReport();

    static QString formCountersReport();
    static QString formSystemReport(); 

    static void getOsNameAndVersion( QString & name, QString & version );

    static void getFirstLaunchInfo(bool& allVersions, bool& majorVersions);
    static bool enabled();

    //ugly stub for convenience - calls ShtirlitzPlugin::tr
    static QString tr( const char * str );

private:
    //loads uuid from settings if necessary
    //creates the new one if nothing was found
    static QUuid getUniqueUgeneId();

    static const QString SEPARATOR;
};

//Task which performs sending of data
class ShtirlitzTask : public Task {
    Q_OBJECT
public:
    ShtirlitzTask( const QString & _report );
    void run();
private:
    QString report;
};

class ShtirlitzStartupTask : public Task {
    Q_OBJECT
public:
    ShtirlitzStartupTask();
    void prepare();
};

} // U2


#endif
