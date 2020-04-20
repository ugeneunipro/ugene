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

#ifndef __U2_WORKFLOW_BASE_SLOTS_H_
#define __U2_WORKFLOW_BASE_SLOTS_H_

#include <U2Core/U2Type.h>

#include <U2Lang/Descriptor.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT BaseSlots : public QObject {
    Q_OBJECT
public:
    static const Descriptor DNA_SEQUENCE_SLOT();
    static const Descriptor MULTIPLE_ALIGNMENT_SLOT();
    static const Descriptor ANNOTATION_TABLE_SLOT();
    static const Descriptor TEXT_SLOT();
    static const Descriptor URL_SLOT();
    static const Descriptor FASTA_HEADER_SLOT();
    static const Descriptor VARIATION_TRACK_SLOT();
    static const Descriptor ASSEMBLY_SLOT();
    static const Descriptor DATASET_SLOT();
}; // BaseSlots

} // Workflow
} // U2

#endif // __U2_WORKFLOW_BASE_SLOTS_H_
