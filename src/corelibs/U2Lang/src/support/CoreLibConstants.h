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

#ifndef _U2_WORKFLOW_CORE_LIB_CONSTANTS_H_
#define _U2_WORKFLOW_CORE_LIB_CONSTANTS_H_

#include <QString>
#include <U2Core/global.h>

namespace U2 {

namespace Workflow {

class U2LANG_EXPORT CoreLibConstants {
public:
    static const QString WRITE_FASTA_PROTO_ID;
    static const QString READ_TEXT_PROTO_ID;
    static const QString WRITE_TEXT_PROTO_ID;
    static const QString WRITE_GENBANK_PROTO_ID;
    static const QString WRITE_FASTQ_PROTO_ID;
    static const QString GENERIC_READ_MA_PROTO_ID;
    static const QString GENERIC_READ_SEQ_PROTO_ID;
    static const QString WRITE_CLUSTAL_PROTO_ID;
    static const QString WRITE_STOCKHOLM_PROTO_ID;
    static const QString WRITE_MSA_PROTO_ID;
    static const QString WRITE_SEQ_PROTO_ID;
    static const QString GROUPER_ID;

    static const QString TEXT_TYPESET_ID;

    static const QString WORKFLOW_ON_CLOUD_TASK_ID;
    static const QString WORKFLOW_SCHEMA_ATTR;
    static const QString DATA_IN_ATTR;
    static const QString DATA_OUT_ATTR;
    static const QString GROUPER_SLOT_ATTR;
    static const QString GROUPER_OUT_SLOTS_ATTR;
    static const QString GROUPER_OPER_ATTR;

}; // CoreLibConstants

} // Workflow

} // U2

#endif // _U2_WORKFLOW_CORE_LIB_CONSTANTS_H_
