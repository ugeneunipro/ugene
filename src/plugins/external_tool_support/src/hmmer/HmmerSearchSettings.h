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

#ifndef _U2_HMMER_SEARCH_SETTINGS_H_
#define _U2_HMMER_SEARCH_SETTINGS_H_

#include <U2Core/AnnotationCreationPattern.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>

namespace U2 {

class AnnotationTableObject;

class HmmerSearchSettings {
public:
    enum BitCutoffs {
        None,                   // disabled
        p7H_GA,                 // gathering thresholds available
        p7H_TC,                 // trusted cutoffs available
        p7H_NC                  // noise cutoffs available
    };

    HmmerSearchSettings();

    bool validate() const;

    double e;                   // -E: report sequences <= this e-value threshold in output
    double t;                   // -T: report sequences >= this score threshold in output
    double z;                   // -Z: set # of camparisons done, for e-value calculation
    double domE;                // --domE: report domains <= this e-value threshold in output
    double domT;                // --domT: report domains >= this score cutoff in output
    double domZ;                // --domZ: set number of significant seqs, for domain e-value calibration
    BitCutoffs useBitCutoffs;   // --cut_ga: use profile's GA gathering cutoffs to set -T, --domT
                                // --cut_nc: use profile's NC noising cutoffs to set -T, --domT
                                // --cut_tc: use profile's TC trusted cutoffs to set -T, --domT

    double f1;                  // --F1: Stage 1 (MSV) threshold: promote hits w/ P <= F1
    double f2;                  // --F2: Stage 2 (Vit) threshold: promote hits w/ P <= F2
    double f3;                  // --F3: Stage 3 (Fwd) threshold: promote hits w/ P <= F3

    bool doMax;                 // --max: Turn all heuristic filters off ( less speed more power )
    bool noBiasFilter;          // --nobias: turn off composition bias filter
    bool noNull2;               // --nonull2: turn off biased composition score corrections
    bool noali;                 // --noali: turn off alignment writing

    int seed;                   // --seed : set RNG seed ( if 0: one-time arbitrary seed )

    QString workingDir;
    QString hmmProfileUrl;
    QString sequenceUrl;
    QPointer<U2SequenceObject> sequence;

    QPointer<AnnotationTableObject> annotationTable;
    AnnotationCreationPattern pattern;

    static const double OPTION_NOT_SET;
};

}   // namespace U2

#endif // _U2_HMMER_SEARCH_SETTINGS_H_
