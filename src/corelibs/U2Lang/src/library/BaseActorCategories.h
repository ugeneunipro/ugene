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

#ifndef _U2_WORKFLOW_BASE_LIB_H_
#define _U2_WORKFLOW_BASE_LIB_H_

#include <U2Lang/Datatype.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT BaseActorCategories : public QObject {
    Q_OBJECT
public:
    static const Descriptor CATEGORY_DATASRC();
    static const Descriptor CATEGORY_DATASINK();
    static const Descriptor CATEGORY_CONVERTERS();
    static const Descriptor CATEGORY_BASIC();
    static const Descriptor CATEGORY_ALIGNMENT();
    static const Descriptor CATEGORY_ASSEMBLY();
    static const Descriptor CATEGORY_NGS_BASIC();
    static const Descriptor CATEGORY_NGS_MAP_ASSEMBLE_READS();
    static const Descriptor CATEGORY_RNA_SEQ();
    static const Descriptor CATEGORY_VARIATION_ANALYSIS();
    static const Descriptor CATEGORY_TRANSCRIPTION();
    static const Descriptor CATEGORY_SCRIPT();
    static const Descriptor CATEGORY_STATISTIC();
    static const Descriptor CATEGORY_EXTERNAL();
    static const Descriptor CATEGORY_DATAFLOW();
    static const Descriptor CATEGORY_INCLUDES();
    static const Descriptor CATEGORY_SNP_ANNOTATION();
    static const Descriptor CATEGORY_CHIP_SEQ();

}; // BaseActorCategories

}//Workflow namespace
}//GB2 namespace

#endif
