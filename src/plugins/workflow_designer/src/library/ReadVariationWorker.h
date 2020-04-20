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

#ifndef _READ_VARIATION_LIST_WORKER_
#define _READ_VARIATION_LIST_WORKER_

#include "GenericReadActor.h"
#include "GenericReadWorker.h"

namespace U2 {
namespace LocalWorkflow {



class ReadVariationProto : public GenericReadDocProto {
public:
    enum SplitAlleles{
        NOSPLIT,
        SPLIT
    };
    static const QString SPLIT_ATTR;

    ReadVariationProto();
}; // ReadVariationProto

class ReadVariationWorker : public GenericDocReader {
    Q_OBJECT
public:
    ReadVariationWorker(Actor *p);
    virtual void init() ;

protected:
    virtual void onTaskFinished(Task *task);
    virtual QString addReadDbObjectToData(const QString &objUrl, QVariantMap &data);

protected:
    virtual Task * createReadTask(const QString &url, const QString &datasetName);
private:
    ReadVariationProto::SplitAlleles splitMode;
}; // ReadVariationWorker

class ReadVariationWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    ReadVariationWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker * createWorker(Actor *a);

}; // ReadVariationWorkerFactory

class ReadVariationTask : public Task {
    Q_OBJECT
public:
    ReadVariationTask(const QString &url, const QString &datasetName, DbiDataStorage *storage, bool splitAlleles = false);
    virtual ~ReadVariationTask();

    virtual void prepare();
    virtual void run();

    const QString & getUrl() const;
    const QString & getDatasetName() const;
    QList<QVariantMap> takeResults();

private:
    QString url;
    QString datasetName;
    DbiDataStorage *storage;
    bool splitAlleles;
    QList<QVariantMap> results;
};

} // LocalWorkflow
} // U2

#endif // _READ_VARIATION_LIST_WORKER_
