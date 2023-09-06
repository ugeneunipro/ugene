/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QByteArray>
#include <QFlags>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QString>

#include <U2Core/global.h>

namespace U2 {

class DNAAlphabet;

#define QUALIFIER_LEFT_TERM "left_end_term"
#define QUALIFIER_LEFT_OVERHANG "left_end_seq"
#define QUALIFIER_LEFT_TYPE "left_end_type"
#define QUALIFIER_LEFT_STRAND "left_end_strand"
#define QUALIFIER_RIGHT_TERM "right_end_term"
#define QUALIFIER_RIGHT_OVERHANG "right_end_seq"
#define QUALIFIER_RIGHT_TYPE "right_end_type"
#define QUALIFIER_RIGHT_STRAND "right_end_strand"
#define QUALIFIER_SOURCE "fragment_source"
#define QUALIFIER_INVERTED "fragment_inverted"
#define OVERHANG_TYPE_BLUNT "blunt"
#define OVERHANG_TYPE_STICKY "sticky"
#define OVERHANG_STRAND_DIRECT "direct"
#define OVERHANG_STRAND_COMPL "rev-compl"

#define ANNOTATION_GROUP_FRAGMENTS "fragments"
#define ANNOTATION_GROUP_ENZYME "enzyme"

#define ENZYME_CUT_UNKNOWN 0x7FFFFF
#define ENZYME_LIST_SEPARATOR ","
#define SUPPLIERS_LIST_SEPARATOR "\n"

class U2ALGORITHM_EXPORT EnzymeSettings {
public:
    static const QString DATA_DIR_KEY;
    static const QString DATA_FILE_KEY;
    static const QString LAST_SELECTION;
    static const QString CHECKED_SUPPLIERS;
    static const QString MIN_ENZYME_LENGTH;
    static const QString MAX_ENZYME_LENGTH;
    static const QString OVERHANG_TYPE;
    static const QString SHOW_PALINDROMIC;
    static const QString SHOW_UNINTERRUPTED;
    static const QString SHOW_NONDEGENERATE;
    static const QString ENABLE_HIT_COUNT;
    static const QString MAX_HIT_VALUE;
    static const QString MIN_HIT_VALUE;
    static const QString MAX_RESULTS;
    static const QString COMMON_ENZYMES;
};

class U2ALGORITHM_EXPORT EnzymeData : public QSharedData {
public:
    QString id;
    QString accession;
    QString type;
    QByteArray seq;
    int cutDirect = ENZYME_CUT_UNKNOWN;  // starts from the first char in direct strand
    int cutComplement = ENZYME_CUT_UNKNOWN;  // starts from the first char in complement strand, negative->right offset
    QString organizm;
    QStringList suppliers; // commercial sources of the current enzyme
    const DNAAlphabet* alphabet = nullptr;

    // Cuts (cleavages) on direct and reverse-complementary strands
    // form overhang (located between these two cuts).
    // These overhangs could have following types:
    enum OverhangType {
        // Enzyme has no cuts and, consequently, no overhangs.
        NoOverhang = 1 << 1,
        // Both cuts are located in the middle of the site.
        Blunt = 1 << 2,
        // Both cuts are located anywhere but the middle of the site.
        Sticky = 1 << 3,
        // The same as "Sticky", but overhang between cuts has only A, C, G or T (no extended or N) symbols.
        NondegenerateSticky = 1 << 4,
        // The cut of the forward strand is to the left of the cut of the reverse-complementary strand.
        Cut5 = 1 << 5,
        // The cut of the forward strand is to the right of the cut of the reverse-complementary strand.
        Cut3 = 1 << 6
    };
    Q_DECLARE_FLAGS(OverhangTypes, OverhangType);
    OverhangTypes overhangTypes;
    // Forward and reverse-complementary enzymes are equal
    bool palindromic = false;
    // No insternal N's
    bool uninterrupted = false;
    // A, C, G and T only
    bool nondegenerate = false;

    static constexpr const char UNDEFINED_BASE = 'N';

    /*
     * Calculates full enzyme length (with leading and trailing N).
     * For example, "AbaSI" has only one defined nucleotide, but 11 trailing N's:
     * 5'  C NNNNNNNNN NN 3'
     * 3'  G NNNNNNNNN    5'
     * Which means, that sequence length is 1, but full enzyme length is 1 + 11 = 12.
     * \return Returns full enzyme length - leading N's number + enzyme length + trailing N's number
     */
    int getFullLength() const;

};

typedef QSharedDataPointer<EnzymeData> SEnzymeData;

}  // namespace U2

Q_DECLARE_OPERATORS_FOR_FLAGS(U2::EnzymeData::OverhangTypes)
