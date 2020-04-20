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

#ifndef _U2_SITECON_IO_H_
#define _U2_SITECON_IO_H_

#include <QObject>
#include <QString>

#include "SiteconAlgorithm.h"

#include <U2Core/Task.h>

namespace U2 {

class IOAdapterFactory;
class DiPropertySitecon;

class SiteconIO : public QObject {
    Q_OBJECT
public:

    static const QString SITECON_ID;
    static const QString SITECON_EXT;
    static QString getFileFilter(bool includeAll = true);
    static SiteconModel readModel(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si);
    static void writeModel(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const SiteconModel& model);
};

class SiteconReadTask: public Task {
    Q_OBJECT
public:
    SiteconReadTask(const QString& url) 
        : Task(tr("Read SITECON Model"), TaskFlag_None), url(url) {}
    void run();
    SiteconModel getResult() const {return model;}
    QString getURL() const {return url;}
private:
    QString url;
    SiteconModel model;
};

class SiteconWriteTask : public Task {
    Q_OBJECT
public:
    SiteconWriteTask(const QString& url, const SiteconModel& model, uint f = 0) 
        : Task(tr("Save SITECON model"), TaskFlag_None), url(url), model(model), fileMode(f) {}
    virtual void run();
private:
    QString url;
    SiteconModel model;
    uint fileMode;
};


}//namespace

#endif
