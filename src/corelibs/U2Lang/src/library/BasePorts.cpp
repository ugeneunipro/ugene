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

#include "BasePorts.h"

namespace U2 {
namespace Workflow {

const QString BasePorts::OUT_MSA_PORT_ID() {
    return "out-msa";
}

const QString BasePorts::IN_MSA_PORT_ID() {
    return "in-msa";
}

const QString BasePorts::OUT_SEQ_PORT_ID() {
    return "out-sequence";
}

const QString BasePorts::IN_SEQ_PORT_ID() {
    return "in-sequence";
}

const QString BasePorts::OUT_ANNOTATIONS_PORT_ID() {
    return "out-annotations";
}

const QString BasePorts::IN_ANNOTATIONS_PORT_ID() {
    return "in-annotations";
}

const QString BasePorts::OUT_TEXT_PORT_ID() {
    return "out-text";
}

const QString BasePorts::IN_TEXT_PORT_ID() {
    return "in-text";
}

const QString BasePorts::OUT_VARIATION_TRACK_PORT_ID() {
    return "out-variations";
}

const QString BasePorts::IN_VARIATION_TRACK_PORT_ID() {
    return "in-variations";
}

const QString BasePorts::OUT_ASSEMBLY_PORT_ID() {
    return "out-assembly";
}

const QString BasePorts::IN_ASSEMBLY_PORT_ID() {
    return "in-assembly";
}

} // Workflow
} // U2
