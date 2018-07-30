/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <limits.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "SamtoolMpileupWorker.h"

namespace U2 {

namespace LocalWorkflow {


const QString CallVariantsWorkerFactory::ACTOR_ID("call_variants");

static const QString REF_SEQ_PORT_ID("ref-seq-port-id");
static const QString ASSEMBLY_PORT_ID("assembly-port-id");

const QString OUT_URL("variants-url");
const QString REF_SOURCE("reference-source");
const QString REF_SOURCE_PORT("port");
const QString REF_SOURCE_FILE("file");
const QString REF_URL("reference-url");

//mpileup
const QString ILLUMINA13("illumina13-encoding");
const QString USE_ORPHAN("use_orphan");
const QString DISABLE_BAQ("disable_baq");
const QString CAPQ_THRES("capq_thres");
const QString MAX_DEPTH("max_depth");
const QString EXT_BAQ("ext_baq");
const QString BED("bed");
const QString REG("reg");
const QString MIN_MQ("min_mq");
const QString MIN_BASEQ("min_baseq");
const QString EXTQ("extQ");
const QString TANDEMQ("tandemQ");
const QString NO_INDEL("no_indel");
const QString MAX_INDEL_DEPTH("max_indel_depth");
const QString OPENQ("openQ");
const QString PL_LIST("pl_list");

//bcf view
const QString KEEPALT("keepalt");
const QString FIX_PL("fix_pl");
const QString NO_GENO("no_geno");
const QString ACGT_ONLY("acgt_only");
const QString BCF_BED("bcf_bed");
const QString QCALL("qcall");
const QString SAMPLES("samples");
const QString MIN_SMPL_FRAC("min_smpl_frac");
const QString CALL_GT("call_gt");
const QString INDEL_FRAC("indel_frac");
const QString PREF("pref");
const QString PTYPE("ptype");
const QString THETA("theta");
const QString CCALL("ccall");
const QString N1("n1");
const QString N_PERM("n_perm");
const QString MIN_PERM_P("min_perm_p");

//varFilter
const QString MIN_QUAL("min-qual");
const QString MIN_DEP("min-dep");
const QString MAX_DEP("max-dep");
const QString MIN_ALT("min-alt-bases");
const QString GAP_SIZE("gap-size");
const QString WINDOW("window");
const QString PVALUE1("min-strand");
const QString PVALUE2("min-baseQ");
const QString PVALUE3("min-mapQ");
const QString PVALUE4("min-end-distance");
const QString PVALUE_HWE("min-hwe");
const QString PRINT("print-filtered");

class EmptySlotValidator : public ConfigurationValidator {
public:
    EmptySlotValidator(const QString& slot): screenedSlot(slot) {}

    virtual bool validate(const Configuration* cfg, ProblemList &problemList) const {
        const IntegralBusPort* vport = static_cast<const IntegralBusPort*>(cfg);
        assert(vport);

        StrStrMap bm = vport->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
        QMapIterator<QString,QString> it(bm);
        while (it.hasNext()){
            it.next();
            const QString& slot = it.key();
            QString slotName = vport->getType()->getDatatypeDescriptor(slot).getDisplayName();
            //assert(!slotName.isEmpty());
            if (it.value().isEmpty()) {
                if (screenedSlot == slot) {
                    problemList.append(Problem(CallVariantsWorker::tr("Empty input slot: %1").arg(slotName)));
                    return false;
                }
            }
        }

        return true;
    }
protected:
    QString screenedSlot;
};


void CallVariantsWorkerFactory::init() {
    //port descriptor
    QList<PortDescriptor*> p;
    {
        QMap<Descriptor, DataTypePtr> refSeqMap;
        refSeqMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr inSet(new MapDataType(REF_SEQ_PORT_ID, refSeqMap));
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(), CallVariantsWorker::tr("Input sequences"),
            CallVariantsWorker::tr("A nucleotide reference sequence."));
        p << new PortDescriptor(id, inSet, true);

        QMap<Descriptor, DataTypePtr> assMap;
        assMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        assMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr inAssemblySet(new MapDataType(ASSEMBLY_PORT_ID, assMap));
        Descriptor idA(BasePorts::IN_ASSEMBLY_PORT_ID(), CallVariantsWorker::tr("Input assembly"),
            CallVariantsWorker::tr("Position sorted alignment file"));
        p << new PortDescriptor(idA, inAssemblySet, true, false, IntegralBusPort::BLIND_INPUT);

        QMap<Descriptor, DataTypePtr> varMap;
        varMap[BaseSlots::VARIATION_TRACK_SLOT()] = BaseTypes::VARIATION_TRACK_TYPE();
        varMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outVariants(new MapDataType("variants", varMap));
        Descriptor idV(BasePorts::OUT_VARIATION_TRACK_PORT_ID(), CallVariantsWorker::tr("Output variations"),
            CallVariantsWorker::tr("Output tracks with SNPs and short INDELs"));
        p << new PortDescriptor(idV, outVariants, false, true);

    }

