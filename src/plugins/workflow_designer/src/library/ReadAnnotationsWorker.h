/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_READ_ANNOTATIONS_WORKER_
#define _U2_READ_ANNOTATIONS_WORKER_

#include "GenericReadActor.h"
#include "GenericReadWorker.h"

namespace U2 {
namespace LocalWorkflow {

class ReadAnnotationsProto : public GenericReadDocProto {
public:
    enum Mode {
        SPLIT,
        MERGE,
        MERGE_FILES
    };

    ReadAnnotationsProto();
};  // ReadAnnotationsProto

class ReadAnnotationsWorker : public GenericDocReader {
    Q_OBJECT
public:
    ReadAnnotationsWorker(Actor* p);
    virtual void init();
    Task *tick() override;

protected:
    virtual void onTaskFinished(Task* task);
    virtual Task* createReadTask(const QString& url, const QString& datasetName);
    virtual QString addReadDbObjectToData(const QString& objUrl, QVariantMap& data);

private:
    void sendData(const QList<QVariantMap>& data);

private:
    ReadAnnotationsProto::Mode mode;
    QList<QVariantMap> datasetData;
};  // ReadAnnotationsWorker

class ReadAnnotationsWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    ReadAnnotationsWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    static void init();
    virtual Worker* createWorker(Actor* a);

};  // ReadAnnotationsWorkerFactory

class ReadAnnotationsTask : public Task {
    Q_OBJECT
public:
    ReadAnnotationsTask(const QString &url,
                        const QString &datasetName,
                        WorkflowContext *context,
                        const ReadAnnotationsProto::Mode &mergeAnnotations,
                        const QString &mergedAnnTableName = QString());
    virtual void prepare();
    virtual void run();
    virtual void cleanup();

    QList<QVariantMap> takeResults();

private:
    QString url;
    QString datasetName;
    ReadAnnotationsProto::Mode mergeAnnotations;
    QString mergedAnnTableName;
    WorkflowContext* context;

    QList<QVariantMap> results;
};

// If the ReadAnnotationsProto::MERGE_FILES mode is selected, then after reading all files, merges all annotation tables
// into one.
class MergeAnnotationsTask : public Task {
    Q_OBJECT
public:
    // datasetData is a list of SharedDbiDataHandlers. These tables will be deleted from the db at the end of the run
    // method. tableName and storage must be non-null.
    MergeAnnotationsTask(const QList<QVariantMap> &datasetData,
                         const QString &mergedAnnTableName,
                         DbiDataStorage *storage);
    void run() override;
    QList<QVariantMap> takeResults() const;

private:
    QList<QVariantMap> datasetData;
    QString mergedAnnTableName;
    DbiDataStorage *storage;
    QList<QVariantMap> results;
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif  // _U2_READ_ANNOTATIONS_WORKER_
