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

#include "U2FeatureType.h"

#include <U2Core/FeatureColors.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

QHash<U2FeatureType, int> U2FeatureTypes::typeInfoIndexByType = QHash<U2FeatureType, int>();
const QList<U2FeatureTypes::U2FeatureTypeInfo> U2FeatureTypes::typeInfos = U2FeatureTypes::initFeatureTypes();

QList<U2FeatureTypes::U2FeatureType> U2FeatureTypes::getTypes(const Alphabets& alphabets) {
    QList<U2FeatureTypes::U2FeatureType> types;
    foreach (const U2FeatureTypeInfo& info, typeInfos) {
        if (info.alphabets & alphabets) {
            types << info.featureType;
        }
    }
    return types;
}

QString U2FeatureTypes::getVisualName(const U2FeatureType& type) {
    int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", "");
    return typeInfos[typeInfoIndex].visualName;
}

U2FeatureTypes::Alphabets U2FeatureTypes::getAlphabets(const U2FeatureType& type) {
    int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", Alphabet_None);
    return typeInfos[typeInfoIndex].alphabets;
}

QColor U2FeatureTypes::getColor(const U2FeatureType& type) {
    int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", {});
    return typeInfos[typeInfoIndex].color;
}

QColor U2FeatureTypes::getDescription(const U2FeatureType& type) {
    int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", {});
    return typeInfos[typeInfoIndex].description;
}

bool U2FeatureTypes::isShowOnAminoFrame(const U2FeatureType& type) {
    int typeInfoIndex = typeInfoIndexByType.value(type, -1);
    SAFE_POINT(typeInfoIndex >= 0, "Unexpected feature type", {});
    return typeInfos[typeInfoIndex].isShowOnAminoFrame;
}