    Descriptor desc(ACTOR_ID, CallVariantsWorker::tr("Call Variants with SAMtools"),
        CallVariantsWorker::tr("Calls SNPs and INDELS with SAMtools mpileup and bcftools.")
        );


    //attributes
    QList<Attribute*> attributes;

    Descriptor outUrl(OUT_URL,
        CallVariantsWorker::tr("Output variants file"),
        CallVariantsWorker::tr("The url to the file with the extracted variations."));

    Descriptor refLocation(REF_SOURCE,
        CallVariantsWorker::tr("Use reference from"),
        CallVariantsWorker::tr("<p>Specify \"File\" to set a single reference sequence for all input NGS assemblies. "
                               "The reference should be set in the \"Reference\" parameter.</p>"
                               "<p>Specify \"Input port\" to be able to set different references for difference NGS assemblies. "
                               "The references should be input via the \"Input sequences\" port (e.g. use datasets in the \"Read Sequence\" element).</p>"));

    Descriptor refUrl(REF_URL,
        CallVariantsWorker::tr("Reference"),
        CallVariantsWorker::tr("<p>Specify a file with the reference sequence.</p>"
                               "<p>The sequence will be used as reference for all datasets with NGS assemblies.</p>"));

    Descriptor illumina13Encoding(ILLUMINA13,
        CallVariantsWorker::tr("Illumina-1.3+ encoding"),
        CallVariantsWorker::tr("Assume the quality is in the Illumina 1.3+ encoding (mpileup)(-6)."));

    Descriptor useOrphan(USE_ORPHAN,
        CallVariantsWorker::tr("Count anomalous read pairs"),
        CallVariantsWorker::tr("Do not skip anomalous read pairs in variant calling(mpileup)(-A)."));

    Descriptor disableBaq(DISABLE_BAQ,
        CallVariantsWorker::tr("Disable BAQ computation"),
        CallVariantsWorker::tr("Disable probabilistic realignment for the computation of base "
        "alignment quality (BAQ). BAQ is the Phred-scaled probability of a read base being misaligned. "
        "Applying this option greatly helps to reduce false SNPs caused by misalignments. (mpileup)(-B)."));

    Descriptor capqThres(CAPQ_THRES,
        CallVariantsWorker::tr("Mapping quality downgrading coefficient"),
        CallVariantsWorker::tr("Coefficient for downgrading mapping quality for reads containing excessive mismatches. "
        "Given a read with a phred-scaled mapping quality q of being generated from the mapped position, the new mapping quality "
        "is about sqrt((INT-q)/INT)*INT. A zero value disables this functionality; if enabled, the recommended value for BWA is 50 (mpileup)(-C)."));

    Descriptor maxDepth(MAX_DEPTH,
        CallVariantsWorker::tr("Max number of reads per input BAM"),
        CallVariantsWorker::tr("At a position, read maximally the number of reads per input BAM (mpileup)(-d)."));

    Descriptor extBaq(EXT_BAQ,
        CallVariantsWorker::tr("Extended BAQ computation"),
        CallVariantsWorker::tr("Extended BAQ computation. This option helps sensitivity especially for MNPs,"
        " but may hurt specificity a little bit (mpileup)(-E)."));

    Descriptor bed(BED,
        CallVariantsWorker::tr("BED or position list file"),
        CallVariantsWorker::tr("BED or position list file containing a list of regions or sites where"
        " pileup or BCF should be generated (mpileup)(-l)."));

    Descriptor reg(REG,
        CallVariantsWorker::tr("Pileup region"),
        CallVariantsWorker::tr("Only generate pileup in region STR (mpileup)(-r)."));

    Descriptor minMq(MIN_MQ,
        CallVariantsWorker::tr("Minimum mapping quality"),
        CallVariantsWorker::tr("Minimum mapping quality for an alignment to be used (mpileup)(-q)."));

    Descriptor minBaseq(MIN_BASEQ,
        CallVariantsWorker::tr("Minimum base quality"),
        CallVariantsWorker::tr("Minimum base quality for a base to be considered (mpileup)(-Q)."));

    Descriptor extq(EXTQ,
        CallVariantsWorker::tr("Gap extension error"),
        CallVariantsWorker::tr("Phred-scaled gap extension sequencing error probability. Reducing INT leads to longer indels (mpileup)(-e)."));

    Descriptor tandemq(TANDEMQ,
        CallVariantsWorker::tr("Homopolymer errors coefficient"),
        CallVariantsWorker::tr("Coefficient for modeling homopolymer errors. Given an l-long homopolymer run, the sequencing error of an indel of size s is modeled as INT*s/l (mpileup)(-h)."));

