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

#ifndef _U2_FIND_PRIMER_GROUPS_WORKER_H_
#define _U2_FIND_PRIMER_GROUPS_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/DNASequence.h>

#include "PrimerStatistics.h"

namespace U2 {
namespace LocalWorkflow {


class PrimersGrouperPromter : public PrompterBase<PrimersGrouperPromter> {
    Q_OBJECT
public:
    PrimersGrouperPromter( Actor * p = 0 ) : PrompterBase<PrimersGrouperPromter>(p) {};
protected:
    QString composeRichDoc();
};

class PrimersGrouperWorker : public BaseWorker {
    Q_OBJECT
public:
    PrimersGrouperWorker( Actor * p ) : BaseWorker(p), inPort(NULL), outPort(NULL) {};

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();
private:
    IntegralBus* inPort;
    IntegralBus* outPort;
private slots:
    void sl_onTaskFinished(Task* t);
private:
    QList<DNASequence> data;
};

class PrimersGrouperWorkerFactory : public DomainFactory {
public:
    const static QString ACTOR_ID;
    const static QString OUT_FILE;
    PrimersGrouperWorkerFactory() : DomainFactory(ACTOR_ID) {};
    static void init();
    virtual Worker* createWorker( Actor * a ) { return new PrimersGrouperWorker(a); }
};

} //LocalWorkflow namespace

class PrimerGrouperTask : public Task {
    Q_OBJECT
public:
    PrimerGrouperTask(const QString& outputFileUrl, const QList<DNASequence>& sequences);

    void run();
    QString getReport() const { return report; }

    static QString createCell(const QString& value, bool center = false, const QString &attrs = "");
    static QString createColumn(const QString& name, const QString &attrs = "");
private:
    void createReport(const QList<QList<int> >& groups);
    void fillReportTable(const QList<QList<int> >& groups);
    void writeReportToFile();

    QString createRow(const QString& groupName, const QString& forwardName, const QString& reverseName, const QString& forwardTm, const QString& reverseTm);

    bool isCompatiblePairs(int firstPairIndex, int secondPairIndex);
    void findCompatibleGroups();
private:
    typedef QPair<DNASequence, DNASequence> PrimersPair;

    QList<PrimersPair> primerPairs;
    QString     report;
    QString     outputUrl;
};

} //U2 namespace

#endif
