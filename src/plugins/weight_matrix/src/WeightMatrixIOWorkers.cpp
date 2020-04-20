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

#include "WeightMatrixIOWorkers.h"
#include "WeightMatrixWorkers.h"
#include "WeightMatrixIO.h"
#include "PWMSearchDialogController.h"
#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2SafePoints.h>


/* TRANSLATOR U2::WeightMatrixIO */

namespace U2 {
namespace LocalWorkflow {

const QString PWMatrixReader::ACTOR_ID("wmatrix-read");
const QString PWMatrixWriter::ACTOR_ID("wmatrix-write");
const QString PFMatrixReader::ACTOR_ID("fmatrix-read");
const QString PFMatrixWriter::ACTOR_ID("fmatrix-write");

static const QString FMATRIX_OUT_PORT_ID("out-fmatrix"); // FIXME: redifinition
static const QString FMATRIX_IN_PORT_ID("in-fmatrix"); // FIXME: redifinition
static const QString WMATRIX_OUT_PORT_ID("out-wmatrix"); // FIXME: redifinition
static const QString WMATRIX_IN_PORT_ID("in-wmatrix"); // FIXME: redifinition

const QString PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE_ID("pwmatrix.model");
const QString PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE_ID("pfmatrix.model");

DataTypePtr const PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(WEIGHT_MATRIX_MODEL_TYPE_ID, WeightMatrixIO::tr("Weight matrix"), "")));
        startup = false;
    }
    return dtr->getById(WEIGHT_MATRIX_MODEL_TYPE_ID);
}

const Descriptor PWMatrixWorkerFactory::WMATRIX_SLOT("wmatrix", QObject::tr("Weight matrix"), "");

const Descriptor PWMatrixWorkerFactory::WEIGHT_MATRIX_CATEGORY() {return Descriptor("hweightmatrix", WeightMatrixIO::tr("Weight matrix"), "");}

PWMatrixIOProto::PWMatrixIOProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
    : IntegralBusActorPrototype(_desc, _ports, _attrs) {
}

bool PWMatrixIOProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params, const QString & urlAttrId ) const {
    if (md->hasUrls()) {
        QList<QUrl> urls = md->urls();
        if (urls.size() == 1)
        {
            QString url = urls.at(0).toLocalFile();
            QString ext = GUrlUtils::getUncompressedExtension(GUrl(url, GUrl_File));
            if (WeightMatrixIO::WEIGHT_MATRIX_EXT == ext) {
                if (params) {
                    params->insert(urlAttrId, url);
                }
                return true;
            }
        }
    }
    return false;
}

ReadPWMatrixProto::ReadPWMatrixProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
    : PWMatrixIOProto( _desc, _ports, _attrs ) {

        attrs << new Attribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
        QMap<QString, PropertyDelegate*> delegateMap;
        delegateMap[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(WeightMatrixIO::getPWMFileFilter(), WeightMatrixIO::WEIGHT_MATRIX_ID, true, false, false);
        setEditor(new DelegateEditor(delegateMap));
        setIconPath(":weight_matrix/images/weight_matrix.png");
}

bool ReadPWMatrixProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return PWMatrixIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_IN_ATTRIBUTE().getId() );
}

WritePWMatrixProto::WritePWMatrixProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
    : PWMatrixIOProto( _desc, _ports, _attrs ) {
        attrs << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true );
        attrs << new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);

        QMap<QString, PropertyDelegate*> delegateMap;
        delegateMap[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(WeightMatrixIO::getPWMFileFilter(), WeightMatrixIO::WEIGHT_MATRIX_ID, false );
        delegateMap[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(false);

        setEditor(new DelegateEditor(delegateMap));
        setIconPath(":weight_matrix/images/weight_matrix.png");
        setValidator(new ScreenedParamValidator(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ports.first()->getId(), BaseSlots::URL_SLOT().getId()));
        setPortValidator(WMATRIX_IN_PORT_ID, new ScreenedSlotValidator(BaseSlots::URL_SLOT().getId()));
}

bool WritePWMatrixProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return PWMatrixIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_OUT_ATTRIBUTE().getId() );
}