    Descriptor noIndel(NO_INDEL,
        CallVariantsWorker::tr("No INDELs"),
        CallVariantsWorker::tr("Do not perform INDEL calling (mpileup)(-I)."));

    Descriptor maxIndelDepth(MAX_INDEL_DEPTH,
        CallVariantsWorker::tr("Max INDEL depth"),
        CallVariantsWorker::tr("Skip INDEL calling if the average per-sample depth is above INT (mpileup)(-L)."));

    Descriptor openq(OPENQ,
        CallVariantsWorker::tr("Gap open error"),
        CallVariantsWorker::tr("Phred-scaled gap open sequencing error probability. Reducing INT leads to more indel calls (mpileup)(-o)."));

    Descriptor plList(PL_LIST,
        CallVariantsWorker::tr("List of platforms for indels"),
        CallVariantsWorker::tr("Comma dilimited list of platforms (determined by @RG-PL) from which indel candidates are obtained."
        "It is recommended to collect indel candidates from sequencing technologies that have low indel error rate such as ILLUMINA (mpileup)(-P)."));

    //bcf view
    Descriptor keepalt(KEEPALT,
        CallVariantsWorker::tr("Retain all possible alternate"),
        CallVariantsWorker::tr("Retain all possible alternate alleles at variant sites. By default, the view command discards unlikely alleles (bcf view)(-A)."));

    Descriptor fixPl(FIX_PL,
        CallVariantsWorker::tr("Indicate PL"),
        CallVariantsWorker::tr("Indicate PL is generated by r921 or before (ordering is different) (bcf view)(-F)."));

    Descriptor noGeo(NO_GENO,
        CallVariantsWorker::tr("No genotype information"),
        CallVariantsWorker::tr("Suppress all individual genotype information (bcf view)(-G)."));

    Descriptor acgtO(ACGT_ONLY,
        CallVariantsWorker::tr("A/C/G/T only"),
        CallVariantsWorker::tr("Skip sites where the REF field is not A/C/G/T (bcf view)(-N)."));

    Descriptor bcfBed(BCF_BED,
        CallVariantsWorker::tr("List of sites"),
        CallVariantsWorker::tr("List of sites at which information are outputted (bcf view)(-l)."));

    Descriptor qcall(QCALL,
        CallVariantsWorker::tr("QCALL likelihood"),
        CallVariantsWorker::tr("Output the QCALL likelihood format (bcf view)(-Q)."));

    Descriptor attrSamples(SAMPLES,
        CallVariantsWorker::tr("List of samples"),
        CallVariantsWorker::tr("List of samples to use. The first column in the input gives"
        " the sample names and the second gives the ploidy, which can only be 1 or 2. When "
        "the 2nd column is absent, the sample ploidy is assumed to be 2. In the output, the ordering of samples "
        "will be identical to the one in FILE (bcf view)(-s)."));

    Descriptor minSmpl(MIN_SMPL_FRAC,
        CallVariantsWorker::tr("Min samples fraction"),
        CallVariantsWorker::tr("skip loci where the fraction of samples covered by reads is below FLOAT (bcf view)(-d)."));

    Descriptor callGt(CALL_GT,
        CallVariantsWorker::tr("Per-sample genotypes"),
        CallVariantsWorker::tr("Call per-sample genotypes at variant sites (bcf view)(-g)."));

    Descriptor indelFrac(INDEL_FRAC,
        CallVariantsWorker::tr("INDEL-to-SNP Ratio"),
        CallVariantsWorker::tr("Ratio of INDEL-to-SNP mutation rate (bcf view)(-i)."));

    Descriptor pref(PREF,
        CallVariantsWorker::tr("Max P(ref|D)"),
        CallVariantsWorker::tr("A site is considered to be a variant if P(ref|D)<FLOAT (bcf view)(-p)."));

    Descriptor ptype(PTYPE,
        CallVariantsWorker::tr("Prior allele frequency spectrum"),
        CallVariantsWorker::tr("If STR can be full, cond2, flat or the file consisting of error output from a previous variant calling run (bcf view)(-P)."));

    Descriptor theta(THETA,
        CallVariantsWorker::tr("Mutation rate"),
        CallVariantsWorker::tr("Scaled mutation rate for variant calling (bcf view)(-t)."));

    Descriptor ccall(CCALL,
        CallVariantsWorker::tr("Pair/trio calling"),
        CallVariantsWorker::tr("Enable pair/trio calling. For trio calling, option -s is usually needed to be"
        " applied to configure the trio members and their ordering. In the file supplied to the option -s, the "
        "first sample must be the child, the second the father and the third the mother. The valid values of STR "
        "are 'pair', 'trioauto', 'trioxd' and 'trioxs', where 'pair' calls differences between two input samples, and 'trioxd' ('trioxs')"
        "specifies that the input is from the X chromosome non-PAR regions and the child is a female (male) (bcf view)(-T)."));

