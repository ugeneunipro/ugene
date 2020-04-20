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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/FailTask.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "GeneByGeneReportWorker.h"
#include "GeneByGeneReportTask.h"

namespace U2 {
namespace LocalWorkflow {

const QString GeneByGeneReportWorkerFactory::ACTOR_ID("genebygene-report-id");

static const QString ANNOT_SLOT_ID("gene-ann");
static const QString SEQ_SLOT_ID("gene-seq");

static const QString IN_TYPE_ID("genebygene-data");

static const QString IN_PORT_DESCR("in-data");

static const QString OUTPUT_FILE("output-file");
static const QString EXISTING_FILE("existing");
static const QString IDENTITY("identity");
static const QString ANN_NAME("annotation_name");


/************************************************************************/
/* Worker */
/************************************************************************/
GeneByGeneReportWorker::GeneByGeneReportWorker(Actor *p)
    : BaseWorker(p)
    , inChannel(NULL)
{

}

void GeneByGeneReportWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
}

Task *GeneByGeneReportWorker::tick() {
    U2OpStatus2Log os;

    while (inChannel->hasMessage()) {

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        QVariant annVar;
        if (!data.contains(SEQ_SLOT_ID)) {
            os.setError("Sequence slot is empty");
            return new FailTask(os.getError());
        }

        if (data.contains(ANNOT_SLOT_ID)){
            annVar = data[ANNOT_SLOT_ID];
        }
        const QList<SharedAnnotationData> annData = StorageUtils::getAnnotationTable(context->getDataStorage(), annVar);

        SharedDbiDataHandler seqId = data.value(SEQ_SLOT_ID).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (NULL == seqObj.data()) {
            return NULL;
        }
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        geneData.insert(seqObj->getSequenceName(), qMakePair(seq, annData));
    }

    if (!inChannel->isEnded()) {
        return NULL;
    }

    GeneByGeneReportSettings settings;

    settings.outFile = getValue<QString>(OUTPUT_FILE);
    settings.existingFile = getValue<QString>(EXISTING_FILE);
    settings.identity = getValue<float>(IDENTITY);
    settings.annName = getValue<QString>(ANN_NAME);


    Task* t = new GeneByGeneReportTask(settings, geneData);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

void GeneByGeneReportWorker::cleanup() {
    geneData.clear();
}

void GeneByGeneReportWorker::sl_taskFinished() {
    GeneByGeneReportTask *t = dynamic_cast<GeneByGeneReportTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    outFiles << t->getSettings().outFile;

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
    }
}

QStringList GeneByGeneReportWorker::getOutputFiles() {
    return outFiles;
}

/************************************************************************/
/* Factory */
/************************************************************************/


void GeneByGeneReportWorkerFactory::init() {

    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor annDesc(ANNOT_SLOT_ID,
        GeneByGeneReportWorker::tr("Input annotations"),
        GeneByGeneReportWorker::tr("Gene annotations."));
    inTypeMap[annDesc] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();
    Descriptor seqDesc(SEQ_SLOT_ID,
        GeneByGeneReportWorker::tr("Input sequences"),
        GeneByGeneReportWorker::tr("Gene sequences."));
    inTypeMap[seqDesc] = BaseTypes::DNA_SEQUENCE_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        GeneByGeneReportWorker::tr("Gene by gene report data."),
        GeneByGeneReportWorker::tr("Gene with similar regions to for report."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    QList<Attribute*> attrs;
    {
        Descriptor outFile(OUTPUT_FILE,
            GeneByGeneReportWorker::tr("Output file"),
            GeneByGeneReportWorker::tr("File to store a report."));
        Descriptor annName(ANN_NAME,
            GeneByGeneReportWorker::tr("Annotation name"),
            GeneByGeneReportWorker::tr("Annotation name used to compare genes and reference genomes."));
        Descriptor existingFile(EXISTING_FILE,
            GeneByGeneReportWorker::tr("Existing file"),
            GeneByGeneReportWorker::tr("If a target report already exists you should specify how to handle that. "
            "<b>Merge</b> two table in one. <b>Overwrite</b> or <b>Rename</b> existing file."));
        Descriptor identitiDescr(IDENTITY,
            GeneByGeneReportWorker::tr("Identity cutoff"),
            GeneByGeneReportWorker::tr("Identity between gene sequence length and annotation length in per cent. BLAST identity (if specified) is checked after."));


        attrs << new Attribute(outFile, BaseTypes::STRING_TYPE(), true, QVariant(""));
        attrs << new Attribute(annName, BaseTypes::STRING_TYPE(), true, QVariant("blast_result"));
        attrs << new Attribute(existingFile, BaseTypes::STRING_TYPE(), false, QVariant(GeneByGeneReportSettings::MERGE_EXISTING));
        attrs << new Attribute(identitiDescr, BaseTypes::NUM_TYPE(), false, QVariant(90.0f));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[OUTPUT_FILE] = new URLDelegate("", "", false, false);
        {
            QVariantMap vm;
            vm[GeneByGeneReportSettings::MERGE_EXISTING] = GeneByGeneReportSettings::MERGE_EXISTING;
            vm[GeneByGeneReportSettings::OVERWRITE_EXISTING] = GeneByGeneReportSettings::OVERWRITE_EXISTING;
            vm[GeneByGeneReportSettings::RENAME_EXISTING] = GeneByGeneReportSettings::RENAME_EXISTING;
            delegates[EXISTING_FILE] = new ComboBoxDelegate(vm);
        }
        {
            QVariantMap vm;
            vm["minimum"] = 0;
            vm["maximum"] = 100;
            vm["singleStep"] = 1;
            vm["decimals"] = 4;
            vm["suffix"] = "%";
            delegates[IDENTITY] = new DoubleSpinBoxDelegate(vm);
        }
    }

    Descriptor protoDesc(GeneByGeneReportWorkerFactory::ACTOR_ID,
        GeneByGeneReportWorker::tr("Gene-by-gene Approach Report"),
        GeneByGeneReportWorker::tr("Output a table of genes found in a reference sequence."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new GeneByGeneReportPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new GeneByGeneReportWorkerFactory());
}

Worker *GeneByGeneReportWorkerFactory::createWorker(Actor *a) {
    return new GeneByGeneReportWorker(a);
}

QString GeneByGeneReportPrompter::composeRichDoc() {
    QString res = "";

    Actor* seqProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(SEQ_SLOT_ID);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString annUrl = seqProducer ? seqProducer->getLabel() : unsetStr;

    QString file = getHyperlink(OUTPUT_FILE, getURL(OUTPUT_FILE));
    float identity = getParameter(IDENTITY).toFloat();
    QString existing = getParameter(EXISTING_FILE).toString();
    QString aName = getParameter(ANN_NAME).toString();

    res.append(tr("For annotated genes from <u>%1</u> build gene-by-gene approach report.").arg(annUrl));
    res.append(tr(" With sequence identity of <u>%1</u>.").arg(identity));
    res.append(tr(" Using <u>%1</u> to compare gene and genome sequences.").arg(aName));
    res.append(tr(" Output result table to <u>%1</u>.").arg(file));
    res.append(tr(" If a report file already exists, <u>%1</u> it").arg(existing));
    res.append(".");

    return res;
}

} // LocalWorkflow
} // U2

