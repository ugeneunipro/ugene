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

#ifndef _U2_VECTOR_NTI_SEQUENCE_FORMAT_H_
#define _U2_VECTOR_NTI_SEQUENCE_FORMAT_H_

#include <U2Core/AnnotationData.h>

#include "GenbankPlainTextFormat.h"

namespace U2 {

class AnnotationGroup;

class U2FORMATS_EXPORT VectorNtiSequenceFormat : public GenbankPlainTextFormat {
    Q_OBJECT
public:
    VectorNtiSequenceFormat(QObject* parent);

    virtual void storeEntry(IOAdapter *io, const QMap<GObjectType, QList<GObject *> > &objectsMap, U2OpStatus &os);

private:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;

    QList<StrPair> processCommentKeys(QMultiMap<QString, QVariant> &tags);
    void createCommentAnnotation(const QStringList &comments, int sequenceLength, AnnotationTableObject *annTable) const;
    U2Qualifier createQualifier(const QString &qualifierName, const QString &qualifierValue, bool containsDoubleQuotes) const;
    U2FeatureType getFeatureType(const QString &typeString) const;
    QString getFeatureTypeString(U2FeatureType featureType, bool isAmino) const;
    StrStrMap parseComments(const QStringList &comments) const;
    static QString parseDate(int date);
    QList<SharedAnnotationData> prepareAnnotations(const QList<GObject *> &tablesList, bool isAmino, U2OpStatus &os) const;
    void writeAnnotations(IOAdapter *io, const QList<GObject *> &aos, bool isAmino, U2OpStatus &os);
    void prepareQualifiersToWrite(QMap<AnnotationGroup *, QList<SharedAnnotationData> > &annotationsByGroups, bool isAmino) const;

    enum VntiDnaFeatureTypes {
        DnaAttenuator          = 0,
        DnaBHlhDomain          = 1,
        DnaCRegion             = 2,
        DnaCaatSignal          = 3,
        DnaCds                 = 4,
        DnaCentromere          = 5,
        DnaDLoop               = 6,
        DnaDSegment            = 7,
        DnaEnhancer            = 9,
        DnaGcSignal            = 10,
        DnaGlycosylationSite   = 11,
        DnaHomeodomain         = 12,
        DnaIDna                = 13,
        DnaInsertion           = 14,
        DnaIntron              = 15,
        DnaJSegment            = 16,
        DnaLeucineZipperDomain = 17,
        DnaLoci                = 18,
        DnaLtr                 = 19,
        DnaMiscBindingSite     = 20,
        DnaMiscFeature         = 21,
        DnaMiscMarker          = 22,
        DnaModifiedBase        = 23,
        DnaNRegion             = 24,
        DnaPolyASignal         = 25,
        DnaPolyASite           = 26,
        DnaPrimer              = 27,
        DnaPrimerBindingSite   = 28,
        DnaPromoterEukaryotic  = 29,
        DnaPromoterProkaryotic = 30,
        DnaProteinBindingSite  = 31,
        DnaRbs                 = 32,
        DnaReplicationOrigin   = 33,
        DnaRepeatRegion        = 34,
        DnaRepeatUnit          = 35,
        DnaSRegion             = 36,
        DnaSilencer            = 37,
        DnaSplicingSignal      = 38,
        DnaStemLoop            = 39,
        DnaSts                 = 40,
        DnaTataSignal          = 41,
        DnaTelomere            = 42,
        DnaTerminator          = 43,
        DnaTransposon          = 44,
        DnaVRegion             = 45,
        DnaZincFingerDomain    = 46,
        DnaMinus10Signal       = 47,
        DnaMinus35Signal       = 48,
        DnaThreePrimeClip      = 49,
        DnaThreePrimeUtr       = 50,
        DnaFivePrimeClip       = 51,
        DnaFivePrimeUtr        = 52,
        DnaRnaMisc             = 53,
        DnaMRna                = 54,
        DnaPrecursorRna        = 55,
        DnaScRna               = 56,
        DnaSnRna               = 57,
        DnaRRna                = 58,
        DnaTRna                = 59,
        DnaGene                = 60,
        DnaExon                = 61,
        DnaMutation            = 62,
        DnaAllele              = 81,
        DnaCellular            = 82,
        DnaConflict            = 83,
        DnaMaturePeptide       = 84,
        DnaMiscDifference      = 85,
        DnaMiscRecombination   = 86,
        DnaMiscSignal          = 87,
        DnaMiscStructure       = 88,
        DnaOldSequence         = 89,
        DnaPrimerTranscript    = 91,
        DnaProvirus            = 92,
        DnaSatellite           = 93,
        DnaSignalPeptide       = 94,
        DnaTransitPeptide      = 95,
        DnaVariation           = 96,
        DnaVirion              = 97,
        DnaSource              = 98,
        DnaUnsure              = 99,
        DnaVSegment            = 100,
        DnaGap                 = 102,
        DnaOperon              = 103,
        DnaOriT                = 104,
        DnaSnoRna              = 105,
        DnaOverhang            = 274
    };

