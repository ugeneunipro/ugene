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

#include "AssemblyConsensusAlgorithmSamtools.h"
#include "BuiltInAssemblyConsensusAlgorithms.h"

#include <cstring>
#include <SamtoolsAdapter.h>
#include <stdio.h>
extern "C" {
#include <bam2bcf.h>
}
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Assembly.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

AssemblyConsensusAlgorithmFactorySamtools::AssemblyConsensusAlgorithmFactorySamtools()
    : AssemblyConsensusAlgorithmFactory(BuiltInAssemblyConsensusAlgorithms::SAMTOOLS_ALGO)
{}

QString AssemblyConsensusAlgorithmFactorySamtools::getName() const {
    return tr("SAMtools");
}

QString AssemblyConsensusAlgorithmFactorySamtools::getDescription() const {
    return tr("Uses SAMtools to calculate consensus with regard to quality of reads");
}

AssemblyConsensusAlgorithm* AssemblyConsensusAlgorithmFactorySamtools::createAlgorithm() {
    return new AssemblyConsensusAlgorithmSamtools(this);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

struct AlgorithmInternal {
    AlgorithmInternal(const U2Region &region_, QByteArray referenceFragment_, U2OpStatus &os_)
        : region(region_), os(os_), referenceFragment(referenceFragment_), result(region_.length, AssemblyConsensusAlgorithm::EMPTY_CHAR)
    {
        lplbuf = bam_lplbuf_init(processBaseCallback, this);
        bam_lplbuf_reset(lplbuf);

        bca = bcf_call_init(0.83, 13);
    }

    void processReads(U2DbiIterator<U2AssemblyRead> *reads) {
        ReadsContainer samtoolsReads;
        os.setDescription(AssemblyConsensusAlgorithmFactorySamtools::tr("Fetching reads from database and converting to SAMtools format"));
        SamtoolsAdapter::reads2samtools(reads, os, samtoolsReads);
        CHECK_OP(os,);
        os.setDescription(AssemblyConsensusAlgorithmFactorySamtools::tr("Sorting reads"));
        samtoolsReads.sortByStartPos();

        os.setDescription(AssemblyConsensusAlgorithmFactorySamtools::tr("Calculating consensus"));
        const int readsCount = samtoolsReads.size();
        for(int i = 0; i < readsCount; ++i) {
            bam_lplbuf_push(&samtoolsReads[i], lplbuf);
            os.setProgress(i*100/readsCount);
            CHECK_OP(os,);
        }
        bam_lplbuf_push(0, lplbuf);
    }

    void processBase(uint32_t /*tid*/, uint32_t pos, int n, const bam_pileup1_t *pl) {
        if(pos < region.startPos || pos >= region.endPos() || os.isCoR()) {
            return;
        }
        uint32_t posInArray = pos - region.startPos;
        // From bam_tview.c, tv_pl_func function
        int i, j, rb;
        uint32_t call = 0;
        bcf_callret1_t bcr;

        rb = referenceFragment.isEmpty() ? 'N' : referenceFragment[posInArray];

        int qsum[4], a1, a2, tmp;
        double p[3], prior = 30;
        bcf_call_glfgen(n, pl, bam_nt16_table[rb], bca, &bcr);

        for (i = 0; i < 4; ++i) qsum[i] = bcr.qsum[i]<<2 | i;
        for (i = 1; i < 4; ++i) // insertion sort
            for (j = i; j > 0 && qsum[j] > qsum[j-1]; --j)
                tmp = qsum[j], qsum[j] = qsum[j-1], qsum[j-1] = tmp;
        a1 = qsum[0]&3; a2 = qsum[1]&3;
        p[0] = bcr.p[a1*5+a1]; p[1] = bcr.p[a1*5+a2] + prior; p[2] = bcr.p[a2*5+a2];
        if ("ACGT"[a1] != toupper(rb)) p[0] += prior + 3;
        if ("ACGT"[a2] != toupper(rb)) p[2] += prior + 3;
        if (p[0] < p[1] && p[0] < p[2]) call = (1<<a1)<<16 | (int)((p[1]<p[2]?p[1]:p[2]) - p[0] + .499);
        else if (p[2] < p[1] && p[2] < p[0]) call = (1<<a2)<<16 | (int)((p[0]<p[1]?p[0]:p[1]) - p[2] + .499);
        else call = (1<<a1|1<<a2)<<16 | (int)((p[0]<p[2]?p[0]:p[2]) - p[1] + .499);

        char consensusChar = bam_nt16_rev_table[call>>16&0xf];
        result[posInArray] = consensusChar;
    }

    static int processBaseCallback(uint32_t tid, uint32_t pos, int n, const bam_pileup1_t *pl, void *data) {
        AlgorithmInternal *algorithm = (AlgorithmInternal*)data;
        algorithm->processBase(tid, pos, n, pl);
        return 0;
    }

    QByteArray getResult() { return result; }

    ~AlgorithmInternal() {
        bcf_call_destroy(bca);
        bam_lplbuf_destroy(lplbuf);
    }

private:
    const U2Region & region;
    U2OpStatus & os;
    QByteArray referenceFragment;
    bam_lplbuf_t * lplbuf;
    bcf_callaux_t * bca;
    QByteArray result;
};


QByteArray AssemblyConsensusAlgorithmSamtools::getConsensusRegion(const U2Region &region, U2DbiIterator<U2AssemblyRead> *reads, QByteArray referenceFragment, U2OpStatus &os) {
    AlgorithmInternal algorithm(region, referenceFragment, os);

    algorithm.processReads(reads);

    return algorithm.getResult();
}

} // namespace
