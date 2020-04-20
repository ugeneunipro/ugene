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
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "HMMBuildDialogController.h"
#include "HMMBuildWorker.h"
#include "HMMIOWorker.h"
#include "u_calibrate/HMMCalibrateTask.h"

namespace U2 {
namespace LocalWorkflow {

/******************************
 * HMMBuildWorkerFactory
 ******************************/
const QString HMMBuildWorkerFactory::ACTOR("hmm2-build");
static const QString OUT_HMM_PORT_ID("out-hmm2");
static const QString MODE_ATTR("strategy");
static const QString NAME_ATTR("profile-name");
// int     nsample;      // number of random seqs to sample
// int     seed;         // random number seed             
// int     fixedlen;     // fixed length, or 0 if unused
// float   lenmean;      // mean of length distribution
// float   lensd;        // std dev of length distribution 

static const QString CALIBRATE_ATTR("calibrate");
static const QString THREADS_ATTR("calibration-threads");
static const QString FIXEDLEN_ATTR("fix-samples-length");
static const QString LENMEAN_ATTR("mean-samples-length");
static const QString NUM_ATTR("samples-num");
static const QString LENDEV_ATTR("deviation");
static const QString SEED_ATTR("seed");

static const QString HMM_PROFILE_DEFAULT_NAME("hmm_profile");

static bool isDefaultCfg(PrompterBaseImpl* actor) {
    return int(P7_LS_CONFIG) == actor->getParameter(MODE_ATTR).toInt()
        && 5000 == actor->getParameter(NUM_ATTR).toInt()
        && 0 == actor->getParameter(SEED_ATTR).toInt()
        && 0 == actor->getParameter(FIXEDLEN_ATTR).toInt()
        && 325 == actor->getParameter(LENMEAN_ATTR).toInt()
        && double(200) == actor->getParameter(LENDEV_ATTR).toDouble();
}

void HMMBuildWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    {
        Descriptor id(BasePorts::IN_MSA_PORT_ID(), HMMBuildWorker::tr("Input MSA"), 
            HMMBuildWorker::tr("Input multiple sequence alignment for building statistical model."));
        Descriptor od(OUT_HMM_PORT_ID, HMMBuildWorker::tr("HMM profile"), HMMBuildWorker::tr("Produced HMM profile"));
        
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("hmm.build.in", inM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[HMMLib::HMM2_SLOT()] = HMMLib::HMM_PROFILE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("hmm.build", outM)), false /*input*/, true /*multi*/);
    }

    Descriptor mod(MODE_ATTR, HMMBuildWorker::tr("HMM strategy"), HMMBuildWorker::tr("Specifies kind of alignments you want to allow."));
    Descriptor nad(NAME_ATTR, HMMBuildWorker::tr("Profile name"), HMMBuildWorker::tr("Descriptive name of the HMM profile."));
    // int     nsample;      // number of random seqs to sample
    // int     seed;         // random number seed             
    // int     fixedlen;     // fixed length, or 0 if unused
    // float   lenmean;      // mean of length distribution
    // float   lensd;        // std dev of length distribution 

    Descriptor cad(CALIBRATE_ATTR, HMMBuildWorker::tr("Calibrate profile"), HMMBuildWorker::tr("Enables/disables optional profile calibration." 
        "<p>An empirical HMM calibration costs time but it only has to be done once per model, and can greatly increase the sensitivity of a database search."));
    Descriptor td(THREADS_ATTR, HMMBuildWorker::tr("Parallel calibration"), 
        HMMBuildWorker::tr("Number of parallel threads that the calibration will run in."));