    Descriptor n1(N1,
        CallVariantsWorker::tr("N group-1 samples"),
        CallVariantsWorker::tr("Number of group-1 samples. This option is used for dividing the samples "
        "into two groups for contrast SNP calling or association test. When this option is in use, the following"
        "VCF INFO will be outputted: PC2, PCHI2 and QCHI2 (bcf view)(-1)."));

    Descriptor n_perm(N_PERM,
        CallVariantsWorker::tr("N permutations"),
        CallVariantsWorker::tr("Number of permutations for association test (effective only with -1) (bcf view)(-U)."));

    Descriptor min_perm_p(MIN_PERM_P,
        CallVariantsWorker::tr("Max P(chi^2)"),
        CallVariantsWorker::tr("Only perform permutations for P(chi^2)<FLOAT (N permutations) (bcf view)(-X)."));

    //varFilter
    Descriptor minQual(MIN_QUAL,
        CallVariantsWorker::tr("Minimum RMS quality"),
        CallVariantsWorker::tr("Minimum RMS mapping quality for SNPs (varFilter) (-Q)."));
    Descriptor minDep(MIN_DEP,
        CallVariantsWorker::tr("Minimum read depth"),
        CallVariantsWorker::tr("Minimum read depth (varFilter) (-d)."));
    Descriptor maxDep(MAX_DEP,
        CallVariantsWorker::tr("Maximum read depth"),
        CallVariantsWorker::tr("Maximum read depth (varFilter) (-D)."));
    Descriptor minAlt(MIN_ALT,
        CallVariantsWorker::tr("Alternate bases"),
        CallVariantsWorker::tr("Minimum number of alternate bases (varFilter) (-a)."));
    Descriptor gapSize(GAP_SIZE,
        CallVariantsWorker::tr("Gap size"),
        CallVariantsWorker::tr("SNP within INT bp around a gap to be filtered (varFilter) (-w)."));
    Descriptor window(WINDOW,
        CallVariantsWorker::tr("Window size"),
        CallVariantsWorker::tr("Window size for filtering adjacent gaps (varFilter) (-W)."));
    Descriptor pvalue1(PVALUE1,
        CallVariantsWorker::tr("Strand bias"),
        CallVariantsWorker::tr("Minimum P-value for strand bias (given PV4) (varFilter) (-1)."));
    Descriptor pvalue2(PVALUE2,
        CallVariantsWorker::tr("BaseQ bias"),
        CallVariantsWorker::tr("Minimum P-value for baseQ bias (varFilter) (-2)."));
    Descriptor pvalue3(PVALUE3,
        CallVariantsWorker::tr("MapQ bias"),
        CallVariantsWorker::tr("Minimum P-value for mapQ bias (varFilter) (-3)."));
    Descriptor pvalue4(PVALUE4,
        CallVariantsWorker::tr("End distance bias"),
        CallVariantsWorker::tr("Minimum P-value for end distance bias (varFilter) (-4)."));
    Descriptor pvalueHwe(PVALUE_HWE,
        CallVariantsWorker::tr("HWE"),
        CallVariantsWorker::tr("Minimum P-value for HWE (plus F<0) (varFilter) (-e)."));
    Descriptor printF(PRINT,
        CallVariantsWorker::tr("Log filtered"),
        CallVariantsWorker::tr("Print filtered variants into the log (varFilter) (-p)."));

    attributes << new Attribute(outUrl, BaseTypes::STRING_TYPE(), true, "");

    Attribute* refUrlAttr = new Attribute(refUrl, BaseTypes::STRING_TYPE(), true, "");
    QVariantList refUrlVisibilityValues;
    refUrlVisibilityValues << QVariant(REF_SOURCE_FILE);
    refUrlAttr->addRelation(new VisibilityRelation(REF_SOURCE, refUrlVisibilityValues));
    attributes << refUrlAttr;

    Attribute* refLocationAttr = new Attribute(refLocation, BaseTypes::STRING_TYPE(), false, QVariant(REF_SOURCE_FILE));
    QVariantList refPortVisibilityValues;
    refPortVisibilityValues << QVariant(REF_SOURCE_PORT);
    refLocationAttr->addPortRelation(new PortRelationDescriptor( BasePorts::IN_SEQ_PORT_ID(), refPortVisibilityValues));
    attributes << refLocationAttr;

