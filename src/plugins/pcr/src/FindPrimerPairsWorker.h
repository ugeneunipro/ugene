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

#ifndef _U2_FIND_PRIMER_PAIRS_WORKER_H_
#define _U2_FIND_PRIMER_PAIRS_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/DNASequence.h>

#include "PrimerStatistics.h"

namespace U2 {
namespace LocalWorkflow {

class FindPrimerPairsPromter : public PrompterBase<FindPrimerPairsPromter> {
    Q_OBJECT
public:
    FindPrimerPairsPromter( Actor * p = 0 ) : PrompterBase<FindPrimerPairsPromter>(p) {};
protected:
    QString composeRichDoc();
};

class FindPrimerPairsWorker : public BaseWorker {
    Q_OBJECT
public:
    FindPrimerPairsWorker( Actor * p ) : BaseWorker(p), inPort(NULL), outPort(NULL) {};

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

class FindPrimerPairsWorkerFactory : public DomainFactory {
public:
    const static QString ACTOR_ID;
    const static QString OUT_FILE;
    FindPrimerPairsWorkerFactory() : DomainFactory(ACTOR_ID) {};
    static void init();
    virtual Worker* createWorker( Actor * a ) { return new FindPrimerPairsWorker(a); }
};

} //LocalWorkflow namespace

class FindPrimersTask : public Task {
    Q_OBJECT
public:
    FindPrimersTask(const QString& outputFileUrl, const QList<DNASequence>& sequences);

    void run();
    QString getReport() const { return report; }

private:
    void createReport();
    void writeReportToFile();

    QString createRow(const QString& forwardName, const QString& reverseName, double forwardTm, double reverseTm);
    QString createCell(const QString& value);
    QString createColumn(const QString& name);
private:
    QList<DNASequence> sequences;
    QString     report;
    QString     outputUrl;
    QStringList rows;
};

} //U2 namespace

#endif