    Descriptor fid(FIXEDLEN_ATTR, HMMBuildWorker::tr("Fixed length of samples"), 
        QApplication::translate("HMMBuildWorker", "Fix the length of the random sequences to <n>, where <n> is a positive (and reasonably sized) integer. "
        "<p>The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.", 0));
    Descriptor lmd(LENMEAN_ATTR, HMMBuildWorker::tr("Mean length of samples"), 
        QApplication::translate("HMMBuildWorker", "Mean length of the synthetic sequences, positive real number. The default value is 325.", 0));
    Descriptor nud(NUM_ATTR, HMMBuildWorker::tr("Number of samples"), 
        QApplication::translate("HMMBuildWorker", "Number of synthetic sequences. If <n> is less than about 1000, the fit to the EVD may fail. "
        "<p>Higher numbers of <n> will give better determined EVD parameters. "
        "<p>The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.", 0));
    Descriptor ldd(LENDEV_ATTR, HMMBuildWorker::tr("Standard deviation"), 
        QApplication::translate("HMMBuildWorker", "Standard deviation of the synthetic sequence length. A positive number. "
        "<p>The default is 200. Note that the Gaussian is left-truncated so that no sequences have lengths <= 0.", 0));
    Descriptor sed(SEED_ATTR, HMMBuildWorker::tr("Random seed"),
        QApplication::translate("HMMBuildWorker", "The random seed, where <n> is a positive integer. "
        "<p>The default is to use time() to generate a different seed for each run, "
        "<p>which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. "
        "<p>You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.", 0));

//     nsample      = 5000;
//     fixedlen     = 0;
//     lenmean      = 325.;
//     lensd        = 200.;
//     seed         = (int) time ((time_t *) NULL);

