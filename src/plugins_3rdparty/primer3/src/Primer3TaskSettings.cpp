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

#include <cstdlib>
#include <cstring>

#include <U2Core/U2SafePoints.h>

#include "Primer3TaskSettings.h"

#include "primer3_core/libprimer3.h"

namespace U2 {

bool Primer3TaskSettings::isIncludedRegionValid(const U2Region& r) const {
    if (getMinProductSize() > r.length) {
        return false;
    }
    return true;
}

void Primer3TaskSettings::setSequenceRange(const U2Region& region) {
    sequenceRange = region;
}

const U2Region& Primer3TaskSettings::getSequenceRange() const {
    return sequenceRange;
}

Primer3TaskSettings::Primer3TaskSettings() {
    primerSettings = p3_create_global_settings_default_version_1();
    seqArgs = create_seq_arg();
    isCircular = false;

    initMaps();
}

Primer3TaskSettings::~Primer3TaskSettings() {
    destroy_secundary_structures(primerSettings, p3Retval);
    if (p3Retval != nullptr) {
        destroy_p3retval(p3Retval);
    }
    destroy_seq_args(seqArgs);
    p3_destroy_global_settings(primerSettings);
}

bool Primer3TaskSettings::getIntProperty(const QString& key, int* outValue) const {
    if (!intProperties.contains(key)) {
        return false;
    }
    *outValue = *(intProperties.value(key));
    return true;
}

bool Primer3TaskSettings::getDoubleProperty(const QString& key, double* outValue) const {
    if (!doubleProperties.contains(key)) {
        return false;
    }
    *outValue = *(doubleProperties.value(key));
    return true;
}

bool Primer3TaskSettings::setIntProperty(const QString& key, int value) {
    if (!intProperties.contains(key)) {
        return false;
    }
    *(intProperties.value(key)) = value;
    return true;
}

bool Primer3TaskSettings::setDoubleProperty(const QString& key, double value) {
    if (!doubleProperties.contains(key)) {
        return false;
    }
    *(doubleProperties.value(key)) = value;
    return true;
}

QList<QString> Primer3TaskSettings::getIntPropertyList() const {
    return intProperties.keys();
}

QList<QString> Primer3TaskSettings::getDoublePropertyList() const {
    return doubleProperties.keys();
}

QByteArray Primer3TaskSettings::getSequenceName() const {
    return seqArgs->sequence_name;
}

QByteArray Primer3TaskSettings::getSequence() const {
    return seqArgs->sequence;
}

int Primer3TaskSettings::getSequenceSize() const {
    CHECK(seqArgs->sequence != nullptr, 0);
    
    return strlen(seqArgs->sequence);
}

QList<U2Region> Primer3TaskSettings::getTarget() const {
    QList<U2Region> result;
    for (int i = 0; i < seqArgs->tar2.count; i++) {
        result.append(U2Region(seqArgs->tar2.pairs[i][0], seqArgs->tar2.pairs[i][1]));
    }
    return result;
}

QList<int> Primer3TaskSettings::getOverlapJunctionList() const {
    QList<int> result;
    for (int i = 0; i < seqArgs->primer_overlap_junctions_count; i++) {
        result.append(seqArgs->primer_overlap_junctions[i]);
    }
    return result;
}

QList<int> Primer3TaskSettings::getInternalOverlapJunctionList() const {
    QList<int> result;
    for (int i = 0; i < seqArgs->intl_overlap_junctions_count; i++) {
        result.append(seqArgs->intl_overlap_junctions[i]);
    }
    return result;
}

QList<U2Region> Primer3TaskSettings::getProductSizeRange() const {
    QList<U2Region> result;
    for (int i = 0; i < primerSettings->num_intervals; i++) {
        result.append(U2Region(primerSettings->pr_min[i],
                               primerSettings->pr_max[i] - primerSettings->pr_min[i] + 1));
    }
    return result;
}

QList<QList<int>> Primer3TaskSettings::getOkRegion() const {
    QList<QList<int>> result;
    for (int i = 0; i < seqArgs->ok_regions.count; i++) {
        QList<int> v;
        v << seqArgs->ok_regions.left_pairs[i][0]
          << seqArgs->ok_regions.left_pairs[i][1]
          << seqArgs->ok_regions.right_pairs[i][0]
          << seqArgs->ok_regions.right_pairs[i][1];
        result.append(v);
    }
    return result;//
}

int Primer3TaskSettings::getMinProductSize() const {
    int min = INT_MAX;
    for (int i = 0; i < primerSettings->num_intervals; i++) {
        if (min > primerSettings->pr_min[i]) {
            min = primerSettings->pr_min[i];
        }
    }
    return min;
}

task Primer3TaskSettings::getTask() const {
    return primerSettings->primer_task;
}

QList<U2Region> Primer3TaskSettings::getInternalOligoExcludedRegion() const {
    QList<U2Region> result;
    for (int i = 0; i < seqArgs->excl_internal2.count; i++) {
        result.append(U2Region(seqArgs->excl_internal2.pairs[i][0], seqArgs->excl_internal2.pairs[i][1]));
    }
    return result;
}

QString Primer3TaskSettings::getStartCodonSequence() const {
    return seqArgs->start_codon_seq;
}

QString Primer3TaskSettings::getPrimerMustMatchFivePrime() const {
    return primerSettings->p_args.must_match_five_prime;
}

QString Primer3TaskSettings::getPrimerMustMatchThreePrime() const {
    return primerSettings->p_args.must_match_three_prime;
}

QString Primer3TaskSettings::getInternalPrimerMustMatchFivePrime() const {
    return primerSettings->o_args.must_match_five_prime;
}

QString Primer3TaskSettings::getInternalPrimerMustMatchThreePrime() const {
    return primerSettings->o_args.must_match_three_prime;
}

QByteArray Primer3TaskSettings::getLeftInput() const {
    return seqArgs->left_input;
}

QByteArray Primer3TaskSettings::getRightInput() const {
    return seqArgs->right_input;
}

QByteArray Primer3TaskSettings::getInternalInput() const {
    return seqArgs->internal_input;
}

QByteArray Primer3TaskSettings::getOverhangLeft() const {
    return seqArgs->overhang_left;
}

QByteArray Primer3TaskSettings::getOverhangRight() const {
    return seqArgs->overhang_right;
}

QList<U2Region> Primer3TaskSettings::getExcludedRegion() const {
    QList<U2Region> result;
    for (int i = 0; i < seqArgs->excl2.count; i++) {
        result.append(U2Region(seqArgs->excl2.pairs[i][0], seqArgs->excl2.pairs[i][1]));
    }
    return result;
}

U2Region Primer3TaskSettings::getIncludedRegion() const {
    return U2Region(seqArgs->incl_s, seqArgs->incl_l);
}

QVector<int> Primer3TaskSettings::getSequenceQuality() const {
    CHECK(seqArgs->n_quality != 0, {});

    QVector<int> result;
    result.reserve(seqArgs->n_quality);
    std::copy(seqArgs->quality, seqArgs->quality + seqArgs->n_quality, std::back_inserter(result));

    return result;
}

int Primer3TaskSettings::getFirstBaseIndex() const {
    return primerSettings->first_base_index;
}

bool Primer3TaskSettings::isShowDebugging() const {
    return showDebugging;
}

bool Primer3TaskSettings::isFormatOutput() const {
    return formatOutput;
}

bool Primer3TaskSettings::isExplain() const {
    return explain;
}

void Primer3TaskSettings::setSequenceName(const QByteArray& value) {
    p3_set_sa_sequence_name(seqArgs, value);
}

void Primer3TaskSettings::setSequence(const QByteArray& value, bool isCirc) {
    CHECK(!value.isEmpty(), );

    isCircular = isCirc;
    p3_set_sa_sequence(seqArgs, value);
}

void Primer3TaskSettings::setCircularity(bool isCirc) {
    isCircular = isCirc;
}

void Primer3TaskSettings::setTarget(const QList<U2Region>& value) {
    seqArgs->tar2.count = 0;
    for (int i = 0; i < value.size(); i++) {
        p3_add_to_sa_tar2(seqArgs, value[i].startPos, value[i].length);
    }
}

void Primer3TaskSettings::setOverlapJunctionList(const QList<int>& value) {
    for (int v : value) {
        p3_sa_add_to_overlap_junctions_array(seqArgs, v);
    }
}

void Primer3TaskSettings::setInternalOverlapJunctionList(const QList<int>& value) {
    for (int v : value) {
        p3_sa_add_to_intl_overlap_junctions_array(seqArgs, v);
    }
}

void Primer3TaskSettings::setProductSizeRange(const QList<U2Region>& value) {
    p3_empty_gs_product_size_range(primerSettings);
    for (int i = 0; i < value.size(); i++) {
        p3_add_to_gs_product_size_range(primerSettings, value[i].startPos, value[i].endPos() - 1);
    }
}

void Primer3TaskSettings::setTaskByName(const QString& taskName) {
    p3_set_gs_primer_task(primerSettings, taskName.toLocal8Bit().data());
}

void Primer3TaskSettings::setInternalOligoExcludedRegion(const QList<U2Region>& value) {
    seqArgs->excl_internal2.count = 0;
    for (int i = 0; i < value.size(); i++) {
        p3_add_to_sa_excl_internal2(seqArgs, value[i].startPos, value[i].length);
    }
}

void Primer3TaskSettings::setStartCodonSequence(const QByteArray& value) const {
    p3_sa_set_start_codon_sequence(seqArgs, value);
}

void Primer3TaskSettings::setPrimerMustMatchFivePrime(const QByteArray& value) const {
    p3_set_sa_p_args_must_match_five_prime(primerSettings, value);
}

void Primer3TaskSettings::setPrimerMustMatchThreePrime(const QByteArray& value) const {
    p3_set_sa_p_args_must_match_three_prime(primerSettings, value);
}

void Primer3TaskSettings::setInternalPrimerMustMatchFivePrime(const QByteArray& value) const {
    p3_set_sa_o_args_must_match_five_prime(primerSettings, value);
}

void Primer3TaskSettings::setInternalPrimerMustMatchThreePrime(const QByteArray& value) const {
    p3_set_sa_o_args_must_match_three_prime(primerSettings, value);
}

void Primer3TaskSettings::setLeftInput(const QByteArray& value) {
    p3_set_sa_left_input(seqArgs, value);
}

void Primer3TaskSettings::setLeftOverhang(const QByteArray& value) {
    p3_set_sa_overhang_left(seqArgs, value);
}

void Primer3TaskSettings::setRightInput(const QByteArray& value) {
    p3_set_sa_right_input(seqArgs, value);
}

void Primer3TaskSettings::setRightOverhang(const QByteArray& value) {
    p3_set_sa_overhang_right(seqArgs, value);
}

void Primer3TaskSettings::setInternalInput(const QByteArray& value) {
    p3_set_sa_internal_input(seqArgs, value);
}

void Primer3TaskSettings::setExcludedRegion(const QList<U2Region>& value) {
    seqArgs->excl2.count = 0;
    for (int i = 0; i < value.size(); i++) {
        p3_add_to_sa_excl2(seqArgs, value[i].startPos, value[i].length);
    }
}

void Primer3TaskSettings::setOkRegion(const QList<QList<int>>& value) {
    seqArgs->ok_regions.count = 0;
    for (const QList<int>& v : value) {
        p3_add_to_sa_ok_regions(seqArgs, v.value(0), v.value(1), v.value(2), v.value(3));
    }
}

void Primer3TaskSettings::setIncludedRegion(const U2Region& value) {
    seqArgs->incl_s = static_cast<int>(value.startPos);
    seqArgs->incl_l = static_cast<int>(value.length);
}

void Primer3TaskSettings::setIncludedRegion(const qint64& startPos, const qint64& length) {
    seqArgs->incl_s = static_cast<int>(startPos);
    seqArgs->incl_l = static_cast<int>(length);
}

void Primer3TaskSettings::setSequenceQuality(const QVector<int>& value) {
    p3_set_sa_empty_quality(seqArgs);
    seqArgs->quality_storage_size = 0;
    free(seqArgs->quality);
    for (int qual : value) {
        p3_sa_add_to_quality_array(seqArgs, qual);
    }
}

void Primer3TaskSettings::setRepeatLibraryPath(const QByteArray& value) {
    repeatLibraryPath = value;
}

void Primer3TaskSettings::setMishybLibraryPath(const QByteArray& value) {
    mishybLibraryPath = value;
}

void Primer3TaskSettings::setThermodynamicParametersPath(const QByteArray& value) {
    thermodynamicParametersPath = value;
}

void Primer3TaskSettings::setShowDebugging(bool value) {
    showDebugging = value;
}

void Primer3TaskSettings::setFormatOutput(bool value) {
    formatOutput = value;
}

void Primer3TaskSettings::setExplain(bool value) {
    explain = value;
}

const QByteArray& Primer3TaskSettings::getRepeatLibraryPath() const {
    return repeatLibraryPath;
}

const QByteArray& Primer3TaskSettings::getMishybLibraryPath() const {
    return mishybLibraryPath;
}

const QByteArray& Primer3TaskSettings::getThermodynamicParametersPath() const {
    return thermodynamicParametersPath;
}

p3_global_settings* Primer3TaskSettings::getPrimerSettings() const {
    return primerSettings;
}

seq_args* Primer3TaskSettings::getSeqArgs() const {
    return seqArgs;
}

void Primer3TaskSettings::setP3RetVal(p3retval* ret) {
    SAFE_POINT(p3Retval == nullptr, "retvalue already exists", );

    p3Retval = ret;
}

p3retval* Primer3TaskSettings::getP3RetVal() const {
    return p3Retval;
}

// span intron/exon boundary settings

const SpanIntronExonBoundarySettings& Primer3TaskSettings::getSpanIntronExonBoundarySettings() const {
    return spanIntronExonBoundarySettings;
}

void Primer3TaskSettings::setSpanIntronExonBoundarySettings(const SpanIntronExonBoundarySettings& settings) {
    spanIntronExonBoundarySettings = settings;
}

const QList<U2Region>& Primer3TaskSettings::getExonRegions() const {
    return spanIntronExonBoundarySettings.regionList;
}

void Primer3TaskSettings::setExonRegions(const QList<U2Region>& regions) {
    spanIntronExonBoundarySettings.regionList = regions;
}

bool Primer3TaskSettings::spanIntronExonBoundaryIsEnabled() const {
    return spanIntronExonBoundarySettings.enabled;
}

bool Primer3TaskSettings::isSequenceCircular() const {
    return isCircular;
}

void Primer3TaskSettings::initMaps() {
    intProperties.insert("SEQUENCE_START_CODON_POSITION", &seqArgs->start_codon_pos);
    intProperties.insert("SEQUENCE_FORCE_LEFT_START", &seqArgs->force_left_start);
    intProperties.insert("SEQUENCE_FORCE_RIGHT_START", &seqArgs->force_right_start);
    intProperties.insert("SEQUENCE_FORCE_LEFT_END", &seqArgs->force_left_end);
    intProperties.insert("SEQUENCE_FORCE_RIGHT_END", &seqArgs->force_right_end);
    intProperties.insert("PRIMER_NUM_RETURN", &primerSettings->num_return);
    intProperties.insert("PRIMER_MIN_SIZE", &primerSettings->p_args.min_size);
    intProperties.insert("PRIMER_OPT_SIZE", &primerSettings->p_args.opt_size);
    intProperties.insert("PRIMER_MAX_SIZE", &primerSettings->p_args.max_size);
    intProperties.insert("PRIMER_TM_FORMULA", (int*)&primerSettings->tm_santalucia);
    intProperties.insert("PRIMER_SALT_CORRECTIONS", (int*)&primerSettings->salt_corrections);
    intProperties.insert("PRIMER_MAX_NS_ACCEPTED", &primerSettings->p_args.num_ns_accepted);
    intProperties.insert("PRIMER_MAX_POLY_X", &primerSettings->p_args.max_poly_x);
    intProperties.insert("PRIMER_FIRST_BASE_INDEX", &primerSettings->first_base_index);
    intProperties.insert("PRIMER_MAX_END_GC", &primerSettings->max_end_gc);
    intProperties.insert("PRIMER_GC_CLAMP", &primerSettings->gc_clamp);
    intProperties.insert("PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE", &primerSettings->min_left_three_prime_distance);
    intProperties.insert("PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE", &primerSettings->min_right_three_prime_distance);
    intProperties.insert("PRIMER_MIN_5_PRIME_OVERLAP_OF_JUNCTION", &primerSettings->p_args.min_5_prime_overlap_of_junction);
    intProperties.insert("PRIMER_MIN_3_PRIME_OVERLAP_OF_JUNCTION", &primerSettings->p_args.min_3_prime_overlap_of_junction);
    intProperties.insert("PRIMER_SEQUENCING_SPACING", &primerSettings->sequencing.spacing);
    intProperties.insert("PRIMER_SEQUENCING_LEAD", &primerSettings->sequencing.lead);
    intProperties.insert("PRIMER_SEQUENCING_INTERVAL", &primerSettings->sequencing.interval);
    intProperties.insert("PRIMER_SEQUENCING_ACCURACY", &primerSettings->sequencing.accuracy);
    intProperties.insert("PRIMER_INTERNAL_MIN_SIZE", &primerSettings->o_args.min_size);
    intProperties.insert("PRIMER_INTERNAL_OPT_SIZE", &primerSettings->o_args.opt_size);
    intProperties.insert("PRIMER_INTERNAL_MAX_SIZE", &primerSettings->o_args.max_size);
    intProperties.insert("PRIMER_INTERNAL_MIN_QUALITY", &primerSettings->o_args.min_quality);
    intProperties.insert("PRIMER_INTERNAL_MAX_NS_ACCEPTED", &primerSettings->o_args.num_ns_accepted);
    intProperties.insert("PRIMER_INTERNAL_MAX_POLY_X", &primerSettings->o_args.max_poly_x);
    intProperties.insert("PRIMER_MIN_QUALITY", &primerSettings->p_args.min_quality);
    intProperties.insert("PRIMER_MIN_END_QUALITY", &primerSettings->p_args.min_end_quality);
    intProperties.insert("PRIMER_QUALITY_RANGE_MIN", &primerSettings->quality_range_min);
    intProperties.insert("PRIMER_QUALITY_RANGE_MAX", &primerSettings->quality_range_max);
    intProperties.insert("PRIMER_INTERNAL_MIN_3_PRIME_OVERLAP_OF_JUNCTION", &primerSettings->o_args.min_3_prime_overlap_of_junction);
    intProperties.insert("PRIMER_INTERNAL_MIN_5_PRIME_OVERLAP_OF_JUNCTION", &primerSettings->o_args.min_5_prime_overlap_of_junction);
    intProperties.insert("PRIMER_INTERNAL_MIN_THREE_PRIME_DISTANCE", &primerSettings->min_internal_three_prime_distance);
    intProperties.insert("PRIMER_PRODUCT_OPT_SIZE", &primerSettings->product_opt_size);
    //boolean
    intProperties.insert("PRIMER_PICK_LEFT_PRIMER", &primerSettings->pick_left_primer);
    intProperties.insert("PRIMER_PICK_RIGHT_PRIMER", &primerSettings->pick_right_primer);
    intProperties.insert("PRIMER_PICK_INTERNAL_OLIGO", &primerSettings->pick_internal_oligo);
    intProperties.insert("PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", &primerSettings->thermodynamic_oligo_alignment);
    intProperties.insert("PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT", &primerSettings->thermodynamic_template_alignment);
    intProperties.insert("PRIMER_LIBERAL_BASE", &primerSettings->liberal_base);
    intProperties.insert("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", &primerSettings->lib_ambiguity_codes_consensus);
    intProperties.insert("PRIMER_LOWERCASE_MASKING", &primerSettings->lowercase_masking);
    intProperties.insert("PRIMER_PICK_ANYWAY", &primerSettings->pick_anyway);
    intProperties.insert("PRIMER_SECONDARY_STRUCTURE_ALIGNMENT", &primerSettings->show_secondary_structure_alignment);

    doubleProperties.insert("PRIMER_MAX_END_STABILITY", &primerSettings->max_end_stability);
    doubleProperties.insert("PRIMER_MAX_LIBRARY_MISPRIMING", &primerSettings->p_args.max_repeat_compl);
    doubleProperties.insert("PRIMER_PAIR_MAX_LIBRARY_MISPRIMING", &primerSettings->pair_repeat_compl);
    doubleProperties.insert("PRIMER_MIN_TM", &primerSettings->p_args.min_tm);
    doubleProperties.insert("PRIMER_OPT_TM", &primerSettings->p_args.opt_tm);
    doubleProperties.insert("PRIMER_MAX_TM", &primerSettings->p_args.max_tm);
    doubleProperties.insert("PRIMER_PRODUCT_MIN_TM", &primerSettings->product_min_tm);
    doubleProperties.insert("PRIMER_PRODUCT_OPT_TM", &primerSettings->product_opt_tm);
    doubleProperties.insert("PRIMER_PRODUCT_MAX_TM", &primerSettings->product_max_tm);
    doubleProperties.insert("PRIMER_MIN_GC", &primerSettings->p_args.min_gc);
    doubleProperties.insert("PRIMER_OPT_GC_PERCENT", &primerSettings->p_args.opt_gc_content);
    doubleProperties.insert("PRIMER_MAX_GC", &primerSettings->p_args.max_gc);
    doubleProperties.insert("PRIMER_MAX_SELF_ANY_TH", &primerSettings->p_args.max_self_any_th);
    doubleProperties.insert("PRIMER_MAX_SELF_END_TH", &primerSettings->p_args.max_self_end_th);
    doubleProperties.insert("PRIMER_PAIR_MAX_COMPL_ANY_TH", &primerSettings->pair_compl_any_th);
    doubleProperties.insert("PRIMER_PAIR_MAX_COMPL_END_TH", &primerSettings->pair_compl_end_th);
    doubleProperties.insert("PRIMER_MAX_HAIRPIN_TH", &primerSettings->p_args.max_hairpin_th);
    doubleProperties.insert("PRIMER_MAX_SELF_ANY", &primerSettings->p_args.max_self_any);
    doubleProperties.insert("PRIMER_MAX_SELF_END", &primerSettings->p_args.max_self_end);
    doubleProperties.insert("PRIMER_PAIR_MAX_COMPL_ANY", &primerSettings->pair_compl_any);
    doubleProperties.insert("PRIMER_PAIR_MAX_COMPL_END", &primerSettings->pair_compl_end);
    doubleProperties.insert("PRIMER_MAX_TEMPLATE_MISPRIMING_TH", &primerSettings->p_args.max_template_mispriming_th);
    doubleProperties.insert("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING_TH", &primerSettings->pair_max_template_mispriming_th);
    doubleProperties.insert("PRIMER_MAX_TEMPLATE_MISPRIMING", &primerSettings->p_args.max_template_mispriming);
    doubleProperties.insert("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING", &primerSettings->pair_max_template_mispriming);
    doubleProperties.insert("PRIMER_INSIDE_PENALTY", &primerSettings->inside_penalty);
    doubleProperties.insert("PRIMER_OUTSIDE_PENALTY", &primerSettings->outside_penalty);
    doubleProperties.insert("PRIMER_SALT_MONOVALENT", &primerSettings->p_args.salt_conc);
    doubleProperties.insert("PRIMER_SALT_DIVALENT", &primerSettings->p_args.divalent_conc);
    doubleProperties.insert("PRIMER_DNTP_CONC", &primerSettings->p_args.dntp_conc);
    doubleProperties.insert("PRIMER_DNA_CONC", &primerSettings->p_args.dna_conc);
    doubleProperties.insert("PRIMER_PAIR_MAX_DIFF_TM", &primerSettings->max_diff_tm);
    doubleProperties.insert("PRIMER_INTERNAL_MIN_TM", &primerSettings->o_args.min_tm);
    doubleProperties.insert("PRIMER_INTERNAL_OPT_TM", &primerSettings->o_args.opt_tm);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_TM", &primerSettings->o_args.max_tm);
    doubleProperties.insert("PRIMER_INTERNAL_MIN_GC", &primerSettings->o_args.min_gc);
    doubleProperties.insert("PRIMER_INTERNAL_OPT_GC_PERCENT", &primerSettings->o_args.opt_gc_content);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_GC", &primerSettings->o_args.max_gc);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_SELF_ANY_TH", &primerSettings->o_args.max_self_any_th);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_SELF_END_TH", &primerSettings->o_args.max_self_end_th);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_HAIRPIN_TH", &primerSettings->o_args.max_hairpin_th);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_SELF_ANY", &primerSettings->o_args.max_self_any);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_SELF_END", &primerSettings->o_args.max_self_end);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_LIBRARY_MISHYB", &primerSettings->o_args.max_repeat_compl);
    doubleProperties.insert("PRIMER_INTERNAL_SALT_MONOVALENT", &primerSettings->o_args.salt_conc);
    doubleProperties.insert("PRIMER_INTERNAL_DNA_CONC", &primerSettings->o_args.dna_conc);
    doubleProperties.insert("PRIMER_INTERNAL_SALT_DIVALENT", &primerSettings->o_args.divalent_conc);
    doubleProperties.insert("PRIMER_INTERNAL_DNTP_CONC", &primerSettings->o_args.dntp_conc);
    doubleProperties.insert("PRIMER_WT_TM_LT", &primerSettings->p_args.weights.temp_lt);
    doubleProperties.insert("PRIMER_WT_TM_GT", &primerSettings->p_args.weights.temp_gt);
    doubleProperties.insert("PRIMER_WT_SIZE_LT", &primerSettings->p_args.weights.length_lt);
    doubleProperties.insert("PRIMER_WT_SIZE_GT", &primerSettings->p_args.weights.length_gt);
    doubleProperties.insert("PRIMER_WT_GC_PERCENT_LT", &primerSettings->p_args.weights.gc_content_lt);
    doubleProperties.insert("PRIMER_WT_GC_PERCENT_GT", &primerSettings->p_args.weights.gc_content_gt);
    doubleProperties.insert("PRIMER_WT_SELF_ANY_TH", &primerSettings->p_args.weights.compl_any_th);
    doubleProperties.insert("PRIMER_WT_SELF_END_TH", &primerSettings->p_args.weights.compl_end_th);
    doubleProperties.insert("PRIMER_WT_HAIRPIN_TH", &primerSettings->p_args.weights.hairpin_th);
    doubleProperties.insert("PRIMER_WT_TEMPLATE_MISPRIMING_TH", &primerSettings->p_args.weights.template_mispriming_th);
    doubleProperties.insert("PRIMER_WT_SELF_ANY", &primerSettings->p_args.weights.compl_any);
    doubleProperties.insert("PRIMER_WT_SELF_END", &primerSettings->p_args.weights.compl_end);
    doubleProperties.insert("PRIMER_WT_TEMPLATE_MISPRIMING", &primerSettings->p_args.weights.template_mispriming);
    doubleProperties.insert("PRIMER_WT_NUM_NS", &primerSettings->p_args.weights.num_ns);
    doubleProperties.insert("PRIMER_WT_LIBRARY_MISPRIMING", &primerSettings->p_args.weights.repeat_sim);
    doubleProperties.insert("PRIMER_WT_SEQ_QUAL", &primerSettings->p_args.weights.seq_quality);
    doubleProperties.insert("PRIMER_WT_END_QUAL", &primerSettings->p_args.weights.end_quality);
    doubleProperties.insert("PRIMER_WT_POS_PENALTY", &primerSettings->p_args.weights.pos_penalty);
    doubleProperties.insert("PRIMER_WT_END_STABILITY", &primerSettings->p_args.weights.end_stability);
    doubleProperties.insert("PRIMER_WT_MASK_FAILURE_RATE", &primerSettings->p_args.weights.failure_rate);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_SIZE_LT", &primerSettings->pr_pair_weights.product_size_lt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_SIZE_GT", &primerSettings->pr_pair_weights.product_size_gt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_TM_LT", &primerSettings->pr_pair_weights.product_tm_lt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_TM_GT", &primerSettings->pr_pair_weights.product_tm_gt);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_ANY_TH", &primerSettings->pr_pair_weights.compl_any_th);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_END_TH", &primerSettings->pr_pair_weights.compl_end_th);
    doubleProperties.insert("PRIMER_PAIR_WT_TEMPLATE_MISPRIMING_TH", &primerSettings->pr_pair_weights.template_mispriming_th);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_ANY", &primerSettings->pr_pair_weights.compl_any);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_END", &primerSettings->pr_pair_weights.compl_end);
    doubleProperties.insert("PRIMER_PAIR_WT_TEMPLATE_MISPRIMING", &primerSettings->pr_pair_weights.template_mispriming);
    doubleProperties.insert("PRIMER_PAIR_WT_DIFF_TM", &primerSettings->pr_pair_weights.diff_tm);
    doubleProperties.insert("PRIMER_PAIR_WT_LIBRARY_MISPRIMING", &primerSettings->pr_pair_weights.repeat_sim);
    doubleProperties.insert("PRIMER_PAIR_WT_PR_PENALTY", &primerSettings->pr_pair_weights.primer_quality);
    doubleProperties.insert("PRIMER_PAIR_WT_IO_PENALTY", &primerSettings->pr_pair_weights.io_quality);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SIZE_LT", &primerSettings->o_args.weights.length_lt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SIZE_GT", &primerSettings->o_args.weights.length_gt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_TM_LT", &primerSettings->o_args.weights.temp_lt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_TM_GT", &primerSettings->o_args.weights.temp_gt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_GC_PERCENT_LT", &primerSettings->o_args.weights.gc_content_lt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_GC_PERCENT_GT", &primerSettings->o_args.weights.gc_content_gt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SELF_ANY_TH", &primerSettings->o_args.weights.compl_any_th);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SELF_END_TH", &primerSettings->o_args.weights.compl_end_th);
    doubleProperties.insert("PRIMER_INTERNAL_WT_HAIRPIN_TH", &primerSettings->o_args.weights.hairpin_th);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SELF_ANY", &primerSettings->o_args.weights.compl_any);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SELF_END", &primerSettings->o_args.weights.compl_end);
    doubleProperties.insert("PRIMER_INTERNAL_WT_NUM_NS", &primerSettings->o_args.weights.num_ns);
    doubleProperties.insert("PRIMER_INTERNAL_WT_LIBRARY_MISHYB", &primerSettings->o_args.weights.repeat_sim);
    doubleProperties.insert("PRIMER_INTERNAL_WT_SEQ_QUAL", &primerSettings->o_args.weights.seq_quality);
    doubleProperties.insert("PRIMER_INTERNAL_WT_END_QUAL", &primerSettings->o_args.weights.end_quality);
    doubleProperties.insert("PRIMER_ANNEALING_TEMP", &primerSettings->annealing_temp);
    doubleProperties.insert("PRIMER_DMSO_CONC", &primerSettings->p_args.dmso_conc);
    doubleProperties.insert("PRIMER_DMSO_FACTOR", &primerSettings->p_args.dmso_fact);
    doubleProperties.insert("PRIMER_FORMAMIDE_CONC", &primerSettings->p_args.formamide_conc);
    doubleProperties.insert("PRIMER_INTERNAL_DMSO_CONC", &primerSettings->o_args.dmso_conc);
    doubleProperties.insert("PRIMER_INTERNAL_DMSO_FACTOR", &primerSettings->o_args.dmso_fact);
    doubleProperties.insert("PRIMER_INTERNAL_FORMAMIDE_CONC", &primerSettings->o_args.formamide_conc);
    doubleProperties.insert("PRIMER_INTERNAL_MIN_BOUND", &primerSettings->o_args.min_bound);
    doubleProperties.insert("PRIMER_INTERNAL_OPT_BOUND", &primerSettings->o_args.opt_bound);
    doubleProperties.insert("PRIMER_INTERNAL_MAX_BOUND", &primerSettings->o_args.max_bound);
    doubleProperties.insert("PRIMER_MIN_BOUND", &primerSettings->p_args.min_bound);
    doubleProperties.insert("PRIMER_OPT_BOUND", &primerSettings->p_args.opt_bound);
    doubleProperties.insert("PRIMER_MAX_BOUND", &primerSettings->p_args.max_bound);
    doubleProperties.insert("PRIMER_INTERNAL_WT_BOUND_LT", &primerSettings->o_args.weights.bound_lt);
    doubleProperties.insert("PRIMER_INTERNAL_WT_BOUND_GT", &primerSettings->o_args.weights.bound_gt);
    doubleProperties.insert("PRIMER_WT_BOUND_LT", &primerSettings->p_args.weights.bound_lt);
    doubleProperties.insert("PRIMER_WT_BOUND_GT", &primerSettings->p_args.weights.bound_gt);

}

}  // namespace U2
