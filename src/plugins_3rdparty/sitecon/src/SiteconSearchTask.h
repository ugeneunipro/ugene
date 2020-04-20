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

#ifndef _U2_SITECON_SEARCH_TASK_H_
#define _U2_SITECON_SEARCH_TASK_H_

#include "SiteconAlgorithm.h"

#include <U2Core/U2Region.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/SequenceWalkerTask.h>

#include <QMutex>

namespace U2 {

class SiteconSearchResult {
public:
    SiteconSearchResult() : strand(U2Strand::Direct), psum(-1), err1(0), err2(1){}

    SharedAnnotationData toAnnotation(const QString &name) const {
        SharedAnnotationData data(new AnnotationData);
        data->name = name;
        data->location->regions << region;
        data->setStrand(strand);
        if (!modelInfo.isEmpty()) {
            data->qualifiers.append(U2Qualifier("sitecon_model", modelInfo));
        }
        data->qualifiers.append(U2Qualifier("psum", QString::number(psum)));
        data->qualifiers.append(U2Qualifier("error_1", QString::number(err1, 'g', 4)));
        data->qualifiers.append(U2Qualifier("error_2", QString::number(err2, 'g', 4)));
        return data;
    }

    static QList<SharedAnnotationData> toTable(const QList<SiteconSearchResult> &res, const QString &name) {
        QList<SharedAnnotationData> list;
        foreach (const SiteconSearchResult &f, res) {
            list.append(f.toAnnotation(name));
        }
        return list;
    }


    U2Region region;
    U2Strand strand;
    float   psum;
    float   err1;
    float   err2;
    QString modelInfo;
};

class SiteconSearchCfg {
public:
    SiteconSearchCfg() : minPSUM(0), minE1(0.), maxE2(1.), complTT(NULL), complOnly(false) {}
    int minPSUM;
    float minE1;
    float maxE2;
    DNATranslation* complTT;
    bool complOnly; //FIXME use strand instead
};

class SiteconSearchTask : public Task, public SequenceWalkerCallback {
    Q_OBJECT
public:
    SiteconSearchTask(const SiteconModel& model, const QByteArray& seq, const SiteconSearchCfg& cfg, int resultsOffset);
    virtual ~SiteconSearchTask();
    virtual void cleanup();
    
    virtual void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti);
    QList<SiteconSearchResult> takeResults();

private:
    void addResult(const SiteconSearchResult& r);

    QMutex                      *lock;
    SiteconModel                *model;
    SiteconSearchCfg            *cfg;
    QList<SiteconSearchResult>  results;
    int                         resultsOffset;
    QByteArray                  wholeSeq;
};

}//namespace

#endif
