/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "U2FeatureType.h"

#include <U2Core/U2SafePoints.h>

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
    QList<U2FeatureTypeInfo> typeInfoList;
    int typeInfoIndex = 0;

    // Registers U2FeatureType.
    auto r = [&typeInfoList, &typeInfoIndex](const U2FeatureType &type, const QString &name, const Alphabets &alphabets) {
        typeInfoList << U2FeatureTypeInfo(type, name, alphabets);
        typeInfoIndexByType[type] = typeInfoIndex;
        typeInfoIndex++;
    };

    // The DDBJ/ENA/GenBank/EMBL Feature Table Definition.
    // See https://www.insdc.org/files/feature_table.html#7.2 (Genbank)
    // or https://www.ebi.ac.uk/ena/WebFeat/ (EMBL).
    r(AssemblyGap, "assembly_gap", Alphabet_Nucleic);
    r(CRegion, "C_region", Alphabet_Nucleic);
    r(Cds, "CDS", Alphabet_Nucleic);
    r(Centromere, "centromere", Alphabet_Nucleic);
    r(DLoop, "D-Loop", Alphabet_Nucleic);
    r(DSegment, "D_segment", Alphabet_Nucleic);
    r(Exon, "exon", Alphabet_Nucleic);
    r(Gap, "gap", Alphabet_Nucleic);
    r(Gene, "gene", Alphabet_Nucleic);
    r(IDna, "iDNA", Alphabet_Nucleic);
    r(Intron, "intron", Alphabet_Nucleic);
    r(JSegment, "J_segment", Alphabet_Nucleic);
    r(MaturePeptide, "mat_peptide", Alphabet_Nucleic);
    r(MiscBindingSite, "misc_binding", Alphabet_Nucleic | Alphabet_Amino);
    r(MiscDifference, "misc_difference", Alphabet_Nucleic);
    r(MiscFeature, "misc_feature", Alphabet_Nucleic | Alphabet_Amino);
    r(MiscRecombination, "misc_recomb", Alphabet_Nucleic);
    r(MiscRna, "misc_RNA", Alphabet_Nucleic);
    r(MiscStructure, "misc_structure", Alphabet_Nucleic);
    r(MobileElement, "mobile_element", Alphabet_Nucleic);
    r(ModifiedBase, "modified_base", Alphabet_Nucleic);
    r(MRna, "mRNA", Alphabet_Nucleic);
    r(NcRna, "ncRNA", Alphabet_Nucleic);
    r(NRegion, "N_region", Alphabet_Nucleic);
    r(OldSequence, "old_sequence", Alphabet_Nucleic);
    r(Operon, "operon", Alphabet_Nucleic);
    r(OriT, "oriT", Alphabet_Nucleic);
    r(PolyASite, "polyA_site", Alphabet_Nucleic);
    r(PrecursorRna, "precursor_RNA", Alphabet_Nucleic);
    r(PrimaryTranscript, "prim_transcript", Alphabet_Nucleic);
    r(PrimerBindingSite, "primer_bind", Alphabet_Nucleic);
    r(Propeptide, "propeptide", Alphabet_Nucleic | Alphabet_Amino);
    r(ProteinBindingSite, "protein_bind", Alphabet_Nucleic);
    r(Regulatory, "regulatory", Alphabet_Nucleic);
    r(RepeatRegion, "repeat_region", Alphabet_Nucleic);
    r(ReplicationOrigin, "rep_origin", Alphabet_Nucleic);
    r(RRna, "rRNA", Alphabet_Nucleic);
    r(SRegion, "S_region", Alphabet_Nucleic);
    r(SignalPeptide, "sig_peptide", Alphabet_Nucleic);
    r(Source, "source", Alphabet_Nucleic | Alphabet_Amino);
    r(StemLoop, "stem_loop", Alphabet_Nucleic);
    r(Sts, "STS", Alphabet_Nucleic);
    r(Telomere, "telomere", Alphabet_Nucleic);
    r(TmRna, "tmRNA", Alphabet_Nucleic);
    r(TransitPeptide, "transit_peptide", Alphabet_Nucleic | Alphabet_Amino);
    r(TRna, "tRNA", Alphabet_Nucleic);
    r(Unsure, "unsure", Alphabet_Nucleic);
    r(VRegion, "V_region", Alphabet_Nucleic);
    r(VSegment, "V_segment", Alphabet_Nucleic);
    r(Variation, "variation", Alphabet_Nucleic | Alphabet_Amino);
    r(ThreePrimeUtr, "3'UTR", Alphabet_Nucleic);
    r(FivePrimeUtr, "5'UTR", Alphabet_Nucleic);

    // Extra types not related to any public format/specification.
    // Mapped to 'misc_feature' when saved to EMBL/Genbank format. The original name is saved/loaded using a special qualifier (GBFeatureUtils::QUALIFIER_NAME).
    r(AaRich, "AA-Rich", Alphabet_Amino);
    r(Acetylation, "Acetylation", Alphabet_Amino);
    r(Acetylation, "Active Site", Alphabet_Amino);
    r(Acetylation, "Adenylation", Alphabet_Amino);
    r(Allele, "Allele", Alphabet_Nucleic);
    r(AlphaHelix, "Alpha-Helix", Alphabet_Amino);
    r(AlteredSite, "Altered Site", Alphabet_Amino);
    r(Amidation, "Amidation", Alphabet_Amino);
    r(Attenuator, "Attenuator", Alphabet_Nucleic);
    r(BHlhDomain, "bHLH Domain", Alphabet_Nucleic);
    r(Basic, "Basic", Alphabet_Amino);
    r(BetaSheet, "Beta-Sheet", Alphabet_Amino);
    r(BetaStrandRegion, "Beta-strand region", Alphabet_Amino);
    r(BiotinBindingSite, "Biotin Binding Site", Alphabet_Amino);
    r(Blocked, "Blocked", Alphabet_Amino);
    r(C2, "C2", Alphabet_Amino);
    r(CaatSignal, "CAAT Signal", Alphabet_Nucleic);
    r(Calcium, "Calcium", Alphabet_Amino);
    r(CatalyticRegion, "Catalytic Region", Alphabet_Amino);
    r(CellAttachment, "Cell Attachment", Alphabet_Amino);
    r(Cellular, "Cellular", Alphabet_Nucleic);
    r(CholesterolBindingSite, "Cholesterol Binding Site", Alphabet_Amino);
    r(CleavageSite, "Cleavage Site", Alphabet_Amino);
    r(CoiledCoil, "Coiled coil", Alphabet_Amino);
    r(CollagenType, "Collagen-type", Alphabet_Amino);
    r(Comment, "Comment", Alphabet_None);
    r(Conflict, "Conflict", Alphabet_Nucleic | Alphabet_Amino);
    r(ConnectingPeptide, "Connecting Peptide", Alphabet_Amino);
    r(Cub, "CUB", Alphabet_Amino);
    r(Cytoplasmic, "Cytoplasmic", Alphabet_Amino);
    r(Disulfide, "Disulfide", Alphabet_Amino);
    r(Egf, "EGF", Alphabet_Amino);
    r(Enhancer, "Enhancer", Alphabet_Nucleic);
    r(Exoplasmic, "Exoplasmic", Alphabet_Amino);
    r(Extracellular, "Extracellular", Alphabet_Amino);
    r(Farnesyl, "Farnesyl", Alphabet_Amino);
    r(Fibronectin, "Fibronectin", Alphabet_Amino);
    r(FivePrimeClip, "5' Clip", Alphabet_Nucleic);
    r(Formylation, "Formylation", Alphabet_Amino);
    r(GammaCarboxyglumaticAcid, "Gamma-Carboxyglumatic Acid", Alphabet_Amino);
    r(GcSignal, "GC-Signal", Alphabet_Nucleic);
    r(GeranylGeranyl, "Geranyl-Geranyl", Alphabet_Amino);
    r(Glycosylation, "Glycosylation", Alphabet_Amino);
    r(GlycosylationSite, "Glycosylation Site", Alphabet_Nucleic);
    r(GpiAnchor, "GPI-Anchor", Alphabet_Amino);
    r(HelicalRegion, "Helical region", Alphabet_Amino);
    r(HemeBindingSite, "Heme Binding Site", Alphabet_Amino);
    r(HmgBox, "HMG-Box", Alphabet_Amino);
    r(Homeodomain, "Homeodomain", Alphabet_Nucleic | Alphabet_Amino);
    r(Hth, "H-T-H", Alphabet_Amino);
    r(HydrogenBondedTurn, "Hydrogen bonded turn", Alphabet_Amino);
    r(Hydroxylation, "Hydroxylation", Alphabet_Amino);
    r(Immunoglobulin, "Immunoglobulin", Alphabet_Amino);
    r(Insertion, "Insertion", Alphabet_Nucleic);
    r(Intracellular, "Intracellular", Alphabet_Amino);
    r(JRegion, "J-Region", Alphabet_Nucleic);
    r(Kh, "KH", Alphabet_Amino);
    r(Kinase, "Kinase", Alphabet_Amino);
    r(LeucineZipper, "Leucine Zipper", Alphabet_Amino);
    r(LeucineZipperDomain, "Leucine Zipper Domain", Alphabet_Nucleic);
    r(Loci, "Loci", Alphabet_Nucleic);
    r(Ltr, "LTR", Alphabet_Nucleic);
    r(MatureChain, "Mature chain", Alphabet_Amino);
    r(Methylation, "Methylation", Alphabet_Amino);
    r(Minus10Signal, "-10 Signal", Alphabet_Nucleic);
    r(Minus35Signal, "-35 Signal", Alphabet_Nucleic);
    r(MiscBond, "Bond: Misc", Alphabet_Amino);
    r(MiscDnaRnaBindingRegion, "DNA/RNA binding region: Misc", Alphabet_Amino);
    r(MiscDomain, "Domain: Misc", Alphabet_Amino);
    r(MiscLipid, "Lipid: Misc", Alphabet_Amino);
    r(MiscMarker, "Misc. Marker", Alphabet_Nucleic);
    r(MiscMetal, "Metal: Misc", Alphabet_Amino);
    r(MiscNpBindingRegion, "NP binding region: Misc", Alphabet_Amino);
    r(MiscRegion, "Region: Misc", Alphabet_Amino);
    r(MiscResidueModification, "Residue Modification: Misc", Alphabet_Amino);
    r(MiscSignal, "Misc. Signal", Alphabet_Nucleic | Alphabet_Amino);
    r(MiscSite, "Site: Misc", Alphabet_Amino);
    r(Mutation, "Mutation", Alphabet_Nucleic);
    r(Myristate, "Myristate", Alphabet_Amino);
    r(NAcylDiglyceride, "N-Acyl Diglyceride", Alphabet_Amino);
    r(NonConsecutiveResidues, "Non Consecutive Residues", Alphabet_Amino);
    r(Nuclease, "Nuclease", Alphabet_Amino);
    r(Overhang, "Overhang", Alphabet_Nucleic);
    r(Palmitate, "Palmitate", Alphabet_Amino);
    r(Periplasmic, "Periplasmic", Alphabet_Amino);
    r(Ph, "PH", Alphabet_Amino);
    r(Phosphorylation, "Phosphorylation", Alphabet_Amino);
    r(PolyASignal, "PolyA Signal", Alphabet_Nucleic);
    r(PolyAa, "Poly-AA", Alphabet_Amino);
    r(Precursor, "Precursor", Alphabet_Amino);
    r(Primer, "Primer", Alphabet_Nucleic);
    r(ProcessedActivePeptide, "Processed active peptide", Alphabet_Amino);
    r(Promoter, "Promoter", Alphabet_Nucleic);
    r(PromoterEukaryotic, "Promoter Eukaryotic", Alphabet_Nucleic);
    r(PromoterProkaryotic, "Promoter Prokaryotic", Alphabet_Nucleic);
    r(Proprotein, "Proprotein", Alphabet_Amino);
    r(Protease, "Protease", Alphabet_Amino);
    r(Protein, "Protein", Alphabet_Nucleic);
    r(Provirus, "Provirus", Alphabet_Nucleic);
    r(PyridoxalPhBindingSite, "Pyridoxal Ph. Binding Site", Alphabet_Amino);
    r(PyrrolidoneCarboxylicAcid, "Pyrrolidone Carboxylic Acid", Alphabet_Amino);
    r(Rbs, "RBS", Alphabet_Nucleic);
    r(Region, "Region", Alphabet_Nucleic);
    r(RepeatUnit, "Repeat Unit", Alphabet_Nucleic);
    r(RepetitiveRegion, "Repetitive region", Alphabet_Amino);
    r(RestrictionSite, "Restriction Site", Alphabet_Nucleic);
    r(Satellite, "Satellite", Alphabet_Nucleic);
    r(ScRna, "scRNA", Alphabet_Nucleic);
    r(SecondaryStructure, "Secondary structure", Alphabet_Amino);
    r(Sh2, "SH2", Alphabet_Amino);
    r(Sh3, "SH3", Alphabet_Amino);
    r(SignalSequence, "Signal Sequence", Alphabet_Amino);
    r(Silencer, "Silencer", Alphabet_Nucleic);
    r(Similarity, "Similarity", Alphabet_Amino);
    r(Site, "Site", Alphabet_Nucleic);
    r(SnRna, "snRNA", Alphabet_Nucleic);
    r(SnoRna, "snoRNA", Alphabet_Nucleic);
    r(SplicingSignal, "Splicing Signal", Alphabet_Nucleic);
    r(SplicingVariant, "Splicing Variant", Alphabet_Amino);
    r(Sulfatation, "Sulfatation", Alphabet_Amino);
    r(TataSignal, "TATA Signal", Alphabet_Nucleic);
    r(Terminator, "Terminator", Alphabet_Nucleic);
    r(Thioether, "Thioether", Alphabet_Amino);
    r(Thiolester, "Thiolester", Alphabet_Amino);
    r(ThreePrimeClip, "3' Clip", Alphabet_Nucleic);
    r(ThreePrimeUtr, "3' UTR", Alphabet_Nucleic);
    r(TransmembraneRegion, "Transmembrane Region", Alphabet_Amino);
    r(Transposon, "Transposon", Alphabet_Nucleic);
    r(Uncertainty, "Uncertainty", Alphabet_Amino);
    r(Variant, "Variant", Alphabet_Amino);
    r(Virion, "Virion", Alphabet_Nucleic);
    r(ZincFinger, "Zinc Finger", Alphabet_Amino);
    r(ZincFingerDomain, "Zinc Finger Domain", Alphabet_Nucleic);

    return typeInfoList;
}

U2FeatureTypes::U2FeatureTypeInfo::U2FeatureTypeInfo(U2FeatureType featureType, const QString &visualName, Alphabets alphabets)
    : featureType(featureType),
      visualName(visualName),
      alphabets(alphabets) {
}

}  // namespace U2
