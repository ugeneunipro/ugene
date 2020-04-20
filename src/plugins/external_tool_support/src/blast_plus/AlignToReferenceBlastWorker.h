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

#ifndef _U2_ALIGN_TO_REFERENCE_BLAST_WORKER_H_
#define _U2_ALIGN_TO_REFERENCE_BLAST_WORKER_H_

#include <U2Lang/BaseDatasetWorker.h>
#include <U2Lang/WorkflowUtils.h>


namespace U2 {

namespace Workflow {
class BlastReadsSubTask;
class FormatDBSubTask;
class ComposeResultSubTask;
}

namespace LocalWorkflow {

/************************************************************************/
/* AlignToReferenceBlastTask */
/************************************************************************/
class AlignToReferenceBlastTask : public Task {
    Q_OBJECT
public:
    AlignToReferenceBlastTask(const QString& refUrl,
                              const QString &resultUrl,
                              const SharedDbiDataHandler &reference,
                              const QList<SharedDbiDataHandler> &reads,
                              const QMap<SharedDbiDataHandler, QString> &readsNames,
                              int minIdentityPercent,
                              DbiDataStorage *storage);
    QString getResultUrl() const;
    SharedDbiDataHandler getAnnotations() const;
    QList<QPair<QString, QPair<int, bool> > > getAcceptedReads() const;
    QList<QPair<QString, int> > getDiscardedReads() const;

private:
    void prepare();
    QString generateReport() const;
    QList<Task*> onSubTaskFinished(Task *subTask);
    ReportResult report();

    const QString referenceUrl;
    const QString resultUrl;
    const SharedDbiDataHandler reference;
    const QList<SharedDbiDataHandler> reads;
    const QMap<SharedDbiDataHandler, QString> readsNames;
    const int minIdentityPercent;

    FormatDBSubTask* formatDbSubTask;
    BlastReadsSubTask* blastTask;
    ComposeResultSubTask *composeSubTask;
    SaveDocumentTask *saveTask;

    DbiDataStorage *storage;
};

/************************************************************************/
/* AlignToReferenceBlastPrompter */
/************************************************************************/
class AlignToReferenceBlastPrompter : public PrompterBase<AlignToReferenceBlastPrompter> {
    Q_OBJECT
public:
    AlignToReferenceBlastPrompter(Actor *a);

protected:
    QString composeRichDoc();
};

/************************************************************************/
/* AlignToReferenceBlastWorker */
/************************************************************************/
class AlignToReferenceBlastWorker : public BaseDatasetWorker {
    Q_OBJECT
public:
    AlignToReferenceBlastWorker(Actor *a);

protected:
    Task * createPrepareTask(U2OpStatus &os) const override;
    void onPrepared(Task *task, U2OpStatus &os) override;

    Task * createTask(const QList<Message> &messages) const override;
    QVariantMap getResult(Task *task, U2OpStatus &os) const override;
    MessageMetadata generateMetadata(const QString &datasetName) const override;

private:
    QString getReadName(const Message &message) const;

    SharedDbiDataHandler reference;
    QString referenceUrl;
};

/************************************************************************/
/* AlignToReferenceBlastWorkerFactory */
/************************************************************************/
class AlignToReferenceBlastWorkerFactory : public DomainFactory {
public:
    AlignToReferenceBlastWorkerFactory();
    Worker * createWorker(Actor *a);

    static void init();

    static const QString ACTOR_ID;
    static const QString ROW_NAMING_SEQUENCE_NAME;
    static const QString ROW_NAMING_FILE_NAME;
    static const QString ROW_NAMING_SEQUENCE_NAME_VALUE;
    static const QString ROW_NAMING_FILE_NAME_VALUE;
};

} // LocalWorkflow
} // U2

#endif // _U2_ALIGN_TO_REFERENCE_BLAST_WORKER_H_
