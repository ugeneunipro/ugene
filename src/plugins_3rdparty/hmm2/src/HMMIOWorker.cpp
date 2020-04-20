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

#include "HMMIOWorker.h"

#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseAttributes.h>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/TaskSignalMapper.h>

#include "HMMIO.h"
#include "u_search/HMMSearchDialogController.h"
#include "u_build/HMMBuildWorker.h"
#include "u_search/HMMSearchWorker.h"

/* TRANSLATOR U2::LocalWorkflow::HMMLib */
namespace U2 {
namespace LocalWorkflow {

const QString HMMReader::ACTOR("hmm2-read-profile");
const QString HMMWriter::ACTOR("hmm2-write-profile");
static const QString HMM_OUT_PORT_ID("out-hmm2");
static const QString HMM_IN_PORT_ID("in-hmm2");
const QString HMMLib::HMM_PROFILE_TYPE_ID("hmm.profile");

DataTypePtr HMMLib::HMM_PROFILE_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(HMM_PROFILE_TYPE_ID, tr("HMM Profile"), "")));
        startup = false;
    }
    return dtr->getById(HMM_PROFILE_TYPE_ID);
}


const Descriptor HMMLib::HMM2_SLOT() {return Descriptor(HMM2_SLOT_ID, QObject::tr("HMM Profile"), "");}
const Descriptor HMMLib::HMM_CATEGORY() {return Descriptor("hmmer", tr("HMMER2 Tools"), "");}

HMMIOProto::HMMIOProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: IntegralBusActorPrototype(_desc, _ports, _attrs) {
}

bool HMMIOProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params, const QString & urlAttrId ) const {
    if (md->hasUrls()) {
        QList<QUrl> urls = md->urls();
        if (urls.size() == 1)
        {
            QString url = urls.at(0).toLocalFile();
            QString ext = GUrlUtils::getUncompressedExtension(url);
            if (ext == HMMIO::HMM_EXT) {
                if (params) {
                    params->insert(urlAttrId, url);
                }
                return true;
            }
        }
    }
    return false;
}

ReadHMMProto::ReadHMMProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: HMMIOProto( _desc, _ports, _attrs ) {
    
    attrs << new Attribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate( HMMIO::getHMMFileFilter(), HMMIO::HMM_ID, true, false, false );
    setEditor(new DelegateEditor(delegateMap));
    setIconPath( ":/hmm2/images/hmmer_16.png" );
}

bool ReadHMMProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return HMMIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_IN_ATTRIBUTE().getId() );
}

WriteHMMProto::WriteHMMProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs ) 
: HMMIOProto( _desc, _ports, _attrs ) {
    attrs << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true );
    attrs << new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);
    
    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(HMMIO::getHMMFileFilter(), HMMIO::HMM_ID, false );
    delegateMap[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(false);
    
    setEditor(new DelegateEditor(delegateMap));
    setIconPath( ":/hmm2/images/hmmer_16.png" );
    setValidator(new ScreenedParamValidator(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ports.first()->getId(), BaseSlots::URL_SLOT().getId()));
    setPortValidator(HMM_IN_PORT_ID, new ScreenedSlotValidator(BaseSlots::URL_SLOT().getId()));
}

bool WriteHMMProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return HMMIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_OUT_ATTRIBUTE().getId() );
}

void HMMIOWorkerFactory::init() {
 
     ActorPrototypeRegistry* r = WorkflowEnv::getProtoRegistry();
     assert(r);
     {
         Descriptor id(HMM_IN_PORT_ID, HMMLib::tr("HMM profile"), HMMLib::tr("Input HMM profile"));
         Descriptor ud(BaseSlots::URL_SLOT().getId(), HMMLib::tr("Location"), HMMLib::tr("Location hint for the target file."));

         QMap<Descriptor, DataTypePtr> m;
         m[ud] = BaseTypes::STRING_TYPE();
         m[HMMLib::HMM2_SLOT()] = HMMLib::HMM_PROFILE_TYPE();
         DataTypePtr t(new MapDataType(Descriptor("write.hmm.content"), m));
        
         QList<PortDescriptor*> p; QList<Attribute*> a;
         p << new PortDescriptor(id, t, true /*input*/);
         
         Descriptor desc(HMMWriter::ACTOR, HMMLib::tr("Write HMM2 Profile"), HMMLib::tr("Saves all input HMM profiles to specified location."));
         IntegralBusActorPrototype* proto = new WriteHMMProto(desc, p, a);
         proto->setPrompter(new HMMWritePrompter());
         r->registerProto(HMMLib::HMM_CATEGORY(), proto);
     }
     {
         Descriptor od(HMM_OUT_PORT_ID, HMMLib::tr("HMM profile"), HMMLib::tr("Loaded HMM profile"));

         QList<PortDescriptor*> p; QList<Attribute*> a;
         QMap<Descriptor, DataTypePtr> outM;
         outM[HMMLib::HMM2_SLOT()] = HMMLib::HMM_PROFILE_TYPE();
         p << new PortDescriptor(od, DataTypePtr(new MapDataType("hmm.read.out", outM)), false /*output*/, true);
         
         Descriptor desc(HMMReader::ACTOR, HMMLib::tr("Read HMM2 Profile"), HMMLib::tr("Reads HMM profiles from file(s). The files can be local or Internet URLs."));
         IntegralBusActorPrototype* proto = new ReadHMMProto(desc, p, a);
         proto->setPrompter(new HMMReadPrompter());
         r->registerProto(HMMLib::HMM_CATEGORY(), proto);
     }
 
     DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
     localDomain->registerEntry(new HMMIOWorkerFactory(HMMReader::ACTOR));
     localDomain->registerEntry(new HMMIOWorkerFactory(HMMWriter::ACTOR));
}

