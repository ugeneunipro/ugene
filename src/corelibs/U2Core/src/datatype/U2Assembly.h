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

#include <QSharedData>

#include <U2Core/U2Range.h>
#include <U2Core/U2Sequence.h>

namespace U2 {

/**
 * CIGAR string operation.
 * CIGAR string represents complex ins/del model for short-reads.
 *
 * See also: U2AssemblyUtils::getCigarExtraLength(), ShortReadIterator.
 */
enum U2CigarOp {
    U2CigarOp_Invalid = 0,

    /**
     * D - 'deletion'.
     * Deletion from the reference. Gaps must be inserted to the read when read is aligned to reference.
     * Deleted regions must be added to the 'real' read length.
     * Consumes query: no. Consumes reference: yes.
     */
    U2CigarOp_D = 1,

    /**
     * I - 'insertion'.
     * Insertion to the reference. Residues marked as 'I' must be skipped when
     * counting 'real' read length and must be skipped when read is aligned to reference.
     * Consumes query: yes. Consumes reference: no.
     */
    U2CigarOp_I = 2,

    /**
     * H - 'hard clipping'.
     * Regions which do not match to the reference, skipped by hardware (not present in read sequence).
     * Hard clipping does not affects read length or visualization.
     * Consumes query: no. Consumes reference: no.
     */
    U2CigarOp_H = 3,

    /**
     * M - 'alignment match'.
     * Either match or mismatch to reference.
     * Consumes query: yes. Consumes reference: yes.
     */
    U2CigarOp_M = 4,

    /**
     * N - 'skip'.
     *  Skipped region from the reference. Skips behave exactly as deletions,
     *  however have different biological meaning: they make sense _only_ in
     *  mRNA-to-genome alignment where represent an intron.
     *  Consumes query: no. Consumes reference: yes.
     */
    U2CigarOp_N = 5,

    /**
     * - P - 'padding'.
     *  Silent Deletion from padded reference.
     *  Someday we should find out how to handle this.
     *  Padding does not affect read length.
     *  Consumes query: no. Consumes reference: no.
     */
    U2CigarOp_P = 6,

    /**
     * S - 'soft clipping'.
     * Regions which do not match to the reference, behave exactly as insertions.
     * Must be located at the start or the end of the read (see SAM spec and CigarValidator).
     * Consumes query: yes. Consumes reference: no.
     */
    U2CigarOp_S = 7,

    /*
     *  = - 'sequence match'.
     * Exact match to reference.
     * Consumes query: yes. Consumes reference: yes.
     *
     */
    U2CigarOp_EQ = 8,

    /*
     * X - 'sequence mismatch'.
     * Mismatch to reference.
     *  Consumes query: yes. Consumes reference: yes.
     */
    U2CigarOp_X = 9
};

/**
    CIGAR token: operation + count
*/
class U2CORE_EXPORT U2CigarToken {
public:
    U2CigarToken()
        : op(U2CigarOp_M), count(1) {
    }
    U2CigarToken(U2CigarOp _op, int _count)
        : op(_op), count(_count) {
    }

    U2CigarOp op;
    int count;
};

/** BAM read flags. */
enum ReadFlag {
    None = 0,
    /** Read is paired in sequencing, no matter whether it is mapped in a pair . */
    Paired = 1 << 0,
    /** Read is mapped in a proper pair. */
    ProperPair = 1 << 1,
    /** Read itself is unmapped; Exclusive with ProperPair. */
    Unmapped = 1 << 2,
    /** Next segment (mate) in the template unmapped. */
    MateUnmapped = 1 << 3,
    /** Read is mapped to the reverse strand: SEQ being reverse complemented.*/
    Reverse = 1 << 4,
    /** Mate is mapped to the reverse strand : SEQ of the next segment in the template being reversed. */
    MateReverse = 1 << 5,
    /** The first segment in the template (read1). */
    FirstInTemplate = 1 << 6,
    /** The last segment in the template (read2, mate). */
    LastInTemplate = 1 << 7,
    /** Not a primary, but a secondary alignment. */
    Secondary = 1 << 8,
    /** Not passing quality controls. */
    QcFail = 1 << 9,
    /** Optical or PCR duplicate. */
    Duplicate = 1 << 10,
    /** Supplementary alignment */
    Supplementary = 1 << 11,
};

Q_DECLARE_FLAGS(ReadFlags, ReadFlag)

/**
    Utility class to work with flags
 */
class ReadFlagsUtils {
public:
    static bool isComplementaryRead(qint64 flags) {
        return flags & Reverse;
    }

    static bool isPairedRead(qint64 flags) {
        return flags & Paired;
    }

    static bool isUnmappedRead(qint64 flags) {
        return flags & Unmapped;
    }
};

/**
 * Auxiliary data from BAM/SAM.
 */
class U2CORE_EXPORT U2AuxData {
public:
    U2AuxData()
        : type(0), subType(0) {
    }
    /** Two bytes for tag */
    char tag[2];
    /** One byte for type: AcCsSiIfZHB */
    char type;
    /** Value size depends on the type */
    QByteArray value;
    /** Type of array data. Only for array auxes */
    char subType;
};

/**
    Row of assembly: sequence, leftmost position and CIGAR
*/
class U2CORE_EXPORT U2AssemblyReadData : public U2Entity, public QSharedData {
public:
    U2AssemblyReadData()
        : leftmostPos(0), effectiveLen(0),
          packedViewRow(0), mappingQuality(255), flags(0), rnext("*"), pnext(0) {
    }

    /** Name of the read, ASCII string */
    QByteArray name;

    /**
        Zero-based left-most position of the read
    */
    qint64 leftmostPos;

    /** Length of the read with CIGAR affect applied */
    qint64 effectiveLen;

    /**
        Position of the read in packed view
    */
    qint64 packedViewRow;

    /**
        CIGAR info for the read
    */
    QList<U2CigarToken> cigar;

    /**
        Sequence of the read.
        The array is not empty only if sequence is embedded into the read
    */
    QByteArray readSequence;

    /** Quality string */
    QByteArray quality;

    /** Mapping quality */
    quint8 mappingQuality;

    /** Read flags */
    qint64 flags;

    /** Reference sequence name of the next mate read */
    QByteArray rnext;

    /** Left-most position of the next mate read */
    qint64 pnext;

    /** The list of auxiliary data of BAM/SAM formats */
    QList<U2AuxData> aux;
};

typedef QSharedDataPointer<U2AssemblyReadData> U2AssemblyRead;

/**
    Assembly representation
*/
class U2CORE_EXPORT U2Assembly : public U2Object {
public:
    U2Assembly() {
    }
    U2Assembly(U2DataId id, QString dbId, qint64 version)
        : U2Object(id, dbId, version) {
    }

    U2DataId referenceId;

    U2DataType getType() const {
        return U2Type::Assembly;
    }
};

/** Statistics information collected during the reads packing algorithm */
class U2AssemblyPackStat {
public:
    U2AssemblyPackStat()
        : maxProw(0), readsCount(0) {
    }
    /** Maximum packed row value after pack algorithm */
    int maxProw;
    /** Number of reads packed */
    qint64 readsCount;
};

/** Statistics information collected during the reads packing algorithm */
typedef QVector<qint32> U2AssemblyCoverageStat;

}  // namespace U2
