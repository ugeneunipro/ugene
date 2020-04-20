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

#ifndef _U2_HMM_IO_WORKER_H_
#define _U2_HMM_IO_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Core/SaveDocumentTask.h>

namespace U2 {
namespace LocalWorkflow {

#define HMM2_SLOT_ID QString("hmm2-profile")

class HMMLib : public QObject {
    Q_OBJECT
public:
    static const Descriptor HMM_CATEGORY();
    static const Descriptor HMM2_SLOT();
    static DataTypePtr HMM_PROFILE_TYPE();
    static const QString HMM_PROFILE_TYPE_ID;
    static void init();
    static void cleanup();
};

class HMMIOProto : public IntegralBusActorPrototype {
public:
    HMMIOProto(const Descriptor& desc, const QList<PortDescriptor*>& ports, 
        const QList<Attribute*>& attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const = 0;
    bool isAcceptableDrop(const QMimeData*, QVariantMap*, const QString&) const;
};

class ReadHMMProto : public HMMIOProto {
public:
    ReadHMMProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, 
        const QList<Attribute*>& _attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class WriteHMMProto : public HMMIOProto {
public:
    WriteHMMProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, 
        const QList<Attribute*>& _attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class HMMReadPrompter : public PrompterBase<HMMReadPrompter> {
    Q_OBJECT
public:
    HMMReadPrompter(Actor* p = 0) : PrompterBase<HMMReadPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class HMMWritePrompter : public PrompterBase<HMMWritePrompter> {
    Q_OBJECT
public:
    HMMWritePrompter(Actor* p = 0) : PrompterBase<HMMWritePrompter>(p) {}
protected:
    QString composeRichDoc();
};


class HMMReader : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR;
    HMMReader(Actor* a) : BaseWorker(a), output(NULL) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    CommunicationChannel *output;
    QStringList urls;
}; 

class HMMWriter : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR;
    HMMWriter(Actor* a) : BaseWorker(a), input(NULL), done(false), fileMode(SaveDoc_Overwrite) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}

protected:
    CommunicationChannel *input;
    QString url;
    QMap<QString,int> counter;
    bool done;
    uint fileMode;
}; 

class HMMIOWorkerFactory : public DomainFactory {
public:
    static void init();
    static void cleanup();
    HMMIOWorkerFactory(const Descriptor& d) : DomainFactory(d) {}
    virtual Worker* createWorker(Actor* a);
};

} // Workflow namespace
} // U2 namespace

#include <hmmer2/structs.h>
Q_DECLARE_METATYPE(plan7_s *);

#endif

