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

#ifndef _U2_REMOTE_DB_FETCHER_H_
#define _U2_REMOTE_DB_FETCHER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class RemoteDBFetcherPrompter : public PrompterBase<RemoteDBFetcherPrompter>
{
    Q_OBJECT

public:
    RemoteDBFetcherPrompter(Actor *p = 0) : PrompterBase<RemoteDBFetcherPrompter>(p) {}

protected:
    virtual QString composeRichDoc();
};

class RemoteDBFetcherWorker : public BaseWorker
{
    Q_OBJECT

public:
    RemoteDBFetcherWorker (Actor *a);

    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual bool isDone() const;
    virtual void cleanup();

private slots:
    void sl_taskFinished();

protected:
    QString nextId();
    QString getIdFromList();
    QString getIdFromFile();

    CommunicationChannel *output;

    QString dbid;

    QString idsSource;
    QStringList idsFilePaths;
    QStringList seqids;

    QString fullPathDir;
};

class RemoteDBFetcherFactory : public DomainFactory
{
public:
    static const QString ACTOR_ID;
    static const QMap<QString, QString> cuteDbNames;
    static const QString idsListString;
    static const QString localFileString;

private:
    static QMap<QString, QString> initCuteDbNames();

public:
    RemoteDBFetcherFactory() : DomainFactory(ACTOR_ID) {}

    static void init();
    virtual Worker* createWorker(Actor *a) { return new RemoteDBFetcherWorker(a); }
};


class FetchSequenceByIdFromAnnotationPrompter : public PrompterBase<FetchSequenceByIdFromAnnotationPrompter>
{
    Q_OBJECT

public:
    FetchSequenceByIdFromAnnotationPrompter(Actor *p = 0) : PrompterBase<FetchSequenceByIdFromAnnotationPrompter>(p) {}

protected:
    virtual QString composeRichDoc();
};

class FetchSequenceByIdFromAnnotationWorker : public BaseWorker
{
    Q_OBJECT

public:
    FetchSequenceByIdFromAnnotationWorker (Actor *a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished();

protected:
    IntegralBus *input;
    IntegralBus *output;
    QString fullPathDir;
    QString dbId;
};

class FetchSequenceByIdFromAnnotationFactory : public DomainFactory
{
public:
    static const QString ACTOR_ID;

public:
    FetchSequenceByIdFromAnnotationFactory() : DomainFactory(ACTOR_ID) {}

    static void init();
    virtual Worker* createWorker(Actor *a) { return new FetchSequenceByIdFromAnnotationWorker(a); }
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif  // #ifndef _U2_REMOTE_DB_FETCHER_H_
