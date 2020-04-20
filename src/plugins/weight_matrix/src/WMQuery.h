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

#ifndef _U2_WM_QUERY_H_
#define _U2_WM_QUERY_H_

#include "WeightMatrixSearchTask.h"

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Core/Task.h>


namespace U2 {

class PWMatrixReadTask;
class WMQDTask : public Task {
    Q_OBJECT
public:
    WMQDTask(const QString& url, const WeightMatrixSearchCfg& cfg, const DNASequence& sqnc,
        const QString& resName, const QVector<U2Region>& location);
    QList<Task*> onSubTaskFinished(Task* subTask);
    QList<WeightMatrixSearchResult> takeResults();
private:
    WeightMatrixSearchCfg settings;
    DNASequence dnaSeq;
    QString resultName;
    PWMatrixReadTask* readTask;
    QList<WeightMatrixSearchResult> res;
    QVector<U2Region> location;
};

class QDWMActor : public QDActor {
    Q_OBJECT
public:
    QDWMActor(QDActorPrototype const* proto);
    int getMinResultLen() const;
    int getMaxResultLen() const;
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0xff,0xf8,0); }
private slots:
    void sl_onAlgorithmTaskFinished(Task* t);
};

class QDWMActorPrototype : public QDActorPrototype {
public:
    QDWMActorPrototype();
    QIcon getIcon() const { return QIcon(":weight_matrix/images/weight_matrix.png"); }
    virtual QDActor* createInstance() const { return new QDWMActor(this); }
};

}//namespace

#endif
