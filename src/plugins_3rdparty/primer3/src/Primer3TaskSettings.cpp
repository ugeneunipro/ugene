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

#include <cstring>
#include <cstdlib>
#include "primer3_core/boulder_input.h"
#include "Primer3TaskSettings.h"

namespace U2 {

bool Primer3TaskSettings::checkIncludedRegion(const U2Region &r) const {
    int minProductSize = getMinProductSize();
    if (minProductSize > r.length && getTask() != pick_hyb_probe_only
            && getTask() != pick_left_only
            && getTask() != pick_right_only) {
        return false;
    }
    return true;
}

Primer3TaskSettings::Primer3TaskSettings()
{
    pr_set_default_global_args(&primerArgs);

    std::memset(&primerArgs.glob_err,0,sizeof(primerArgs.glob_err));
    std::memset(&seqArgs,0,sizeof(seqArgs));
    seqArgs.start_codon_pos = PR_DEFAULT_START_CODON_POS;
    isCircular = false;

    initMaps();
}

Primer3TaskSettings::Primer3TaskSettings(const Primer3TaskSettings &settings):
    sequenceName(settings.sequenceName),
    sequence(settings.sequence),
    isCircular(settings.isCircular),
    leftInput(settings.leftInput),
    rightInput(settings.rightInput),
    internalInput(settings.internalInput),
    sequenceQuality(settings.sequenceQuality),
    repeatLibrary(settings.repeatLibrary),
    mishybLibrary(settings.mishybLibrary),
    primerArgs(settings.primerArgs),
    seqArgs(settings.seqArgs),
    spanIntronExonBoundarySettings(settings.spanIntronExonBoundarySettings)
{
    initMaps();
}

Primer3TaskSettings &Primer3TaskSettings::operator=(const Primer3TaskSettings &settings)
{
    sequenceName = settings.sequenceName;
    sequence = settings.sequence;
    isCircular = settings.isCircular;
    leftInput = settings.leftInput;
    rightInput = settings.rightInput;
    internalInput = settings.internalInput;
    sequenceQuality = settings.sequenceQuality;
    primerArgs = settings.primerArgs;
    repeatLibrary = settings.repeatLibrary;
    mishybLibrary = settings.mishybLibrary;
    seqArgs = settings.seqArgs;
    spanIntronExonBoundarySettings = settings.spanIntronExonBoundarySettings;
    initMaps();
    return *this;
}

Primer3TaskSettings::~Primer3TaskSettings()
{
    if(NULL != seqArgs.error.data)
    {
        std::free(seqArgs.error.data);
        seqArgs.error.data = NULL;
    }
    if(NULL != primerArgs.glob_err.data)
    {
        std::free(primerArgs.glob_err.data);
        primerArgs.glob_err.data = NULL;
    }
    if(NULL != seqArgs.trimmed_seq)
    {
        std::free(seqArgs.trimmed_seq);
        seqArgs.trimmed_seq = NULL;
    }
    if(NULL != seqArgs.trimmed_orig_seq)
    {
        std::free(seqArgs.trimmed_orig_seq);
        seqArgs.trimmed_orig_seq = NULL;
    }

    if(NULL != seqArgs.upcased_seq)
    {
        std::free(seqArgs.upcased_seq);
        seqArgs.upcased_seq = NULL;
    }
    if(NULL != seqArgs.upcased_seq_r)
    {
        std::free(seqArgs.upcased_seq_r);
        seqArgs.upcased_seq_r = NULL;
    }
    if(NULL != seqArgs.error.data)
    {
        std::free(seqArgs.error.data);
        seqArgs.error.data = NULL;
    }
    if(NULL != seqArgs.warning.data)
    {
        std::free(seqArgs.warning.data);
        seqArgs.warning.data = NULL;
    }

    free_seq_lib(&primerArgs.repeat_lib);
    free_seq_lib(&primerArgs.io_mishyb_library);
}

bool Primer3TaskSettings::getIntProperty(const QString &key, int *outValue)const
{
    if(!intProperties.contains(key))
    {
        return false;
    }
    *outValue = *(intProperties.value(key));
    return true;
}

bool Primer3TaskSettings::getDoubleProperty(const QString &key, double *outValue)const
{
    if(!doubleProperties.contains(key))
    {
        return false;
    }
    *outValue = *(doubleProperties.value(key));
    return true;
}

bool Primer3TaskSettings::getAlignProperty(const QString &key, short *outValue)const
{
    if(!alignProperties.contains(key))
    {
        return false;
    }
    *outValue = *(alignProperties.value(key));
    return true;
}

bool Primer3TaskSettings::setIntProperty(const QString &key,int value)
{
    if(!intProperties.contains(key))
    {
        return false;
    }
    *(intProperties.value(key)) = value;
    return true;
}

bool Primer3TaskSettings::setDoubleProperty(const QString &key,double value)
{
    if(!doubleProperties.contains(key))
    {
        return false;
    }
    *(doubleProperties.value(key)) = value;
    return true;
}
bool Primer3TaskSettings::setAlignProperty(const QString &key,short value)
{
    if(!alignProperties.contains(key))
    {
        return false;
    }
    *(alignProperties.value(key)) = value;
    return true;
}

QList<QString> Primer3TaskSettings::getIntPropertyList()const
{
    return intProperties.keys();
}

QList<QString> Primer3TaskSettings::getDoublePropertyList()const
{
    return doubleProperties.keys();
}

QList<QString> Primer3TaskSettings::getAlignPropertyList()const
{
    return alignProperties.keys();
}

QByteArray Primer3TaskSettings::getSequenceName()const
{
    return sequenceName;
}

QByteArray Primer3TaskSettings::getSequence()const
{
    return sequence;
}

int Primer3TaskSettings::getSequenceSize() const
{
    return sequence.size();
}

QList< U2Region > Primer3TaskSettings::getTarget()const
{
    QList< U2Region > result;
    for(int i=0;i < seqArgs.num_targets;i++)
    {
        result.append(U2Region(seqArgs.tar[i][0], seqArgs.tar[i][1]));
    }
    return result;
}

QList< U2Region > Primer3TaskSettings::getProductSizeRange()const
{
    QList< U2Region > result;
    for(int i=0;i < primerArgs.num_intervals;i++)
    {
        result.append(U2Region(primerArgs.pr_min[i],
                               primerArgs.pr_max[i] - primerArgs.pr_min[i] + 1));
    }
    return result;
}

int Primer3TaskSettings::getMinProductSize() const {
    int min = INT_MAX;
    for(int i=0;i < primerArgs.num_intervals;i++)
    {
        if (min > primerArgs.pr_min[i]) {
            min = primerArgs.pr_min[i];
        }
    }
    return min;
}

task Primer3TaskSettings::getTask()const
{
    return primerArgs.primer_task;
}

QList< U2Region > Primer3TaskSettings::getInternalOligoExcludedRegion()const
{
    QList< U2Region> result;
    for(int i=0;i < seqArgs.num_internal_excl;i++)
    {
        result.append(U2Region(seqArgs.excl_internal[i][0], seqArgs.excl_internal[i][1]));
    }
    return result;
}

QByteArray Primer3TaskSettings::getLeftInput()const
{
    return leftInput;
}

QByteArray Primer3TaskSettings::getRightInput()const
{
    return rightInput;
}

QByteArray Primer3TaskSettings::getInternalInput()const
{
    return internalInput;
}

QList<U2Region> Primer3TaskSettings::getExcludedRegion()const
{
    QList< U2Region > result;
    for(int i=0;i < seqArgs.num_excl;i++)
    {
        result.append(U2Region(seqArgs.excl[i][0], seqArgs.excl[i][1]));
    }
    return result;
}

U2Region Primer3TaskSettings::getIncludedRegion()const
{
    return U2Region(seqArgs.incl_s, seqArgs.incl_l);
}

QVector<int> Primer3TaskSettings::getSequenceQuality()const
{
    return sequenceQuality;
}

QByteArray Primer3TaskSettings::getError()const
{
    return QByteArray(seqArgs.error.data);
}

int Primer3TaskSettings::getFirstBaseIndex()const
{
    return primerArgs.first_base_index;
}

void Primer3TaskSettings::setSequenceName(const QByteArray &value)
{
    sequenceName = value;
    if(!value.isEmpty())
    {
        seqArgs.sequence_name = sequenceName.constData();
    }
    else
    {
        seqArgs.sequence_name = NULL;
    }
}

void Primer3TaskSettings::setSequence(const QByteArray &value, bool isCirc)
{
    sequence = value;
    isCircular = isCirc;
    seqArgs.sequence = sequence.constData();
}

void Primer3TaskSettings::setCircularity(bool isCirc) {
    isCircular = isCirc;
}

void Primer3TaskSettings::setTarget(const QList< U2Region > &value)
{
    for(int i=0;i < value.size();i++)
    {
        if(i >= PR_MAX_INTERVAL_ARRAY)
        {
            break;
        }
        seqArgs.tar[i][0] = value[i].startPos;
        seqArgs.tar[i][1] = value[i].length;
    }
    seqArgs.num_targets = value.size();
}

void Primer3TaskSettings::setProductSizeRange(const QList< U2Region > &value)
{
    for(int i=0;i < value.size();i++)
    {
        if(i >= PR_MAX_INTERVAL_ARRAY)
        {
            break;
        }
        primerArgs.pr_min[i] = value[i].startPos;
        primerArgs.pr_max[i] = value[i].endPos() - 1;
    }
    primerArgs.num_intervals = value.size();
}

void Primer3TaskSettings::setTask(const task &value)
{
    primerArgs.primer_task = value;
}

void Primer3TaskSettings::setInternalOligoExcludedRegion(const QList< U2Region > &value)
{
    for(int i=0;i < value.size();i++)
    {
        if(i >= PR_MAX_INTERVAL_ARRAY)
        {
            break;
        }
        seqArgs.excl_internal[i][0] = value[i].startPos;
        seqArgs.excl_internal[i][1] = value[i].length;
    }
    seqArgs.num_internal_excl = value.size();
}

void Primer3TaskSettings::setLeftInput(const QByteArray &value)
{
    leftInput = value;
    if(!value.isEmpty())
    {
        seqArgs.left_input = leftInput.constData();
    }
    else
    {
        seqArgs.left_input = NULL;
    }
}

void Primer3TaskSettings::setRightInput(const QByteArray &value)
{
    rightInput = value;
    if(!value.isEmpty())
    {
        seqArgs.right_input = rightInput.constData();
    }
    else
    {
        seqArgs.right_input = NULL;
    }
}

void Primer3TaskSettings::setInternalInput(const QByteArray &value)
{
    internalInput = value;
    if(!value.isEmpty())
    {
        seqArgs.internal_input = internalInput.constData();
    }
    else
    {
        seqArgs.internal_input = NULL;
    }
}

void Primer3TaskSettings::setExcludedRegion(const QList< U2Region > &value)
{
    for(int i=0;i < value.size();i++)
    {
        if(i >= PR_MAX_INTERVAL_ARRAY)
        {
            break;
        }
        seqArgs.excl[i][0] = value[i].startPos;
        seqArgs.excl[i][1] = value[i].length;
    }
    seqArgs.num_excl = value.size();
}

void Primer3TaskSettings::setIncludedRegion(const U2Region &value)
{
    seqArgs.incl_s = static_cast<int>(value.startPos);
    seqArgs.incl_l = static_cast<int>(value.length);
}

void Primer3TaskSettings::setIncludedRegion(const qint64 &startPos, const qint64 &length) {
    seqArgs.incl_s = static_cast<int>(startPos);
    seqArgs.incl_l = static_cast<int>(length);
}

void Primer3TaskSettings::setSequenceQuality(const QVector<int> &value)
{
    sequenceQuality = value;
    if(!value.isEmpty())
    {
        seqArgs.quality = sequenceQuality.constData();
    }
    else
    {
        seqArgs.quality = NULL;
    }
}

void Primer3TaskSettings::setRepeatLibrary(const QByteArray &value)
{
    repeatLibrary = value;
}

void Primer3TaskSettings::setMishybLibrary(const QByteArray &value)
{
    mishybLibrary = value;
}

QByteArray Primer3TaskSettings::getRepeatLibrary()const
{
    return repeatLibrary;
}

QByteArray Primer3TaskSettings::getMishybLibrary()const
{
    return mishybLibrary;
}

primer_args *Primer3TaskSettings::getPrimerArgs()
{
    return &primerArgs;
}

seq_args *Primer3TaskSettings::getSeqArgs()
{
    return &seqArgs;
}

void Primer3TaskSettings::initMaps()
{
    intProperties.insert("PRIMER_OPT_SIZE",&primerArgs.primer_opt_size);
    intProperties.insert("PRIMER_MIN_SIZE",&primerArgs.primer_min_size);
    intProperties.insert("PRIMER_MAX_SIZE",&primerArgs.primer_max_size);
    intProperties.insert("PRIMER_NUM_NS_ACCEPTED",&primerArgs.num_ns_accepted);
    intProperties.insert("PRIMER_MAX_POLY_X",&primerArgs.max_poly_x);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_POLY_X",&primerArgs.io_max_poly_x);
    intProperties.insert("PRIMER_FILE_FLAG",&primerArgs.file_flag);
    intProperties.insert("PRIMER_EXPLAIN_FLAG",&primerArgs.explain_flag);
    intProperties.insert("PRIMER_FIRST_BASE_INDEX",&primerArgs.first_base_index);
    doubleProperties.insert("PRIMER_MIN_TM",&primerArgs.min_tm);
    doubleProperties.insert("PRIMER_MAX_TM",&primerArgs.max_tm);
    doubleProperties.insert("PRIMER_MAX_GC",&primerArgs.max_gc);
    doubleProperties.insert("PRIMER_MIN_GC",&primerArgs.min_gc);
    alignProperties.insert("PRIMER_SELF_END",&primerArgs.self_end);
    alignProperties.insert("PRIMER_SELF_ANY",&primerArgs.self_any);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_OPT_SIZE",&primerArgs.io_primer_opt_size);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_MIN_SIZE",&primerArgs.io_primer_min_size);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_SIZE",&primerArgs.io_primer_max_size);
    alignProperties.insert("PRIMER_INTERNAL_OLIGO_SELF_ANY",&primerArgs.io_self_any);
    alignProperties.insert("PRIMER_INTERNAL_OLIGO_SELF_END",&primerArgs.io_self_end);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_TM",&primerArgs.io_max_tm);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_MIN_TM",&primerArgs.io_min_tm);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_GC",&primerArgs.io_max_gc);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_MIN_GC",&primerArgs.io_min_gc);
    intProperties.insert("PRIMER_DEFAULT_SIZE",&primerArgs.primer_opt_size);
    intProperties.insert("PRIMER_START_CODON_POSITION",&seqArgs.start_codon_pos);
    doubleProperties.insert("PRIMER_OPT_TM",&primerArgs.opt_tm);
    doubleProperties.insert("PRIMER_OPT_GC_PERCENT",&primerArgs.opt_gc_content);
    doubleProperties.insert("PRIMER_MAX_DIFF_TM",&primerArgs.max_diff_tm);
    intProperties.insert("PRIMER_TM_SANTALUCIA",&primerArgs.tm_santalucia);
    intProperties.insert("PRIMER_SALT_CORRECTIONS",&primerArgs.salt_corrections);
    doubleProperties.insert("PRIMER_SALT_CONC",&primerArgs.salt_conc);
    doubleProperties.insert("PRIMER_DIVALENT_CONC",&primerArgs.divalent_conc);
    doubleProperties.insert("PRIMER_DNTP_CONC",&primerArgs.dntp_conc);
    doubleProperties.insert("PRIMER_DNA_CONC",&primerArgs.dna_conc);
    intProperties.insert("PRIMER_PRODUCT_OPT_SIZE",&primerArgs.product_opt_size);
    intProperties.insert("PRIMER_PICK_ANYWAY",&primerArgs.pick_anyway);
    intProperties.insert("PRIMER_GC_CLAMP",&primerArgs.gc_clamp);
    intProperties.insert("PRIMER_LIBERAL_BASE",&primerArgs.liberal_base);
    intProperties.insert("PRIMER_NUM_RETURN",&primerArgs.num_return);
    intProperties.insert("PRIMER_MIN_QUALITY",&primerArgs.min_quality);
    intProperties.insert("PRIMER_MIN_END_QUALITY",&primerArgs.min_end_quality);
    intProperties.insert("PRIMER_QUALITY_RANGE_MIN",&primerArgs.quality_range_min);
    intProperties.insert("PRIMER_QUALITY_RANGE_MAX",&primerArgs.quality_range_max);
    doubleProperties.insert("PRIMER_PRODUCT_MAX_TM",&primerArgs.product_max_tm);
    doubleProperties.insert("PRIMER_PRODUCT_MIN_TM",&primerArgs.product_min_tm);
    doubleProperties.insert("PRIMER_PRODUCT_OPT_TM",&primerArgs.product_opt_tm);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_OPT_TM",&primerArgs.io_opt_tm);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT",&primerArgs.io_opt_gc_content);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_SALT_CONC",&primerArgs.io_salt_conc);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_DIVALENT_CONC",&primerArgs.io_divalent_conc);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_DNTP_CONC",&primerArgs.io_dntp_conc);
    doubleProperties.insert("PRIMER_INTERNAL_OLIGO_DNA_CONC",&primerArgs.io_dna_conc);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_NUM_NS",&primerArgs.io_num_ns_accepted);
    intProperties.insert("PRIMER_INTERNAL_OLIGO_MIN_QUALITY",&primerArgs.io_min_quality);
    alignProperties.insert("PRIMER_MAX_MISPRIMING",&primerArgs.repeat_compl);
    alignProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_MISHYB",&primerArgs.io_repeat_compl);
    alignProperties.insert("PRIMER_PAIR_MAX_MISPRIMING",&primerArgs.pair_repeat_compl);
    alignProperties.insert("PRIMER_MAX_TEMPLATE_MISPRIMING",&primerArgs.max_template_mispriming);
    alignProperties.insert("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING",&primerArgs.pair_max_template_mispriming);
    alignProperties.insert("PRIMER_INTERNAL_OLIGO_MAX_TEMPLATE_MISHYB",&primerArgs.io_max_template_mishyb);
    intProperties.insert("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS",&primerArgs.lib_ambiguity_codes_consensus);
    doubleProperties.insert("PRIMER_INSIDE_PENALTY",&primerArgs.inside_penalty);
    doubleProperties.insert("PRIMER_OUTSIDE_PENALTY",&primerArgs.outside_penalty);
    doubleProperties.insert("PRIMER_MAX_END_STABILITY",&primerArgs.max_end_stability);
    intProperties.insert("PRIMER_LOWERCASE_MASKING",&primerArgs.lowercase_masking);
    doubleProperties.insert("PRIMER_WT_TM_GT",&primerArgs.primer_weights.temp_gt);
    doubleProperties.insert("PRIMER_WT_TM_LT",&primerArgs.primer_weights.temp_lt);
    doubleProperties.insert("PRIMER_WT_GC_PERCENT_GT",&primerArgs.primer_weights.gc_content_gt);
    doubleProperties.insert("PRIMER_WT_GC_PERCENT_LT",&primerArgs.primer_weights.gc_content_lt);
    doubleProperties.insert("PRIMER_WT_SIZE_LT",&primerArgs.primer_weights.length_lt);
    doubleProperties.insert("PRIMER_WT_SIZE_GT",&primerArgs.primer_weights.length_gt);
    doubleProperties.insert("PRIMER_WT_COMPL_ANY",&primerArgs.primer_weights.compl_any);
    doubleProperties.insert("PRIMER_WT_COMPL_END",&primerArgs.primer_weights.compl_end);
    doubleProperties.insert("PRIMER_WT_NUM_NS",&primerArgs.primer_weights.num_ns);
    doubleProperties.insert("PRIMER_WT_REP_SIM",&primerArgs.primer_weights.repeat_sim);
    doubleProperties.insert("PRIMER_WT_SEQ_QUAL",&primerArgs.primer_weights.seq_quality);
    doubleProperties.insert("PRIMER_WT_END_QUAL",&primerArgs.primer_weights.end_quality);
    doubleProperties.insert("PRIMER_WT_POS_PENALTY",&primerArgs.primer_weights.pos_penalty);
    doubleProperties.insert("PRIMER_WT_END_STABILITY",&primerArgs.primer_weights.end_stability);
    doubleProperties.insert("PRIMER_WT_TEMPLATE_MISPRIMING",&primerArgs.primer_weights.template_mispriming);
    doubleProperties.insert("PRIMER_IO_WT_TM_GT",&primerArgs.io_weights.temp_gt);
    doubleProperties.insert("PRIMER_IO_WT_TM_LT",&primerArgs.io_weights.temp_lt);
    doubleProperties.insert("PRIMER_IO_WT_GC_PERCENT_GT",&primerArgs.io_weights.gc_content_gt);
    doubleProperties.insert("PRIMER_IO_WT_GC_PERCENT_LT",&primerArgs.io_weights.gc_content_lt);
    doubleProperties.insert("PRIMER_IO_WT_SIZE_LT",&primerArgs.io_weights.length_lt);
    doubleProperties.insert("PRIMER_IO_WT_SIZE_GT",&primerArgs.io_weights.length_gt);
    doubleProperties.insert("PRIMER_IO_WT_COMPL_ANY",&primerArgs.io_weights.compl_any);
    doubleProperties.insert("PRIMER_IO_WT_COMPL_END",&primerArgs.io_weights.compl_end);
    doubleProperties.insert("PRIMER_IO_WT_NUM_NS",&primerArgs.io_weights.num_ns);
    doubleProperties.insert("PRIMER_IO_WT_REP_SIM",&primerArgs.io_weights.repeat_sim);
    doubleProperties.insert("PRIMER_IO_WT_SEQ_QUAL",&primerArgs.io_weights.seq_quality);
    doubleProperties.insert("PRIMER_IO_WT_END_QUAL",&primerArgs.io_weights.end_quality);
    doubleProperties.insert("PRIMER_IO_WT_TEMPLATE_MISHYB",&primerArgs.io_weights.template_mispriming);
    doubleProperties.insert("PRIMER_PAIR_WT_PR_PENALTY",&primerArgs.pr_pair_weights.primer_quality);
    doubleProperties.insert("PRIMER_PAIR_WT_IO_PENALTY",&primerArgs.pr_pair_weights.io_quality);
    doubleProperties.insert("PRIMER_PAIR_WT_DIFF_TM",&primerArgs.pr_pair_weights.diff_tm);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_ANY",&primerArgs.pr_pair_weights.compl_any);
    doubleProperties.insert("PRIMER_PAIR_WT_COMPL_END",&primerArgs.pr_pair_weights.compl_end);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_TM_LT",&primerArgs.pr_pair_weights.product_tm_lt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_TM_GT",&primerArgs.pr_pair_weights.product_tm_gt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_SIZE_GT",&primerArgs.pr_pair_weights.product_size_gt);
    doubleProperties.insert("PRIMER_PAIR_WT_PRODUCT_SIZE_LT",&primerArgs.pr_pair_weights.product_size_lt);
    doubleProperties.insert("PRIMER_PAIR_WT_REP_SIM",&primerArgs.pr_pair_weights.repeat_sim);
    doubleProperties.insert("PRIMER_PAIR_WT_TEMPLATE_MISPRIMING",&primerArgs.pr_pair_weights.template_mispriming);
}

} // namespace U2