U2FeatureType U2FeatureTypes::getTypeByName(const QString& visualName) {
    foreach (const U2FeatureTypeInfo& info, typeInfos) {
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
    auto r = [&typeInfoList, &typeInfoIndex](const U2FeatureType& type,
                                             const QString& name,
                                             const Alphabets& alphabets,
                                             const QString& description = "",
                                             const QString& colorName = "",
                                             bool isShowOnAminoFrame = false) {
        SAFE_POINT(colorName.isEmpty() || colorName.startsWith("#"), "Got invalid color name: " + colorName, );
        QColor color(colorName);
        if (!color.isValid()) {
            color = FeatureColors::genLightColor(colorName);
        }
        SAFE_POINT(color.isValid(), "Got invalid color for feature: " + name, );
        SAFE_POINT(alphabets.testFlag(U2FeatureTypes::Alphabet_Nucleic) || !isShowOnAminoFrame, "Only features with nucleic alphabet may have isShowOnAminoFrame ON", );
        typeInfoList << U2FeatureTypeInfo(type, name, alphabets, color, description, isShowOnAminoFrame);
        typeInfoIndexByType[type] = typeInfoIndex;
        typeInfoIndex++;
    };

    // The DDBJ/ENA/GenBank/EMBL Feature Table Definition.
    // See https://www.insdc.org/files/feature_table.html#7.2 (Genbank)
    // or https://www.ebi.ac.uk/ena/WebFeat/ (EMBL).
    r(AssemblyGap, "assembly_gap", Alphabet_Nucleic, QObject::tr("Gap between two components of a genome or transcriptome assembly"));
    r(CRegion, "C_region", Alphabet_Nucleic, QObject::tr("Span of the C immunological feature"));
    r(Cds, "CDS", Alphabet_Nucleic, QObject::tr("Sequence coding for amino acids in protein (includes stop codon)"), "#9bffff", true);
    r(Centromere, "centromere", Alphabet_Nucleic, QObject::tr("Region of biological interest identified as a centromere and which has been experimentally characterized"));
    r(DLoop, "D-Loop", Alphabet_Nucleic, QObject::tr("Displacement loop"));
    r(DSegment, "D_segment", Alphabet_Nucleic, QObject::tr("Span of the D immunological feature"));
    r(Exon, "exon", Alphabet_Nucleic, QObject::tr("Region that codes for part of spliced mRNA"));
    r(Gap, "gap", Alphabet_Nucleic, QObject::tr("Gap in the sequence"));
    r(Gene, "gene", Alphabet_Nucleic, QObject::tr("Region that defines a functional gene, possibly including upstream (promoter, enhancer, etc) and downstream control elements, and for which a name has been assigned."), "#00ffc8");
    r(IDna, "iDNA", Alphabet_Nucleic, QObject::tr("Intervening DNA eliminated by recombination"));
    r(Intron, "intron", Alphabet_Nucleic, QObject::tr("Transcribed region excised by mRNA splicing"));
    r(JSegment, "J_segment", Alphabet_Nucleic, QObject::tr("Joining segment of immunoglobulin light and heavy chains, and T-cell receptor alpha, beta, and gamma chains"));
    r(MaturePeptide, "mat_peptide", Alphabet_Nucleic, QObject::tr("Mature peptide coding region (does not include stop codon)"), "", true);
    r(MiscBindingSite, "misc_binding", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Miscellaneous binding site"));
    r(MiscDifference, "misc_difference", Alphabet_Nucleic, QObject::tr("Miscellaneous difference feature"));
    r(MiscFeature, "misc_feature", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Region of biological significance that cannot be described by any other feature"), "#ffff99");
    r(MiscRecombination, "misc_recomb", Alphabet_Nucleic, QObject::tr("Miscellaneous, recombination feature"));
    r(MiscRna, "misc_RNA", Alphabet_Nucleic, QObject::tr("Miscellaneous transcript feature not defined by other RNA keys"));
    r(MiscStructure, "misc_structure", Alphabet_Nucleic, QObject::tr("Miscellaneous DNA or RNA structure"));
    r(MobileElement, "mobile_element", Alphabet_Nucleic, QObject::tr("Region of genome containing mobile elements"));
    r(ModifiedBase, "modified_base", Alphabet_Nucleic, QObject::tr("The indicated base is a modified nucleotide"));
    r(MRna, "mRNA", Alphabet_Nucleic, QObject::tr("Messenger RNA"));
    r(NcRna, "ncRNA", Alphabet_Nucleic, QObject::tr("A non-protein-coding gene, other than ribosomal RNA and transfer RNA, the functional molecule of which is the RNA transcript"));
    r(NRegion, "N_region", Alphabet_Nucleic, QObject::tr("Span of the N immunological feature"));
    r(OldSequence, "old_sequence", Alphabet_Nucleic, QObject::tr("Presented sequence revises a previous version"));
    r(Operon, "operon", Alphabet_Nucleic, QObject::tr("Region containing polycistronic transcript including a cluster of genes that are under the control of the same regulatory sequences/promoter and in the same biological pathway"));
    r(OriT, "oriT", Alphabet_Nucleic, QObject::tr("Origin of transfer; region of a DNA molecule where transfer is initiated during the process of conjugation or mobilization"));
    r(PolyASite, "polyA_site", Alphabet_Nucleic, QObject::tr("Site at which polyadenine is added to mRNA"));
    r(PrecursorRna, "precursor_RNA", Alphabet_Nucleic, QObject::tr("Any RNA species that is not yet the mature RNA product"));
    r(PrimaryTranscript, "prim_transcript", Alphabet_Nucleic, QObject::tr("Primary (unprocessed) transcript"));
    r(PrimerBindingSite, "primer_bind", Alphabet_Nucleic, QObject::tr("Non-covalent primer binding site"));
    r(Propeptide, "propeptide", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Coding sequence for the domain of a proprotein that is cleaved to form the mature protein product"));
    r(ProteinBindingSite, "protein_bind", Alphabet_Nucleic, QObject::tr("Non-covalent protein binding site on DNA or RNA"));
    // This feature has replaced the following Feature Keys on 15-DEC-2014:
    // enhancer, promoter, CAAT_signal, TATA_signal, -35_signal, -10_signal, RBS, GC_signal, polyA_signal, attenuator, terminator, misc_signal.
    r(Regulatory, "regulatory", Alphabet_Nucleic, QObject::tr("Any region of sequence that functions in the regulation of transcription or translation"));
    r(RepeatRegion, "repeat_region", Alphabet_Nucleic, QObject::tr("Sequence containing repeated subsequences"), "#ccccff");
    r(ReplicationOrigin, "rep_origin", Alphabet_Nucleic, QObject::tr("Replication origin for duplex DNA"));
    r(RRna, "rRNA", Alphabet_Nucleic, QObject::tr("Ribosomal RNA"));
    r(SRegion, "S_region", Alphabet_Nucleic, QObject::tr("Span of the S immunological feature"));
    r(SignalPeptide, "sig_peptide", Alphabet_Nucleic, QObject::tr("Signal peptide coding region"));
    r(Source, "source", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Identifies the biological source of the specified span of the sequence"), "#cccccc");
    r(StemLoop, "stem_loop", Alphabet_Nucleic, QObject::tr("Hair-pin loop structure in DNA or RNA"));
    r(Sts, "STS", Alphabet_Nucleic, QObject::tr("Sequence Tagged Site; operationally unique sequence that identifies the combination of primer spans used in a PCR assay"), "#00dcdc");
    r(Telomere, "telomere", Alphabet_Nucleic, QObject::tr("Region of biological interest identified as a telomere and which has been experimentally characterized"));
    r(TmRna, "tmRNA", Alphabet_Nucleic, QObject::tr("Transfer messenger RNA; tmRNA acts as a tRNA first, and then as an mRNA that encodes a peptide tag; the ribosome translates this mRNA region of tmRNA and attaches the encoded peptide tag to the C-terminus of the unfinished protein; this attached tag targets the protein for destruction or proteolysis"));
    r(TransitPeptide, "transit_peptide", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Transit peptide coding region"));
    r(TRna, "tRNA", Alphabet_Nucleic, QObject::tr("Transfer RNA"), "#c8fac8");
    r(Unsure, "unsure", Alphabet_Nucleic, QObject::tr("Authors are unsure about the sequence in this region"));
    r(VRegion, "V_region", Alphabet_Nucleic, QObject::tr("Span of the V immunological feature"));
    r(VSegment, "V_segment", Alphabet_Nucleic, QObject::tr("Variable segment of immunoglobulin light and heavy chains, and T-cell receptor alpha, beta, and gamma chains; codes for most of the variable region (V_region) and the last few amino acids of the leader peptide"));
    r(Variation, "variation", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("A related population contains stable mutation"), "#e32636");
    r(ThreePrimeUtr, "3'UTR", Alphabet_Nucleic, QObject::tr("3' untranslated region (trailer)"), "#ffcde6");
    r(FivePrimeUtr, "5'UTR", Alphabet_Nucleic, QObject::tr("5' untranslated region (leader)"), "#ffc8c8");

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
    r(Attenuator, "Attenuator", Alphabet_Nucleic, QObject::tr("Sequence related to transcription termination"));
    r(BHlhDomain, "bHLH Domain", Alphabet_Nucleic);
    r(Basic, "Basic", Alphabet_Amino);
    r(BetaSheet, "Beta-Sheet", Alphabet_Amino);
    r(BetaStrandRegion, "Beta-strand region", Alphabet_Amino);
    r(BiotinBindingSite, "Biotin Binding Site", Alphabet_Amino);
    r(Blocked, "Blocked", Alphabet_Amino);
    r(C2, "C2", Alphabet_Amino);
    r(CaatSignal, "CAAT Signal", Alphabet_Nucleic, QObject::tr("`CAAT box' in eukaryotic promoters"));
    r(Calcium, "Calcium", Alphabet_Amino);
    r(CatalyticRegion, "Catalytic Region", Alphabet_Amino);
    r(CellAttachment, "Cell Attachment", Alphabet_Amino);
    r(Cellular, "Cellular", Alphabet_Nucleic);
    r(CholesterolBindingSite, "Cholesterol Binding Site", Alphabet_Amino);
    r(CleavageSite, "Cleavage Site", Alphabet_Amino);
    r(CoiledCoil, "Coiled coil", Alphabet_Amino);
    r(CollagenType, "Collagen-type", Alphabet_Amino);
    r(Comment, "Comment", Alphabet_None);
    r(Conflict, "Conflict", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Independent sequence determinations differ"));
    r(ConnectingPeptide, "Connecting Peptide", Alphabet_Amino);
    r(Cub, "CUB", Alphabet_Amino);
    r(Cytoplasmic, "Cytoplasmic", Alphabet_Amino);
    r(Disulfide, "Disulfide", Alphabet_Amino, QObject::tr("Describes disulfide bonds (for protein files)"));
    r(Egf, "EGF", Alphabet_Amino);
    r(Enhancer, "Enhancer", Alphabet_Nucleic, QObject::tr("Cis-acting enhancer of promoter function"));
    r(Exoplasmic, "Exoplasmic", Alphabet_Amino);
    r(Extracellular, "Extracellular", Alphabet_Amino);
    r(Farnesyl, "Farnesyl", Alphabet_Amino);
    r(Fibronectin, "Fibronectin", Alphabet_Amino);
    r(FivePrimeClip, "5' Clip", Alphabet_Nucleic, QObject::tr("5'-most region of a precursor transcript removed in processing"));
    r(Formylation, "Formylation", Alphabet_Amino);
    r(GammaCarboxyglumaticAcid, "Gamma-Carboxyglumatic Acid", Alphabet_Amino);
    r(GcSignal, "GC-Signal", Alphabet_Nucleic, QObject::tr("`GC box' in eukaryotic promoters"));
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
    r(JRegion, "J-Region", Alphabet_Nucleic, QObject::tr("Span of the J immunological feature"));
    r(Kh, "KH", Alphabet_Amino);
    r(Kinase, "Kinase", Alphabet_Amino);
    r(LeucineZipper, "Leucine Zipper", Alphabet_Amino);
    r(LeucineZipperDomain, "Leucine Zipper Domain", Alphabet_Nucleic);
    r(Loci, "Loci", Alphabet_Nucleic);
    r(Ltr, "LTR", Alphabet_Nucleic, QObject::tr("Long terminal repeat"));
    r(MatureChain, "Mature chain", Alphabet_Amino);
    r(Methylation, "Methylation", Alphabet_Amino);
    r(Minus10Signal, "-10 Signal", Alphabet_Nucleic, QObject::tr("`Pribnow box' in prokaryotic promoters"));
    r(Minus35Signal, "-35 Signal", Alphabet_Nucleic, QObject::tr("`-35 box' in prokaryotic promoters"));
    r(MiscBond, "Bond: Misc", Alphabet_Amino);
    r(MiscDnaRnaBindingRegion, "DNA/RNA binding region: Misc", Alphabet_Amino);
    r(MiscDomain, "Domain: Misc", Alphabet_Amino);
    r(MiscLipid, "Lipid: Misc", Alphabet_Amino);
    r(MiscMarker, "Misc. Marker", Alphabet_Nucleic);
    r(MiscMetal, "Metal: Misc", Alphabet_Amino);
    r(MiscNpBindingRegion, "NP binding region: Misc", Alphabet_Amino);
    r(MiscRegion, "Region: Misc", Alphabet_Amino);
    r(MiscResidueModification, "Residue Modification: Misc", Alphabet_Amino);
    r(MiscSignal, "Misc. Signal", Alphabet_Nucleic | Alphabet_Amino, QObject::tr("Miscellaneous signal"));
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
    r(PolyASignal, "PolyA Signal", Alphabet_Nucleic, QObject::tr("Signal for cleavage & polyadenylation"));
    r(PolyAa, "Poly-AA", Alphabet_Amino);
    r(Precursor, "Precursor", Alphabet_Amino);
    r(Primer, "Primer", Alphabet_Nucleic, QObject::tr("Primer binding region used with PCR"));
    r(ProcessedActivePeptide, "Processed active peptide", Alphabet_Amino);
    r(Promoter, "Promoter", Alphabet_Nucleic, QObject::tr("A region involved in transcription initiation"));
    r(PromoterEukaryotic, "Promoter Eukaryotic", Alphabet_Nucleic);
    r(PromoterProkaryotic, "Promoter Prokaryotic", Alphabet_Nucleic);
    r(Proprotein, "Proprotein", Alphabet_Amino);
    r(Protease, "Protease", Alphabet_Amino);
    r(Protein, "Protein", Alphabet_Nucleic);
    r(Provirus, "Provirus", Alphabet_Nucleic);
    r(PyridoxalPhBindingSite, "Pyridoxal Ph. Binding Site", Alphabet_Amino);
    r(PyrrolidoneCarboxylicAcid, "Pyrrolidone Carboxylic Acid", Alphabet_Amino);
    r(Rbs, "RBS", Alphabet_Nucleic, QObject::tr("Ribosome binding site"));
    r(Region, "Region", Alphabet_Nucleic);
    r(RepeatUnit, "Repeat Unit", Alphabet_Nucleic, QObject::tr("One repeated unit of a repeat_region"), "#ccccff");
    r(RepetitiveRegion, "Repetitive region", Alphabet_Amino);
    r(RestrictionSite, "Restriction Site", Alphabet_Nucleic);
    r(Satellite, "Satellite", Alphabet_Nucleic, QObject::tr("Satellite repeated sequence"));
    r(ScRna, "scRNA", Alphabet_Nucleic, QObject::tr("Small cytoplasmic RNA"));
    r(SecondaryStructure, "Secondary structure", Alphabet_Amino);
    r(Sh2, "SH2", Alphabet_Amino);
    r(Sh3, "SH3", Alphabet_Amino);
    r(SignalSequence, "Signal Sequence", Alphabet_Amino);
    r(Silencer, "Silencer", Alphabet_Nucleic);
    r(Similarity, "Similarity", Alphabet_Amino);
    r(Site, "Site", Alphabet_Nucleic);
    r(SnRna, "snRNA", Alphabet_Nucleic, QObject::tr("Small nuclear RNA"));
    r(SnoRna, "snoRNA", Alphabet_Nucleic);
    r(SplicingSignal, "Splicing Signal", Alphabet_Nucleic);
    r(SplicingVariant, "Splicing Variant", Alphabet_Amino);
    r(Sulfatation, "Sulfatation", Alphabet_Amino);
    r(TataSignal, "TATA Signal", Alphabet_Nucleic, QObject::tr("`TATA box' in eukaryotic promoters"));
    r(Terminator, "Terminator", Alphabet_Nucleic, QObject::tr("Sequence causing transcription termination"));
    r(Thioether, "Thioether", Alphabet_Amino);
    r(Thiolester, "Thiolester", Alphabet_Amino);
    r(ThreePrimeClip, "3' Clip", Alphabet_Nucleic, QObject::tr("3'-most region of a precursor transcript removed in processing"));
    r(TransmembraneRegion, "Transmembrane Region", Alphabet_Amino);
    r(Transposon, "Transposon", Alphabet_Nucleic, QObject::tr("Transposable element (TN)"));
    r(Uncertainty, "Uncertainty", Alphabet_Amino);
    r(Variant, "Variant", Alphabet_Amino);
    r(Virion, "Virion", Alphabet_Nucleic);
    r(ZincFinger, "Zinc Finger", Alphabet_Amino);
    r(ZincFingerDomain, "Zinc Finger Domain", Alphabet_Nucleic);

    return typeInfoList;
}

U2FeatureTypes::U2FeatureTypeInfo::U2FeatureTypeInfo(const U2FeatureType& _featureType,
                                                     const QString& _visualName,
                                                     const Alphabets& _alphabets,
                                                     const QColor& _color,
                                                     const QString& _description,
                                                     bool _isShowOnAminoFrame)
    : featureType(_featureType),
      visualName(_visualName),
      alphabets(_alphabets),
      color(_color),
      description(_description),
      isShowOnAminoFrame(_isShowOnAminoFrame) {
}

}  // namespace U2