    attributes << new Attribute(illumina13Encoding, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(useOrphan, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(disableBaq, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(capqThres, BaseTypes::NUM_TYPE(), false, QVariant(0));
    attributes << new Attribute(maxDepth, BaseTypes::NUM_TYPE(), false, QVariant(250));
    attributes << new Attribute(extBaq, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(bed, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(reg, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(minMq, BaseTypes::NUM_TYPE(), false, QVariant(0));
    attributes << new Attribute(minBaseq, BaseTypes::NUM_TYPE(), false, QVariant(13));
    attributes << new Attribute(extq, BaseTypes::NUM_TYPE(), false, QVariant(20));
    attributes << new Attribute(tandemq, BaseTypes::NUM_TYPE(), false, QVariant(100));
    attributes << new Attribute(noIndel, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(maxIndelDepth, BaseTypes::NUM_TYPE(), false, QVariant(250));
    attributes << new Attribute(openq, BaseTypes::NUM_TYPE(), false, QVariant(40));
    attributes << new Attribute(plList, BaseTypes::STRING_TYPE(), false, QVariant(""));

    //bcf vew
    attributes << new Attribute(keepalt, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(fixPl, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(noGeo, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(acgtO, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(bcfBed, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(qcall, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(attrSamples, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(minSmpl, BaseTypes::NUM_TYPE(), false, QVariant(0));
    attributes << new Attribute(callGt, BaseTypes::BOOL_TYPE(), false, QVariant(true));
    attributes << new Attribute(indelFrac, BaseTypes::NUM_TYPE(), false, QVariant(-1.0));
    attributes << new Attribute(pref, BaseTypes::NUM_TYPE(), false, QVariant(0.5));
    attributes << new Attribute(ptype, BaseTypes::STRING_TYPE(), false, QVariant("full"));
    attributes << new Attribute(theta, BaseTypes::NUM_TYPE(), false, QVariant(0.001));
    attributes << new Attribute(ccall, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(n1, BaseTypes::NUM_TYPE(), false, QVariant(0.0));
    attributes << new Attribute(n_perm, BaseTypes::NUM_TYPE(), false, QVariant(0));
    attributes << new Attribute(min_perm_p, BaseTypes::NUM_TYPE(), false, QVariant(0.01));

    //varFilter
    attributes << new Attribute(minQual, BaseTypes::NUM_TYPE(), false, 10);
    attributes << new Attribute(minDep, BaseTypes::NUM_TYPE(), false, 2);
    attributes << new Attribute(maxDep, BaseTypes::NUM_TYPE(), false, 10000000);
    attributes << new Attribute(minAlt, BaseTypes::NUM_TYPE(), false, 2);
    attributes << new Attribute(gapSize, BaseTypes::NUM_TYPE(), false, 3);
    attributes << new Attribute(window, BaseTypes::NUM_TYPE(), false, 10);
    attributes << new Attribute(pvalue1, BaseTypes::NUM_TYPE(), false, 0.0001);
    attributes << new Attribute(pvalue2, BaseTypes::STRING_TYPE(), false, "1e-100");
    attributes << new Attribute(pvalue3, BaseTypes::NUM_TYPE(), false, 0);
    attributes << new Attribute(pvalue4, BaseTypes::NUM_TYPE(), false, 0.0001);
    attributes << new Attribute(pvalueHwe, BaseTypes::NUM_TYPE(), false, 0.0001);
    attributes << new Attribute(printF, BaseTypes::BOOL_TYPE(), false, false);

    //prototype
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, attributes);
    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[CAPQ_THRES] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MAX_DEPTH] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MIN_MQ] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 1;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MIN_BASEQ] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 1;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[EXTQ] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 1;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[TANDEMQ] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 1;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MAX_INDEL_DEPTH] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 1;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[OPENQ] = new SpinBoxDelegate(vm);
    }

    delegates[BED] = new URLDelegate("", "", false, false, false);

    //bcf view
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = 1;
        vm["singleStep"] = 0.1;
        delegates[MIN_SMPL_FRAC] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = -1.0;
        vm["maximum"] = 1.0;
        vm["singleStep"] = 0.1;
        delegates[INDEL_FRAC] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = 1;
        vm["singleStep"] = 0.1;
        delegates[PREF] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm[""] = "";
        vm["full"] = "full";
        vm["cond2"] = "cond2";
        vm["flat"] = "flat";
        delegates[PTYPE] = new ComboBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = 1;
        vm["singleStep"] = 0.1;
        delegates[THETA] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm[""] = "";
        vm["pair"] = "pair";
        vm["trioauto"] = "trioauto";
        vm["trioxd"] = "trioxd";
        vm["trioxs"] = "trioxs";
        delegates[CCALL] = new ComboBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[N1] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[N_PERM] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = 1;
        vm["singleStep"] = 0.1;
        delegates[MIN_PERM_P] = new DoubleSpinBoxDelegate(vm);
    }

    //varFilter
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MIN_QUAL] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MIN_DEP] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MAX_DEP] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MIN_ALT] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[GAP_SIZE] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[WINDOW] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 0.0001;
        delegates[PVALUE1] = new DoubleSpinBoxDelegate(vm);
    }
    {
        delegates[PVALUE2] = new ScientificDoubleDelegate();
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 0.0001;
        delegates[PVALUE3] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 0.0001;
        delegates[PVALUE4] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 0.0001;
        delegates[PVALUE_HWE] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm[CallVariantsWorker::tr("Input port")] = REF_SOURCE_PORT;
        vm[CallVariantsWorker::tr("File")] = REF_SOURCE_FILE;
        delegates[REF_SOURCE] = new ComboBoxDelegate(vm);
    }

    delegates[OUT_URL] = new URLDelegate("", "", false, false, true, NULL, "vcf");
    delegates[REF_URL] = new URLDelegate("", "", false, false, false);
    delegates[BCF_BED] = new URLDelegate("", "", false, false, false);
    delegates[SAMPLES] = new URLDelegate("", "", false, false, false);


    // Init and register the actor prototype
    proto->setEditor(new DelegateEditor(delegates));

    proto->setPrompter(new CallVariantsPrompter());

    proto->setPortValidator(BasePorts::IN_ASSEMBLY_PORT_ID(), new EmptySlotValidator(BaseSlots::URL_SLOT().getId()));
    proto->setPortValidator(BasePorts::IN_SEQ_PORT_ID(), new EmptySlotValidator(BaseSlots::URL_SLOT().getId()));
    proto->addExternalTool("SAMtools");
    proto->addExternalTool("BCFtools");
    proto->addExternalTool("perl");
    proto->addExternalTool("vcfutils");

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_VARIATION_ANALYSIS(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new CallVariantsWorkerFactory());
}


QString CallVariantsPrompter::composeRichDoc() {
    QString reference;
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    Port* refPort = target->getPort(BasePorts::IN_SEQ_PORT_ID());
    if (refPort->isEnabled()) {
        Actor* seqProducer = qobject_cast<IntegralBusPort*>(refPort)->getProducer(BaseSlots::URL_SLOT().getId());
        reference = seqProducer ? seqProducer->getLabel() : unsetStr;
    } else {
        reference = getHyperlink(REF_URL, getURL(REF_URL));
    }
    QString seqName = tr("For reference sequence from <u>%1</u>,").arg(reference);

    Actor* assemblyProducer = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_ASSEMBLY_PORT_ID()))->getProducer(BaseSlots::URL_SLOT().getId());
    QString assemblyName = tr("with assembly data provided by <u>%1</u>").arg(assemblyProducer ? assemblyProducer->getLabel() : unsetStr);

    QString doc = tr("%1 call variants %2.")
        .arg(seqName)
        .arg(assemblyName);

    return doc;
}

CallVariantsWorker::CallVariantsWorker(Actor* a)
    : BaseWorker(a, false),
      refSeqPort(NULL),
      assemblyPort(NULL),
      output(NULL),
      useDatasets(false)
{
    referenceSource = FromPort;
}

void CallVariantsWorker::initDatasetMode() {
    Port *port = actor->getPort(BasePorts::IN_ASSEMBLY_PORT_ID());
    SAFE_POINT(NULL != port,"Internal error during CallVariantsWorker initializing: assembly port is NULL!", );

    IntegralBusPort *bus = dynamic_cast<IntegralBusPort*>(port);
    SAFE_POINT(NULL != bus, "Internal error during CallVariantsWorker initializing: assembly bus is NULL!", );

    QList<Actor*> producers = bus->getProducers(BaseSlots::DATASET_SLOT().getId());
    useDatasets = !producers.isEmpty();
}

void CallVariantsWorker::init() {
    refSeqPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
    assemblyPort = ports.value(BasePorts::IN_ASSEMBLY_PORT_ID());
    output = ports.value(BasePorts::OUT_VARIATION_TRACK_PORT_ID());

    if (getValue<QString>(REF_SOURCE) == REF_SOURCE_PORT) {
        referenceSource = FromPort;
    } else {
        referenceSource = FromFile;
    }

    settings = getSettings();

    output->addComplement(refSeqPort);
    refSeqPort->addComplement(output);
    initDatasetMode();
}

bool CallVariantsWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    bool assemblyEnded = assemblyPort->isEnded();
    bool assemblyHasMes = (assemblyPort->hasMessage() > 0);

