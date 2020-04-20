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

#ifndef _U2_HR_CONTANTS_H_
#define _U2_HR_CONTANTS_H_

#include <U2Core/global.h>

#undef NO_ERROR

namespace U2 {
namespace WorkflowSerialize {

class U2LANG_EXPORT Constants {
public:
    static const QString BLOCK_START;
    static const QString BLOCK_END;
    static const QString SERVICE_SYM;
    static const QString SINGLE_QUOTE;
    static const QString QUOTE;
    static const QString NEW_LINE;
    static const QString UNKNOWN_ERROR;
    static const QString NO_ERROR;
    static const QString HEADER_LINE;
    static const QString DEPRECATED_HEADER_LINE;
    static const QString OLD_XML_HEADER;
    static const QString INCLUDE;
    static const QString INCLUDE_AS;
    static const QString BODY_START;
    static const QString META_START;
    static const QString DOT_ITERATION_START;
    static const QString ITERATION_START;
    static const QString DATAFLOW_SIGN;
    static const QString EQUALS_SIGN;
    static const QString UNDEFINED_CONSTRUCT;
    static const QString TYPE_ATTR;
    static const QString SCRIPT_ATTR;
    static const QString NAME_ATTR;
    static const QString ELEM_ID_ATTR;
    static const QString DOT;
    static const QString DASH;
    static const QString ITERATION_ID;
    static const QString PARAM_ALIASES_START;
    static const QString PORT_ALIASES_START;
    static const QString PATH_THROUGH;
    static const QString TRUE;
    static const QString FALSE;

    // -------------- backward compatibility --------------
    static const QString ALIASES_HELP_START;
    static const QString OLD_ALIASES_START;
    // ----------------------------------------------------

    static const QString VISUAL_START;
    static const QString UNDEFINED_META_BLOCK;
    static const QString TAB;
    static const QString NO_NAME;
    static const QString COLON;
    static const QString SEMICOLON;
    static const QString INPUT_START;
    static const QString OUTPUT_START;
    static const QString ATTRIBUTES_START;
    static const QString TYPE_PORT;
    static const QString FORMAT_PORT;
    static const QString CMDLINE;
    static const QString DEFAULT_VALUE;
    static const QString DESCRIPTION;
    static const QString PROMPTER;
    static const QString COMMA;
    static const QString MARKER;
    static const QString QUAL_NAME;
    static const QString ANN_NAME;
    static const QString ACTOR_BINDINGS;
    static const QString SOURCE_PORT;
    static const QString ALIAS;
    static const QString IN_SLOT;
    static const QString ACTION;
    static const QString OUT_SLOT_ATTR;
    static const QString DATASET_NAME;
    static const QString DB_SELECT;
    static const QString DB_URL;
    static const QString DB_OBJECT_TYPE;
    static const QString DB_OBJECT_ID;
    static const QString DB_OBJ_CACHED_NAME;
    static const QString DB_OBJ_NAME_FILTER;
    static const QString DB_SEQ_ACC_FILTER;
    static const QString DIRECTORY_URL;
    static const QString FILE_URL;
    static const QString PATH;
    static const QString EXC_FILTER;
    static const QString INC_FILTER;
    static const QString RECURSIVE;
    static const QString ESTIMATIONS;
    static const QString VALIDATOR;
    static const QString V_TYPE;
    static const QString V_SCRIPT;
    static const QString USE_INTEGRATED_TOOL;
    static const QString CUSTOM_TOOL_PATH;
    static const QString INTEGRATED_TOOL_ID;
    static const QString ADD_TO_DASHBOARD;
    static const QString OPEN_WITH_UGENE;
};

} // WorkflowSerialize
} // U2

#endif // _U2_HR_CONTANTS_H_
