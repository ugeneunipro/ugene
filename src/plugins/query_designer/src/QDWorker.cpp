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

#include <QScopedPointer>

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Designer/QDScheduler.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/QDScheme.h>
#include <U2Lang/WorkflowEnv.h>

#include "QDSceneIOTasks.h"
#include "QDWorker.h"
#include "QueryDesignerPlugin.h"

namespace U2 {
namespace LocalWorkflow {

/*************************************************************************
* QDWorkerFactory
*************************************************************************/

static const QString SCHEMA_ATTR(BaseAttributes::URL_IN_ATTRIBUTE().getId());
static const QString OUTPUT_ATTR("merge");
static const QString OFFSET_ATTR("offset");

const QString QDWorkerFactory::ACTOR_ID("query");

void QDWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;

    {
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(),
            QDWorker::tr("Input sequences"),
            QDWorker::tr("A nucleotide sequence to analyze."));

        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(),
            QDWorker::tr("Result annotations"),
            QDWorker::tr("A set of annotations marking found results."));

        QMap<Descriptor, DataTypePtr> inM;
         inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("query.seq", inM)), true /*input*/);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("query.annotations", outM)), false /*input*/, true /*multi*/);
    }
    {
        Descriptor sd(SCHEMA_ATTR, QDWorker::tr("Schema"), QDWorker::tr("Schema file."));
        Descriptor od(OFFSET_ATTR, QDWorker::tr("Offset"),
            QDWorker::tr("Specifies left and right offsets for merged annotation (if 'Merge' parameter is set to true)."));
        Descriptor sad(OUTPUT_ATTR, QDWorker::tr("Merge"),
            QDWorker::tr("Merges regions of each result into single annotation if true."));

        a << new Attribute(sd, BaseTypes::STRING_TYPE(), true);
        a << new Attribute(od, BaseTypes::NUM_TYPE(), false, 0);
        a << new Attribute(sad, BaseTypes::BOOL_TYPE(), false, false);
    }

    Descriptor desc(ACTOR_ID, QDWorker::tr("Annotate with UQL"),
        QDWorker::tr("Analyzes a nucleotide sequence using different algorithms"
                     "(Repeat finder, ORF finder, etc.) imposing constraints"
                     " on the positional relationship of the results."));

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[SCHEMA_ATTR] = new URLDelegate(
            DialogUtils::prepareFileFilter(QDWorker::tr("Query schemes"), QStringList(QUERY_SCHEME_EXTENSION), true),
            QUERY_DESIGNER_ID,
            false, false, false);

        QVariantMap m;
        m["minimum"] = 0; m["maximum"] = INT_MAX;
        delegates[OFFSET_ATTR] = new SpinBoxDelegate(m);
    }

    proto->setPrompter(new QDPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":query_designer/images/query_designer.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new QDWorkerFactory());
}

/******************************
* QDPrompter
******************************/
QString QDPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr("from %1").arg(producer ? producer->getLabel() : unsetStr);
    QString schemaFile = getRequiredParam(SCHEMA_ATTR);

    QString doc = tr("Analyze each nucleotide sequence <u>%1</u> with <u>%2</u>.")
        .arg(producerName)
        .arg(getHyperlink(SCHEMA_ATTR, schemaFile));

    return doc;
}

/******************************
* QDWorker
******************************/
QDWorker::QDWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL), scheme(NULL) {
}

void QDWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* QDWorker::tick() {
    QString schemaUri = actor->getParameter(SCHEMA_ATTR)->getAttributePureValue().toString();
    QDDocument doc;

    QFileInfo fi(schemaUri);
    if (!fi.exists()) {
        QString defaultDir = QDir::searchPaths(PATH_PREFIX_DATA).first() + QUERY_SAMPLES_PATH;
        QDir dir(defaultDir);
        QStringList names(QString("*.%1").arg(QUERY_SCHEME_EXTENSION));
        foreach(const QFileInfo& fi, dir.entryInfoList(names, QDir::Files|QDir::NoSymLinks)) {
            if (fi.fileName()==schemaUri || fi.baseName()==schemaUri) {
                schemaUri = fi.absoluteFilePath();
                break;
            }
        }
    }

    QFile f(schemaUri);
    if (!f.open(QIODevice::ReadOnly)) {
        return new FailTask(L10N::errorOpeningFileRead(schemaUri));
    }
    QByteArray data = f.readAll();
    QString content = QString::fromUtf8(data);
    f.close();

    bool res = doc.setContent(content);
    if (!res) {
        return NULL;
    }

    scheme = new QDScheme;

    QList<QDDocument*> docs;
    docs << &doc;
    bool ok = QDSceneSerializer::doc2scheme(docs, scheme);
    if (!ok) {
        return NULL;
    }

    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        QVariantMap map = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = map.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        QDRunSettings settings;
        settings.annotationsObj = new AnnotationTableObject(GObjectTypes::getTypeInfo(GObjectTypes::ANNOTATION_TABLE).name,
            context->getDataStorage()->getDbiRef());
        settings.scheme = scheme;
        settings.dnaSequence = seq;
        settings.region = U2Region(0, seq.length());
        scheme->setSequence(settings.dnaSequence);
        scheme->setEntityRef(seqObj->getSequenceRef());
        bool outputType = actor->getParameter(OUTPUT_ATTR)->getAttributeValueWithoutScript<bool>();
        if (outputType) {
            settings.outputType = QDRunSettings::Single;
            settings.offset = actor->getParameter(OFFSET_ATTR)->getAttributeValueWithoutScript<int>();
        } else {
            settings.outputType = QDRunSettings::Group;
        }

        QDScheduler* scheduler = new QDScheduler(settings);
        connect(new TaskSignalMapper(scheduler), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return scheduler;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void QDWorker::cleanup() {
}

void annObjToAnnDataList(AnnotationTableObject *annObj, QList<SharedAnnotationData> &result) {
    foreach (Annotation *a, annObj->getAnnotations()) {
        a->addQualifier(U2Qualifier(GBFeatureUtils::QUALIFIER_GROUP, a->getGroup()->getName()));
        result.append(a->getData());
    }
}

void QDWorker::sl_taskFinished(Task* t) {
    delete scheme;
    SAFE_POINT(NULL != t, "Invalid task is encountered",);
    if (t->isCanceled()) {
        return;
    }
    if (output) {
        QDScheduler* sched = qobject_cast<QDScheduler*>(t);
        QList<SharedAnnotationData> res;
        AnnotationTableObject *ao = sched->getSettings().annotationsObj;
        annObjToAnnDataList(ao, res);
        QVariant v = qVariantFromValue< QList<SharedAnnotationData> >(res);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
    }
}

}//Workflow namespace
}//U2 namespace
