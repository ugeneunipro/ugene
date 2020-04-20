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

#ifndef _U2_GENBANK_FEATURES_H_
#define _U2_GENBANK_FEATURES_H_

#include <U2Core/U2FeatureType.h>

#include <QMultiMap>
#include <QVector>
#include <QMutex>
#include <QColor>

namespace U2 {

enum GBFeatureKey {

    GBFeatureKey_assembly_gap = 0,// Gap between two components of a genome or transcriptome assembly

    GBFeatureKey_attenuator,      // Sequence related to transcription termination
    GBFeatureKey_bond,            // Describes disulfide bonds (for protein files)
    GBFeatureKey_C_region,        // Span of the C immunological feature
    GBFeatureKey_CAAT_signal,     // `CAAT box' in eukaryotic promoters
    GBFeatureKey_CDS,             // Sequence coding for amino acids in protein (includes stop codon)
    GBFeatureKey_conflict,        // Independent sequence determinations differ
    GBFeatureKey_centromere,      // Region of biological interest identified as a centromere and which has been experimentally characterized
    GBFeatureKey_D_loop,          // Displacement loop
    GBFeatureKey_D_segment,       // Span of the D immunological feature
    GBFeatureKey_enhancer,        // Cis-acting enhancer of promoter function
    GBFeatureKey_exon,            // Region that codes for part of spliced mRNA
    GBFeatureKey_gap,             // Gap in the sequence
    GBFeatureKey_gene,            // Region that defines a functional gene, possibly including upstream (promotor, enhancer, etc) and downstream control elements, and for which a name has been assigned.
    GBFeatureKey_GC_signal,       // `GC box' in eukaryotic promoters
    GBFeatureKey_iDNA,            // Intervening DNA eliminated by recombination
    GBFeatureKey_intron,          // Transcribed region excised by mRNA splicing
    GBFeatureKey_J_region,        // Span of the J immunological feature
    GBFeatureKey_J_segment,       // Joining segment of immunoglobulin light and heavy chains, and T-cell receptor alpha, beta, and gamma chains    // it might duplicate J_region
    GBFeatureKey_LTR,             // Long terminal repeat
    GBFeatureKey_mat_peptide,     // Mature peptide coding region (does not include stop codon)
    GBFeatureKey_misc_binding,    // Miscellaneous binding site
    GBFeatureKey_misc_difference, // Miscellaneous difference feature
    GBFeatureKey_misc_feature,    // Region of biological significance that cannot be described by any other feature
    GBFeatureKey_misc_recomb,     // Miscellaneous, recombination feature
    GBFeatureKey_misc_RNA,        // Miscellaneous transcript feature not defined by other RNA keys
    GBFeatureKey_misc_signal,     // Miscellaneous signal
    GBFeatureKey_misc_structure,  // Miscellaneous DNA or RNA structure
    GBFeatureKey_mobile_element,  // Region of genome containing mobile elements
    GBFeatureKey_modified_base,   // The indicated base is a modified nucleotide
    GBFeatureKey_mRNA,            // Messenger RNA
    GBFeatureKey_ncRNA,           // A non-protein-coding gene, other than ribosomal RNA and transfer RNA, the functional molecule of which is the RNA transcrip
    GBFeatureKey_N_region,        // Span of the N immunological feature
    GBFeatureKey_old_sequence,    // Presented sequence revises a previous version
    GBFeatureKey_operon,          // Region containing polycistronic transcript including a cluster of genes that are under the control of the same regulatory sequences/promotor and in the same biological pathway
    GBFeatureKey_oriT,            // Origin of transfer; region of a DNA molecule where transfer is initiated during the process of conjugation or mobilization
    GBFeatureKey_polyA_signal,    // Signal for cleavage & polyadenylation
    GBFeatureKey_polyA_site,      // Site at which polyadenine is added to mRNA
    GBFeatureKey_precursor_RNA,   // Any RNA species that is not yet the mature RNA product
    GBFeatureKey_prim_transcript, // Primary (unprocessed) transcript
    GBFeatureKey_primer,          // Primer binding region used with PCR
    GBFeatureKey_primer_bind,     // Non-covalent primer binding site
    GBFeatureKey_promoter,        // A region involved in transcription initiation
    GBFeatureKey_protein_bind,    // Non-covalent protein binding site on DNA or RNA
    GBFeatureKey_RBS,             // Ribosome binding site
    GBFeatureKey_rep_origin,      // Replication origin for duplex DNA
    GBFeatureKey_repeat_region,   // Sequence containing repeated subsequences
    GBFeatureKey_repeat_unit,     // One repeated unit of a repeat_region
    GBFeatureKey_rRNA,            // Ribosomal RNA
    GBFeatureKey_S_region,        // Span of the S immunological feature
    GBFeatureKey_satellite,       // Satellite repeated sequence
    GBFeatureKey_scRNA,           // Small cytoplasmic RNA
    GBFeatureKey_sig_peptide,     // Signal peptide coding region
    GBFeatureKey_snRNA,           // Small nuclear RNA
    GBFeatureKey_source,          // Identifies the biological source of the specified span of the sequence
    GBFeatureKey_stem_loop,       // Hair-pin loop structure in DNA or RNA
    GBFeatureKey_STS,             // Sequence Tagged Site; operationally unique sequence that identifies the combination of primer spans used in a PCR assay
    GBFeatureKey_TATA_signal,     // `TATA box' in eukaryotic promoters
    GBFeatureKey_telomere,        // Region of biological interest identified as a telomere and which has been experimentally characterized
    GBFeatureKey_terminator,      // Sequence causing transcription termination
    GBFeatureKey_tmRNA,           // Transfer messenger RNA; tmRNA acts as a tRNA first, and then as an mRNA that encodes a peptide tag; the ribosome translates this mRNA region of tmRNA and attaches the encoded peptide tag to the C-terminus of the unfinished protein; this attached tag targets the protein for destruction or proteolysis
    GBFeatureKey_transit_peptide, // Transit peptide coding region
    GBFeatureKey_transposon,      // Transposable element (TN)
    GBFeatureKey_tRNA,            // Transfer RNA
    GBFeatureKey_unsure,          // Authors are unsure about the sequence in this region
    GBFeatureKey_V_region,        // Span of the V immunological feature
    GBFeatureKey_V_segment,       // Variable segment of immunoglobulin light and heavy chains, and T-cell receptor alpha, beta, and gamma chains; codes for most of the variable region (V_region) and the last few amino acids of the leader peptide
    GBFeatureKey_variation,       // A related population contains stable mutation
    GBFeatureKey__10_signal,      // `Pribnow box' in prokaryotic promoters
    GBFeatureKey__35_signal,      // `-35 box' in prokaryotic promoters
    GBFeatureKey_3_clip,          // 3'-most region of a precursor transcript removed in processing
    GBFeatureKey_3_UTR,           // 3' untranslated region (trailer)
    GBFeatureKey_5_clip,          // 5'-most region of a precursor transcript removed in processing
    GBFeatureKey_5_UTR,           // 5' untranslated region (leader)
    GBFeatureKey_regulatory,      // any region of sequence that functions in the regulation of transcription or translation
    GBFeatureKey_Protein,         //
    GBFeatureKey_Region,          //
    GBFeatureKey_Site,            //
    GBFeatureKey_NUM_KEYS,
    GBFeatureKey_UNKNOWN = GBFeatureKey_NUM_KEYS
};

class U2CORE_EXPORT GBFeatureKeyInfo {
public:
    GBFeatureKeyInfo() : id (GBFeatureKey_UNKNOWN), showOnaminoFrame(false) {}
    GBFeatureKeyInfo(GBFeatureKey _id, U2FeatureType type, const QString& _text, const QColor& _color, bool _aminoFrame, QString _desc)
        : id (_id), type(type), text(_text), color(_color), showOnaminoFrame(_aminoFrame), desc(_desc) {}

