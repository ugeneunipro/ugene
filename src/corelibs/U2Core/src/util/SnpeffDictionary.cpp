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

#include "SnpeffDictionary.h"

namespace U2 {

const QMap<QString, QString> SnpeffDictionary::impactDescriptions = SnpeffDictionary::initImpactDescriptions();
const QMap<QString, QString> SnpeffDictionary::effectDescriptions = SnpeffDictionary::initEffectDescriptions();
const QMap<QString, QString> SnpeffDictionary::messageDescriptions = SnpeffDictionary::initMessageDescriptions();

QMap<QString, QString> SnpeffDictionary::initImpactDescriptions() {
    QMap<QString, QString> result;
    result.insert("HIGH", "The variant is assumed to have high (disruptive) impact in the protein, probably causing protein truncation, loss of function or triggering nonsense mediated decay.");
    result.insert("MODERATE", "A non-disruptive variant that might change protein effectiveness.");
    result.insert("LOW", "Assumed to be mostly harmless or unlikely to change protein behavior.");
    result.insert("MODIFIER", "Usually non-coding variants or variants affecting non-coding genes, where predictions are difficult or there is no evidence of impact.");
    return result;
}

QMap<QString, QString> SnpeffDictionary::initEffectDescriptions() {
    QMap<QString, QString> result;

    // Seq. Ontology effects
    result.insert("coding_sequence_variant", "The variant hits a CDS. / One or many codons are changed. E.g.: An MNP of size multiple of 3.");
    result.insert("chromosome", "A large parte (over 1%) of the chromosome was deleted.");
    result.insert("inframe_insertion", "One or many codons are inserted. E.g.: An insert multiple of three in a codon boundary.");
    result.insert("disruptive_inframe_insertion", "One codon is changed and one or many codons are inserted. E.g.: An insert of size multiple of three, not at codon boundary.");
    result.insert("inframe_deletion", "One or many codons are deleted. E.g.: A deletion multiple of three at codon boundary.");
    result.insert("disruptive_inframe_deletion", "One codon is changed and one or more codons are deleted. E.g.: A deletion of size multiple of three, not at codon boundary.");
    result.insert("downstream_gene_variant", "Downstream of a gene (default length: 5K bases).");
    result.insert("exon_variant", "The variant hits an exon (from a non-coding transcript) or a retained intron.");
    result.insert("exon_loss_variant", "A deletion removes the whole exon.");
    result.insert("frameshift_variant", "Insertion or deletion causes a frame shift. E.g.: An indel size is not multiple of 3.");
    result.insert("gene_variant", "The variant hits a gene.");
    result.insert("intergenic_region", "The variant is in an intergenic region.");
    result.insert("conserved_intergenic_variant", "The variant is in a highly conserved intergenic region.");
    result.insert("intragenic_variant", "The variant hits a gene, but no transcripts within the gene.");
    result.insert("intron_variant", "Variant hits and intron. Technically, hits no exon in the transcript.");
    result.insert("conserved_intron_variant", "The variant is in a highly conserved intronic region.");
    result.insert("miRNA", "Variant affects an miRNA.");
    result.insert("missense_variant", "Variant causes a codon that produces a different amino acid. E.g.: Tgg/Cgg, W/R.");
    result.insert("initiator_codon_variant", "Variant causes start codon to be mutated into another start codon (the new codon produces a different AA). E.g.: Atg/Ctg, M/L (ATG and CTG can be START codons).");
    result.insert("stop_retained_variant", "Variant causes stop codon to be mutated into another stop codon (the new codon produces a different AA). E.g.: Atg/Ctg, M/L (ATG and CTG can be START codons).");
    result.insert("rare_amino_acid_variant", "The variant hits a rare amino acid thus is likely to produce protein loss of function.");
    result.insert("splice_acceptor_variant", "The variant hits a splice acceptor site (defined as two bases before exon start, except for the first exon).");
    result.insert("splice_donor_variant", "The variant hits a Splice donor site (defined as two bases after coding exon end, except for the last exon).");
    result.insert("splice_region_variant", "A sequence variant in which a change has occurred within the region of the splice site, either within 1-3 bases of the exon or 3-8 bases of the intron. / "
                                           "A varaint affective putative (Lariat) branch point, located in the intron. / "
                                           "A varaint affective putative (Lariat) branch point from U12 splicing machinery, located in the intron.");
    result.insert("stop_lost", "Variant causes stop codon to be mutated into a non-stop codon. E.g.: Tga/Cga, */R.");
    result.insert("5_prime_UTR_premature_start_codon_gain_variant", "A variant in 5'UTR region produces a three base sequence that can be a START codon.");
    result.insert("start_lost", "Variant causes start codon to be mutated into a non-start codon. E.g.: aTg/aGg, M/R.");
    result.insert("stop_gained", "Variant causes a STOP codon/ e.g.: Cag/Tag, Q/*.");
    result.insert("synonymous_variant", "Variant causes a codon that produces the same amino acid. E.g.: Ttg/Ctg, L/L.");
    result.insert("start_retained", "Variant causes start codon to be mutated into another start codon. E.g.: Ttg/Ctg, L/L (TTG and CTG can be START codons).");
    result.insert("stop_retained_variant", "Variant causes stop codon to be mutated into another stop codon. E.g.: taA/taG, */*.");
    result.insert("transcript_variant", "The variant hits a transcript.");
    result.insert("regulatory_region_variant", "The variant hits a known regulatory feature (non-coding).");
    result.insert("upstream_gene_variant", "Upstream of a gene (default length: 5K bases).");
    result.insert("3_prime_UTR_variant", "Variant hits 3'UTR region.");
    result.insert("3_prime_UTR_truncation", "The variant deletes an exon which is in the 3'UTR of the transcript.");
    result.insert("5_prime_UTR_variant", "Variant hits 5'UTR region.");
    result.insert("5_prime_UTR_truncation", "The variant deletes an exon which is in the 5'UTR of the transcript.");
    result.insert("sequence_feature", "A 'NextProt' based annotation. Details are provided in the 'feature type' sub-field (ANN), or in the effect details (EFF).");

    // Classic effects
    result.insert("CDS", "The variant hits a CDS.");
    result.insert("CHROMOSOME_LARGE_DELETION", "A large parte (over 1%) of the chromosome was deleted.");
    result.insert("CODON_CHANGE", "One or many codons are changed. E.g.: An MNP of size multiple of 3.");
    result.insert("CODON_INSERTION", "One or many codons are inserted. E.g.: An insert multiple of three in a codon boundary.");
    result.insert("CODON_CHANGE_PLUS_CODON_INSERTION", "One codon is changed and one or many codons are inserted. E.g.: An insert of size multiple of three, not at codon boundary.");
    result.insert("CODON_DELETION", "One or many codons are deleted. E.g.: A deletion multiple of three at codon boundary.");
    result.insert("CODON_CHANGE_PLUS_CODON_DELETION", "One codon is changed and one or more codons are deleted. E.g.: A deletion of size multiple of three, not at codon boundary.");
    result.insert("DOWNSTREAM", "Downstream of a gene (default length: 5K bases).");
    result.insert("EXON", "The variant hits an exon (from a non-coding transcript) or a retained intron.");
    result.insert("EXON_DELETED", "A deletion removes the whole exon.");
    result.insert("FRAME_SHIFT", "Insertion or deletion causes a frame shift. E.g.: An indel size is not multiple of 3.");
    result.insert("GENE", "The variant hits a gene.");
    result.insert("INTERGENIC", "The variant is in an intergenic region.");
    result.insert("INTERGENIC_CONSERVED", "The variant is in a highly conserved intergenic region.");
    result.insert("INTRAGENIC", "The variant hits a gene, but no transcripts within the gene.");
    result.insert("INTRON", "Variant hits and intron. Technically, hits no exon in the transcript.");
    result.insert("INTRON_CONSERVED", "The variant is in a highly conserved intronic region.");
    result.insert("MICRO_RNA", "Variant affects an miRNA.");
    result.insert("NON_SYNONYMOUS_CODING", "Variant causes a codon that produces a different amino acid. E.g.: Tgg/Cgg, W/R.");
    result.insert("NON_SYNONYMOUS_START", "Variant causes start codon to be mutated into another start codon (the new codon produces a different AA). E.g.: Atg/Ctg, M/L (ATG and CTG can be START codons).");
    result.insert("NON_SYNONYMOUS_STOP", "Variant causes stop codon to be mutated into another stop codon (the new codon produces a different AA). E.g.: Atg/Ctg, M/L (ATG and CTG can be START codons).");
    result.insert("RARE_AMINO_ACID", "The variant hits a rare amino acid thus is likely to produce protein loss of function.");
    result.insert("SPLICE_SITE_ACCEPTOR", "The variant hits a splice acceptor site (defined as two bases before exon start, except for the first exon).");
    result.insert("SPLICE_SITE_DONOR", "The variant hits a Splice donor site (defined as two bases after coding exon end, except for the last exon).");
    result.insert("SPLICE_SITE_REGION", "A sequence variant in which a change has occurred within the region of the splice site, either within 1-3 bases of the exon or 3-8 bases of the intron.");
    result.insert("SPLICE_SITE_BRANCH", "A varaint affective putative (Lariat) branch point, located in the intron.");
    result.insert("SPLICE_SITE_BRANCH_U12", "A varaint affective putative (Lariat) branch point from U12 splicing machinery, located in the intron.");
    result.insert("STOP_LOST", "Variant causes stop codon to be mutated into a non-stop codon. E.g.: Tga/Cga, */R.");
    result.insert("START_GAINED", "A variant in 5'UTR region produces a three base sequence that can be a START codon.");
    result.insert("START_LOST", "Variant causes start codon to be mutated into a non-start codon. E.g.: aTg/aGg, M/R.");
    result.insert("STOP_GAINED", "Variant causes a STOP codon/ e.g.: Cag/Tag, Q/*.");
    result.insert("SYNONYMOUS_CODING", "Variant causes a codon that produces the same amino acid. E.g.: Ttg/Ctg, L/L.");
    result.insert("SYNONYMOUS_START", "Variant causes start codon to be mutated into another start codon. E.g.: Ttg/Ctg, L/L (TTG and CTG can be START codons).");
    result.insert("SYNONYMOUS_STOP", "Variant causes stop codon to be mutated into another stop codon. E.g.: taA/taG, */*.");
    result.insert("TRANSCRIPT", "The variant hits a transcript.");
    result.insert("REGULATION", "The variant hits a known regulatory feature (non-coding).");
    result.insert("UPSTREAM", "Upstream of a gene (default length: 5K bases).");
    result.insert("UTR_3_PRIME", "Variant hits 3'UTR region.");
    result.insert("UTR_3_DELETED", "The variant deletes an exon which is in the 3'UTR of the transcript.");
    result.insert("UTR_5_PRIME", "Variant hits 5'UTR region.");
    result.insert("UTR_5_DELETED", "The variant deletes an exon which is in the 5'UTR of the transcript.");
    result.insert("NEXT_PROT", "A 'NextProt' based annotation. Details are provided in the 'feature type' sub-field (ANN), or in the effect details (EFF).");

    return result;
}

QMap<QString, QString> SnpeffDictionary::initMessageDescriptions() {
    QMap<QString, QString> result;

    // code
    result.insert("E1", "Chromosome does not exists in reference genome database. Typically indicates a mismatch between the chromosome names in the input file and the chromosome names used in the reference genome.");
    result.insert("E2", "The variant’s genomic coordinate is greater than chromosome's length.");
    result.insert("W1", "This means that the ‘REF’ field in the input VCF file does not match the reference genome. This warning may indicate a conflict between input data and data from reference genome (for instance is the input VCF was aligned to a different reference genome).");
    result.insert("W2", "Reference sequence is not available, thus no inference could be performed.");
    result.insert("W3", "A protein coding transcript having a non-multiple of 3 length. It indicates that the reference genome has missing information about this particular transcript.");
    result.insert("W4", "A protein coding transcript has two or more STOP codons in the middle of the coding sequence (CDS). This should not happen and it usually means the reference genome may have an error in this transcript.");
    result.insert("W5", "A protein coding transcript does not have a proper START codon. It is rare that a real transcript does not have a START codon, so this probably indicates an error or missing information in the reference genome.");
    result.insert("I1", "Variant has been realigned to the most 3-prime position within the transcript. This is usually done to to comply with HGVS specification to always report the most 3-prime annotation.");
    result.insert("I2", "This effect is a result of combining more than one variants (e.g. two consecutive SNPs that conform an MNP, or two consecutive frame_shift variants that compensate frame).");
    result.insert("I3", "An alternative reference sequence was used to calculate this annotation (e.g. cancer sample comparing somatic vs. germline).");

    // Message type
    result.insert("ERROR_CHROMOSOME_NOT_FOUND", result["E1"]);
    result.insert("ERROR_OUT_OF_CHROMOSOME_RANGE", result["E2"]);
    result.insert("WARNING_REF_DOES_NOT_MATCH_GENOME", result["W1"]);
    result.insert("WARNING_SEQUENCE_NOT_AVAILABLE", result["W2"]);
    result.insert("WARNING_TRANSCRIPT_INCOMPLETE", result["W3"]);
    result.insert("WARNING_TRANSCRIPT_MULTIPLE_STOP_CODONS", result["W4"]);
    result.insert("WARNING_TRANSCRIPT_NO_START_CODON", result["W5"]);
    result.insert("INFO_REALIGN_3_PRIME", result["I1"]);
    result.insert("INFO_COMPOUND_ANNOTATION", result["I2"]);
    result.insert("INFO_NON_REFERENCE_ANNOTATION", result["I3"]);

    return result;
}

}   // namespace U2