    if (referenceSource == FromFile) {
        return assemblyHasMes || assemblyEnded;
    }

    bool seqEnded = refSeqPort->isEnded();
    bool seqHasMes = (refSeqPort->hasMessage() > 0);
    if (seqHasMes && assemblyHasMes) {
        return true;
    }

    if ((seqEnded && assemblyHasMes) || (assemblyEnded && seqHasMes)) {
        return true;
    }

    return seqEnded && assemblyEnded;
}

Task* CallVariantsWorker::tick() {
    U2OpStatus2Log os;

    //put variant tracks
    while (!cache.isEmpty()) {
        output->put(cache.takeFirst());
    }

    checkState(os);
    CHECK_OP_EXT(os, setDone(), NULL);

    //take assemblies from one dataset
    if (assemblyPort->hasMessage() && settings.assemblyUrls.isEmpty()) {
        takeAssembly(os);
        CHECK_OP_EXT(os, processError(os), NULL);
    } else if (settings.assemblyUrls.isEmpty() && !assemblyUrls.isEmpty()) {
        settings.assemblyUrls = assemblyUrls;
        assemblyUrls.clear();
    }

    //take reference sequence
    if (referenceSource == FromPort) {
        if (refSeqPort->hasMessage() && settings.refSeqUrl.isEmpty()) {
            takeReference(os);
            CHECK_OP_EXT(os, processError(os), NULL);
        }
    } else if (settings.refSeqUrl.isEmpty()) {
        settings.refSeqUrl = getValue<QString>(REF_URL);
    }

    //do
    if (cache.isEmpty() && !settings.refSeqUrl.isEmpty() && !settings.assemblyUrls.isEmpty()) {
        settings.variationsUrl = GUrlUtils::rollFileName(getValue<QString>(OUT_URL), "_", QSet<QString>());
        CallVariantsTask* t = new CallVariantsTask(settings, context->getDataStorage());
        t->addListeners(createLogListeners(3));
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));

