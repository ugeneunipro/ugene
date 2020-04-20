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

#ifndef _U2_SITECON_QUERY_H_
#define _U2_SITECON_QUERY_H_

#include "SiteconSearchTask.h"

#include <U2Lang/QueryDesignerRegistry.h>


namespace U2 {

class QDSiteconActor : public QDActor {
    Q_OBJECT
public:
    QDSiteconActor(QDActorPrototype const* proto);
    int getMinResultLen() const { return 20; }
    int getMaxResultLen() const { return 50; }
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0xff,0xf8,0); }
private slots:
    void sl_onAlgorithmTaskFinished(Task*);
private:
    SiteconSearchCfg settings;
};

class QDSiteconActorPrototype : public QDActorPrototype {
public:
    QDSiteconActorPrototype();
    virtual QDActor* createInstance() const { return new QDSiteconActor(this); }
    virtual QIcon getIcon() const { return QIcon(":sitecon/images/sitecon.png"); }
};

class SiteconReadMultiTask : public Task {
    Q_OBJECT
public:
    SiteconReadMultiTask(const QStringList& urls);
    QList<SiteconModel> getResult() { return models; }
protected:
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
private:
    QList<SiteconModel> models;
};

class QDSiteconTask : public Task {
    Q_OBJECT
public:
    QDSiteconTask(const QStringList& urls, const SiteconSearchCfg& cfg,
        const DNASequence& dna, const QVector<U2Region>& searchRegion);
    QList<SiteconSearchResult> getResults() const { return results; }
protected:
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    SiteconReadMultiTask* loadModelsTask;
    SiteconSearchCfg cfg;
    DNASequence dnaSeq;
    QVector<U2Region> searchRegion;
    QList<SiteconSearchResult> results;
};

}//namespace

#endif