void PWMatrixWorkerFactory::init()
{
    ActorPrototypeRegistry* r = WorkflowEnv::getProtoRegistry();
    assert(r);
    {
        QMap<Descriptor, DataTypePtr> m;
        Descriptor sd(BaseSlots::URL_SLOT().getId(), WeightMatrixIO::tr("Location"), WeightMatrixIO::tr("Location hint for the target file."));
        m[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        m[WMATRIX_SLOT] = WEIGHT_MATRIX_MODEL_TYPE();
        DataTypePtr t(new MapDataType(Descriptor("write.pwmatrix.content"), m));

        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(WMATRIX_IN_PORT_ID, WeightMatrixIO::tr("Weight matrix"), WeightMatrixIO::tr("Input weight matrix"));
        p << new PortDescriptor(pd, t, true /*input*/);
        Descriptor desc(PWMatrixWriter::ACTOR_ID, WeightMatrixIO::tr("Write Weight Matrix"), WeightMatrixIO::tr("Saves all input weight matrices to specified location."));
        IntegralBusActorPrototype* proto = new WritePWMatrixProto(desc, p, a);
        proto->setPrompter(new PWMatrixWritePrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }
    {
        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(WMATRIX_OUT_PORT_ID, WeightMatrixIO::tr("Weight matrix"), WeightMatrixIO::tr("Loaded weight matrices data."));

        QMap<Descriptor, DataTypePtr> outM;
        outM[WMATRIX_SLOT] = WEIGHT_MATRIX_MODEL_TYPE();
        p << new PortDescriptor(pd, DataTypePtr(new MapDataType("wmatrix.read.out", outM)), false /*input*/, true /*multi*/);

        Descriptor desc(PWMatrixReader::ACTOR_ID, WeightMatrixIO::tr("Read Weight Matrix"), WeightMatrixIO::tr("Reads weight matrices from file(s). The files can be local or Internet URLs."));
        IntegralBusActorPrototype* proto = new ReadPWMatrixProto(desc, p, a);
        proto->setPrompter(new PWMatrixReadPrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }

    PWMatrixBuildWorker::registerProto();
    PWMatrixSearchWorker::registerProto();

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new PWMatrixWorkerFactory(PWMatrixReader::ACTOR_ID));
    localDomain->registerEntry(new PWMatrixWorkerFactory(PWMatrixWriter::ACTOR_ID));
    localDomain->registerEntry(new PWMatrixWorkerFactory(PWMatrixSearchWorker::ACTOR_ID));
    localDomain->registerEntry(new PWMatrixWorkerFactory(PWMatrixBuildWorker::ACTOR_ID));
}

Worker* PWMatrixWorkerFactory::createWorker(Actor* a) {
    BaseWorker* w = NULL;
    if (PWMatrixReader::ACTOR_ID == a->getProto()->getId()) {
        w = new PWMatrixReader(a);
    }
    else if (PWMatrixWriter::ACTOR_ID == a->getProto()->getId()) {
        w = new PWMatrixWriter(a);
    }
    else if (PWMatrixBuildWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new PWMatrixBuildWorker(a);
    }
    else if (PWMatrixSearchWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new PWMatrixSearchWorker(a);
    }

    return w;
}

QString PWMatrixReadPrompter::composeRichDoc() {
    return tr("Read model(s) from <u>%1</u>.").arg(getHyperlink(BaseAttributes::URL_IN_ATTRIBUTE().getId(), getURL(BaseAttributes::URL_IN_ATTRIBUTE().getId())));
}

QString PWMatrixWritePrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort*>(target->getPort(WMATRIX_IN_PORT_ID));
    SAFE_POINT(NULL != input, "NULL input port", "");
    QString from = getProducersOrUnset(WMATRIX_IN_PORT_ID, PWMatrixWorkerFactory::WMATRIX_SLOT.getId());
    QString url = getScreenedURL(input, BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId());
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    return tr("Save the profile(s) from <u>%1</u> to %2.").arg(from).arg(url);
}

void PWMatrixReader::init() {
    output = ports.value(WMATRIX_OUT_PORT_ID);
    urls = WorkflowUtils::expandToUrls(actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
    mtype = PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE();
}

Task* PWMatrixReader::tick() {
    if (urls.isEmpty() && tasks.isEmpty()) {
        setDone();
        output->setEnded();
    } else {
        Task* t = new PWMatrixReadTask(urls.takeFirst());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        tasks.append(t);
        return t;
    }
    return NULL;
}

void PWMatrixReader::sl_taskFinished() {
    PWMatrixReadTask* t = qobject_cast<PWMatrixReadTask*>(sender());
    if (t->getState() != Task::State_Finished) return;
    if (output) {
        if (!t->hasError()) {
            QVariant v = qVariantFromValue<PWMatrix>(t->getResult());
            output->put(Message(mtype, v));
        }
        ioLog.info(tr("Loaded weight matrix from %1").arg(t->getURL()));
    }
    tasks.removeAll(t);
}

void PWMatrixWriter::init() {
    input = ports.value(WMATRIX_IN_PORT_ID);
}

Task* PWMatrixWriter::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            return NULL;
        }
        url = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
        fileMode = actor->getParameter(BaseAttributes::FILE_MODE_ATTRIBUTE().getId())->getAttributeValue<uint>(context);
        QVariantMap data = inputMessage.getData().toMap();

        PWMatrix model = data.value(PWMatrixWorkerFactory::WMATRIX_SLOT.getId()).value<PWMatrix>();
        QString anUrl = url;
        if (anUrl.isEmpty()) {
            anUrl = data.value(BaseSlots::URL_SLOT().getId()).toString();
        }
        if (anUrl.isEmpty()) {
            QString err = tr("Unspecified URL for writing weight matrix");
            //if (failFast) {
                return new FailTask(err);
            /*} else {
                ioLog.error(err);
                return NULL;
            }*/
        }
        assert(!anUrl.isEmpty());
        anUrl = context->absolutePath(anUrl);
        int count = ++counter[anUrl];
        if (count != 1) {
            anUrl = GUrlUtils::prepareFileName(anUrl, count, QStringList(WeightMatrixIO::WEIGHT_MATRIX_EXT));
        } else {
            anUrl = GUrlUtils::ensureFileExt( anUrl, QStringList(WeightMatrixIO::WEIGHT_MATRIX_EXT)).getURLString();
        }
        ioLog.info(tr("Writing weight matrix to %1").arg(anUrl));
        return new PWMatrixWriteTask(anUrl, model, fileMode);
    } else if (input->isEnded()) {
        setDone();
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
// PFMatrix workers
//////////////////////////////////////////////////////////////////////////
DataTypePtr const PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(FREQUENCY_MATRIX_MODEL_TYPE_ID, WeightMatrixIO::tr("Frequency matrix"), "")));
        startup = false;
    }
    return dtr->getById(FREQUENCY_MATRIX_MODEL_TYPE_ID);
}

