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

#ifndef _U2_CALCULATE_COVERAGE_PER_BASE_TASK_H_
#define _U2_CALCULATE_COVERAGE_PER_BASE_TASK_H_

#include <QMap>

#include <U2Core/U2Assembly.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

class CoveragePerBaseInfo {
public:
    CoveragePerBaseInfo() :
        coverage(0) {}

    int coverage;
    QMap<char, int> basesCount;
};
class GetAssemblyLengthTask : public Task {
    Q_OBJECT
public:
    GetAssemblyLengthTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId) :
        Task(tr("Get length of Assembly"), TaskFlag_None), dbiRef(dbiRef), assemblyId(assemblyId) {}

    void run();

    qint64 getAssemblyLength() const { return length; }
private:
    const U2DbiRef dbiRef;
    const U2DataId assemblyId;
    qint64 length;
};

class CalculateCoveragePerBaseOnRegionTask : public Task {
    Q_OBJECT
public:
    CalculateCoveragePerBaseOnRegionTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const U2Region &region);
    ~CalculateCoveragePerBaseOnRegionTask();

    void run();

    const U2Region &getRegion() const;
    QVector<CoveragePerBaseInfo> *takeResult();

private:
    void processRead(const U2AssemblyRead &read);
    U2CigarOp nextCigarOp(const QVector<U2CigarOp> &cigarVector, int &index, int &insertionsCount);

    const U2DbiRef dbiRef;
    const U2DataId assemblyId;
    const U2Region region;
    QVector<CoveragePerBaseInfo> *results;
};

class CalculateCoveragePerBaseTask : public Task {
    Q_OBJECT
public:
    CalculateCoveragePerBaseTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId);
    ~CalculateCoveragePerBaseTask();

    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

    bool isResultReady(qint64 startPos) const;
    bool areThereUnprocessedResults() const;
    QVector<CoveragePerBaseInfo> *takeResult(qint64 startPos);

signals:
    void si_regionIsProcessed(qint64 startPos);

    private slots:

private:
    const U2DbiRef dbiRef;
    const U2DataId assemblyId;
    QHash<qint64, QVector<CoveragePerBaseInfo> *> results;

    static const qint64 MAX_REGION_LENGTH = 100000;
    GetAssemblyLengthTask *getLengthTask;
};

}   // namespace U2

#endif // _U2_CALCULATE_COVERAGE_PER_BASE_TASK_H_
