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

#ifndef _U2_WORKFLOW_DOC_ACTORS_H_
#define _U2_WORKFLOW_DOC_ACTORS_H_

#include <U2Core/global.h>

#include <U2Lang/Attribute.h>
#include <U2Lang/ReadDbObjActorPrototype.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/BaseDocumentFormats.h>

#include <U2Designer/DelegateEditors.h>

namespace U2 {
namespace Workflow {

class DocActorProto : public ReadDbObjActorPrototype {
public:
    DocActorProto(const DocumentFormatId& _fid, const Descriptor& desc, const QList<PortDescriptor*>& ports,
        const QList<Attribute*>& attrs = QList<Attribute*>());
    DocActorProto(const Descriptor& desc, const GObjectType& t, const QList<PortDescriptor*>& ports,
        const QList<Attribute*>& attrs = QList<Attribute*>());

    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const = 0;
    bool isAcceptableDrop(const QMimeData*, QVariantMap*, const QString & urlAttrId ) const;
protected:
    QString prepareDocumentFilter();

protected:
    DocumentFormatId fid;
    GObjectType type;
};

class ReadDocActorProto : public DocActorProto {
public:
    ReadDocActorProto( const DocumentFormatId& fid, const Descriptor& desc, const QList<PortDescriptor*>& ports,
        const QList<Attribute*>& attrs = QList<Attribute*>() );
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class WriteDocActorProto : public DocActorProto {
public:
    WriteDocActorProto(const DocumentFormatId &fid, const Descriptor &desc, const QList<PortDescriptor *> &ports, const QString &portId,
        const QList<Attribute *> &attrs = QList<Attribute *>(), bool canWriteToSharedDB = true, bool addValidator = true, bool addPortValidator = true );
    WriteDocActorProto(const Descriptor &desc, const GObjectType &t, const QList<PortDescriptor *> &ports, const QString &portId,
        const QList<Attribute *> &attrs = QList<Attribute *>(), bool canWriteToSharedDB = true, bool addValidator = true, bool addPortValidator = true );

    Attribute * getUrlAttr() {return urlAttr;}

private:
    void construct(bool canWriteToSharedDb, bool addValidator, bool addPortValidator);
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
    Attribute *urlAttr;

private:
    QString outPortId;
};

class ReadDocPrompter;
typedef PrompterBase<ReadDocPrompter> ReadDocPrompterBase;

class ReadDocPrompter : public ReadDocPrompterBase {
    Q_OBJECT
public:
    ReadDocPrompter(const QString& s) : spec(s) {}
    ReadDocPrompter(Actor* p = 0) : ReadDocPrompterBase(p) {}
    virtual ActorDocument* createDescription(Actor* a) {
        ReadDocPrompter* doc = static_cast<ReadDocPrompter*>(ReadDocPrompterBase::createDescription(a));
        doc->spec = this->spec;
        return doc;
    }
protected:
    QString composeRichDoc();
    QString spec;
};

class WriteDocPrompter;
typedef PrompterBase<WriteDocPrompter> WriteDocPrompterBase;

class WriteDocPrompter : public WriteDocPrompterBase {
    Q_OBJECT
public:
    WriteDocPrompter(const QString& spec, const QString& slot) : spec(spec), slot(slot){}
    WriteDocPrompter(Actor* p = 0) : WriteDocPrompterBase(p) {}
    virtual ActorDocument* createDescription(Actor* a) {
        WriteDocPrompter* doc = static_cast<WriteDocPrompter*>(WriteDocPrompterBase::createDescription(a));
        doc->spec = this->spec;
        doc->slot = this->slot;
        return doc;
    }
protected:
    QString composeRichDoc();
    QString spec,slot;
};

class WriteGenbankPrompter : public PrompterBase<WriteGenbankPrompter> {
    Q_OBJECT
public:
    WriteGenbankPrompter(Actor* p = 0) : PrompterBase<WriteGenbankPrompter>(p){}

protected:
    QString composeRichDoc();
};

class WriteFastaPrompter : public PrompterBaseImpl {
    Q_OBJECT
public:
    WriteFastaPrompter(const QString & formatId, Actor* p = 0) : PrompterBaseImpl(p), format(formatId){}

    virtual QString composeRichDoc();
    virtual ActorDocument * createDescription(Actor*);

private:
    QString format;
};

} // Workflow namespace
} // U2 namespace

#endif
