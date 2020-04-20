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

#include <QObject>

#include "Constants.h"

namespace U2 {
namespace WorkflowSerialize {

const QString Constants::BLOCK_START           = "{";
const QString Constants::BLOCK_END             = "}";
const QString Constants::SERVICE_SYM           = "#";
const QString Constants::SINGLE_QUOTE          = "'";
const QString Constants::QUOTE                 = "\"";
const QString Constants::NEW_LINE              = "\n";
const QString Constants::UNKNOWN_ERROR         = QObject::tr("Error: unknown exception caught");
const QString Constants::NO_ERROR              = "";
const QString Constants::HEADER_LINE           = "#@UGENE_WORKFLOW";
const QString Constants::DEPRECATED_HEADER_LINE= "#!UGENE_WORKFLOW";
const QString Constants::OLD_XML_HEADER        = "<!DOCTYPE GB2WORKFLOW>";
const QString Constants::INCLUDE               = "include";
const QString Constants::INCLUDE_AS            = "as";
const QString Constants::BODY_START            = "workflow";
const QString Constants::META_START            = ".meta";
const QString Constants::DOT_ITERATION_START   = ".iteration";
const QString Constants::ITERATION_START       = "iteration";
const QString Constants::DATAFLOW_SIGN         = "->";
const QString Constants::EQUALS_SIGN           = ":";
const QString Constants::UNDEFINED_CONSTRUCT   = QObject::tr("Undefined construct at '%1 %2'");
const QString Constants::TYPE_ATTR             = "type";
const QString Constants::SCRIPT_ATTR           = "script";
const QString Constants::NAME_ATTR             = "name";
const QString Constants::ELEM_ID_ATTR          = "elem-id";
const QString Constants::DOT                   = ".";
const QString Constants::DASH                  = "-";
const QString Constants::ITERATION_ID          = "id";
const QString Constants::PARAM_ALIASES_START   = "parameter-aliases";
const QString Constants::PORT_ALIASES_START    = "port-aliases";
const QString Constants::PATH_THROUGH          = "path-through";
const QString Constants::TRUE                  = "true";
const QString Constants::FALSE                 = "false";

// -------------- backward compatibility --------------
    const QString Constants::ALIASES_HELP_START    = "help";
    const QString Constants::OLD_ALIASES_START     = "aliases";
// ----------------------------------------------------

const QString Constants::VISUAL_START          = "visual";
const QString Constants::UNDEFINED_META_BLOCK  = QObject::tr("Undefined block in .meta: '%1'");
const QString Constants::TAB                   = "    ";
const QString Constants::NO_NAME               = "";
const QString Constants::COLON                 = ":";
const QString Constants::SEMICOLON             = ";";
const QString Constants::INPUT_START           = ".inputs";
const QString Constants::OUTPUT_START          = ".outputs";
const QString Constants::ATTRIBUTES_START      = ".attributes";
const QString Constants::TYPE_PORT             = "type";
const QString Constants::FORMAT_PORT           = "format";
const QString Constants::CMDLINE               = "cmdline";
const QString Constants::DEFAULT_VALUE         = "default-value";
const QString Constants::DESCRIPTION           = "description";
const QString Constants::PROMPTER              = "templatedescription";
const QString Constants::COMMA                 = ",";
const QString Constants::MARKER                = "marker";
const QString Constants::QUAL_NAME             = "qualifier-name";
const QString Constants::ANN_NAME              = "annotation-name";
const QString Constants::ACTOR_BINDINGS        = ".actor-bindings";
const QString Constants::SOURCE_PORT           = "source";
const QString Constants::ALIAS                 = "alias";
const QString Constants::IN_SLOT               = "in-slot";
const QString Constants::ACTION                = "action";
const QString Constants::OUT_SLOT_ATTR         = "out-slot";
const QString Constants::DATASET_NAME          = "dataset";
const QString Constants::DB_SELECT             = "database-select";
const QString Constants::DB_URL                = "url";
const QString Constants::DB_OBJECT_TYPE        = "type";
const QString Constants::DB_OBJECT_ID          = "id";
const QString Constants::DB_OBJ_CACHED_NAME    = "cached-name";
const QString Constants::DB_OBJ_NAME_FILTER    = "object-name";
const QString Constants::DB_SEQ_ACC_FILTER     = "accession";
const QString Constants::DIRECTORY_URL         = "dir";
const QString Constants::FILE_URL              = "file";
const QString Constants::PATH                  = "path";
const QString Constants::EXC_FILTER            = "exclude-name-filter";
const QString Constants::INC_FILTER            = "include-name-filter";
const QString Constants::RECURSIVE             = "recursive";
const QString Constants::ESTIMATIONS           = "estimations";
const QString Constants::VALIDATOR             = ".validator";
const QString Constants::V_TYPE                = "type";
const QString Constants::V_SCRIPT              = "script";
const QString Constants::USE_INTEGRATED_TOOL   = "use-integrated-tool";
const QString Constants::CUSTOM_TOOL_PATH      = "custom-tool-path";
const QString Constants::INTEGRATED_TOOL_ID    = "integrated-tool-id";
const QString Constants::ADD_TO_DASHBOARD      = "add-to-dashboard";
const QString Constants::OPEN_WITH_UGENE       = "open-with-ugene";

} // WorkflowSerialize
} // U2
