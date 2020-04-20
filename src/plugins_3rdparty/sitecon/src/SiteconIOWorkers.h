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

#ifndef _U2_SITECON_IO_WORKER_H_
#define _U2_SITECON_IO_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "SiteconAlgorithm.h"

#include <U2Core/SaveDocumentTask.h>

Q_DECLARE_METATYPE(U2::SiteconModel)

namespace U2 {
namespace LocalWorkflow {

class SiteconIOProto : public IntegralBusActorPrototype {
public:
    SiteconIOProto(const Descriptor& desc, const QList<PortDescriptor*>& ports, 
        const QList<Attribute*>& attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const = 0;
    bool isAcceptableDrop(const QMimeData*, QVariantMap*, const QString & urlAttrId ) const;
};

class ReadSiteconProto : public SiteconIOProto {
public:
    ReadSiteconProto(const Descriptor& desc, const QList<PortDescriptor*>& ports, 
        const QList<Attribute*>& attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class WriteSiteconProto : public SiteconIOProto {
public:
    WriteSiteconProto(const Descriptor& desc, const QList<PortDescriptor*>& ports, 
        const QList<Attribute*>& attrs = QList<Attribute*>());
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class SiteconReadPrompter : public PrompterBase<SiteconReadPrompter> {
    Q_OBJECT
public:
    SiteconReadPrompter(Actor* p = 0) : PrompterBase<SiteconReadPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class SiteconWritePrompter : public PrompterBase<SiteconWritePrompter> {
    Q_OBJECT
public:
    SiteconWritePrompter(Actor* p = 0) : PrompterBase<SiteconWritePrompter>(p) {}
protected:
    QString composeRichDoc();
};


class SiteconReader : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    SiteconReader(Actor* a) : BaseWorker(a), output(NULL) {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    CommunicationChannel *output;
    QStringList urls;
    QList<Task*> tasks;
    DataTypePtr mtype;
}; 

class SiteconWriter : public BaseWorker {
    Q_OBJECT
public:
    static const QString ACTOR_ID;
    SiteconWriter(Actor* a) : BaseWorker(a), input(NULL), done(false), fileMode(SaveDoc_Overwrite) {}
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

} // Workflow namespace
} // U2 namespace

#endif

