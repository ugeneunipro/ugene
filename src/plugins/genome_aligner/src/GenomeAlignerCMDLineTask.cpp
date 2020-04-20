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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>

#include "GenomeAlignerTask.h"
#include "GenomeAlignerCMDLineTask.h"

namespace U2 {

#define OPTION_INDEX_PATH   "index"
#define OPTION_SHORTREADS   "short-reads"
#define OPTION_RESULT       "result"
#define OPTION_BUILD_INDEX  "build-index"
#define OPTION_REFERENCE    "reference"
#define OPTION_MEMSIZE      "memsize"
#define OPTION_USE_OPENCL   "use-opencl"
#define OPTION_REF_FRAG     "ref-size"
#define OPTION_N_MISMATHES  "n-mis"
#define OPTION_PT_MISMATHES "pt-mis"
#define OPTION_REVERSE      "rev-comp"
#define OPTION_BEST_MODE    "best"
#define OPTION_OMIT         "omit-size"
#define OPTION_SAM          "sam"


 GenomeAlignerCMDLineTask::GenomeAlignerCMDLineTask()
:Task( tr( "Run genome aligner from command line" ), TaskFlags_NR_FOSCOE), onlyBuildIndex(false)
{
    mismatchCount = 0;
    ptMismatchCount = 0;
    memSize = 1000;
    refSize = 10;
    qualityThreshold = 0;
    useOpenCL = false;
    alignRevCompl = false;
    bestMode = false;
    onlyBuildIndex = false;
    samOutput = false;

    // parse options

    QList<StrStrPair> options = AppContext::getCMDLineRegistry()->getParameters();

    foreach (const StrStrPair& opt, options ) {
        if (opt.first == OPTION_INDEX_PATH  ) {
            indexPath = opt.second;
        }else if (opt.first == OPTION_BUILD_INDEX ) {
            onlyBuildIndex = true;
        } else if (opt.first == OPTION_REFERENCE) {
            refPath = opt.second;
        } else if (opt.first == OPTION_RESULT) {
            resultPath = opt.second;
        } else if (opt.first == OPTION_SHORTREADS) {
            QStringList urls = opt.second.split(";");
            foreach(const QString& url, urls) {
                shortReadUrls.append(url);
            }
        } else if (opt.first == OPTION_USE_OPENCL) {
            useOpenCL = true;
        } else if (opt.first == OPTION_REF_FRAG) {
            refSize = opt.second.toInt();
            assert(refSize != 0);
            if (refSize == 0) {
                refSize = 10;
            }
        } else if (opt.first == OPTION_N_MISMATHES) {
            mismatchCount = opt.second.toInt();
            ptMismatchCount = 0;
            if (mismatchCount<0) {
                mismatchCount = 0;
            }
        } else if (opt.first == OPTION_PT_MISMATHES) {
            ptMismatchCount = opt.second.toInt();
            mismatchCount = 0;
            if (ptMismatchCount<0) {
                ptMismatchCount = 0;
            }
        } else if (opt.first == OPTION_REVERSE) {
            alignRevCompl = true;
        } else if (opt.first == OPTION_MEMSIZE) {
            memSize = opt.second.toInt();
        } else if (opt.first == OPTION_BEST_MODE) {
            bestMode = true;
        } else if (opt.first == OPTION_OMIT) {
            qualityThreshold = opt.second.toInt();
            if (qualityThreshold<0) {
                qualityThreshold = 0;
            }
        } else if (opt.first == OPTION_SAM) {
            samOutput = true;
        }
    }

    coreLog.info( tr( "Finished parsing genome aligner options." ) );

}

GenomeAlignerCMDLineTask::~GenomeAlignerCMDLineTask()
{
    // clean up resources
}

void GenomeAlignerCMDLineTask::prepare()
{
    if (onlyBuildIndex && refPath.isEmpty()) {
        setError(tr("Path to reference sequence is not set."));
        return;
    }

    if (!onlyBuildIndex) {
        if ( shortReadUrls.isEmpty() ) {
            setError(tr("Short reads list is empty."));
            return;
        } else {
            if (refPath.isEmpty() && indexPath.isEmpty()) {
                setError(tr("Reference (index or sequence) is not set."));
                return;
            }
        }
    }

    if (resultPath.isEmpty()) {
        resultPath =  QDir::current().path() + "/output.sam";
    }

    settings.resultFileName = resultPath;
    foreach (const GUrl& url, shortReadUrls ) {
        settings.shortReadSets.append(url);
    }
    settings.refSeqUrl = refPath;
    settings.indexFileName = indexPath;
    settings.openView = false;
    settings.samOutput = samOutput;
    if (onlyBuildIndex) {
        settings.prebuiltIndex = false;
    } else {
        settings.prebuiltIndex = !indexPath.isEmpty();
    }

    settings.setCustomValue(GenomeAlignerTask::OPTION_READS_MEMORY_SIZE, memSize);
    settings.setCustomValue(GenomeAlignerTask::OPTION_ALIGN_REVERSED, alignRevCompl);
    settings.setCustomValue(GenomeAlignerTask::OPTION_OPENCL, useOpenCL);
    settings.setCustomValue(GenomeAlignerTask::OPTION_SEQ_PART_SIZE, refSize);
    settings.setCustomValue(GenomeAlignerTask::OPTION_IF_ABS_MISMATCHES, ptMismatchCount == 0);
    settings.setCustomValue(GenomeAlignerTask::OPTION_MISMATCHES, mismatchCount);
    settings.setCustomValue(GenomeAlignerTask::OPTION_PERCENTAGE_MISMATCHES, ptMismatchCount);
    settings.setCustomValue(GenomeAlignerTask::OPTION_BEST, bestMode);
    settings.setCustomValue(GenomeAlignerTask::OPTION_QUAL_THRESHOLD, qualityThreshold);


    GenomeAlignerTask* task = new GenomeAlignerTask(settings, onlyBuildIndex);
    addSubTask(task);

}

QString GenomeAlignerCMDLineTask::getArgumentsDescritption()
{
    QString desc;
    int fieldSize = -11;
    desc += tr("  --%1    Use this flag to only build index for reference sequence.\n\n").arg(OPTION_BUILD_INDEX, fieldSize);
    desc += tr("  --%1    Path to reference genome sequence\n\n").arg(OPTION_REFERENCE, fieldSize);
    desc += tr("  --%1    Path to short-reads data in FASTA or FASTQ format\n\n").arg(OPTION_SHORTREADS, fieldSize);
    desc += tr("  --%1    Path to prebuilt index (base file name or with .idx extension). If not set, index is searched in system temporary folder. If --build-index option is applied, index will be saved to specified path.\n\n").arg(OPTION_INDEX_PATH, fieldSize);
    desc += tr("  --%1    Path to output alignment in UGENEDB or SAM format (see --%2)\n\n").arg(OPTION_RESULT, fieldSize).arg(OPTION_SAM);
    desc += tr("  --%1    Memory size (in Mbs) reserved for short-reads. The bigger value the faster algorithm works. Default value depends on available system memory.\n\n").arg(OPTION_MEMSIZE, fieldSize);
    desc += tr("  --%1    Index fragmentation size (in Mbs). Small fragments better fit into RAM, allowing to load more short reads. Default value is 10.\n\n").arg(OPTION_REF_FRAG, fieldSize);
    desc += tr("  --%1    Absolute amount of allowed mismatches per every short-read (mutually exclusive with --%2). Default value is 0.\n\n").arg(OPTION_N_MISMATHES, fieldSize).arg(OPTION_PT_MISMATHES);
    desc += tr("  --%1    Percentage amount of allowed mismatches per every short-read (mutually exclusive with --%2). Default value is 0.\n\n").arg(OPTION_PT_MISMATHES, fieldSize).arg(OPTION_N_MISMATHES);
    desc += tr("  --%1    Use both the read and its reverse complement during the aligning.\n\n").arg(OPTION_REVERSE, fieldSize);
    desc += tr("  --%1    Report only about best alignments (in terms of mismatches).\n\n").arg(OPTION_BEST_MODE, fieldSize);
    desc += tr("  --%1    Omit reads with qualities lower than the specified value. Reads which have no qualities are not omitted. Default value is 0.\n\n").arg(OPTION_OMIT, fieldSize);
    desc += tr("  --%1    Output aligned reads in SAM format. Default value is false.\n\n").arg(OPTION_SAM, fieldSize);

    return desc;
}


}//namespace

