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

#ifndef _U2_PHMMER_SEARCH_SETTINGS_H_
#define _U2_PHMMER_SEARCH_SETTINGS_H_

#include <U2Core/AnnotationCreationPattern.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>

namespace U2 {

class PhmmerSearchSettings {
public:
    PhmmerSearchSettings();

    bool validate() const;

    double e;                   // -E: report sequences <= this e-value threshold in output
    double t;                   // -T: report sequences >= this score threshold in output
    double z;                   // -Z: set # of camparisons done, for e-value calculation
    double domE;                // --domE: report domains <= this e-value threshold in output
    double domT;                // --domT: report domains >= this score cutoff in output
    double domZ;                // --domZ: set number of significant seqs, for domain e-value calibration

    double f1;                  // --F1: Stage 1 (MSV) threshold: promote hits w/ P <= F1
    double f2;                  // --F2: Stage 2 (Vit) threshold: promote hits w/ P <= F2
    double f3;                  // --F3: Stage 3 (Fwd) threshold: promote hits w/ P <= F3

    bool doMax;                 // --max: Turn all heuristic filters off ( less speed more power )
    bool noBiasFilter;          // --nobias: turn off composition bias filter
    bool noNull2;               // --nonull2: turn off biased composition score corrections

    int eml;                    // --EmL. length of sequences for MSV Gumbel mu fit
    int emn;                    // --EmN. number of sequences for MSV Gumbel mu fit
    int evl;                    // --EvL. length of sequences for Viterbi Gumbel mu fit
    int evn;                    // --EvN. number of sequences for Viterbi Gumbel mu fit
    int efl;                    // --EfL. length of sequences for forward exp tail mu fit
    int efn;                    // --Efn. number of sequences for forward exp tail mu fit
    double eft;                 // --Eft. tail mass for forward exponential tail mu fit

    double popen;               // --popen: gap open probability
    double pextend;             // --pextend: gap extend probability

    int seed;                   // --seed : set RNG seed ( if 0: one-time arbitrary seed )

    QString workingDir;
    QString querySequenceUrl;
    QString targetSequenceUrl;
    QPointer<U2SequenceObject> targetSequence;

    QPointer<AnnotationTableObject> annotationTable;
    AnnotationCreationPattern pattern;

    static const double OPTION_NOT_SET;
};

}   // namespace U2

#endif // _U2_PHMMER_SEARCH_SETTINGS_H_