    GBFeatureKey  id;
    U2FeatureType type;
    QString     text;
    QColor      color;
    bool        showOnaminoFrame;
    QString     desc;
    QStringList namingQuals;
};

class U2CORE_EXPORT GBFeatureUtils : public QObject {
    Q_OBJECT
public:
    static QMutex allKeys_mutex;
    static const QVector<GBFeatureKeyInfo>& allKeys();

    static QMutex getKeyGroups_mutex;
    static const GBFeatureKeyInfo& getKeyInfo(GBFeatureKey key) {return allKeys().at(key);}

    static QMutex getKey_mutex;
    static const QMultiMap<QString, GBFeatureKey>& getKeyGroups();

    static GBFeatureKey getKey(const QString& text);
    static GBFeatureKey getKey(U2FeatureType featureType);

    // Some features do not have values in GenBank (e.g. "/pseudo")
    static bool isFeatureHasNoValue(const QString& featureName);

    static const QByteArray QUALIFIER_AMINO_STRAND;
    static const QByteArray QUALIFIER_AMINO_STRAND_YES;
    static const QByteArray QUALIFIER_AMINO_STRAND_NO;

    static const QByteArray QUALIFIER_NAME;
    static const QByteArray QUALIFIER_GROUP;

    static const QString DEFAULT_KEY;

    static const QString QUALIFIER_CUT;
    static const QString QUALIFIER_NOTE;
    static const QString QUALIFIER_TRANSLATION;
};

}//namespace
#endif

