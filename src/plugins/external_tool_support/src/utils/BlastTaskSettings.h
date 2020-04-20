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

#ifndef _U2_BLAST_TASK_SETTINGS_H
#define _U2_BLAST_TASK_SETTINGS_H

#include <U2Core/DNASequenceObject.h>

namespace U2 {

class AnnotationTableObject;

/*Options for BlastAll
blastall 2.2.21 not included arguments:

  -I  Show GI's in deflines [T/F]
    default = F
  -v  Number of database sequences to show one-line descriptions for (V) [Integer]
    default = 500
  -b  Number of database sequence to show alignments for (B) [Integer]
    default = 250
  -Q  Query Genetic code to use [Integer]
    default = 1
  -D  DB Genetic code (for tblast[nx] only) [Integer]
    default = 1
  -O  SeqAlign file [File Out]  Optional
  -J  Believe the query defline [T/F]
    default = F
  -z  Effective length of the database (use zero for the real size) [Real]
    default = 0
  -P  0 for multiple hit, 1 for single hit (does not apply to blastn) [Integer]
    default = 0
  -Y  Effective length of the search space (use zero for the real size) [Real]
    default = 0
  -S  Query strands to search against database (for blast[nx], and tblastx)
       3 is both, 1 is top, 2 is bottom [Integer]
    default = 3
  -T  Produce HTML output [T/F]
    default = F
  -l  Restrict search of database to list of GI's [String]  Optional
  -U  Use lower case filtering of FASTA sequence [T/F]  Optional
  -R  PSI-TBLASTN checkpoint file [File In]  Optional
  -L  Location on query sequence [String]  Optional
  -w  Frame shift penalty (OOF algorithm for blastx) [Integer]
    default = 0
  -t  Length of the largest intron allowed in a translated nucleotide sequence when linking multiple distinct alignments. (0 invokes default behavior; a negative value disables linking.) [Integer]
    default = 0
  -B  Number of concatenated queries, for blastn and tblastn [Integer]  Optional
    default = 0
  -V  Force use of the legacy BLAST engine [T/F]  Optional
    default = F
  -C  Use composition-based score adjustments for blastp or tblastn:
      As first character:
      D or d: default (equivalent to T)
      0 or F or f: no composition-based statistics
      2 or T or t: Composition-based score adjustments as in Bioinformatics 21:902-911,
      1: Composition-based statistics as in NAR 29:2994-3005, 2001
          2005, conditioned on sequence properties
      3: Composition-based score adjustment as in Bioinformatics 21:902-911,
          2005, unconditionally
      For programs other than tblastn, must either be absent or be D, F or 0.
           As second character, if first character is equivalent to 1, 2, or 3:
      U or u: unified p-value combining alignment p-value and compositional p-value in round 1 only
 [String]
    default = D
  -s  Compute locally optimal Smith-Waterman alignments (This option is only
      available for gapped tblastn.) [T/F]
    default = F
*/

class BlastTaskSettings {
public:
    enum StrandSource {
        HitFrame,
        QueryFrame
    };

    BlastTaskSettings();
    void reset();

    QString         programName;            //-p Program Name [String]
    QString         databaseNameAndPath;    //-d database [String]
    QString         filter;                 //-F Filter query sequence (DUST with blastn, SEG with others) [String] : default = T
    float           expectValue;            //-e Expectation value [Real] : default = 10.0
    bool            megablast;              //-n MegaBlast search [T/F]

    int             wordSize;               //-W Word size, default if zero (blastn 11, megablast 28, all others 3) [Integer] up to 99
    int             gapOpenCost;            //-G Cost to open a gap (-1 invokes default behavior) [Integer]
    int             gapExtendCost;          //-E Cost to extend a gap (-1 invokes default behavior) [Integer]
    bool            isDefaultCosts;

    bool            isGappedAlignment;      //-g Perform gapped alignment (not available with tblastx)
    //only for nucleotide sequence
    int             mismatchPenalty;        //-q Penalty for a nucleotide mismatch (blastn only) [Integer]
    int             matchReward;            //-r Reward for a nucleotide match (blastn only) [Integer]
    bool            isDefautScores;
    //only for protein sequence
    QString         matrix;                 //-M Matrix [String] : default = BLOSUM62
    bool            isDefaultMatrix;

    //X dropoff
    int             xDropoffGA;             //-X  X dropoff value for gapped alignment (in bits) blastn 30, megablast 20, tblastx 0, all others 15 [Integer]
    float           xDropoffUnGA;           //-y  X dropoff value for ungapped extensions in bits blastn 20, megablast 10, all others 7 [Real]
    int             xDropoffFGA;            //-Z  X dropoff value for final gapped alignment in bits blastn/megablast 100, tblastx 0, all others 25 [Integer]

    int             windowSize;             //-A  Multiple Hits window size, default if zero (blastn/megablast 0, all others 40 [Integer]
    bool            isDefaultThreshold;
    float           threshold;              //-f  Threshold for extending hits, default if zero blastp 11, blastn 0, blastx 12, tblastn 13 tblastx 13, megablast 0 [Real]

    QString         queryFile;              //-i Query File [File In]
    QByteArray      querySequence;
    bool            isSequenceCircular;     // if the sequence is circular, UGENE will expand the sequence
                                            // and results on circular junction will be found
    QString         outputResFile;          //Output file with annotations
    QString         outputOriginalFile;     //-o BLAST report Output File [File Out]  Optional
    int             outputType;             //-m Type of BLAST report Output File [File Out]  Optional

    int             numberOfProcessors;     //-a Number of processors to use [Integer]
    int             numberOfHits;           //-K Number of best hits from a region to keep. Off by default.  If used a value of 100 is recommended.

    QString         compStats;              //-comp_based_stats <String> (-C in blastall)

    bool            isNucleotideSeq;
    const DNAAlphabet*    alphabet;
    //settings for annotations
    bool                    needCreateAnnotations;
    StrandSource            strandSource;
    AnnotationTableObject *   aobj;
    QString                 groupName;
    QString                 annDescription;
    int                     offsInGlobalSeq;
    TriState                directStrand;

    // service settings
    U2SequenceObject *querySequenceObject; // can be NULL
};
}
#endif // _U2_BLAST_TASK_SETTINGS_H
