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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/AbstractVariationFormat.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "library/DocActors.h"

#include "WriteVariationWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString WriteVariationWorkerFactory::ACTOR_ID("write-variations");

/************************************************************************/
/* Worker */
/************************************************************************/
WriteVariationWorker::WriteVariationWorker(Actor *p, const DocumentFormatId& fid)
: BaseDocWriter(p, fid)
{

}

void WriteVariationWorker::data2doc(Document *doc, const QVariantMap &data) {
    Q_UNUSED(doc);
    Q_UNUSED(data);
    SAFE_POINT(false, tr("Write variants: internal error"), );
}

void WriteVariationWorker::storeEntry(IOAdapter *io, const QVariantMap &data, int entryNum) {
    CHECK(hasDataToWrite(data), );
    U2OpStatusImpl os;
    QScopedPointer<VariantTrackObject> trackObj(NULL);
    {
        SharedDbiDataHandler objId = data.value(BaseSlots::VARIATION_TRACK_SLOT().getId())
            .value<SharedDbiDataHandler>();
        trackObj.reset(StorageUtils::getVariantTrackObject(context->getDataStorage(), objId));
        SAFE_POINT(!trackObj.isNull(), "Can't get track object", );
    }

    QMap< GObjectType, QList<GObject*> > objectsMap;
    {
        QList<GObject*> tracks; tracks << trackObj.data();
        objectsMap[GObjectTypes::VARIANT_TRACK] = tracks;
    }
    if(1 == entryNum) {
        AbstractVariationFormat* variationFormat = qobject_cast<AbstractVariationFormat*>(format);
        if(NULL != variationFormat) {
            variationFormat->storeHeader(trackObj.data(), io, os);
            SAFE_POINT_OP(os, );
        }
    }
    format->storeEntry(io, objectsMap, os);
    SAFE_POINT_OP(os, );
}

bool WriteVariationWorker::hasDataToWrite(const QVariantMap &data) const {
    return data.contains(BaseSlots::VARIATION_TRACK_SLOT().getId());
}

QSet<GObject *> WriteVariationWorker::getObjectsToWrite(const QVariantMap &data) const {
    SharedDbiDataHandler objId = data[BaseSlots::VARIATION_TRACK_SLOT().getId()].value<SharedDbiDataHandler>();
    return QSet<GObject *>() << StorageUtils::getVariantTrackObject(context->getDataStorage(), objId);
}

/************************************************************************/
/* Factory */
/************************************************************************/
void WriteVariationWorkerFactory::init() {
    DocumentFormatConstraints constr;
    constr.supportedObjectTypes.insert(GObjectTypes::VARIANT_TRACK);
    constr.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    constr.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> supportedFormats = AppContext::getDocumentFormatRegistry()->selectFormats(constr);

    if(!supportedFormats.isEmpty()) {
        DocumentFormatId format = supportedFormats.contains(BaseDocumentFormats::SNP) ? BaseDocumentFormats::SNP : supportedFormats.first();

        Descriptor inDesc(BasePorts::IN_VARIATION_TRACK_PORT_ID(),
            WriteVariationWorker::tr("Variation track"),
            WriteVariationWorker::tr("Variation track"));
        Descriptor protoDesc(WriteVariationWorkerFactory::ACTOR_ID,
            WriteVariationWorker::tr("Write Variants"),
            WriteVariationWorker::tr("The element gets message(s) with variations data and saves the data"
                                     " to the specified file(s) in one of the appropriate formats"
                                     " (e.g. VCF)."));

        QList<PortDescriptor*> portDescs;
        {
            QMap<Descriptor, DataTypePtr> inTypeMap;
            Descriptor writeUrlD(BaseSlots::URL_SLOT().getId(),
                WriteVariationWorker::tr("Location"),
                WriteVariationWorker::tr("Location for writing data"));
            inTypeMap[writeUrlD] = BaseTypes::STRING_TYPE();
            inTypeMap[BaseSlots::VARIATION_TRACK_SLOT()] = BaseTypes::VARIATION_TRACK_TYPE();
            DataTypePtr writeVariationsType(new MapDataType(BasePorts::IN_VARIATION_TRACK_PORT_ID(), inTypeMap));

            portDescs << new PortDescriptor(inDesc, writeVariationsType, true);
        }

        QList<Attribute*> attrs;
        Attribute *docFormatAttr = NULL;
        {
            Attribute *accumulateAttr = new Attribute(BaseAttributes::ACCUMULATE_OBJS_ATTRIBUTE(), BaseTypes::BOOL_TYPE(), false, true);
            accumulateAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
            attrs << accumulateAttr;

            docFormatAttr = new Attribute(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, format);
            docFormatAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
            attrs << docFormatAttr;
        }

        WriteDocActorProto *childProto = new WriteDocActorProto(format, protoDesc, portDescs, inDesc.getId(), attrs, true, false);
        IntegralBusActorPrototype *proto = childProto;
        docFormatAttr->addRelation(new FileExtensionRelation(childProto->getUrlAttr()->getId()));

        QVariantMap formatsMap;
        foreach (const DocumentFormatId &fid, supportedFormats) {
            formatsMap[AppContext::getDocumentFormatRegistry()->getFormatById(fid)->getFormatName()] = fid;
        }
        proto->getEditor()->addDelegate(new ComboBoxDelegate(formatsMap), BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
        proto->setPrompter(new WriteDocPrompter(WriteVariationWorker::tr("Save all variations from <u>%1</u> to <u>%2</u>."),
            BaseSlots::VARIATION_TRACK_SLOT().getId()));

        WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASINK(), proto);
        WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new WriteVariationWorkerFactory());
    }
}

Worker *WriteVariationWorkerFactory::createWorker(Actor *a) {
    Attribute *formatAttr = a->getParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
    QString fid = formatAttr->getAttributePureValue().toString();
    return new WriteVariationWorker(a, fid);
}

}
} // U2