        settings.assemblyUrls.clear();
        settings.refSeqUrl.clear();

        return t;
    }
    return NULL;
}

void CallVariantsWorker::sl_taskFinished() {
    CallVariantsTask* t = qobject_cast<CallVariantsTask*>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()){
        return;
    }

    DataTypePtr mtype = output->getBusType();
    const QList<QVariantMap>& res = t->getResults();
    foreach(const QVariantMap &m, res) {
        cache.append(Message(mtype, m));
    }
    t->clearResults();
    monitor()->addOutputFile(t->getResultUrl(), getActorId());
}

void CallVariantsWorker::cleanup() {
}

void CallVariantsWorker::takeAssembly(U2OpStatus &os) {
    Message m = assemblyPort->lookMessage();
    CHECK(!m.isEmpty(), );

    QVariantMap data = m.getData().toMap();
    if (!data.contains(BaseSlots::URL_SLOT().getId())){
        os.setError(tr("Assembly URL slot is empty. Please, specify the URL slot"));
        return;
    }

    QString dataset = data[BaseSlots::DATASET_SLOT().getId()].toString();
    if (currentDatasetName.isEmpty()) {
        currentDatasetName = dataset;
    }
    if (currentDatasetName == dataset) {
        assemblyUrls << data.value(BaseSlots::URL_SLOT().getId()).value<QString>();
        assemblyPort->get();
    }
    if (dataset != currentDatasetName && settings.assemblyUrls.isEmpty()) {
        settings.assemblyUrls = assemblyUrls;
        assemblyUrls.clear();
        currentDatasetName = dataset;
    }
}

void CallVariantsWorker::takeReference(U2OpStatus &os) {
    Message m = getMessageAndSetupScriptValues(refSeqPort);
    CHECK_EXT(!m.isEmpty(), output->transit(), );

    QVariantMap data = m.getData().toMap();
    if (!data.contains(BaseSlots::URL_SLOT().getId())){
        os.setError(CallVariantsWorker::tr("Ref sequence URL slot is empty. Please, specify the URL slot"));
        return;
    }
    settings.refSeqUrl = data.value(BaseSlots::URL_SLOT().getId()).value<QString>();
}

