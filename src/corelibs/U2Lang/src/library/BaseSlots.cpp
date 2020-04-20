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

#include "BaseSlots.h"

static const QString SEQ_SLOT_ID("sequence");
static const QString MA_SLOT_ID("msa");
static const QString FEATURE_TABLE_SLOT_ID("annotations");
static const QString TEXT_SLOT_ID("text");
static const QString URL_SLOT_ID("url");
static const QString FASTA_HEADER_SLOT_ID("fasta-header");
static const QString VARIATION_TRACK_SLOT_ID("variation-track");
static const QString ASSEMBLY_SLOT_ID("assembly");
static const QString DATASET_SLOT_ID("dataset");

namespace U2 {
namespace Workflow {

const Descriptor BaseSlots::DNA_SEQUENCE_SLOT() {
    return Descriptor(SEQ_SLOT_ID, tr("Sequence"), tr("A biological sequence"));
}

const Descriptor BaseSlots::MULTIPLE_ALIGNMENT_SLOT() {
    return Descriptor(MA_SLOT_ID, tr("MSA"), tr("A multiple sequence alignment"));
}

const Descriptor BaseSlots::ANNOTATION_TABLE_SLOT() {
    return Descriptor(FEATURE_TABLE_SLOT_ID, tr("Set of annotations"), tr("A set of annotated regions"));
}

const Descriptor BaseSlots::TEXT_SLOT() {
    return Descriptor(TEXT_SLOT_ID, tr("Plain text"), tr("Plain text reading or splitting to strings."));
}

const Descriptor BaseSlots::URL_SLOT() {
    return Descriptor(URL_SLOT_ID, tr("Source URL"), tr("Location of a corresponding input file."));
}

const Descriptor BaseSlots::FASTA_HEADER_SLOT() {
    return Descriptor(FASTA_HEADER_SLOT_ID, tr("FASTA header"), tr("A header line for the FASTA record."));
}

const Descriptor BaseSlots::VARIATION_TRACK_SLOT() {
    return Descriptor(VARIATION_TRACK_SLOT_ID, tr("Variation track"), tr("Set of variations"));
}

const Descriptor BaseSlots::ASSEMBLY_SLOT() {
    return Descriptor(ASSEMBLY_SLOT_ID, tr("Assembly data"), tr("Assembly data"));
}

const Descriptor BaseSlots::DATASET_SLOT() {
    return Descriptor(DATASET_SLOT_ID, tr("Dataset name"), tr("Dataset name"));
}

} // Workflow
} // U2
