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

#include "SiteconIOWorkers.h"
#include "SiteconWorkers.h"
#include "SiteconSearchDialogController.h"
#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2SafePoints.h>
#include "SiteconIO.h"

/* TRANSLATOR U2::SiteconIO */

namespace U2 {
namespace LocalWorkflow {

const QString SiteconReader::ACTOR_ID("sitecon-read");
const QString SiteconWriter::ACTOR_ID("sitecon-write");

static const QString SITECON_OUT_PORT_ID("out-sitecon");
static const QString SITECON_IN_PORT_ID("in-sitecon");

const QString SiteconWorkerFactory::SITECON_MODEL_TYPE_ID("sitecon.model");

DataTypePtr const SiteconWorkerFactory::SITECON_MODEL_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(SITECON_MODEL_TYPE_ID, SiteconIO::tr("Sitecon model"), "")));
        startup = false;
    }
    return dtr->getById(SITECON_MODEL_TYPE_ID);
}

const Descriptor SiteconWorkerFactory::SITECON_SLOT("sitecon-model", "Sitecon model", "");

const Descriptor SiteconWorkerFactory::SITECON_CATEGORY() {return Descriptor("hsitecon", SiteconIO::tr("SITECON"), "");}

SiteconIOProto::SiteconIOProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: IntegralBusActorPrototype(_desc, _ports, _attrs) {
}

bool SiteconIOProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params, const QString & urlAttrId ) const {
    if (md->hasUrls()) {
        QList<QUrl> urls = md->urls();
        if (urls.size() == 1)
        {
            QString url = urls.at(0).toLocalFile();
            QString ext = GUrlUtils::getUncompressedExtension(GUrl(url, GUrl_File));
            if (SiteconIO::SITECON_EXT == ext) {
                if (params) {
                    params->insert(urlAttrId, url);
                }
                return true;
            }
        }
    }
    return false;
}

ReadSiteconProto::ReadSiteconProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: SiteconIOProto( _desc, _ports, _attrs ) {
    
    attrs << new Attribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(SiteconIO::getFileFilter(), SiteconIO::SITECON_ID, true, false, false);
    setEditor(new DelegateEditor(delegateMap));
    setIconPath(":sitecon/images/sitecon.png");
}

bool ReadSiteconProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return SiteconIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_IN_ATTRIBUTE().getId() );
}

WriteSiteconProto::WriteSiteconProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: SiteconIOProto( _desc, _ports, _attrs ) {
    attrs << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true );
    attrs << new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);

    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(SiteconIO::getFileFilter(), SiteconIO::SITECON_ID);
    delegateMap[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(false);
    
    setEditor(new DelegateEditor(delegateMap));
    setIconPath(":sitecon/images/sitecon.png");
    setValidator(new ScreenedParamValidator(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ports.first()->getId(), BaseSlots::URL_SLOT().getId()));
    setPortValidator(SITECON_IN_PORT_ID, new ScreenedSlotValidator(BaseSlots::URL_SLOT().getId()));
}

bool WriteSiteconProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return SiteconIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_OUT_ATTRIBUTE().getId() );
}

