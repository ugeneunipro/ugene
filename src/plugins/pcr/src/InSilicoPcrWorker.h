/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_INSILICO_PCR_WORKER_H_
#define _U2_INSILICO_PCR_WORKER_H_

#include <U2Core/Primer.h>

#include <U2Lang/BaseThroughWorker.h>
#include <U2Lang/WorkflowUtils.h>

#include "InSilicoPcrTask.h"

namespace U2 {
namespace LocalWorkflow {

class InSilicoPcrPrompter : public PrompterBase<InSilicoPcrPrompter> {
    Q_OBJECT
public:
    InSilicoPcrPrompter(Actor* a);

protected:
    QString composeRichDoc();
};

class InSilicoPcrWorkerFactory : public DomainFactory {
public:
    InSilicoPcrWorkerFactory();
    Worker* createWorker(Actor* a);

    static const QString ACTOR_ID;
    static void init();
};

class InSilicoPcrReportTask : public Task {
    Q_OBJECT
public:
    class TableRow {
    public:
        QString sequenceName;
        QMap<int, int> productsNumber;  // pair number -> products count
    };

    InSilicoPcrReportTask(const QList<TableRow>& table, const QList<QPair<Primer, Primer>>& primers, const QString& reportUrl, const QString& primersUrl);
    void run();

private:
    QByteArray chapterName(const QString& name) const;
    QString createReport();
    QByteArray productsTable() const;
    QString primerDetails();
    QByteArray chapterContent(const QByteArray& content) const;
    QByteArray chapter(const QByteArray& name, const QByteArray& content) const;
    QString readHtml() const;

private:
    QList<TableRow> table;
    QList<QPair<Primer, Primer>> primers;
    QString reportUrl;
    QString primersUrl;
};

class InSilicoPcrWorker : public BaseThroughWorker {
    Q_OBJECT
public:
    InSilicoPcrWorker(Actor* a);

protected:
    // BaseThroughWorker
    Task* createPrepareTask(U2OpStatus& os) const;
    void onPrepared(Task* task, U2OpStatus& os);
    Task* onInputEnded();
    QList<Message> fetchResult(Task* task, U2OpStatus& os);
    Task* createTask(const Message& message, U2OpStatus& os);

private:
    void fetchPrimers(const QList<GObject*>& objects, U2OpStatus& os);
    Primer createPrimer(GObject* object, bool& skipped, U2OpStatus& os);
    int createMetadata(const InSilicoPcrTaskSettings& settings, const U2Region& productRegion, int pairNumber);
    QByteArray createReport() const;
    QVariant fetchSequence(Document* doc);
    QVariant fetchAnnotations(Document* doc);

private:
    QList<QPair<Primer, Primer>> primers;
    QList<SharedDbiDataHandler> sequences;
    QList<InSilicoPcrReportTask::TableRow> table;
    bool reported;
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif  // _U2_INSILICO_PCR_WORKER_H_
