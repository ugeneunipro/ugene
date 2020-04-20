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

#ifndef __U2_WORKFLOW_BASE_PORTS_H_
#define __U2_WORKFLOW_BASE_PORTS_H_

#include <QString>
#include <U2Core/global.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT BasePorts {
public:
    static const QString OUT_MSA_PORT_ID();
    static const QString IN_MSA_PORT_ID();
    static const QString OUT_SEQ_PORT_ID();
    static const QString IN_SEQ_PORT_ID();
    static const QString OUT_ANNOTATIONS_PORT_ID();
    static const QString IN_ANNOTATIONS_PORT_ID();
    static const QString OUT_TEXT_PORT_ID();
    static const QString IN_TEXT_PORT_ID();
    static const QString OUT_VARIATION_TRACK_PORT_ID();
    static const QString IN_VARIATION_TRACK_PORT_ID();
    static const QString OUT_ASSEMBLY_PORT_ID();
    static const QString IN_ASSEMBLY_PORT_ID();

}; // BasePorts

} // Workflow
} // U2

#endif // __U2_WORKFLOW_BASE_PORTS_H_