void SiteconWorkerFactory::init() 
{
    ActorPrototypeRegistry* r = WorkflowEnv::getProtoRegistry();
    assert(r);
    {        
        QMap<Descriptor, DataTypePtr> m;
        m[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        m[SiteconWorkerFactory::SITECON_SLOT] = SiteconWorkerFactory::SITECON_MODEL_TYPE();
        DataTypePtr t(new MapDataType(Descriptor("write.sitecon.content"), m));

        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(SITECON_IN_PORT_ID, SiteconIO::tr("Sitecon model"), SiteconIO::tr("Input Sitecon model"));
        p << new PortDescriptor(pd, t, true /*input*/);
        Descriptor desc(SiteconWriter::ACTOR_ID, SiteconIO::tr("Write SITECON Model"), SiteconIO::tr("Saves all input SITECON profiles to specified location."));
        IntegralBusActorPrototype* proto = new WriteSiteconProto(desc, p, a);
        proto->setPrompter(new SiteconWritePrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }
    {
        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(SITECON_OUT_PORT_ID, SiteconIO::tr("Sitecon model"), SiteconIO::tr("Loaded SITECON profile data."));
        
        QMap<Descriptor, DataTypePtr> outM;
        outM[SiteconWorkerFactory::SITECON_SLOT] = SiteconWorkerFactory::SITECON_MODEL_TYPE();
        p << new PortDescriptor(pd, DataTypePtr(new MapDataType("sitecon.read.out", outM)), false /*input*/, true /*multi*/);
        
        Descriptor desc(SiteconReader::ACTOR_ID, SiteconIO::tr("Read SITECON Model"), SiteconIO::tr("Reads SITECON profiles from file(s). The files can be local or Internet URLs."));
        IntegralBusActorPrototype* proto = new ReadSiteconProto(desc, p, a);
        proto->setPrompter(new SiteconReadPrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }

    SiteconBuildWorker::registerProto();
    SiteconSearchWorker::registerProto();

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new SiteconWorkerFactory(SiteconReader::ACTOR_ID));
    localDomain->registerEntry(new SiteconWorkerFactory(SiteconWriter::ACTOR_ID));
    localDomain->registerEntry(new SiteconWorkerFactory(SiteconSearchWorker::ACTOR_ID));
    localDomain->registerEntry(new SiteconWorkerFactory(SiteconBuildWorker::ACTOR_ID));
}

Worker* SiteconWorkerFactory::createWorker(Actor* a) {
    BaseWorker* w = NULL;
    if (SiteconReader::ACTOR_ID == a->getProto()->getId()) {
        w = new SiteconReader(a);
    } 
    else if (SiteconWriter::ACTOR_ID == a->getProto()->getId()) {
        w = new SiteconWriter(a);
    }
    else if (SiteconBuildWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new SiteconBuildWorker(a);
    }
    else if (SiteconSearchWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new SiteconSearchWorker(a);
    }

    return w;    
}

QString SiteconReadPrompter::composeRichDoc() {
    return tr("Read model(s) from <u>%1</u>.").arg(getHyperlink(BaseAttributes::URL_IN_ATTRIBUTE().getId(), getURL(BaseAttributes::URL_IN_ATTRIBUTE().getId())));
}

QString SiteconWritePrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort*>(target->getPort(SITECON_IN_PORT_ID));
    SAFE_POINT(NULL != input, "NULL input port", "");
    QString from = getProducersOrUnset(SITECON_IN_PORT_ID, SiteconWorkerFactory::SITECON_SLOT.getId());
    QString url = getScreenedURL(input, BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId());
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    return tr("Save the profile(s) from <u>%1</u> to %2.").arg(from).arg(url);
}

void SiteconReader::init() {
    output = ports.value(SITECON_OUT_PORT_ID);
    urls = WorkflowUtils::expandToUrls(actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
    mtype = SiteconWorkerFactory::SITECON_MODEL_TYPE();
}

Task* SiteconReader::tick() {
    if (urls.isEmpty() && tasks.isEmpty()) {
        setDone();
        output->setEnded();
    } else {
        Task* t = new SiteconReadTask(urls.takeFirst());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        tasks.append(t);
        return t;
    }
    return NULL;
}

void SiteconReader::sl_taskFinished() {
    SiteconReadTask* t = qobject_cast<SiteconReadTask*>(sender());
    if ( t->isCanceled( ) ) {
        return;
    }
    if (t->getState() != Task::State_Finished) return;
    if (output) {
        if (!t->hasError()) {
            QVariant v = qVariantFromValue<SiteconModel>(t->getResult());
            output->put(Message(mtype, v));
        }
        algoLog.info(tr("Loaded SITECON model from %1").arg(t->getURL()));
    }
    tasks.removeAll(t);
}

void SiteconWriter::init() {
    input = ports.value(SITECON_IN_PORT_ID);
}

Task* SiteconWriter::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            return NULL;
        }
        url = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
        fileMode = actor->getParameter(BaseAttributes::FILE_MODE_ATTRIBUTE().getId())->getAttributeValue<uint>(context);
        QVariantMap data = inputMessage.getData().toMap();
        SiteconModel model = data.value(SiteconWorkerFactory::SITECON_SLOT.getId()).value<SiteconModel>();
        
        QString anUrl = url;
        if (anUrl.isEmpty()) {
            anUrl = data.value(BaseSlots::URL_SLOT().getId()).toString();
        }
        if (anUrl.isEmpty()) {
            QString err = tr("Unspecified URL for writing Sitecon");
            //if (failFast) {
                return new FailTask(err);
            /*} else {
                algoLog.error(err);
                return NULL;
            }*/
        }
        assert(!anUrl.isEmpty());
        anUrl = context->absolutePath(anUrl);
        int count = ++counter[anUrl];
        if (count != 1) {
            anUrl = GUrlUtils::prepareFileName(anUrl, count, QStringList("sitecon"));
        } else {
            anUrl = GUrlUtils::ensureFileExt( anUrl, QStringList("sitecon")).getURLString();
        }
        ioLog.info(tr("Writing SITECON model to %1").arg(anUrl));
        return new SiteconWriteTask(anUrl, model, fileMode);
    } else if (input->isEnded()) {
        setDone();
    }
    return NULL;
}

} //namespace LocalWorkflow
} //namespace U2

