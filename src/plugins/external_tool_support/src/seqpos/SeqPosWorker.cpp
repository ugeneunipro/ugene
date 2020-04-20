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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/Settings.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "SeqPosSupport.h"

#include "SeqPosWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString SeqPosWorkerFactory::ACTOR_ID("seqpos-id");

static const QString ANNOT_SLOT_ID("cp_treat-ann");

static const QString IN_TYPE_ID("seqpos-data");

static const QString IN_PORT_DESCR("in-data");

static const QString OUTPUT_DIR("output-dir");

static const QString GENOME_ASSEMBLY("assembly");
static const QString FIND_DE_NOVO("de_novo");
static const QString MOTIF_DB("motif_db");
static const QString OUT_NAME("out_name");
static const QString REG_WIDTH("reg_width");
static const QString P_VAL("p_val");

/************************************************************************/
/* Worker */
/************************************************************************/
SeqPosWorker::SeqPosWorker(Actor *p)
: BaseWorker(p)
, inChannel(NULL)
{

}

void SeqPosWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
}

Task *SeqPosWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        if (!data.contains(ANNOT_SLOT_ID)) {
            os.setError("Annotations slot is empty");
            return new FailTask(os.getError());
        }

        QList<SharedDbiDataHandler> treatData = StorageUtils::getAnnotationTableHandlers(data[ANNOT_SLOT_ID]);

        SeqPosSettings settings = createSeqPosSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        SeqPosTask* t = new SeqPosTask(settings, context->getDataStorage(), treatData);
        t->addListeners(createLogListeners());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }else if (inChannel->isEnded()) {
        setDone();
    }
    return NULL;
}

void SeqPosWorker::cleanup() {

}

void SeqPosWorker::sl_taskFinished() {
    SeqPosTask *t = dynamic_cast<SeqPosTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    const QStringList& resFileNames = t->getOutputFiles();
    foreach(const QString& fn, resFileNames){
        context->getMonitor()->addOutputFile(fn, getActor()->getId(), true);
    }

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
    }
}

SeqPosSettings SeqPosWorker::createSeqPosSettings( U2OpStatus &/*os*/ ){
    SeqPosSettings settings;

    settings.outDir = getValue<QString>(OUTPUT_DIR);
    settings.genomeAssembly = getValue<QString>(GENOME_ASSEMBLY);
    settings.findDeNovo = getValue<bool>(FIND_DE_NOVO);
    settings.motifDB = getValue<QString>(MOTIF_DB);
    settings.outName = getValue<QString>(OUT_NAME);
    settings.regWidth = getValue<int>(REG_WIDTH);
    settings.pVal = getValue<float>(P_VAL);

    return settings;
}

/************************************************************************/
/* SeqPosComboBoxWithChecksDelegate */
/************************************************************************/
PropertyWidget * SeqPosComboBoxWithChecksDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    SeqPosComboBoxWithChecksWidget* widget = new SeqPosComboBoxWithChecksWidget(items, parent);
    widget->setHint("<div  align=\"justify\"><font color=\"green\"><b>" + tr("Hint:") + "</b> " + tr("Use 'cistrome.xml' to descrease the computation time. It is a comprehensive collection of motifs from the other databases with similar motifs deleted.") + "</font></div>");
    return widget;
}

/************************************************************************/
/* SeqPosComboBoxWithChecksWidget */
/************************************************************************/
SeqPosComboBoxWithChecksWidget::SeqPosComboBoxWithChecksWidget(const QVariantMap &items, QWidget *parent) :
    ComboBoxWithChecksWidget(items, parent),
    hintLabel(NULL) {
    QLayout* l = layout();
    if (l) {
        delete l;
    }

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 2);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    hintLabel = new QLabel;
    hintLabel->setWordWrap(true);
    hintLabel->hide();

    mainLayout->addWidget(comboBox);
    mainLayout->addWidget(hintLabel);
}

void SeqPosComboBoxWithChecksWidget::setValue(const QVariant &value) {
    ComboBoxWithChecksWidget::setValue(value);
    checkHint();
}

void SeqPosComboBoxWithChecksWidget::sl_valueChanged(int) {
    checkHint();
    ComboBoxWithChecksWidget::sl_valueChanged(0);
}

void SeqPosComboBoxWithChecksWidget::checkHint() {
    QStringList values = ComboBoxWithChecksWidget::value().toString().split(",");
    if (values.count() != 1 || values.first() != SeqPosSettings::MOTIF_DB_CISTROME) {
        hintLabel->show();
        layout()->setSpacing(6);
        layout()->setContentsMargins(0, 0, 0, 3);
    } else {
        hintLabel->hide();
        layout()->setSpacing(0);
    }
}

/************************************************************************/
/* SeqPosComboBoxWithUrlsDelegate */
/************************************************************************/
void SeqPosComboBoxWithUrlsDelegate::updateUgeneSettings() {
    updateDataPath(getDataPathName(), SeqPosSupport::ASSEMBLY_DIR_NAME, true);
}

QString SeqPosComboBoxWithUrlsDelegate::getDataPathName() {
    return SeqPosSupport::ASSEMBLY_DIR;
}

QString SeqPosComboBoxWithUrlsDelegate::getAttributeName() {
    return GENOME_ASSEMBLY;
}

/************************************************************************/
/* Factory */
/************************************************************************/


void SeqPosWorkerFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(SeqPosSupport::ASSEMBLY_DIR);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }

    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor treatDesc(ANNOT_SLOT_ID,
        SeqPosWorker::tr("Input regions"),
        SeqPosWorker::tr("Peak summits."));
    inTypeMap[treatDesc] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        SeqPosWorker::tr("SeqPos data"),
        SeqPosWorker::tr("Finds motifs enriched in a set of regions."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    QList<Attribute*> attrs;
    {
         Descriptor outDir(OUTPUT_DIR,
             SeqPosWorker::tr("Output folder"),
             SeqPosWorker::tr("The folder to store seqpos results."));
         Descriptor genAssembly(GENOME_ASSEMBLY,
             SeqPosWorker::tr("Genome assembly version"),
             SeqPosWorker::tr("UCSC database version (GENOME)."));
         Descriptor findDeNovo(FIND_DE_NOVO,
             SeqPosWorker::tr("De novo motifs"),
             SeqPosWorker::tr("Run de novo motif search (-d)."));
         Descriptor motifDB(MOTIF_DB,
             SeqPosWorker::tr("Motif database"),
             SeqPosWorker::tr("Known motif collections (-m). Warning: computation time increases with selecting additional databases. "
             "It is recommended to use cistrome.xml. It is a comprehensive collection of motifs from the other databases with similar motifs deleted."));
         Descriptor outName(OUT_NAME,
             SeqPosWorker::tr("Output file name"),
             SeqPosWorker::tr("Name of the output file which stores new motifs found during a de novo search (-n)."));
         Descriptor regW(REG_WIDTH,
             SeqPosWorker::tr("Region width"),
             SeqPosWorker::tr("Width of the region to be scanned for motifs; depends on a resolution of assay (-w)."));
         Descriptor pVal(P_VAL,
             SeqPosWorker::tr("Pvalue cutoff"),
             SeqPosWorker::tr("Pvalue cutoff for the motif significance (-p)."));


        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, QVariant(""));
        Attribute* assemblyVerAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                assemblyVerAttr = new Attribute(genAssembly, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
            }else{
                assemblyVerAttr = new Attribute(genAssembly, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            assemblyVerAttr = new Attribute(genAssembly, BaseTypes::STRING_TYPE(), true);
        }
        attrs << assemblyVerAttr;
        attrs << new Attribute(outName, BaseTypes::STRING_TYPE(), true, QVariant("Default"));
        attrs << new Attribute(findDeNovo, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(motifDB, BaseTypes::STRING_TYPE(), false, QVariant(SeqPosSettings::MOTIF_DB_CISTROME));

        attrs << new Attribute(regW, BaseTypes::NUM_TYPE(), false, QVariant(600));
        attrs << new Attribute(pVal, BaseTypes::NUM_TYPE(), false, QVariant(0.001));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
          delegates[OUTPUT_DIR] = new URLDelegate("", "", false, true);
         {
            QVariantMap vm;
            vm["minimum"] = QVariant(0);
            vm["maximum"] = INT_MAX;
            vm["singleStep"] = QVariant(100);

            delegates[REG_WIDTH] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap contentMap;
             contentMap[SeqPosSettings::MOTIF_DB_CISTROME] = true;
             contentMap[SeqPosSettings::MOTIF_DB_PDM] = false;
             contentMap[SeqPosSettings::MOTIF_DB_Y1H] = false;
             contentMap[SeqPosSettings::MOTIF_DB_TRANSFAC] = false;
             contentMap[SeqPosSettings::MOTIF_DB_HDPI] = false;
             contentMap[SeqPosSettings::MOTIF_DB_JASPAR] = false;
             delegates[MOTIF_DB] = new SeqPosComboBoxWithChecksDelegate(contentMap);
         }
         {
             QVariantMap vm;
             if (dataPath) {
                 vm = dataPath->getDataItemsVariantMap();
             }
             delegates[GENOME_ASSEMBLY] = new SeqPosComboBoxWithUrlsDelegate(vm, true);
         }
         {
             QVariantMap vm;
             vm["minimum"] = QVariant(0);
             vm["maximum"] = QVariant(1);
             vm["singleStep"] = QVariant(0.001);

             delegates[P_VAL] = new DoubleSpinBoxDelegate(vm);
         }
    }

    Descriptor protoDesc(SeqPosWorkerFactory::ACTOR_ID,
    SeqPosWorker::tr("Collect Motifs with SeqPos"),
    SeqPosWorker::tr("Finds motifs enriched in a set of regions."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new SeqPosPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(SeqPosSupport::ET_SEQPOS_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new SeqPosWorkerFactory());
}

Worker *SeqPosWorkerFactory::createWorker(Actor *a) {
    return new SeqPosWorker(a);
}

QString SeqPosPrompter::composeRichDoc() {
    QString res = "";

     Actor* annProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(ANNOT_SLOT_ID);

     QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
     QString annUrl = annProducer ? annProducer->getLabel() : unsetStr;

     QString dir = getHyperlink(OUTPUT_DIR, getURL(OUTPUT_DIR));
     bool deNovo = getParameter(FIND_DE_NOVO).toBool();
     QString motifDb = getParameter(MOTIF_DB).toString();
     QString genome = getParameter(GENOME_ASSEMBLY).toString();

     res.append(tr("Uses regions from <u>%1</u> to find motifs enriched in them.").arg(annUrl));
     res.append(tr(" Genome assembly: <u>%1</u>.").arg(genome.isEmpty()? unsetStr : genome));
     if (deNovo){
         res.append(tr(" Finds de novo motifs."));
     }
     if(!motifDb.isEmpty()){
         res.append(tr(" Uses <u>%1</u> known motifs.").arg(motifDb));
     }

     res.append(tr(" Outputs all result files to <u>%1</u> folder").arg(dir.isEmpty() ? unsetStr : dir));
     res.append(".");

    return res;
}

} // LocalWorkflow
} // U2