const Descriptor PFMatrixWorkerFactory::FMATRIX_SLOT("fmatrix", QObject::tr("Frequency matrix"), "");

const Descriptor PFMatrixWorkerFactory::FREQUENCY_MATRIX_CATEGORY() {return Descriptor("hweightmatrix", WeightMatrixIO::tr("Weight matrix"), "");}

PFMatrixIOProto::PFMatrixIOProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
: IntegralBusActorPrototype(_desc, _ports, _attrs) {
}

bool PFMatrixIOProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params, const QString & urlAttrId ) const {
    if (md->hasUrls()) {
        QList<QUrl> urls = md->urls();
        if (urls.size() == 1)
        {
            QString url = urls.at(0).toLocalFile();
            QString ext = GUrlUtils::getUncompressedExtension(GUrl(url, GUrl_File));
            if (WeightMatrixIO::FREQUENCY_MATRIX_EXT == ext) {
                if (params) {
                    params->insert(urlAttrId, url);
                }
                return true;
            }
        }
    }
    return false;
}

ReadPFMatrixProto::ReadPFMatrixProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
: PFMatrixIOProto( _desc, _ports, _attrs ) {

    attrs << new Attribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(WeightMatrixIO::getPFMFileFilter(), WeightMatrixIO::FREQUENCY_MATRIX_ID, true, false, false);
    setEditor(new DelegateEditor(delegateMap));
    setIconPath(":weight_matrix/images/weight_matrix.png");
}

