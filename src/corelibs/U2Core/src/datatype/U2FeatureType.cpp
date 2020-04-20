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

#include <U2Core/U2SafePoints.h>
#include "U2FeatureType.h"

namespace U2 {

QHash<U2FeatureType, int> U2FeatureTypes::typeInfoIndexByType = QHash<U2FeatureType, int>();
const QList<U2FeatureTypes::U2FeatureTypeInfo> U2FeatureTypes::typeInfos = U2FeatureTypes::initFeatureTypes();

QList<U2FeatureTypes::U2FeatureType> U2FeatureTypes::getTypes(const Alphabets &alphabets) {
    QList<U2FeatureTypes::U2FeatureType> types;
    foreach (const U2FeatureTypeInfo &info, typeInfos) {
        if (info.alphabets & alphabets) {
            types << info.featureType;
        }
    }
    return types;
}

QString U2FeatureTypes::getVisualName(U2FeatureType type) {
    const int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", QString());
    return typeInfos[typeInfoIndex].visualName;
}

U2FeatureType U2FeatureTypes::getTypeByName(const QString &visualName) {
    foreach (const U2FeatureTypeInfo &info, typeInfos) {
        if (info.visualName == visualName) {
            return info.featureType;
        }
    }

    return Invalid;
}

QList<U2FeatureTypes::U2FeatureTypeInfo> U2FeatureTypes::initFeatureTypes() {
    QList<U2FeatureTypeInfo> featureTypeInfos;

    int infoCounter = 0;
    featureTypeInfos << U2FeatureTypeInfo(AaRich, "AA-Rich", Alphabet_Amino);
    typeInfoIndexByType[AaRich] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Acetylation, "Acetylation", Alphabet_Amino);
    typeInfoIndexByType[Acetylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Acetylation, "Active Site", Alphabet_Amino);
    typeInfoIndexByType[Acetylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Acetylation, "Adenylation", Alphabet_Amino);
    typeInfoIndexByType[Acetylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Allele, "Allele", Alphabet_Nucleic);
    typeInfoIndexByType[Allele] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(AlphaHelix, "Alpha-Helix", Alphabet_Amino);
    typeInfoIndexByType[AlphaHelix] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(AlteredSite, "Altered Site", Alphabet_Amino);
    typeInfoIndexByType[AlteredSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Amidation, "Amidation", Alphabet_Amino);
    typeInfoIndexByType[Amidation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(AssemblyGap, "Assembly gap", Alphabet_Nucleic);
    typeInfoIndexByType[AssemblyGap] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Attenuator, "Attenuator", Alphabet_Nucleic);
    typeInfoIndexByType[Attenuator] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(BHlhDomain, "bHLH Domain", Alphabet_Nucleic);
    typeInfoIndexByType[BHlhDomain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Basic, "Basic", Alphabet_Amino);
    typeInfoIndexByType[Basic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(BetaSheet, "Beta-Sheet", Alphabet_Amino);
    typeInfoIndexByType[BetaSheet] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(BetaStrandRegion, "Beta-strand region", Alphabet_Amino);
    typeInfoIndexByType[BetaStrandRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(BiotinBindingSite, "Biotin Binding Site", Alphabet_Amino);
    typeInfoIndexByType[BiotinBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Blocked, "Blocked", Alphabet_Amino);
    typeInfoIndexByType[Blocked] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(C2, "C2", Alphabet_Amino);
    typeInfoIndexByType[C2] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CRegion, "C-Region", Alphabet_Nucleic);
    typeInfoIndexByType[CRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CaatSignal, "CAAT Signal", Alphabet_Nucleic);
    typeInfoIndexByType[CaatSignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Calcium, "Calcium", Alphabet_Amino);
    typeInfoIndexByType[Calcium] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CatalyticRegion, "Catalytic Region", Alphabet_Amino);
    typeInfoIndexByType[CatalyticRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Cds, "CDS", Alphabet_Nucleic);
    typeInfoIndexByType[Cds] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CellAttachment, "Cell Attachment", Alphabet_Amino);
    typeInfoIndexByType[CellAttachment] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Cellular, "Cellular", Alphabet_Nucleic);
    typeInfoIndexByType[Cellular] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Centromere, "Centromere", Alphabet_Nucleic);
    typeInfoIndexByType[Centromere] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CholesterolBindingSite, "Cholesterol Binding Site", Alphabet_Amino);
    typeInfoIndexByType[CholesterolBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CleavageSite, "Cleavage Site", Alphabet_Amino);
    typeInfoIndexByType[CleavageSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CoiledCoil, "Coiled coil", Alphabet_Amino);
    typeInfoIndexByType[CoiledCoil] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(CollagenType, "Collagen-type", Alphabet_Amino);
    typeInfoIndexByType[CollagenType] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Comment, "Comment", Alphabet_None);
    typeInfoIndexByType[Comment] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Conflict, "Conflict", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[Conflict] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ConnectingPeptide, "Connecting Peptide", Alphabet_Amino);
    typeInfoIndexByType[ConnectingPeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Cub, "CUB", Alphabet_Amino);
    typeInfoIndexByType[Cub] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Cytoplasmic, "Cytoplasmic", Alphabet_Amino);
    typeInfoIndexByType[Cytoplasmic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(DLoop, "D-Loop", Alphabet_Nucleic);
    typeInfoIndexByType[DLoop] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(DSegment, "D-Segment", Alphabet_Nucleic);
    typeInfoIndexByType[DSegment] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Disulfide, "Disulfide", Alphabet_Amino);
    typeInfoIndexByType[Disulfide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Egf, "EGF", Alphabet_Amino);
    typeInfoIndexByType[Egf] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Enhancer, "Enhancer", Alphabet_Nucleic);
    typeInfoIndexByType[Enhancer] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Exon, "Exon", Alphabet_Nucleic);
    typeInfoIndexByType[Exon] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Exoplasmic, "Exoplasmic", Alphabet_Amino);
    typeInfoIndexByType[Exoplasmic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Extracellular, "Extracellular", Alphabet_Amino);
    typeInfoIndexByType[Extracellular] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Farnesyl, "Farnesyl", Alphabet_Amino);
    typeInfoIndexByType[Farnesyl] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Fibronectin, "Fibronectin", Alphabet_Amino);
    typeInfoIndexByType[Fibronectin] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(FivePrimeClip, "5' Clip", Alphabet_Nucleic);
    typeInfoIndexByType[FivePrimeClip] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(FivePrimeUtr, "5' UTR", Alphabet_Nucleic);
    typeInfoIndexByType[FivePrimeUtr] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Formylation, "Formylation", Alphabet_Amino);
    typeInfoIndexByType[Formylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(GammaCarboxyglumaticAcid, "Gamma-Carboxyglumatic Acid", Alphabet_Amino);
    typeInfoIndexByType[GammaCarboxyglumaticAcid] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Gap, "Gap", Alphabet_Nucleic);
    typeInfoIndexByType[Gap] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(GcSignal, "GC-Signal", Alphabet_Nucleic);
    typeInfoIndexByType[GcSignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Gene, "Gene", Alphabet_Nucleic);
    typeInfoIndexByType[Gene] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(GeranylGeranyl, "Geranyl-Geranyl", Alphabet_Amino);
    typeInfoIndexByType[GeranylGeranyl] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Glycosylation, "Glycosylation", Alphabet_Amino);
    typeInfoIndexByType[Glycosylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(GlycosylationSite, "Glycosylation Site", Alphabet_Nucleic);
    typeInfoIndexByType[GlycosylationSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(GpiAnchor, "GPI-Anchor", Alphabet_Amino);
    typeInfoIndexByType[GpiAnchor] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(HelicalRegion, "Helical region", Alphabet_Amino);
    typeInfoIndexByType[HelicalRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(HemeBindingSite, "Heme Binding Site", Alphabet_Amino);
    typeInfoIndexByType[HemeBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(HmgBox, "HMG-Box", Alphabet_Amino);
    typeInfoIndexByType[HmgBox] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Homeodomain, "Homeodomain", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[Homeodomain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Hth, "H-T-H", Alphabet_Amino);
    typeInfoIndexByType[Hth] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(HydrogenBondedTurn, "Hydrogen bonded turn", Alphabet_Amino);
    typeInfoIndexByType[HydrogenBondedTurn] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Hydroxylation, "Hydroxylation", Alphabet_Amino);
    typeInfoIndexByType[Hydroxylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(IDna, "iDNA", Alphabet_Nucleic);
    typeInfoIndexByType[IDna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Immunoglobulin, "Immunoglobulin", Alphabet_Amino);
    typeInfoIndexByType[Immunoglobulin] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Insertion, "Insertion", Alphabet_Nucleic);
    typeInfoIndexByType[Insertion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Intracellular, "Intracellular", Alphabet_Amino);
    typeInfoIndexByType[Intracellular] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Intron, "Intron", Alphabet_Nucleic);
    typeInfoIndexByType[Intron] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(JRegion, "J-Region", Alphabet_Nucleic);
    typeInfoIndexByType[JRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(JSegment, "J-Segment", Alphabet_Nucleic);
    typeInfoIndexByType[JSegment] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Kh, "KH", Alphabet_Amino);
    typeInfoIndexByType[Kh] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Kinase, "Kinase", Alphabet_Amino);
    typeInfoIndexByType[Kinase] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(LeucineZipper, "Leucine Zipper", Alphabet_Amino);
    typeInfoIndexByType[LeucineZipper] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(LeucineZipperDomain, "Leucine Zipper Domain", Alphabet_Nucleic);
    typeInfoIndexByType[LeucineZipperDomain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Loci, "Loci", Alphabet_Nucleic);
    typeInfoIndexByType[Loci] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Ltr, "LTR", Alphabet_Nucleic);
    typeInfoIndexByType[Ltr] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MRna, "mRNA", Alphabet_Nucleic);
    typeInfoIndexByType[MRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MatureChain, "Mature chain", Alphabet_Amino);
    typeInfoIndexByType[MatureChain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MaturePeptide, "Mature Peptide", Alphabet_Nucleic);
    typeInfoIndexByType[MaturePeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Methylation, "Methylation", Alphabet_Amino);
    typeInfoIndexByType[Methylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Minus10Signal, "-10 Signal", Alphabet_Nucleic);
    typeInfoIndexByType[Minus10Signal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Minus35Signal, "-35 Signal", Alphabet_Nucleic);
    typeInfoIndexByType[Minus35Signal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscBindingSite, "Misc. Binding Site", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[MiscBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscBond, "Bond: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscBond] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscDifference, "Misc. Difference", Alphabet_Nucleic);
    typeInfoIndexByType[MiscDifference] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscDnaRnaBindingRegion, "DNA/RNA binding region: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscDnaRnaBindingRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscDomain, "Domain: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscDomain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscFeature, "Misc. Feature", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[MiscFeature] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscLipid, "Lipid: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscLipid] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscMarker, "Misc. Marker", Alphabet_Nucleic);
    typeInfoIndexByType[MiscMarker] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscMetal, "Metal: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscMetal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscNpBindingRegion, "NP binding region: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscNpBindingRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscRecombination, "Misc. Recombination", Alphabet_Nucleic);
    typeInfoIndexByType[MiscRecombination] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscRegion, "Region: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscResidueModification, "Residue Modification: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscResidueModification] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscRna, "Misc. RNA", Alphabet_Nucleic);
    typeInfoIndexByType[MiscRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscSignal, "Misc. Signal", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[MiscSignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscSite, "Site: Misc", Alphabet_Amino);
    typeInfoIndexByType[MiscSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MiscStructure, "Misc. Structure", Alphabet_Nucleic);
    typeInfoIndexByType[MiscStructure] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(MobileElement, "Mobile element", Alphabet_Nucleic);
    typeInfoIndexByType[MobileElement] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ModifiedBase, "Modified Base", Alphabet_Nucleic);
    typeInfoIndexByType[ModifiedBase] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Mutation, "Mutation", Alphabet_Nucleic);
    typeInfoIndexByType[Mutation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Myristate, "Myristate", Alphabet_Amino);
    typeInfoIndexByType[Myristate] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(NAcylDiglyceride, "N-Acyl Diglyceride", Alphabet_Amino);
    typeInfoIndexByType[NAcylDiglyceride] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(NRegion, "N-Region", Alphabet_Nucleic);
    typeInfoIndexByType[NRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(NcRna, "ncRNA", Alphabet_Nucleic);
    typeInfoIndexByType[NcRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(NonConsecutiveResidues, "Non Consecutive Residues", Alphabet_Amino);
    typeInfoIndexByType[NonConsecutiveResidues] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Nuclease, "Nuclease", Alphabet_Amino);
    typeInfoIndexByType[Nuclease] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(OldSequence, "Old Sequence", Alphabet_Nucleic);
    typeInfoIndexByType[OldSequence] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Operon, "Operon", Alphabet_Nucleic);
    typeInfoIndexByType[Operon] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(OriT, "oriT", Alphabet_Nucleic);
    typeInfoIndexByType[OriT] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Overhang, "Overhang", Alphabet_Nucleic);
    typeInfoIndexByType[Overhang] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Palmitate, "Palmitate", Alphabet_Amino);
    typeInfoIndexByType[Palmitate] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Periplasmic, "Periplasmic", Alphabet_Amino);
    typeInfoIndexByType[Periplasmic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Ph, "PH", Alphabet_Amino);
    typeInfoIndexByType[Ph] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Phosphorylation, "Phosphorylation", Alphabet_Amino);
    typeInfoIndexByType[Phosphorylation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PolyASignal, "PolyA Signal", Alphabet_Nucleic);
    typeInfoIndexByType[PolyASignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PolyASite, "PolyA Site", Alphabet_Nucleic);
    typeInfoIndexByType[PolyASite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PolyAa, "Poly-AA", Alphabet_Amino);
    typeInfoIndexByType[PolyAa] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Precursor, "Precursor", Alphabet_Amino);
    typeInfoIndexByType[Precursor] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PrecursorRna, "Precursor RNA", Alphabet_Nucleic);
    typeInfoIndexByType[PrecursorRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Primer, "Primer", Alphabet_Nucleic);
    typeInfoIndexByType[Primer] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PrimerBindingSite, "Primer Binding Site", Alphabet_Nucleic);
    typeInfoIndexByType[PrimerBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PrimaryTranscript, "Primary Transcript", Alphabet_Nucleic);
    typeInfoIndexByType[PrimaryTranscript] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ProcessedActivePeptide, "Processed active peptide", Alphabet_Amino);
    typeInfoIndexByType[ProcessedActivePeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Promoter, "Promoter", Alphabet_Nucleic);
    typeInfoIndexByType[Promoter] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PromoterEukaryotic, "Promoter Eukaryotic", Alphabet_Nucleic);
    typeInfoIndexByType[PromoterEukaryotic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PromoterProkaryotic, "Promoter Prokaryotic", Alphabet_Nucleic);
    typeInfoIndexByType[PromoterProkaryotic] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Propeptide, "Propeptide", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[Propeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Proprotein, "Proprotein", Alphabet_Amino);
    typeInfoIndexByType[Proprotein] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Protease, "Protease", Alphabet_Amino);
    typeInfoIndexByType[Protease] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Protein, "Protein", Alphabet_Nucleic);
    typeInfoIndexByType[Protein] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ProteinBindingSite, "Protein Binding Site", Alphabet_Nucleic);
    typeInfoIndexByType[ProteinBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Provirus, "Provirus", Alphabet_Nucleic);
    typeInfoIndexByType[Provirus] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PyridoxalPhBindingSite, "Pyridoxal Ph. Binding Site", Alphabet_Amino);
    typeInfoIndexByType[PyridoxalPhBindingSite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(PyrrolidoneCarboxylicAcid, "Pyrrolidone Carboxylic Acid", Alphabet_Amino);
    typeInfoIndexByType[PyrrolidoneCarboxylicAcid] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(RRna, "rRNA", Alphabet_Nucleic);
    typeInfoIndexByType[RRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Rbs, "RBS", Alphabet_Nucleic);
    typeInfoIndexByType[Rbs] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Region, "Region", Alphabet_Nucleic);
    typeInfoIndexByType[Region] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Regulatory, "Regulatory", Alphabet_Nucleic);
    typeInfoIndexByType[Regulatory] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(RepeatRegion, "Repeat Region", Alphabet_Nucleic);
    typeInfoIndexByType[RepeatRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(RepeatUnit, "Repeat Unit", Alphabet_Nucleic);
    typeInfoIndexByType[RepeatUnit] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(RepetitiveRegion, "Repetitive region", Alphabet_Amino);
    typeInfoIndexByType[RepetitiveRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ReplicationOrigin, "Replication Origin", Alphabet_Nucleic);
    typeInfoIndexByType[ReplicationOrigin] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SRegion, "S-Region", Alphabet_Nucleic);
    typeInfoIndexByType[SRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Satellite, "Satellite", Alphabet_Nucleic);
    typeInfoIndexByType[Satellite] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ScRna, "scRNA", Alphabet_Nucleic);
    typeInfoIndexByType[ScRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SeconadaryStructure, "Seconadary structure", Alphabet_Amino);
    typeInfoIndexByType[SeconadaryStructure] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Sh2, "SH2", Alphabet_Amino);
    typeInfoIndexByType[Sh2] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Sh3, "SH3", Alphabet_Amino);
    typeInfoIndexByType[Sh3] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SignalPeptide, "Signal Peptide", Alphabet_Nucleic);
    typeInfoIndexByType[SignalPeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SignalSequence, "Signal Sequence", Alphabet_Amino);
    typeInfoIndexByType[SignalSequence] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Silencer, "Silencer", Alphabet_Nucleic);
    typeInfoIndexByType[Silencer] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Similarity, "Similarity", Alphabet_Amino);
    typeInfoIndexByType[Similarity] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Site, "Site", Alphabet_Nucleic);
    typeInfoIndexByType[Site] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SnRna, "snRNA", Alphabet_Nucleic);
    typeInfoIndexByType[SnRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SnoRna, "snoRNA", Alphabet_Nucleic);
    typeInfoIndexByType[SnoRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Source, "Source", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[Source] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SplicingSignal, "Splicing Signal", Alphabet_Nucleic);
    typeInfoIndexByType[SplicingSignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(SplicingVariant, "Splicing Variant", Alphabet_Amino);
    typeInfoIndexByType[SplicingVariant] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(StemLoop, "Stem Loop", Alphabet_Nucleic);
    typeInfoIndexByType[StemLoop] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Sts, "STS", Alphabet_Nucleic);
    typeInfoIndexByType[Sts] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Sulfatation, "Sulfatation", Alphabet_Amino);
    typeInfoIndexByType[Sulfatation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(TRna, "tRNA", Alphabet_Nucleic);
    typeInfoIndexByType[TRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(TataSignal, "TATA Signal", Alphabet_Nucleic);
    typeInfoIndexByType[TataSignal] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Telomere, "Telomere", Alphabet_Nucleic);
    typeInfoIndexByType[Telomere] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Terminator, "Terminator", Alphabet_Nucleic);
    typeInfoIndexByType[Terminator] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Thioether, "Thioether", Alphabet_Amino);
    typeInfoIndexByType[Thioether] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Thiolester, "Thiolester", Alphabet_Amino);
    typeInfoIndexByType[Thiolester] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ThreePrimeClip, "3' Clip", Alphabet_Nucleic);
    typeInfoIndexByType[ThreePrimeClip] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ThreePrimeUtr, "3' UTR", Alphabet_Nucleic);
    typeInfoIndexByType[ThreePrimeUtr] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(TmRna, "tmRNA", Alphabet_Nucleic);
    typeInfoIndexByType[TmRna] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(TransitPeptide, "Transit Peptide", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[TransitPeptide] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(TransmembraneRegion, "Transmembrane Region", Alphabet_Amino);
    typeInfoIndexByType[TransmembraneRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Transposon, "Transposon", Alphabet_Nucleic);
    typeInfoIndexByType[Transposon] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Uncertainty, "Uncertainty", Alphabet_Amino);
    typeInfoIndexByType[Uncertainty] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Unsure, "Unsure", Alphabet_Nucleic);
    typeInfoIndexByType[Unsure] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(VRegion, "V-Region", Alphabet_Nucleic);
    typeInfoIndexByType[VRegion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(VSegment, "V-Segment", Alphabet_Nucleic);
    typeInfoIndexByType[VSegment] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Variant, "Variant", Alphabet_Amino);
    typeInfoIndexByType[Variant] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Variation, "Variation", Alphabet_Nucleic | Alphabet_Amino);
    typeInfoIndexByType[Variation] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(Virion, "Virion", Alphabet_Nucleic);
    typeInfoIndexByType[Virion] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ZincFinger, "Zinc Finger", Alphabet_Amino);
    typeInfoIndexByType[ZincFinger] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(ZincFingerDomain, "Zinc Finger Domain", Alphabet_Nucleic);
    typeInfoIndexByType[ZincFingerDomain] = infoCounter++;
    featureTypeInfos << U2FeatureTypeInfo(RestrictionSite, "Restriction Site", Alphabet_Nucleic);
    typeInfoIndexByType[RestrictionSite] = infoCounter++;

    return featureTypeInfos;
}

U2FeatureTypes::U2FeatureTypeInfo::U2FeatureTypeInfo(U2FeatureType featureType, const QString &visualName, Alphabets alphabets) :
    featureType(featureType),
    visualName(visualName),
    alphabets(alphabets)
{

}

}   // namespace U2
