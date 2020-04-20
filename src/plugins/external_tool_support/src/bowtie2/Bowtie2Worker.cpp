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

#include <U2Algorithm/DnaAssemblyTask.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "Bowtie2Support.h"
#include "Bowtie2Worker.h"
#include "Bowtie2Task.h"

#include "perl/PerlSupport.h"
#include "python/PythonSupport.h"

namespace U2 {
namespace LocalWorkflow {

const QString Bowtie2WorkerFactory::ACTOR_ID("align-reads-with-bowtie2");

static const QString MODE = "mode";
static const QString MISMATCHES_NUMBER = "mismatches_number";
static const QString SEED_LEN = "seed_len";
static const QString DPAD = "dpad";
static const QString GBAR = "gbar";
static const QString SEED = "seed";
static const QString THREADS = "threads";
static const QString NOMIXED = "nomixed";
static const QString NODISCORDANT = "nodiscordant";
static const QString NOFW = "nofw";
static const QString NORC = "norc";
static const QString NOOVERLAP = "nooverlap";
static const QString NOCONTAIN = "nocontain";

static const QString BASE_Bowtie2_SUBDIR("bowtie");
static const QString BASE_Bowtie2_OUTFILE("out.sam");


/************************************************************************/
/* Worker */
/************************************************************************/
Bowtie2Worker::Bowtie2Worker(Actor *p)
: BaseShortReadsAlignerWorker(p, Bowtie2Task::taskName)
{
}

QVariantMap Bowtie2Worker::getCustomParameters() const {
    QMap<QString, QVariant> customSettings;

    customSettings.insert(Bowtie2Task::OPTION_MODE, getValue<QString>(MODE));
    customSettings.insert(Bowtie2Task::OPTION_MISMATCHES, getValue<QString>(MISMATCHES_NUMBER));
    customSettings.insert(Bowtie2Task::OPTION_SEED_LEN, getValue<int>(SEED_LEN));
    customSettings.insert(Bowtie2Task::OPTION_DPAD, getValue<int>(DPAD));
    customSettings.insert(Bowtie2Task::OPTION_GBAR, getValue<int>(GBAR));
    customSettings.insert(Bowtie2Task::OPTION_SEED, getValue<int>(SEED));
    customSettings.insert(Bowtie2Task::OPTION_THREADS, getValue<int>(THREADS));

    customSettings.insert(Bowtie2Task::OPTION_NOMIXED, getValue<bool>(NOMIXED));
    customSettings.insert(Bowtie2Task::OPTION_NODISCORDANT, getValue<bool>(NODISCORDANT));
    customSettings.insert(Bowtie2Task::OPTION_NOFW, getValue<bool>(NOFW));
    customSettings.insert(Bowtie2Task::OPTION_NORC, getValue<bool>(NORC));
    customSettings.insert(Bowtie2Task::OPTION_NOOVERLAP, getValue<bool>(NOOVERLAP));
    customSettings.insert(Bowtie2Task::OPTION_NOCONTAIN, getValue<bool>(NOCONTAIN));

    return customSettings;
}

QString Bowtie2Worker::getDefaultFileName() const {
    return BASE_Bowtie2_OUTFILE;
}

QString Bowtie2Worker::getBaseSubdir() const {
    return BASE_Bowtie2_SUBDIR;
}

void Bowtie2Worker::setGenomeIndex(DnaAssemblyToRefTaskSettings& settings){
    settings.refSeqUrl = getValue<QString>(REFERENCE_GENOME);

    QStringList suffixes = QStringList() << Bowtie2Task::indexSuffixes << Bowtie2Task::largeIndexSuffixes;
    if (!DnaAssemblyToReferenceTask::isIndexUrl(settings.refSeqUrl.getURLString(), suffixes)) {
        settings.indexFileName = QDir(settings.refSeqUrl.dirPath()).filePath(settings.refSeqUrl.baseFileName());
    }
}
/************************************************************************/
/* Factory */
/************************************************************************/
void Bowtie2WorkerFactory::init() {
    QList<Attribute*> attrs;
    QMap<QString, PropertyDelegate*> delegates;
    addCommonAttributes(attrs, delegates,
                        Bowtie2Worker::tr("Bowtie index folder"),
                        Bowtie2Worker::tr("Bowtie index basename"));
     {
         Descriptor mode(MODE ,
             Bowtie2Worker::tr("Mode"),
             Bowtie2Worker::tr("When the -n option is specified (which is the default), bowtie determines which alignments \
                              \nare valid according to the following policy, which is similar to Maq's default policy. \
                              \nIn -v mode, alignments may have no more than V mismatches, where V may be a number from 0 \
                              \nthrough 3 set using the -v option. Quality values are ignored. The -v option is mutually exclusive with the -n option."));

        Descriptor mismatchesNumber(MISMATCHES_NUMBER,
            Bowtie2Worker::tr("Number of mismatches"),
            Bowtie2Worker::tr("Sets the number of mismatches to allowed in a seed alignment. Can be set to 0 or 1. \
                              \nSetting this higher makes alignment slower (often much slower) but increases sensitivity."));

        Descriptor seedLen(SEED_LEN,
            Bowtie2Worker::tr("Seed length (--L)"),
            Bowtie2Worker::tr("Sets the length of the seed substrings to align. Smaller values make alignment slower but more senstive."));

        Descriptor dpad(DPAD,
            Bowtie2Worker::tr("Add columns to allow gaps (--dpad)"),
            Bowtie2Worker::tr("\"Pads\" dynamic programming problems by specified number of columns on either side to allow gaps."));

        Descriptor gbar(GBAR,
            Bowtie2Worker::tr("Disallow gaps (--gbar)"),
            Bowtie2Worker::tr("Disallow gaps within specified number of positions of the beginning or end of the read."));

        Descriptor seed(SEED,
            Bowtie2Worker::tr("Seed (--seed)"),
            Bowtie2Worker::tr("Use specified value as the seed for pseudo-random number generator."));

        Descriptor threads(THREADS,
            Bowtie2Worker::tr("Threads"),
            Bowtie2Worker::tr("Launch specified number of parallel search threads. Threads will run on separate processors/cores and synchronize \
                              \nwhen parsing reads and outputting alignments. Searching for alignments is highly parallel, and speedup is close to linear."));

        Descriptor nomixed(NOMIXED,
            Bowtie2Worker::tr("No unpaired alignments (--no-mixed)"),
            Bowtie2Worker::tr("If Bowtie2 cannot find a paired-end alignment for a pair, by default it will go on to look for unpaired alignments \
                              \nfor the constituent mates. This is called \"mixed mode.\" To disable mixed mode, set this option. Bowtie2 runs a little \
                              \nfaster in the mixed mode, but will only consider alignment status of pairs per se, not individual mates."));

        Descriptor nodiscordant(NODISCORDANT,
            Bowtie2Worker::tr("No discordant alignments (--no-discordant)"),
            Bowtie2Worker::tr("By default, Bowtie2 looks for discordant alignments if it cannot find any concordant alignments. A discordant alignment \
                              \nis an alignment where both mates align uniquely, but that does not satisfy the paired-end constraints. This option disables that behavior."));

        Descriptor noForward(NOFW,
             Bowtie2Worker::tr("No forward orientation (--nofw)"),
             Bowtie2Worker::tr("If --nofw is specified, bowtie will not attempt to align against the forward reference strand."));

         Descriptor noReverse(NORC,
             Bowtie2Worker::tr("No reverse-complement orientation (--norc)"),
             Bowtie2Worker::tr("If --norc is specified, bowtie will not attempt to align against the reverse-complement reference strand."));

         Descriptor nooverlap(NOOVERLAP,
             Bowtie2Worker::tr("No overlapping mates (--no-overlap)"),
             Bowtie2Worker::tr("If one mate alignment overlaps the other at all, consider that to be non-concordant. Default: mates can overlap in \
                                \na concordant alignment."));

         Descriptor nocontain(NOCONTAIN,
             Bowtie2Worker::tr("No mates containing one another (--no-contain)"),
             Bowtie2Worker::tr("If one mate alignment contains the other, consider that to be non-concordant. Default: a mate can contain the \
                               \nother in a concordant alignment."));

        attrs << new Attribute(mode, BaseTypes::STRING_TYPE(), false, QVariant("--end-to-end"));
        attrs << new Attribute(mismatchesNumber, BaseTypes::NUM_TYPE(), false, QVariant(0));
        attrs << new Attribute(seedLen, BaseTypes::NUM_TYPE(), false, QVariant(20));

        attrs << new Attribute(dpad, BaseTypes::NUM_TYPE(), false, QVariant(15));
        attrs << new Attribute(gbar, BaseTypes::NUM_TYPE(), false, QVariant(4));
        attrs << new Attribute(seed, BaseTypes::NUM_TYPE(), false, QVariant(0));
        attrs << new Attribute(threads, BaseTypes::NUM_TYPE(), false, QVariant(1));

        attrs << new Attribute(nomixed, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(nodiscordant, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(noForward, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(noReverse, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(nooverlap, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(nocontain, BaseTypes::BOOL_TYPE(), false, QVariant(false));
     }

     {
         QVariantMap spinMap;
         spinMap["minimum"] = QVariant(0);
         spinMap["maximum"] = QVariant(INT_MAX);
         delegates[DPAD]  = new SpinBoxDelegate(spinMap);
         delegates[GBAR] = new SpinBoxDelegate(spinMap);
         delegates[SEED] = new SpinBoxDelegate(spinMap);

         QVariantMap mismatchesLimitation;
         mismatchesLimitation["minimum"] = QVariant(0);
         mismatchesLimitation["maximum"] = QVariant(1);
         delegates[MISMATCHES_NUMBER] = new SpinBoxDelegate(mismatchesLimitation);

         QVariantMap seedLengthLimitation;
         seedLengthLimitation["minimum"] = QVariant(4);
         seedLengthLimitation["maximum"] = QVariant(31);
         delegates[SEED_LEN] = new SpinBoxDelegate(seedLengthLimitation);

         QVariantMap threadsLimitation;
         threadsLimitation["minimum"] = QVariant(1);
         threadsLimitation["maximum"] = QVariant(99);
         delegates[THREADS] = new SpinBoxDelegate(threadsLimitation);

         QVariantMap modeMap;
         modeMap["--end-to-end"] = "--end-to-end";
         modeMap["--local"] = "--local";
         delegates[MODE] = new ComboBoxDelegate(modeMap);
    }

    Descriptor protoDesc(Bowtie2WorkerFactory::ACTOR_ID,
        Bowtie2Worker::tr("Map Reads with Bowtie2"),
        Bowtie2Worker::tr("Bowtie2  is a program for mapping short DNA sequence reads to a long reference sequence."
                          " In addition to to the Burrows-Wheeler transform, Bowtie2 uses an FM-index (similar to"
                          " a suffix array) to keep its memory footprint small."
                          "<br/><br/>Bowtie2 is more suited to finding longer, gapped alignments than the first version"
                          " Bowtie, it supports ambiguous characters in the reference (e.g. 'N'), etc."
                          "<br/><br/>Provide URL(s) to FASTA or FASTQ file(s) with NGS reads to the input"
                          " port of the element, set up the reference sequence in the parameters."
                          " The result is saved to the specified SAM file, URL to the file is passed"
                          " to the output port."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, getPortDescriptors(), attrs);
    proto->setPrompter(new ShortReadsAlignerPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPortValidator(IN_PORT_DESCR, new ShortReadsAlignerSlotsValidator());
    proto->addExternalTool(Bowtie2Support::ET_BOWTIE2_ALIGN_ID);
    proto->addExternalTool(PythonSupport::ET_PYTHON_ID);
    proto->addExternalTool(PerlSupport::ET_PERL_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_MAP_ASSEMBLE_READS(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new Bowtie2WorkerFactory());
}

Worker *Bowtie2WorkerFactory::createWorker(Actor *a) {
    return new Bowtie2Worker(a);
}

} // LocalWorkflow
} // U2

