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

#ifndef _U2_CLASSIFICATION_REPORT_WORKER_H_
#define _U2_CLASSIFICATION_REPORT_WORKER_H_

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrl.h>

#include <U2Formats/StreamSequenceReader.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "TaxonomySupport.h"

namespace U2 {
namespace LocalWorkflow {

//////////////////////////////////////////////////
//ClassificationReportValidator
class ClassificationReportValidator : public ActorValidator {
    Q_DECLARE_TR_FUNCTIONS(ClassificationReportValidator)
public:
    bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const;

private:
    bool validateTaxonomyTree(const Actor *actor, NotificationsList &notificationList) const;
};

//////////////////////////////////////////////////
//ClassificationReport
class ClassificationReportPrompter;
typedef PrompterBase<ClassificationReportPrompter> ClassificationReportBase;

class ClassificationReportPrompter : public ClassificationReportBase {
     Q_DECLARE_TR_FUNCTIONS(ClassificationReportPrompter)
public:
    ClassificationReportPrompter(Actor* p = 0) : ClassificationReportBase(p) {}
protected:
    QString composeRichDoc();
};

class ClassificationReportWorker: public BaseWorker {
    Q_OBJECT
public:
    ClassificationReportWorker(Actor *a);
protected:
    void init();
    Task * tick();
    void cleanup() {}

private:
    IntegralBus *input;
    QString producerClassifyToolName;
    const QString getProducerClassifyToolName() const;
    const QString getReportFilePrefix(const Message &message) const;

private slots:
    void sl_taskFinished(Task *task);
};

class ClassificationReportWorkerFactory : public DomainFactory {
    Q_DECLARE_TR_FUNCTIONS(ClassificationReportWorkerFactory)
    static const QString ACTOR_ID;
public:
    static void init();
    static void cleanup();
    ClassificationReportWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new ClassificationReportWorker(a); }
};

class ClassificationReportTask : public Task {
    Q_OBJECT
public:
    enum SortBy {
        NUMBER_OF_READS,
        TAX_ID
    };
    ClassificationReportTask(const QMap<TaxID,uint> &data, uint totalCount, const QString &reportUrl, bool allTaxa, SortBy sortBy);
    QString getUrl() const {return url;}

private:
    void run();

    QMap<TaxID,uint> data;
    const uint totalCount;
    QString url;
    bool allTaxa;
    SortBy sortBy;
};

} //LocalWorkflow
} //U2

#endif