    enum VntiProteinFeatureTypes {
        ProteinSignalSequence              = 200,
        ProteinTransitPeptide              = 201,
        ProteinPropeptide                  = 202,
        ProteinMatureChain                 = 203,
        ProteinProcessedActivePeptide      = 204,
        ProteinRepetitiveRegion            = 205,
        ProteinSimilarity                  = 206,
        ProteinHelicalRegion               = 207,
        ProteinHydrogenBondedTurn          = 208,
        ProteinBetaStrandRegion            = 209,
        ProteinTransmembraneRegion         = 210,
        ProteinCalcium                     = 211,
        ProteinDnaRnaBindingRegionMisc     = 212,
        ProteinNpBindingRegionMisc         = 213,
        ProteinCellAttachment              = 219,
        ProteinActiveSite                  = 220,
        ProteinCleavageSite                = 221,
        ProteinHemeBindingSite             = 223,
        ProteinPyridoxalPhBindingSite      = 224,
        ProteinBiotinBindingSite           = 225,
        ProteinCholesterolBindingSite      = 226,
        ProteinAcetylation                 = 227,
        ProteinAdenylation                 = 228,
        ProteinAmidation                   = 229,
        ProteinBlocked                     = 230,
        ProteinFormylation                 = 231,
        ProteinGammaCarboxyglumaticAcid    = 232,
        ProteinHydroxylation               = 233,
        ProteinMethylation                 = 234,
        ProteinPhosphorylation             = 235,
        ProteinPyrrolidoneCarboxylicAcid   = 236,
        ProteinSulfatation                 = 237,
        ProteinGlycosylation               = 238,
        ProteinResidueModificationMisc     = 239,
        ProteinConflict                    = 240,
        ProteinVariant                     = 241,
        ProteinSplicingVariant             = 242,
        ProteinAlteredSite                 = 243,
        ProteinNonConsecutiveResidues      = 244,
        ProteinUncertainty                 = 245,
        ProteinMyristate                   = 248,
        ProteinPalmitate                   = 249,
        ProteinFarnesyl                    = 250,
        ProteinGeranylGeranyl              = 251,
        ProteinGpiAnchor                   = 252,
        ProteinNAcylDiglyceride            = 253,
        ProteinDisulfide                   = 254,
        ProteinThiolester                  = 255,
        ProteinThioether                   = 256,
        ProteinLipidMisc                   = 262,
        ProteinBondMisc                    = 263,
        ProteinMetalMisc                   = 264,
        ProteinSiteMisc                    = 265,
        ProteinRegionMisc                  = 266,
        ProteinSource                      = 269,
        ProteinProprotein                  = 272,
        ProteinMiscFeature                 = 273,
        ProteinVariation                   = 275,
        ProteinPrecursor                   = 276,
        ProteinSeconadaryStructure         = 277,
        ProteinDomainMisc                  = 1000,
        ProteinExtracellular               = 1001,
        ProteinIntracellular               = 1002,
        ProteinConnectingPeptide           = 1003,
        ProteinCytoplasmic                 = 1004,
        ProteinPeriplasmic                 = 1005,
        ProteinExoplasmic                  = 1006,
        ProteinBindingSiteMisc             = 1007,
        ProteinCatalyticRegion             = 1008,
        ProteinKinase                      = 1011,
        ProteinProtease                    = 1012,
        ProteinNuclease                    = 1013,
        ProteinAaRich                      = 1014,
        ProteinPolyAa                      = 1015,
        ProteinCub                         = 1016,
        ProteinEgf                         = 1017,
        ProteinFibronectin                 = 1018,
        ProteinImmunoglobulin              = 1019,
        ProteinLeucineZipper               = 1020,
        ProteinC2                          = 1021,
        ProteinCoiledCoil                  = 1022,
        ProteinKh                          = 1024,
        ProteinPh                          = 1025,
        ProteinSh2                         = 1026,
        ProteinSh3                         = 1027,
        ProteinZincFinger                  = 1028,
        ProteinCollagenType                = 1029,
        ProteinBasic                       = 1030,
        ProteinHmgBox                      = 1031,
        ProteinHomeodomain                 = 1032,
        ProteinHth                         = 1033,
        ProteinAlphaHelix                  = 1100,
        ProteinBetaSheet                   = 1101
    };

    static const QString vntiCreationDateKey;
    static const QString vntiModificationDateKey;
    static const StrStrMap vntiMetaKeys;                                               // keys in the comment section
    static const QMap<U2FeatureType, VntiDnaFeatureTypes> dnaFeatureTypesMap;           // UGENE feature type - vntifkey qualifier value for DNA sequences
    static const QMap<U2FeatureType, VntiProteinFeatureTypes> proteinFeatureTypesMap;   // UGENE feature type - vntifkey qualifier value for protein sequences
    static const QMap<VntiDnaFeatureTypes, QString> dnaFeatureType2StringMap;           // vntifkey qualifier value - feature type name in vnti file for DNA files
    static const QMap<VntiProteinFeatureTypes, QString> proteinFeatureType2StringMap;   // vntifkey qualifier value - feature type name in vnti file for protein files
    static const QString DEFAULT_FEATURE_TYPE_NAME;
    static const QString QUALIFIER_LABEL;
    static const QString VNTIFKEY_QUALIFIER_NAME;

    static StrStrMap initVntiMetaKeys();
    static QMap<U2FeatureType, VntiDnaFeatureTypes> initDnaFeatureTypesMap();
    static QMap<U2FeatureType, VntiProteinFeatureTypes> initProteinFeatureTypesMap();
    static QMap<VntiDnaFeatureTypes, QString> initDnaFeatureType2StringMap();
    static QMap<VntiProteinFeatureTypes, QString> initProteinFeatureType2StringMap();
};

}   // namespace U2

#endif // _U2_VECTOR_NTI_SEQUENCE_FORMAT_H_