bool ReadPFMatrixProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return PFMatrixIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_IN_ATTRIBUTE().getId());
}

WritePFMatrixProto::WritePFMatrixProto(const Descriptor& _desc, const QList<PortDescriptor*>& _ports, const QList<Attribute*>& _attrs )
: PFMatrixIOProto( _desc, _ports, _attrs ) {
    attrs << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true );
    attrs << new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);

    QMap<QString, PropertyDelegate*> delegateMap;
    delegateMap[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(WeightMatrixIO::getPFMFileFilter(), WeightMatrixIO::FREQUENCY_MATRIX_ID, false );
    delegateMap[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(false);

    setEditor(new DelegateEditor(delegateMap));
    setIconPath(":weight_matrix/images/weight_matrix.png");
    setValidator(new ScreenedParamValidator(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ports.first()->getId(), BaseSlots::URL_SLOT().getId()));
    setPortValidator(FMATRIX_IN_PORT_ID, new ScreenedSlotValidator(BaseSlots::URL_SLOT().getId()));
}

bool WritePFMatrixProto::isAcceptableDrop(const QMimeData * md, QVariantMap * params ) const {
    return PFMatrixIOProto::isAcceptableDrop( md, params, BaseAttributes::URL_OUT_ATTRIBUTE().getId() );
}

void PFMatrixWorkerFactory::init()
{
    ActorPrototypeRegistry* r = WorkflowEnv::getProtoRegistry();
    assert(r);
    {
        QMap<Descriptor, DataTypePtr> m;
        Descriptor sd(BaseSlots::URL_SLOT().getId(), WeightMatrixIO::tr("Location"), WeightMatrixIO::tr("Location hint for the target file."));
        m[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        m[PFMatrixWorkerFactory::FMATRIX_SLOT] = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
        DataTypePtr t(new MapDataType(Descriptor("write.pfmatrix.content"), m));

        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(FMATRIX_IN_PORT_ID, WeightMatrixIO::tr("Frequency matrix"), WeightMatrixIO::tr("Input frequency matrix"));
        p << new PortDescriptor(pd, t, true /*input*/);
        Descriptor desc(PFMatrixWriter::ACTOR_ID, WeightMatrixIO::tr("Write Frequency Matrix"), WeightMatrixIO::tr("Saves all input frequency matrices to specified location."));
        IntegralBusActorPrototype* proto = new WritePFMatrixProto(desc, p, a);
        proto->setPrompter(new PFMatrixWritePrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }
    {
        QList<PortDescriptor*> p; QList<Attribute*> a;
        Descriptor pd(FMATRIX_OUT_PORT_ID, WeightMatrixIO::tr("Frequency matrix"), WeightMatrixIO::tr("Loaded weight matrices data."));

        QMap<Descriptor, DataTypePtr> outM;
        outM[PFMatrixWorkerFactory::FMATRIX_SLOT] = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
        p << new PortDescriptor(pd, DataTypePtr(new MapDataType("fmatrix.read.out", outM)), false /*input*/, true /*multi*/);

        Descriptor desc(PFMatrixReader::ACTOR_ID, WeightMatrixIO::tr("Read Frequency Matrix"), WeightMatrixIO::tr("Reads frequency matrices from file(s). The files can be local or Internet URLs."));
        IntegralBusActorPrototype* proto = new ReadPFMatrixProto(desc, p, a);
        proto->setPrompter(new PFMatrixReadPrompter());
        r->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
    }

    PFMatrixBuildWorker::registerProto();
    PFMatrixConvertWorker::registerProto();

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new PFMatrixWorkerFactory(PFMatrixReader::ACTOR_ID));
    localDomain->registerEntry(new PFMatrixWorkerFactory(PFMatrixWriter::ACTOR_ID));
    localDomain->registerEntry(new PFMatrixWorkerFactory(PFMatrixBuildWorker::ACTOR_ID));
    localDomain->registerEntry(new PFMatrixWorkerFactory(PFMatrixConvertWorker::ACTOR_ID));
}

Worker* PFMatrixWorkerFactory::createWorker(Actor* a) {
    BaseWorker* w = NULL;
    if (PFMatrixReader::ACTOR_ID == a->getProto()->getId()) {
        w = new PFMatrixReader(a);
    }
    else if (PFMatrixWriter::ACTOR_ID == a->getProto()->getId()) {
        w = new PFMatrixWriter(a);
    }
    else if (PFMatrixBuildWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new PFMatrixBuildWorker(a);
    }
    else if (PFMatrixConvertWorker::ACTOR_ID == a->getProto()->getId()) {
        w = new PFMatrixConvertWorker(a);
    }

    return w;
}

QString PFMatrixReadPrompter::composeRichDoc() {
    return tr("Read model(s) from <u>%1</u>.").arg(getHyperlink(BaseAttributes::URL_IN_ATTRIBUTE().getId(), getURL(BaseAttributes::URL_IN_ATTRIBUTE().getId())));
}

QString PFMatrixWritePrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort*>(target->getPort(FMATRIX_IN_PORT_ID));
    SAFE_POINT(NULL != input, "NULL input port", "");
    QString from = getProducersOrUnset(FMATRIX_IN_PORT_ID, PFMatrixWorkerFactory::FMATRIX_SLOT.getId());
    QString url = getScreenedURL(input, BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId());
    url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    return tr("Save the profile(s) from <u>%1</u> to %2.").arg(from).arg(url);
}

void PFMatrixReader::init() {
    output = ports.value(FMATRIX_OUT_PORT_ID);
    urls = WorkflowUtils::expandToUrls(actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
    mtype = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
}

Task* PFMatrixReader::tick() {
    if (urls.isEmpty() && tasks.isEmpty()) {
        setDone();
        output->setEnded();
    } else {
        Task* t = new PFMatrixReadTask(urls.takeFirst());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        tasks.append(t);
        return t;
    }
    return NULL;
}

void PFMatrixReader::sl_taskFinished() {
    PFMatrixReadTask* t = qobject_cast<PFMatrixReadTask*>(sender());
    if (t->getState() != Task::State_Finished) return;
    if (output) {
        if (!t->hasError()) {
            QVariant v = qVariantFromValue<PFMatrix>(t->getResult());
            output->put(Message(mtype, v));
        }
        ioLog.info(tr("Loaded frequency matrix from %1").arg(t->getURL()));
    }
    tasks.removeAll(t);
}

void PFMatrixWriter::init() {
    input = ports.value(FMATRIX_IN_PORT_ID);
}

Task* PFMatrixWriter::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            return NULL;
        }
        url = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
        fileMode = actor->getParameter(BaseAttributes::FILE_MODE_ATTRIBUTE().getId())->getAttributeValue<uint>(context);
        QVariantMap data = inputMessage.getData().toMap();
        PFMatrix model = data.value(PFMatrixWorkerFactory::FMATRIX_SLOT.getId()).value<PFMatrix>();

        QString anUrl = url;
        if (anUrl.isEmpty()) {
            anUrl = data.value(BaseSlots::URL_SLOT().getId()).toString();
        }
        if (anUrl.isEmpty()) {
            QString err = tr("Unspecified URL for writing frequency matrix");
            //if (failFast) {
                return new FailTask(err);
            /*} else {
                ioLog.error(err);
                return NULL;
            }*/
        }
        assert(!anUrl.isEmpty());
        anUrl = context->absolutePath(anUrl);
        int count = ++counter[anUrl];
        if (count != 1) {
            anUrl = GUrlUtils::prepareFileName(anUrl, count, QStringList(WeightMatrixIO::FREQUENCY_MATRIX_EXT));
        } else {
            anUrl = GUrlUtils::ensureFileExt( anUrl, QStringList(WeightMatrixIO::FREQUENCY_MATRIX_EXT)).getURLString();
        }
        ioLog.info(tr("Writing frequency matrix to %1").arg(anUrl));
        return new PFMatrixWriteTask(anUrl, model, fileMode);
    } else if (input->isEnded()) {
        setDone();
    }
    return NULL;
}


} //namespace LocalWorkflow
} //namespace U2
