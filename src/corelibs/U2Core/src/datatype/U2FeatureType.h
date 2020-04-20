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

#ifndef _U2_FEATURE_TYPE_H_
#define _U2_FEATURE_TYPE_H_

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT U2FeatureTypes {
public:
    enum U2FeatureType {
        Invalid                     = 0,
        AaRich                      = 1,
        Acetylation                 = 2,
        ActiveSite                  = 3,
        Adenylation                 = 4,
        Allele                      = 5,
        AlphaHelix                  = 6,
        AlteredSite                 = 7,
        Amidation                   = 8,
        AssemblyGap                 = 9,
        Attenuator                  = 10,
        BHlhDomain                  = 11,
        Basic                       = 12,
        BetaSheet                   = 13,
        BetaStrandRegion            = 14,
        BiotinBindingSite           = 15,
        Blocked                     = 16,
        C2                          = 17,
        CRegion                     = 18,
        CaatSignal                  = 19,
        Calcium                     = 20,
        CatalyticRegion             = 21,
        Cds                         = 22,
        CellAttachment              = 23,
        Cellular                    = 24,
        Centromere                  = 25,
        CholesterolBindingSite      = 26,
        CleavageSite                = 27,
        CoiledCoil                  = 28,
        CollagenType                = 29,
        Comment                     = 30,
        Conflict                    = 31,
        ConnectingPeptide           = 32,
        Cub                         = 33,
        Cytoplasmic                 = 34,
        DLoop                       = 35,
        DSegment                    = 36,
        Disulfide                   = 37,
        Egf                         = 38,
        Enhancer                    = 39,
        Exon                        = 40,
        Exoplasmic                  = 41,
        Extracellular               = 42,
        Farnesyl                    = 43,
        Fibronectin                 = 44,
        FivePrimeClip               = 45,
        FivePrimeUtr                = 46,
        Formylation                 = 47,
        GammaCarboxyglumaticAcid    = 48,
        Gap                         = 49,
        GcSignal                    = 50,
        Gene                        = 51,
        GeranylGeranyl              = 52,
        Glycosylation               = 53,
        GlycosylationSite           = 54,
        GpiAnchor                   = 55,
        HelicalRegion               = 56,
        HemeBindingSite             = 57,
        HmgBox                      = 58,
        Homeodomain                 = 59,
        Hth                         = 60,
        HydrogenBondedTurn          = 61,
        Hydroxylation               = 62,
        IDna                        = 63,
        Immunoglobulin              = 64,
        Insertion                   = 65,
        Intracellular               = 66,
        Intron                      = 67,
        JRegion                     = 68,
        JSegment                    = 69,
        Kh                          = 70,
        Kinase                      = 71,
        LeucineZipper               = 72,
        LeucineZipperDomain         = 73,
        Loci                        = 74,
        Ltr                         = 75,
        MRna                        = 76,
        MatureChain                 = 77,
        MaturePeptide               = 78,
        Methylation                 = 79,
        Minus10Signal               = 80,
        Minus35Signal               = 81,
        MiscBindingSite             = 82,
        MiscBond                    = 83,
        MiscDifference              = 84,
        MiscDnaRnaBindingRegion     = 85,
        MiscDomain                  = 86,
        MiscFeature                 = 87,
        MiscLipid                   = 88,
        MiscMarker                  = 89,
        MiscMetal                   = 90,
        MiscNpBindingRegion         = 91,
        MiscRecombination           = 92,
        MiscRegion                  = 93,
        MiscResidueModification     = 94,
        MiscRna                     = 95,
        MiscSignal                  = 96,
        MiscSite                    = 97,
        MiscStructure               = 98,
        MobileElement               = 99,
        ModifiedBase                = 100,
        Mutation                    = 101,
        Myristate                   = 102,
        NAcylDiglyceride            = 103,
        NRegion                     = 104,
        NcRna                       = 105,
        NonConsecutiveResidues      = 106,
        Nuclease                    = 107,
        OldSequence                 = 108,
        Operon                      = 109,
        OriT                        = 110,
        Overhang                    = 111,
        Palmitate                   = 112,
        Periplasmic                 = 113,
        Ph                          = 114,
        Phosphorylation             = 115,
        PolyASignal                 = 116,
        PolyASite                   = 117,
        PolyAa                      = 118,
        Precursor                   = 119,
        PrecursorRna                = 120,
        Primer                      = 121,
        PrimerBindingSite           = 122,
        PrimaryTranscript           = 123,
        ProcessedActivePeptide      = 124,
        Promoter                    = 125,
        PromoterEukaryotic          = 126,
        PromoterProkaryotic         = 127,
        Propeptide                  = 128,
        Proprotein                  = 129,
        Protease                    = 130,
        Protein                     = 131,
        ProteinBindingSite          = 132,
        Provirus                    = 133,
        PyridoxalPhBindingSite      = 134,
        PyrrolidoneCarboxylicAcid   = 135,
        RRna                        = 136,
        Rbs                         = 137,
        Region                      = 138,
        Regulatory                  = 139,
        RepeatRegion                = 140,
        RepeatUnit                  = 141,
        RepetitiveRegion            = 142,
        ReplicationOrigin           = 143,
        SRegion                     = 144,
        Satellite                   = 145,
        ScRna                       = 146,
        SeconadaryStructure         = 147,
        Sh2                         = 148,
        Sh3                         = 149,
        SignalPeptide               = 150,
        SignalSequence              = 151,
        Silencer                    = 152,
        Similarity                  = 153,
        Site                        = 154,
        SnRna                       = 155,
        SnoRna                      = 156,
        Source                      = 157,
        SplicingSignal              = 158,
        SplicingVariant             = 159,
        StemLoop                    = 160,
        Sts                         = 161,
        Sulfatation                 = 162,
        TRna                        = 163,
        TataSignal                  = 164,
        Telomere                    = 165,
        Terminator                  = 166,
        Thioether                   = 167,
        Thiolester                  = 168,
        ThreePrimeClip              = 169,
        ThreePrimeUtr               = 170,
        TmRna                       = 171,
        TransitPeptide              = 172,
        TransmembraneRegion         = 173,
        Transposon                  = 174,
        Uncertainty                 = 175,
        Unsure                      = 176,
        VRegion                     = 177,
        VSegment                    = 178,
        Variant                     = 179,
        Variation                   = 180,
        Virion                      = 181,
        ZincFinger                  = 182,
        ZincFingerDomain            = 183,
        RestrictionSite             = 184
    };

    enum Alphabet {
        Alphabet_None    = 0,
        Alphabet_Nucleic = 1 << 0,
        Alphabet_Amino   = 1 << 1
    };
    Q_DECLARE_FLAGS(Alphabets, Alphabet)

    static QList<U2FeatureType> getTypes(const Alphabets &alphabets);
    static QString getVisualName(U2FeatureType type);
    static U2FeatureType getTypeByName(const QString &visualName);

    class U2FeatureTypeInfo {
    public:
        U2FeatureTypeInfo(U2FeatureType featureType = U2FeatureTypes::Invalid, const QString &visualName = "", Alphabets alphabets = Alphabet_None);

        U2FeatureType featureType;
        QString visualName;
        Alphabets alphabets;
    };

private:
    static QList<U2FeatureTypeInfo> initFeatureTypes();
    const static QList<U2FeatureTypeInfo> typeInfos;
    static QHash<U2FeatureType, int> typeInfoIndexByType;
};

typedef U2FeatureTypes::U2FeatureType U2FeatureType;

}   // namespace U2

Q_DECLARE_OPERATORS_FOR_FLAGS(U2::U2FeatureTypes::Alphabets)

#endif // _U2_FEATURE_TYPE_H_
