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

#ifndef _U2_FILTER_ANNOTATIONS_WORKER_H_
#define _U2_FILTER_ANNOTATIONS_WORKER_H_

#include <U2Core/AnnotationData.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>


namespace U2 {
namespace LocalWorkflow {

class FilterAnnotationsPrompter : public PrompterBase<FilterAnnotationsPrompter> {
    Q_OBJECT
public:
    FilterAnnotationsPrompter(Actor* p = 0) : PrompterBase<FilterAnnotationsPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class FilterAnnotationsWorker : public BaseWorker {
    Q_OBJECT
public:
    FilterAnnotationsWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {};

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();
private slots:
    void sl_taskFinished(Task *t);
private:
    IntegralBus *input;
    IntegralBus *output;
};

class FilterAnnotationsWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    FilterAnnotationsWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) { return new FilterAnnotationsWorker(a); }
};

class FilterAnnotationsValidator : public ActorValidator {
public:
    bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const;
};

class FilterAnnotationsTask : public Task {
    Q_OBJECT
public:
    FilterAnnotationsTask(const QList<SharedAnnotationData> &annotations, const QString &namesString, const QString &namesUrl, bool accept);

    void run();

    QList<SharedAnnotationData> takeResult();

private:
    QStringList readAnnotationNames(U2OpStatus &os) const;

private:
    QList<SharedAnnotationData> annotations;
    QString namesString;
    QString namesUrl;
    bool accept;
};

} // LocalWorkflow namespace
} // U2 namespace

#endif
