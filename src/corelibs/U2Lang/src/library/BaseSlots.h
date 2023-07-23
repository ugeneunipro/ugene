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

#pragma once

#include <U2Core/U2Type.h>

#include <U2Lang/Descriptor.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT BaseSlots : public QObject {
    Q_OBJECT
public:
    static Descriptor DNA_SEQUENCE_SLOT();
    static Descriptor MULTIPLE_ALIGNMENT_SLOT();
    static Descriptor ANNOTATION_TABLE_SLOT();
    static Descriptor TEXT_SLOT();
    static Descriptor URL_SLOT();
    static Descriptor FASTA_HEADER_SLOT();
    static Descriptor VARIATION_TRACK_SLOT();
    static Descriptor ASSEMBLY_SLOT();
    static Descriptor DATASET_SLOT();
};  // BaseSlots

}  // namespace Workflow
}  // namespace U2
