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

#ifndef _U2_WEIGHT_MATRIX_SEARCH_TASK_H_
#define _U2_WEIGHT_MATRIX_SEARCH_TASK_H_

#include "WeightMatrixAlgorithm.h"

#include <U2Core/U2Region.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/SequenceWalkerTask.h>

#include <QMutex>
#include <QPair>

namespace U2 {

class WeightMatrixSearchResult {
public:
    WeightMatrixSearchResult() : strand(U2Strand::Direct), score(-1) {
        qual = QMap<QString, QString>();
    }

    SharedAnnotationData toAnnotation(U2FeatureType type, const QString& name) const {
        SharedAnnotationData data(new AnnotationData);
        data->type = type;
        data->name = name;
        data->location->regions << region;
        data->setStrand(strand);
        if (!modelInfo.isEmpty()) {
            data->qualifiers.append(U2Qualifier("Weight_matrix_model", modelInfo));
        }
        data->qualifiers.append(U2Qualifier("Score", QString::number(score)));
        QMapIterator<QString, QString> iter(qual);
        while (iter.hasNext()) {
            iter.next();
            data->qualifiers.append(U2Qualifier(iter.key(), iter.value()));
        }
        return data;
    }

    static QList<SharedAnnotationData> toTable(const QList<WeightMatrixSearchResult>& res, U2FeatureType type, const QString& name) {
        QList<SharedAnnotationData> list;
        foreach (const WeightMatrixSearchResult& f, res) {
            list.append(f.toAnnotation(type, name));
        }
        return list;
    }


    U2Region region;
    U2Strand strand;
    float   score;
    QString modelInfo;
    QMap<QString, QString> qual;
};

class WeightMatrixSearchCfg {
public:
    WeightMatrixSearchCfg() : minPSUM(0), modelName(""), complTT(NULL), complOnly(false), algo("") {}
    int minPSUM;
    QString modelName;
    DNATranslation* complTT;
    bool complOnly; //FIXME use strand instead
    QString algo;

    bool operator==(const WeightMatrixSearchCfg &c1) const {
        return c1.minPSUM == minPSUM &&
                c1.modelName == modelName &&
                c1.complOnly == complOnly &&
                c1.algo == algo;
    }
};

class WeightMatrixSearchTask : public Task {
    Q_OBJECT
public:
    WeightMatrixSearchTask(const QList< QPair< PWMatrix, WeightMatrixSearchCfg > >& models, const QByteArray& seq, int resultsOffset);

    QList<WeightMatrixSearchResult> takeResults();

private:
    void addResult(const WeightMatrixSearchResult& r);

    QMutex                                              lock;
    QList< QPair<PWMatrix, WeightMatrixSearchCfg> >     models;
    QList<WeightMatrixSearchResult>                     results;
    int                                                 resultsOffset;
};

class WeightMatrixSingleSearchTask : public Task, public SequenceWalkerCallback {
    Q_OBJECT
public:
    WeightMatrixSingleSearchTask(const PWMatrix& model, const QByteArray& seq, const WeightMatrixSearchCfg& cfg, int resultsOffset);

    virtual void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti);
    QList<WeightMatrixSearchResult> takeResults();

private:
    void addResult(const WeightMatrixSearchResult& r);

    QMutex                              lock;
    PWMatrix                            model;
    WeightMatrixSearchCfg               cfg;
    QList<WeightMatrixSearchResult>     results;
    int                                 resultsOffset;
    QByteArray                          seq;
};

}//namespace

#endif
