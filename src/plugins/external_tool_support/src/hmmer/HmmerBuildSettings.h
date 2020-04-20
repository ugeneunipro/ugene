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

#ifndef _U2_HMMER_BUILD_SETTINGS_H_
#define _U2_HMMER_BUILD_SETTINGS_H_

#include <QString>

namespace U2 {

class HmmerBuildSettings {
public:
    enum p7_archchoice_e {      // model construction strategies
        p7_ARCH_FAST,           // --fast: assign cols >= symfrac residues as consensus
        p7_ARCH_HAND            // --hand: manual construction ( requires reference annotation )
    };

    enum p7_wgtchoice_e {       // relative sequence weighting strategies
        p7_WGT_NONE,            // --wnone: don't do any relative weighting ( set all to 1 )
        p7_WGT_GIVEN,           // --wgiven: use weights as given in msa file
        p7_WGT_GSC,             // --wgsc: Gerstein/Sonnhammer/Chotia tree weights
        p7_WGT_PB,              // --wpb: Henikoff position-based weigths
        p7_WGT_BLOSUM           // --wblosum: Henikoff simple filter weights
    };

    enum p7_effnchoice_e {      // effective sequence weighting strategies
        p7_EFFN_NONE,           // --enone: no effective seq # weighting: just use nseq
        p7_EFFN_SET,            // --eset: seq eff seq # for all models
        p7_EFFN_CLUST,          // --eclust: eff seq # is # of single linkage clusters
        p7_EFFN_ENTROPY         // --eent: adjust eff seq # to achieve relative entropy target
    };

    HmmerBuildSettings();

    bool validate() const;

    p7_archchoice_e modelConstructionStrategy;
    p7_wgtchoice_e relativeSequenceWeightingStrategy;
    p7_effnchoice_e effectiveSequenceWeightingStrategy;

    double eset;                        // --eset argument

    int seed;                           // --seed argument

    float symfrac;                      // --symfrac. sets sym fraction controlling --fast construction
    float fragtresh;                    // --fragtresh. if L < x<L>, tag sequence as a fragment
    double wid;                         // --wid. for --wblosum: set identity cutoff
    double ere;                         // --ere. for --eent:set target relative entropy
    double esigma;                      // --esigma. for --eent: set sigma param to <x>
    double eid;                         // --eid. for --eclust. set fractional identity cutoff
    int eml;                            // --EmL. length of sequences for MSV Gumbel mu fit
    int emn;                            // --EmN. number of sequences for MSV Gumbel mu fit
    int evl;                            // --EvL. length of sequences for Viterbi Gumbel mu fit
    int evn;                            // --EvN. number of sequences for Viterbi Gumbel mu fit
    int efl;                            // --EfL. length of sequences for forward exp tail mu fit
    int efn;                            // --EfN. number of sequences for forward exp tail mu fit
    double eft;                         // --Eft. tail mass for forward exponential tail mu fit

    QString workingDir;
    QString profileUrl;
};

}   // namespace U2

#endif // _U2_HMMER_BUILD_SETTINGS_H_