CallVariantsTaskSettings CallVariantsWorker::getSettings() {
    CallVariantsTaskSettings settings;
    settings.illumina13 = getValue<bool>(ILLUMINA13);
    settings.use_orphan = getValue<bool>(USE_ORPHAN);
    settings.disable_baq = getValue<bool>(DISABLE_BAQ);
    settings.capq_thres = getValue<int>(CAPQ_THRES);
    settings.max_depth = getValue<int>(MAX_DEPTH);
    settings.ext_baq = getValue<bool>(EXT_BAQ);
    settings.bed = getValue<QString>(BED).toLatin1();
    settings.reg = getValue<QString>(REG).toLatin1();
    settings.min_mq = getValue<int>(MIN_MQ);
    settings.min_baseq = getValue<int>(MIN_BASEQ);
    settings.extq = getValue<int>(EXTQ);
    settings.tandemq = getValue<int>(TANDEMQ);
    settings.no_indel = getValue<bool>(NO_INDEL);
    settings.max_indel_depth = getValue<int>(MAX_INDEL_DEPTH);
    settings.openq = getValue<int>(OPENQ);
    settings.pl_list = getValue<QString>(PL_LIST).toLatin1();

    //bcf view
    settings.keepalt = getValue<bool>(KEEPALT);
    settings.fix_pl = getValue<bool>(FIX_PL);
    settings.no_geno = getValue<bool>(NO_GENO);
    settings.acgt_only = getValue<bool>(ACGT_ONLY);
    settings.bcf_bed = getValue<QString>(BCF_BED).toLatin1();
    settings.qcall = getValue<bool>(QCALL);
    settings.samples = getValue<QString>(SAMPLES).toLatin1();
    settings.min_smpl_frac = getValue<float>(MIN_SMPL_FRAC);
    settings.call_gt = getValue<bool>(CALL_GT);
    settings.indel_frac = getValue<float>(INDEL_FRAC);
    settings.pref = getValue<float>(PREF);
    settings.ptype = getValue<QString>(PTYPE).toLatin1();
    settings.theta = getValue<float>(THETA);
    settings.ccall = getValue<QString>(CCALL).toLatin1();
    settings.n1 = getValue<int>(N1);
    settings.n_perm = getValue<int>(N_PERM);
    settings.min_perm_p = getValue<float>(MIN_PERM_P);

    //varFilter
    settings.minQual = getValue<int>(MIN_QUAL);
    settings.minDep = getValue<int>(MIN_DEP);
    settings.maxDep = getValue<int>(MAX_DEP);
    settings.minAlt = getValue<int>(MIN_ALT);
    settings.gapSize = getValue<int>(GAP_SIZE);
    settings.window = getValue<int>(WINDOW);
    settings.pvalue1 = getValue<float>(PVALUE1);
    settings.pvalue2 = getValue<QString>(PVALUE2).toDouble();
    settings.pvalue3 = getValue<float>(PVALUE3);
    settings.pvalue4 = getValue<float>(PVALUE4);
    settings.pvalueHwe = getValue<float>(PVALUE_HWE);
    settings.printFiltered = getValue<bool>(PRINT);
    return settings;
}

void CallVariantsWorker::processError(const U2OpStatus& os) {
    settings.assemblyUrls.clear();
    settings.refSeqUrl.clear();

    WorkflowMonitor* wMonitor = monitor();
    if (wMonitor) {
        wMonitor->addError(os.getError(), actor->getId());
    }
}

void CallVariantsWorker::checkState(U2OpStatus& os) {
    if (referenceSource == FromFile) {
        if (!hasAssembly()) {
            setDone();
        }
        return;
    }

    if (hasAssembly() && !hasReferenceInPort()) {
        os.setError(tr("Not enough references"));
        processError(os);
        setDone();
    } else if (!hasAssembly() && hasReferenceInPort()) {
        if (!useDatasets) {
            os.setError(tr("The dataset slot is not binded, only the first reference sequence "
                           "against all assemblies was processed."));
        } else {
            os.setError(tr("Not enough assemblies"));
        }
        processError(os);
        setDone();
    } else if (!hasAssembly() && !hasReferenceInPort()) {
        output->setEnded();
        setDone();
    }
}

bool CallVariantsWorker::hasAssembly() const {
    return !assemblyUrls.isEmpty() ||
            !settings.assemblyUrls.isEmpty() ||
            !assemblyPort->isEnded() ||
            assemblyPort->hasMessage();
}

bool CallVariantsWorker::hasReferenceInPort() const {
    return !settings.refSeqUrl.isEmpty() ||
            !refSeqPort->isEnded() ||
            refSeqPort->hasMessage();
}

/************************************************************************/
/* ScientificDoubleDelegate */
/************************************************************************/
ScientificDoubleWidget::ScientificDoubleWidget(QWidget *parent)
: PropertyWidget(parent)
{
    lineEdit = new QLineEdit(this);
    QDoubleValidator *validator = new QDoubleValidator();
    validator->setNotation(QDoubleValidator::ScientificNotation);
    lineEdit->setValidator(validator);
    addMainWidget(lineEdit);

    connect(lineEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_valueChanged(const QVariant &)));
}

QVariant ScientificDoubleWidget::value() {
    return lineEdit->text();
}

void ScientificDoubleWidget::setValue(const QVariant &value) {
    lineEdit->setText(value.toString());
}

ScientificDoubleDelegate::ScientificDoubleDelegate(QObject *parent)
: PropertyDelegate(parent)
{

}

// PropertyDelegate
QVariant ScientificDoubleDelegate::getDisplayValue(const QVariant &v) const {
    return v;
}

PropertyDelegate * ScientificDoubleDelegate::clone() {
    return new ScientificDoubleDelegate(parent());
}

PropertyWidget * ScientificDoubleDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new ScientificDoubleWidget(parent);
}

// QItemDelegate
QWidget * ScientificDoubleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const {
    return new ScientificDoubleWidget(parent);
}

void ScientificDoubleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    PropertyWidget *widget = static_cast<PropertyWidget*>(editor);
    widget->setValue(value);
}

void ScientificDoubleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    PropertyWidget *widget = static_cast<PropertyWidget*>(editor);
    model->setData(index, widget->value(), ConfigurationEditor::ItemValueRole);
}

} //namespace LocalWorkflow
} // namespace