    a << new Attribute(nad, BaseTypes::STRING_TYPE(), false, QVariant(HMM_PROFILE_DEFAULT_NAME));
    a << new Attribute(cad, BaseTypes::BOOL_TYPE(), false, QVariant(true));
    a << new Attribute(mod, BaseTypes::NUM_TYPE(), false, QVariant(int(P7_LS_CONFIG)));
    a << new Attribute(nud, BaseTypes::NUM_TYPE(), false, QVariant(5000));
    a << new Attribute(sed, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(fid, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(lmd, BaseTypes::NUM_TYPE(), false, QVariant(325));
    a << new Attribute(ldd, BaseTypes::NUM_TYPE(), false, QVariant(double(200)));
    a << new Attribute(td, BaseTypes::NUM_TYPE(), false, QVariant(1));

    Descriptor desc(HMMBuildWorkerFactory::ACTOR, HMMBuildWorker::tr("HMM2 Build"), HMMBuildWorker::tr("Builds a HMM profile from a multiple sequence alignment."
        "<p>The HMM profile is a statistical model which captures position-specific information"
        " about how conserved each column of the alignment is, and which residues are likely."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;    
    
    {
        QVariantMap lenMap; lenMap["minimum"] = 0; lenMap["maximum"] = INT_MAX;
        delegates[FIXEDLEN_ATTR] = new SpinBoxDelegate(lenMap);
    }
    {
        QVariantMap numMap; numMap["minimum"] = 1; numMap["maximum"] = INT_MAX;
        delegates[NUM_ATTR] = new SpinBoxDelegate(numMap);
    }
    {
        QVariantMap m; m["minimum"] = 0; m["maximum"] = INT_MAX;
        delegates[SEED_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = 1; m["maximum"] = INT_MAX;
        delegates[LENMEAN_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = double(.01); m["maximum"] = double(1000000.00); m["decimals"] = 2;
        delegates[LENDEV_ATTR] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = 1; m["maximum"] = 100;
        delegates[THREADS_ATTR] = new SpinBoxDelegate(m);
    }

    QVariantMap modeMap; 
    modeMap["hmms"] = QVariant(P7_BASE_CONFIG);
    modeMap["hmmfs"] = QVariant(P7_FS_CONFIG);
    modeMap[QString("hmmls (%1)").arg(HMMBuildWorker::tr("Default"))] = QVariant(P7_LS_CONFIG);
    modeMap["hmmsw"] = QVariant(P7_SW_CONFIG);
    delegates[MODE_ATTR] = new ComboBoxDelegate(modeMap);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath( ":/hmm2/images/hmmer_16.png" );
    proto->setPrompter(new HMMBuildPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(HMMLib::HMM_CATEGORY(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new HMMBuildWorkerFactory());
}

void HMMBuildWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR);
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR);
}

/******************************
 * HMMBuildPrompter
 ******************************/
QString HMMBuildPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* msaProducer = input->getProducer(BasePorts::IN_MSA_PORT_ID());

    QString msaName = msaProducer ? tr("For each MSA from <u>%1</u>,").arg(msaProducer->getLabel()) : "";

    QString calibrate;
    if (getParameter(CALIBRATE_ATTR).toBool()) {
        calibrate = tr(" and calibrate");
    }
    QString cfg = isDefaultCfg(this) ? tr("default") : tr("custom");

    QString doc = tr("%1 build%2 HMM profile using <u>%3</u> settings.")
        .arg(msaName)
        .arg(calibrate)
        .arg(cfg);

    return doc;
}

/******************************
* HMMBuildWorker
******************************/
HMMBuildWorker::HMMBuildWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL), nextTick(NULL), calibrate(false) {
}

void HMMBuildWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(OUT_HMM_PORT_ID);
}

bool HMMBuildWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    return nextTick || input->hasMessage() || input->isEnded();
}

Task* HMMBuildWorker::tick() {
	if(calSettings.seed < 0){
		algoLog.error(tr("Incorrect value for seed parameter"));
		return new FailTask(tr("Incorrect value for seed parameter"));
	}
    
    if(nextTick) { // calibrate task
        Task* t = nextTick;
        nextTick = NULL;
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else { // hmm build task
        if (input->hasMessage()) {
            Message inputMessage = getMessageAndSetupScriptValues(input);
            if (inputMessage.isEmpty()) {
                output->transit();
                return NULL;
            }
            cfg.name = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
            if(cfg.name.isEmpty()){
                cfg.name = HMM_PROFILE_DEFAULT_NAME;
                algoLog.details(tr("Schema name not specified. Using default value: '%1'").arg(cfg.name));
            }
            cfg.strategy = HMMBuildStrategy(actor->getParameter(MODE_ATTR)->getAttributeValue<int>(context));
            calSettings.fixedlen = actor->getParameter(FIXEDLEN_ATTR)->getAttributeValue<int>(context);
            calSettings.lenmean = actor->getParameter(LENMEAN_ATTR)->getAttributeValue<int>(context);
            calSettings.nsample = actor->getParameter(NUM_ATTR)->getAttributeValue<int>(context);
            calSettings.lensd = (float)actor->getParameter(LENDEV_ATTR)->getAttributeValue<double>(context);
            calSettings.seed = actor->getParameter(SEED_ATTR)->getAttributeValue<int>(context);
            calSettings.nThreads = actor->getParameter(THREADS_ATTR)->getAttributeValue<int>(context);
            calibrate = actor->getParameter(CALIBRATE_ATTR)->getAttributeValue<bool>(context);

            QVariantMap qm = inputMessage.getData().toMap();
            SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
            QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
            SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
            const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();
            
            Task* t = new HMMBuildTask(cfg, msa);
            connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
            return t;
        } else if (input->isEnded()) {
            setDone();
            output->setEnded();
        }
        return NULL;
    }
}

void HMMBuildWorker::sl_taskFinished() {
    Task * t = qobject_cast<Task*>(sender());
    SAFE_POINT( NULL != t, "Invalid task is encountered", );
    if ( t->isCanceled( ) ) {
        return;
    }
    if( t->getState() != Task::State_Finished ) {
        return;
    }
    sl_taskFinished(t);
}

void HMMBuildWorker::sl_taskFinished(Task* t) {
    HMMBuildTask* build = qobject_cast<HMMBuildTask*>(t);
    SAFE_POINT( NULL != t, "Invalid task is encountered", );
    if ( t->isCanceled( ) ) {
        return;
    }
    plan7_s* hmm = NULL;
    if (build) {
        assert(!nextTick);
        hmm = build->getHMM();
        SAFE_POINT(hmm != nullptr, "HMMReadTask didn't generate \"hmm\" object, stop.", );

        if (calibrate) {
            if (calSettings.nThreads == 1) {
                nextTick = new HMMCalibrateTask(hmm, calSettings);
            } else {
                nextTick = new HMMCalibrateParallelTask(hmm, calSettings);
            }
        } else { // do not calibrate -> put hmm to output
            output->put(Message(HMMLib::HMM_PROFILE_TYPE(), qVariantFromValue<plan7_s*>(hmm)));
        }
        algoLog.info(tr("Built HMM profile"));
    } else {
        HMMCalibrateAbstractTask* calibrate = qobject_cast<HMMCalibrateAbstractTask*>(sender());
        assert(calibrate);
        hmm = calibrate->getHMM();
        output->put(Message(HMMLib::HMM_PROFILE_TYPE(), qVariantFromValue<plan7_s*>(hmm)));
        algoLog.info(tr("Calibrated HMM profile"));
    }
}

bool HMMBuildWorker::isDone() const {
    return BaseWorker::isDone() && !nextTick;
}

void HMMBuildWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
