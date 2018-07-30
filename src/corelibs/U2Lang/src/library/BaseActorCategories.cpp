/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Lang/WorkflowEnv.h>

#include "LocalDomain.h"
#include "BaseActorCategories.h"

namespace U2 {
namespace Workflow {

const Descriptor BaseActorCategories::CATEGORY_DATASRC() {
    return Descriptor("a", tr("Data Readers"), "");
}

const Descriptor BaseActorCategories::CATEGORY_DATASINK() {
    return Descriptor("b", tr("Data Writers"), "");
}

const Descriptor BaseActorCategories::CATEGORY_CONVERTERS() {
    return Descriptor("bc", tr("Data Converters"), "");
}

const Descriptor BaseActorCategories::CATEGORY_BASIC() {
    return Descriptor("c", tr("Basic Analysis"), "");
}

const Descriptor BaseActorCategories::CATEGORY_STATISTIC() {
    return Descriptor("st", tr("Utils"), "");
}

const Descriptor BaseActorCategories::CATEGORY_ALIGNMENT() {
    return Descriptor("d", tr("Multiple Sequence Alignment"), "");
}

const Descriptor BaseActorCategories::CATEGORY_ASSEMBLY() {
    return Descriptor("asm", tr("DNA Assembly"), "");
}

const Descriptor BaseActorCategories::CATEGORY_NGS_BASIC() {
    return Descriptor("ngs_base", tr("NGS: Basic Functions"), "");
}

const Descriptor BaseActorCategories::CATEGORY_NGS_MAP_ASSEMBLE_READS() {
    return Descriptor("ngs_align", tr("NGS: Map/Assemble Reads"), "");
}

const Descriptor BaseActorCategories::CATEGORY_RNA_SEQ() {
    return Descriptor("rna", tr("NGS: RNA-Seq Analysis"), "");
}

const Descriptor BaseActorCategories::CATEGORY_VARIATION_ANALYSIS() {
    return Descriptor("var", tr("NGS: Variant Analysis"), "");
}

const Descriptor BaseActorCategories::CATEGORY_TRANSCRIPTION() {
    return Descriptor("tr", tr("Transcription Factor Binding Sites"), "");
}

const Descriptor BaseActorCategories::CATEGORY_SCRIPT() {
    return Descriptor("sa", tr("Custom Elements with Script"), "");
}

const Descriptor BaseActorCategories::CATEGORY_EXTERNAL() {
    return Descriptor("ex", tr("Custom Elements with CMD Tools"),"");
}

const Descriptor BaseActorCategories::CATEGORY_DATAFLOW() {
    return Descriptor("df", tr("Data Flow"),"");
}

const Descriptor BaseActorCategories::CATEGORY_INCLUDES() {
    return Descriptor("inc", tr("Includes"),"");
}

const Descriptor BaseActorCategories::CATEGORY_SNP_ANNOTATION()
{
    return Descriptor("sch", tr("SNP Annotation"),"");
}

const Descriptor BaseActorCategories::CATEGORY_CHIP_SEQ() {
    return Descriptor("chs", tr("NGS: ChIP-Seq Analysis"), "");
}

}//Workflow namespace
}//GB2 namespace