QString HMMReadPrompter::composeRichDoc() {
    return tr("Read HMM profile(s) from %1.").arg(getHyperlink(BaseAttributes::URL_IN_ATTRIBUTE().getId(), getURL(BaseAttributes::URL_IN_ATTRIBUTE().getId())));
}

QString HMMWritePrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(HMM_IN_PORT_ID));
    Actor* producer = input->getProducer(HMM2_SLOT_ID);
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerStr = producer ? producer->getLabel() : unsetStr;
    QString url = getScreenedURL(input, BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId()); 
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    QString doc = tr("Save HMM profile(s) from <u>%1</u> to <u>%2</u>.")
        .arg(producerStr)
        .arg(url);
    return doc;
}

void HMMIOWorkerFactory::cleanup() {
     DomainFactory* ld = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
     DomainFactory* f = ld->unregisterEntry(HMMReader::ACTOR); 
     delete f;
     f = ld->unregisterEntry(HMMWriter::ACTOR);
     delete f;
 
     ActorPrototypeRegistry* r = WorkflowEnv::getProtoRegistry();
     ActorPrototype* p = r->unregisterProto(HMMReader::ACTOR);
     assert(p); delete p;
     p = r->unregisterProto(HMMWriter::ACTOR);
     assert(p); delete p;
}

Worker* HMMIOWorkerFactory::createWorker(Actor* a) {
    BaseWorker* w = NULL;
     if (HMMReader::ACTOR == a->getProto()->getId()) {
         w = new HMMReader(a);
     } 
     else if (HMMWriter::ACTOR == a->getProto()->getId()) {
         w = new HMMWriter(a);
     }

    return w;    
}

void HMMReader::init() {
    output = ports.value(HMM_OUT_PORT_ID);
    urls = WorkflowUtils::expandToUrls(actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
}

Task* HMMReader::tick() {
    if (urls.isEmpty()) {
        setDone();
        output->setEnded();
    } else {
        Task* t = new HMMReadTask(urls.takeFirst());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }
    return NULL;
}

void HMMReader::sl_taskFinished() {
    HMMReadTask* t = qobject_cast<HMMReadTask*>(sender());
    if (t->getState() != Task::State_Finished) return;
    if (output) {
        if (!t->hasError()) {
            QVariant v = qVariantFromValue<plan7_s*>(t->getHMM());
            output->put(Message(HMMLib::HMM_PROFILE_TYPE(), v));
        }
        ioLog.info(tr("Loaded HMM profile from %1").arg(t->getURL()));
    }
}

void HMMWriter::init() {
    input = ports.value(HMM_IN_PORT_ID);
}

Task* HMMWriter::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            return NULL;
        }
        url = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
        fileMode = actor->getParameter(BaseAttributes::FILE_MODE_ATTRIBUTE().getId())->getAttributeValue<uint>(context);
        QVariantMap data = inputMessage.getData().toMap();
        
        plan7_s* hmm = data.value(HMM2_SLOT_ID).value<plan7_s*>();
        QString anUrl = url;
        if (anUrl.isEmpty()) {
            anUrl = data.value(BaseSlots::URL_SLOT().getId()).toString();
        }
        if (anUrl.isEmpty() || hmm == NULL) {
            QString err = (hmm == NULL) ? tr("Empty HMM passed for writing to %1").arg(anUrl) : tr("Unspecified URL for writing HMM");
            return new FailTask(err);
        }
        assert(!anUrl.isEmpty());
        anUrl = context->absolutePath(anUrl);
        int count = ++counter[anUrl];
        if (count != 1) {
            anUrl = GUrlUtils::prepareFileName(anUrl, count, QStringList(HMMIO::HMM_EXT));
        } else {
            anUrl = GUrlUtils::ensureFileExt( anUrl, QStringList(HMMIO::HMM_EXT)).getURLString();
        }
        ioLog.info(tr("Writing HMM profile to %1").arg(anUrl));
        return new HMMWriteTask(anUrl, hmm, fileMode);
    } else if (input->isEnded()) {
        setDone();
    }
    return NULL;
}


void HMMLib::init() {
    HMMIOWorkerFactory::init();
    HMMBuildWorkerFactory::init();
    HMMSearchWorkerFactory::init();
}

void HMMLib::cleanup() {
    //FIXME need locking
    //HMMIOWorkerFactory::cleanup();
    //HMMBuildWorkerFactory::cleanup();
    //HMMSearchWorkerFactory::cleanup();

    //DataTypeRegistry* dr = WorkflowEnv::getDataTypeRegistry();
    //dr->unregisterEntry(HMM_PROFILE_TYPE->getId());
}

} //namespace LocalWorkflow
} //namespace U2
