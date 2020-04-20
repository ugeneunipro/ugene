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

#include <QApplication>

#include <U2Core/DocumentModel.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/URLAttribute.h>

#include "../util/DatasetValidator.h"
#include "CoreLib.h"
#include "DocActors.h"

namespace U2 {
namespace Workflow {

/****************************
 * DocActorProto
 *****************************/
DocActorProto::DocActorProto(const DocumentFormatId& _fid, const Descriptor& _desc, const QList<PortDescriptor*>& _ports,
                             const QList<Attribute*>& _attrs ) : ReadDbObjActorPrototype(_desc, _ports, _attrs), fid(_fid) {
}

DocActorProto::DocActorProto(const Descriptor& _desc, const GObjectType& t, const QList<PortDescriptor*>& _ports,
                             const QList<Attribute*>& _attrs ) : ReadDbObjActorPrototype(_desc, _ports, _attrs), type(t) {
}

bool DocActorProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params, const QString & urlAttrId ) const {
    QList<DocumentFormat*> fs;
    QString url = WorkflowUtils::getDropUrl(fs, md);
    foreach(DocumentFormat* df, fs) {
        if (fid == df->getFormatId()) {
            if (params) {
                params->insert( urlAttrId, url );
            }
            return true;
        }
    }
    return false;
}

QString DocActorProto::prepareDocumentFilter() {
    if( !fid.isEmpty() ) {
        return FormatUtils::prepareDocumentsFileFilter( fid, true );
    } else {
        assert( !type.isEmpty() );
        return FormatUtils::prepareDocumentsFileFilterByObjType( type, true );
    }
}

/****************************
 * ReadDocActorProto
 *****************************/
ReadDocActorProto::ReadDocActorProto(const DocumentFormatId& _fid, const Descriptor& _desc, const QList<PortDescriptor*>& _ports,
                                     const QList<Attribute*>& _attrs ) : DocActorProto( _fid, _desc, _ports, _attrs ) {
    attrs << new URLAttribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::URL_DATASETS_TYPE(), true);
    setValidator(new DatasetValidator());
}

bool ReadDocActorProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return DocActorProto::isAcceptableDrop( md, params, BaseAttributes::URL_IN_ATTRIBUTE().getId() );
}

/****************************
 * WriteDocActorProto
 *****************************/
WriteDocActorProto::WriteDocActorProto(const DocumentFormatId& _fid, const Descriptor& _desc, const QList<PortDescriptor*>& _ports,
    const QString & portId, const QList<Attribute*>& _attrs, bool canWriteToSharedDB, bool addValidator, bool addPortValidator)
    : DocActorProto( _fid, _desc, _ports, _attrs ), outPortId(portId)
{
    construct(canWriteToSharedDB, addValidator, addPortValidator);
}

WriteDocActorProto::WriteDocActorProto(const Descriptor& _desc, const GObjectType & t, const QList<PortDescriptor*>& _ports,
    const QString & portId, const QList<Attribute*>& _attrs, bool canWriteToSharedDB, bool addValidator, bool addPortValidator)
    : DocActorProto(_desc, t, _ports, _attrs), outPortId(portId)
{
    construct(canWriteToSharedDB, addValidator, addPortValidator);
}

bool WriteDocActorProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return DocActorProto::isAcceptableDrop( md, params, BaseAttributes::URL_OUT_ATTRIBUTE().getId() );
}

