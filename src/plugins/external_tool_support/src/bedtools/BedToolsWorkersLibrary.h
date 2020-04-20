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

#ifndef _U2_BED_TOOLS_WORKERS_LIBRARY_
#define _U2_BED_TOOLS_WORKERS_LIBRARY_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/BaseNGSWorker.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

//////////////////////////////////////////////////
//Slopbed
class SlopbedPrompter;
typedef PrompterBase<SlopbedPrompter> SlopbedBase;
class SlopbedPrompter : public SlopbedBase {
    Q_OBJECT
public:
    SlopbedPrompter(Actor* p = 0) : SlopbedBase(p) {}
protected:
    QString composeRichDoc();
}; //SlopbedPrompter

class SlopbedWorker: public BaseNGSWorker {
    Q_OBJECT
public:
    SlopbedWorker(Actor *a);
protected:
    QVariantMap getCustomParameters() const;
    QString getDefaultFileName() const;
    Task *getTask(const BaseNGSSetting &settings) const;
}; //SlopbedWorker

class SlopbedWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    SlopbedWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new SlopbedWorker(a); }
}; //SlopbedWorkerFactory

class SlopbedTask : public BaseNGSTask{
    Q_OBJECT
public:
    SlopbedTask (const BaseNGSSetting &settings);

protected:
    bool filterLines;
    void prepareStep();
    void finishStep();
    QStringList getParameters(U2OpStatus& os);
};

//////////////////////////////////////////////////
//genomecov
class GenomecovPrompter;
typedef PrompterBase<GenomecovPrompter> GenomecovBase;
class GenomecovPrompter : public GenomecovBase {
    Q_OBJECT
public:
    GenomecovPrompter(Actor* p = 0) : GenomecovBase(p) {}
protected:
    QString composeRichDoc();
}; //GenomecovPrompter

class GenomecovWorker: public BaseNGSWorker {
    Q_OBJECT
public:
    GenomecovWorker(Actor *a);
protected:
    QVariantMap getCustomParameters() const;
    QString getDefaultFileName() const;
    Task *getTask(const BaseNGSSetting &settings) const;
}; //GenomecovWorker

class GenomecovWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    GenomecovWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new GenomecovWorker(a); }
}; //GenomecovWorkerFactory

class GenomecovTask : public BaseNGSTask{
    Q_OBJECT
public:
    GenomecovTask (const BaseNGSSetting &settings);

protected:
    void prepareStep();
    QStringList getParameters(U2OpStatus& os);
};

//////////////////////////////////////////////////
//dedtools intersect
class BedtoolsIntersectWorker : public BaseWorker {
    Q_OBJECT
public:
    BedtoolsIntersectWorker(Actor *a);
    virtual void init();
    virtual Task * tick();
    virtual void cleanup() {}
    virtual bool isReady() const;
private slots:
    void sl_taskFinished(Task *task);
private:
    Task* createTask();
    QList<U2EntityRef> getAnnotationsEntityRefFromMessages(const QList<Message>& mList, const QString& portId);
    U2EntityRef getAnnotationsEntityRef(const Message& m, const QString& portId, U2OpStatus &os);
    void storeMessages(IntegralBus* bus, QList<Message>& store);
    IntegralBus* inputA;
    IntegralBus* inputB;
    IntegralBus* output;

    QList<Message> storeA;
    QList<Message> storeB;
}; //BedtoolsIntersectWorker

class BedtoolsIntersectWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    BedtoolsIntersectWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor *a) { return new BedtoolsIntersectWorker(a); }
}; //BedtoolsIntersectWorkerFactory

class BedtoolsIntersectPrompter : public PrompterBase<BedtoolsIntersectPrompter> {
    Q_OBJECT
public:
    BedtoolsIntersectPrompter(Actor* p = 0) : PrompterBase<BedtoolsIntersectPrompter>(p) {}
protected:
    QString composeRichDoc();
}; //BedtoolsIntersectPrompter


} //LocalWorkflow
} //U2

#endif //_U2_BED_TOOLS_WORKERS_LIBRARY_
