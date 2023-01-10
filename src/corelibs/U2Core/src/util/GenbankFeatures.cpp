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

#include "GenbankFeatures.h"

#include <QHash>
#include <QMutex>

#include <U2Core/U2SafePoints.h>

namespace U2 {

QMutex GBFeatureUtils::allKeys_mutex;
QMutex GBFeatureUtils::getKey_mutex;

const QByteArray GBFeatureUtils::QUALIFIER_AMINO_STRAND("ugene_amino_strand");

const QByteArray GBFeatureUtils::QUALIFIER_NAME("ugene_name");
const QByteArray GBFeatureUtils::QUALIFIER_GROUP("ugene_group");

const QString GBFeatureUtils::DEFAULT_KEY = GBFeatureUtils::getKeyInfo(GBFeatureKey_misc_feature).text;

const QString GBFeatureUtils::QUALIFIER_CUT = "cut";
const QString GBFeatureUtils::QUALIFIER_NOTE = "note";
const QString GBFeatureUtils::QUALIFIER_TRANSLATION = "translation";

#define FKE(key, type, text, quals) \
    features[key] = GBFeatureKeyInfo(key, type, text); \
    if (strlen(quals) > 0) { \
        features[key].namingQuals = QString(quals).split(",", QString::SkipEmptyParts); \
    }

#define FK(key, type, text) \
    FKE(key, type, text, "label")

const QVector<GBFeatureKeyInfo>& GBFeatureUtils::allKeys() {
    QMutexLocker locker(&allKeys_mutex);
    static QVector<GBFeatureKeyInfo> features(GBFeatureKey_NUM_KEYS);
    static bool inited = false;
    if (inited) {
        return features;
    }
    inited = true;
    FK(GBFeatureKey_assembly_gap, U2FeatureTypes::AssemblyGap, "assembly_gap");
    FK(GBFeatureKey_attenuator, U2FeatureTypes::Attenuator, "attenuator");
    FK(GBFeatureKey_bond, U2FeatureTypes::Disulfide, "Bond");
    FK(GBFeatureKey_C_region, U2FeatureTypes::CRegion, "C_region");
    FK(GBFeatureKey_CAAT_signal, U2FeatureTypes::CaatSignal, "CAAT_signal");
    FKE(GBFeatureKey_CDS, U2FeatureTypes::Cds, "CDS", "label,protein_id,locus_tag,gene,function,product");
    FK(GBFeatureKey_conflict, U2FeatureTypes::Conflict, "conflict");
    FK(GBFeatureKey_centromere, U2FeatureTypes::Centromere, "centromere");
    FK(GBFeatureKey_D_loop, U2FeatureTypes::DLoop, "D-loop");
    FK(GBFeatureKey_D_segment, U2FeatureTypes::DSegment, "D_segment");
    FK(GBFeatureKey_enhancer, U2FeatureTypes::Enhancer, "enhancer");
    FK(GBFeatureKey_exon, U2FeatureTypes::Exon, "exon");
    FK(GBFeatureKey_gap, U2FeatureTypes::Gap, "gap");
    FKE(GBFeatureKey_gene, U2FeatureTypes::Gene, "gene", "label,gene,locus_tag,product,function");
    FK(GBFeatureKey_GC_signal, U2FeatureTypes::GcSignal, "GC_signal");
    FK(GBFeatureKey_iDNA, U2FeatureTypes::IDna, "iDNA");
    FK(GBFeatureKey_intron, U2FeatureTypes::Intron, "intron");
    FK(GBFeatureKey_J_region, U2FeatureTypes::JRegion, "J_region");
    FK(GBFeatureKey_J_segment, U2FeatureTypes::JSegment, "J_segment");
    FK(GBFeatureKey_LTR, U2FeatureTypes::Ltr, "LTR");
    FK(GBFeatureKey_mat_peptide, U2FeatureTypes::MaturePeptide, "mat_peptide");
    FK(GBFeatureKey_misc_binding, U2FeatureTypes::MiscBindingSite, "misc_binding");
    FK(GBFeatureKey_misc_difference, U2FeatureTypes::MiscDifference, "misc_difference");
    FKE(GBFeatureKey_misc_feature, U2FeatureTypes::MiscFeature, "misc_feature", "label,note");
    FK(GBFeatureKey_misc_recomb, U2FeatureTypes::MiscRecombination, "misc_recomb");
    FK(GBFeatureKey_misc_RNA, U2FeatureTypes::MiscRna, "misc_RNA");
    FK(GBFeatureKey_misc_signal, U2FeatureTypes::MiscSignal, "misc_signal");
    FK(GBFeatureKey_misc_structure, U2FeatureTypes::MiscStructure, "misc_structure");
    FK(GBFeatureKey_mobile_element, U2FeatureTypes::MobileElement, "mobile_element");
    FK(GBFeatureKey_modified_base, U2FeatureTypes::ModifiedBase, "modified_base");
    FK(GBFeatureKey_mRNA, U2FeatureTypes::MRna, "mRNA");
    FK(GBFeatureKey_ncRNA, U2FeatureTypes::NcRna, "ncRNA")
    FK(GBFeatureKey_N_region, U2FeatureTypes::NRegion, "N_region");
    FK(GBFeatureKey_old_sequence, U2FeatureTypes::OldSequence, "old_sequence");
    FK(GBFeatureKey_operon, U2FeatureTypes::Operon, "operon");
    FK(GBFeatureKey_oriT, U2FeatureTypes::OriT, "oriT");
    FK(GBFeatureKey_polyA_signal, U2FeatureTypes::PolyASignal, "polyA_signal");
    FK(GBFeatureKey_polyA_site, U2FeatureTypes::PolyASite, "polyA_site");
    FK(GBFeatureKey_precursor_RNA, U2FeatureTypes::PrecursorRna, "precursor_RNA");
    FK(GBFeatureKey_prim_transcript, U2FeatureTypes::PrimaryTranscript, "prim_transcript");
    FK(GBFeatureKey_primer, U2FeatureTypes::Primer, "primer");
    FK(GBFeatureKey_primer_bind, U2FeatureTypes::PrimerBindingSite, "primer_bind");
    FK(GBFeatureKey_promoter, U2FeatureTypes::Promoter, "promoter");
    FK(GBFeatureKey_protein_bind, U2FeatureTypes::ProteinBindingSite, "protein_bind");
    FK(GBFeatureKey_RBS, U2FeatureTypes::Rbs, "RBS");
    FK(GBFeatureKey_rep_origin, U2FeatureTypes::ReplicationOrigin, "rep_origin");
    FK(GBFeatureKey_repeat_region, U2FeatureTypes::RepeatRegion, "repeat_region");
    FK(GBFeatureKey_repeat_unit, U2FeatureTypes::RepeatUnit, "repeat_unit");
    FK(GBFeatureKey_rRNA, U2FeatureTypes::RRna, "rRNA");
    FK(GBFeatureKey_S_region, U2FeatureTypes::SRegion, "S_region");
    FK(GBFeatureKey_satellite, U2FeatureTypes::Satellite, "satellite");
    FK(GBFeatureKey_scRNA, U2FeatureTypes::ScRna, "scRNA");
    FK(GBFeatureKey_sig_peptide, U2FeatureTypes::SignalPeptide, "sig_peptide");
    FK(GBFeatureKey_snRNA, U2FeatureTypes::SnRna, "snRNA");
    FK(GBFeatureKey_source, U2FeatureTypes::Source, "source");
    FK(GBFeatureKey_stem_loop, U2FeatureTypes::StemLoop, "stem_loop");
    FK(GBFeatureKey_STS, U2FeatureTypes::Sts, "STS");
    FK(GBFeatureKey_TATA_signal, U2FeatureTypes::TataSignal, "TATA_signal");
    FK(GBFeatureKey_telomere, U2FeatureTypes::Telomere, "telomere");
    FK(GBFeatureKey_terminator, U2FeatureTypes::Terminator, "terminator");
    FK(GBFeatureKey_tmRNA, U2FeatureTypes::TmRna, "tmRNA");
    FK(GBFeatureKey_transit_peptide, U2FeatureTypes::TransitPeptide, "transit_peptide");
    FK(GBFeatureKey_transposon, U2FeatureTypes::Transposon, "transposon");
    FK(GBFeatureKey_tRNA, U2FeatureTypes::TRna, "tRNA");
    FK(GBFeatureKey_unsure, U2FeatureTypes::Unsure, "unsure");
    FK(GBFeatureKey_V_region, U2FeatureTypes::VRegion, "V_region");
    FK(GBFeatureKey_V_segment, U2FeatureTypes::VSegment, "V_segment");
    FK(GBFeatureKey_variation, U2FeatureTypes::Variation, "variation");
    FK(GBFeatureKey__10_signal, U2FeatureTypes::Minus10Signal, "-10_signal");
    FK(GBFeatureKey__35_signal, U2FeatureTypes::Minus35Signal, "-35_signal");
    FK(GBFeatureKey_3_clip, U2FeatureTypes::ThreePrimeClip, "3'clip");
    FK(GBFeatureKey_3_UTR, U2FeatureTypes::ThreePrimeUtr, "3'UTR");
    FK(GBFeatureKey_5_clip, U2FeatureTypes::FivePrimeClip, "5'clip");
    FK(GBFeatureKey_5_UTR, U2FeatureTypes::FivePrimeUtr, "5'UTR");
    FK(GBFeatureKey_Protein, U2FeatureTypes::Protein, "Protein");
    FK(GBFeatureKey_Region, U2FeatureTypes::Region, "Region");
    FK(GBFeatureKey_Site, U2FeatureTypes::Site, "Site");
    FK(GBFeatureKey_regulatory, U2FeatureTypes::Regulatory, "regulatory");

    for (int i = 0; i < features.size(); i++) {
        SAFE_POINT(features[i].id != GBFeatureKey_UNKNOWN && features[i].id == i, "Invalid Genbank feature id", features);
        SAFE_POINT(!features[i].text.isEmpty(), "Invalid Genbank feature key", features);
    }

    return features;
}

bool GBFeatureUtils::isFeatureHasNoValue(const QString& featureName) {
    if (featureName == "pseudo") {
        return true;
    }
    return false;
}

GBFeatureKey GBFeatureUtils::getKey(const QString& text) {
    QMutexLocker locker(&getKey_mutex);
    static QHash<QString, GBFeatureKey> keysByText;
    if (keysByText.isEmpty()) {
        foreach (const GBFeatureKeyInfo& ki, allKeys()) {
            keysByText[ki.text] = ki.id;
        }
    }
    return keysByText.value(text, GBFeatureKey_UNKNOWN);
}

GBFeatureKey GBFeatureUtils::getKey(U2FeatureType featureType) {
    QMutexLocker locker(&getKey_mutex);
    static QHash<U2FeatureType, GBFeatureKey> keysByType;
    if (keysByType.isEmpty()) {
        foreach (const GBFeatureKeyInfo& ki, allKeys()) {
            keysByType[ki.type] = ki.id;
        }
    }
    return keysByType.value(featureType, GBFeatureKey_UNKNOWN);
}

}  // namespace U2