void WriteDocActorProto::construct(bool canWriteToSharedDb, bool addValidator, bool addPortValidator) {
    QMap< QString, PropertyDelegate* > delegateMap;

    if (canWriteToSharedDb) {
        attrs.prepend(new Attribute(BaseAttributes::DATA_STORAGE_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        Attribute *dbAttr = new Attribute(BaseAttributes::DATABASE_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
        dbAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        attrs << dbAttr;
        Attribute *dbPathAttr = new Attribute(BaseAttributes::DB_PATH(), BaseTypes::STRING_TYPE(), true, U2ObjectDbi::ROOT_FOLDER);
        dbPathAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        attrs << dbPathAttr;

        delegateMap[BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId()] = new ComboBoxDelegate(BaseAttributes::DATA_STORAGE_ATTRIBUTE_VALUES_MAP());
        delegateMap[BaseAttributes::DATABASE_ATTRIBUTE().getId()] = new ComboBoxWithDbUrlsDelegate;
    }

    urlAttr = new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false );
    attrs << urlAttr;
    Attribute *suffixAttr = new Attribute(BaseAttributes::URL_SUFFIX(), BaseTypes::STRING_TYPE(), false);
    attrs << suffixAttr;
    Attribute *fileModeAttr = new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);
    attrs << fileModeAttr;

    if (canWriteToSharedDb) {
        urlAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        suffixAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        fileModeAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
    }

    delegateMap[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(prepareDocumentFilter(), QString(), false, false, true, 0, fid);
    delegateMap[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(attrs.size() > 2);

    setEditor(new DelegateEditor(delegateMap));
    if (addPortValidator) {
        setPortValidator(outPortId, new ScreenedSlotValidator(BaseSlots::URL_SLOT().getId()));
    }
    if (addValidator) {
        setValidator(new ScreenedParamValidator(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ports.first()->getId(), BaseSlots::URL_SLOT().getId()));
    }
}

/****************************
 * WriteGenbankPrompter
 *****************************/
QString WriteGenbankPrompter::composeRichDoc() {
    QString outPortId = target->getInputPorts().first()->getId();
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(outPortId));
    Actor* seqProducer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString seqName = seqProducer ? tr(" sequence from <u>%1</u>").arg(seqProducer->getLabel()) : "";
    QString annName = getProducers(outPortId, BaseSlots::ANNOTATION_TABLE_SLOT().getId());
    if (!annName.isEmpty()) {
        annName = tr(" set of annotations from <u>%1</u>").arg(annName);
    }

    QString url = getScreenedURL( qobject_cast<IntegralBusPort*>(target->getPort(outPortId)),
        BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId() );
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);

    QString data;
    if (seqName.isEmpty() && annName.isEmpty()) {
        QString doc = tr("Write sequence(s) in Genbank format to <u>%1</u>.").arg(url);
        return doc;
    } else if (!seqName.isEmpty() && !annName.isEmpty()) {
        data = tr("each %1 and %2").arg(seqName).arg(annName);
    } else {
        data = tr("each ") + seqName + annName;
    }

    return tr("Write %1 in Genbank format, to <u>%2</u>.")
        .arg(data)
        .arg(url);
}

/****************************
 * WriteFastaPrompter
 *****************************/
namespace {

const QString generatedStr = "<font color='blue'>" + QApplication::translate("WriteDocPrompter", "default file") + "</font>";

}
QString WriteFastaPrompter::composeRichDoc() {
    QString outPortId = target->getInputPorts().first()->getId();
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(outPortId));
    QString url = getScreenedURL(qobject_cast<IntegralBusPort*>(target->getPort(outPortId)),
        BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId(), generatedStr);
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);

    Actor* seqProducer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    if (!seqProducer) {
        QString doc = tr("Writes sequence(s) in %1 format to <u>%2</u>.").arg(format).arg(url);
        return doc;
    }

    QString doc = tr("Writes sequence(s) from <u>%1</u> in %2 format to  <u>%3</u>.")
        .arg(seqProducer->getLabel())
        .arg(format)
        .arg(url);

    return doc;
}

ActorDocument* WriteFastaPrompter::createDescription(Actor* a) {
    WriteFastaPrompter* doc = new WriteFastaPrompter(format, a);
    doc->connect(a, SIGNAL(si_labelChanged()), SLOT(sl_actorModified()));
    doc->connect(a, SIGNAL(si_modified()), SLOT(sl_actorModified()));
    foreach(Workflow::Port* input, a->getInputPorts()) {
        doc->connect(input, SIGNAL(bindingChanged()), SLOT(sl_actorModified()));
    }
    return doc;
}

/****************************
 * WriteDocPrompter
 *****************************/
QString WriteDocPrompter::composeRichDoc() {
    const QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";

    const QString outPortId = target->getInputPorts().first()->getId();
    Attribute *dataStorageAttr = target->getParameter(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());
    SAFE_POINT(NULL != dataStorageAttr, "Invalid attribute", QString());
    const QVariant dataStorage = dataStorageAttr->getAttributePureValue();

    QString url;
    const bool storeToDb = dataStorage == BaseAttributes::SHARED_DB_DATA_STORAGE();
    if (storeToDb) {
        Attribute *dbPathAttr = target->getParameter(BaseAttributes::DB_PATH().getId());
        SAFE_POINT(NULL != dbPathAttr, "Invalid attribute", QString());
        url = dbPathAttr->getAttributePureValue().toString();
        url = getHyperlink(BaseAttributes::DB_PATH().getId(), url);
    } else if (dataStorage == BaseAttributes::LOCAL_FS_DATA_STORAGE()) {
        url = getScreenedURL(qobject_cast<IntegralBusPort*>(target->getPort(outPortId)), BaseAttributes::URL_OUT_ATTRIBUTE().getId(),
            BaseSlots::URL_SLOT().getId(), generatedStr);
        url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    } else {
        FAIL("Unexpected attribute value", QString());
    }

    QString result = spec;
    QString producers = getProducers(outPortId, slot);
    if (producers.isEmpty()) {
        producers = unsetStr;
    }
    if (storeToDb) {
        Attribute *dbAttr = target->getParameter(BaseAttributes::DATABASE_ATTRIBUTE().getId());
        SAFE_POINT(NULL != dbAttr, "Invalid attribute", QString());
        const QString dbUrl = dbAttr->getAttributePureValue().toString();
        QString dbName = SharedDbUrlUtils::getDbShortNameFromEntityUrl(dbUrl);
        dbName = dbName.isEmpty() ? unsetStr : getHyperlink(BaseAttributes::DATABASE_ATTRIBUTE().getId(), dbName);

        result = spec.left(spec.size() - 1); // remove last dot
        result += tr(" in the ") + "<u>%3</u>" + tr(" database.");
        return result.arg(producers).arg(url).arg(dbName);
    } else {
        return result.arg(producers).arg(url);
    }
}

/****************************
 * ReadDocPrompter
 *****************************/
QString ReadDocPrompter::composeRichDoc() {
    if (spec.contains("%1")) {
        const QString& id = BaseAttributes::URL_IN_ATTRIBUTE().getId();
        return spec.arg(getHyperlink(id, getURL(id)));
    } else {
        return spec;
    }
}

}//namespace Workflow
}//namespace U2
