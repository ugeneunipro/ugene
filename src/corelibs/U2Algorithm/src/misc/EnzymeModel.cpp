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

#include "EnzymeModel.h"

namespace U2 {

const QString EnzymeSettings::DATA_DIR_KEY("enzymes");
const QString EnzymeSettings::DATA_FILE_KEY("plugin_enzymes/lastFile");
const QString EnzymeSettings::LAST_SELECTION("plugin_enzymes/selection");
const QString EnzymeSettings::ENABLE_HIT_COUNT("plugin_enzymes/enable_hit_count");
const QString EnzymeSettings::MAX_HIT_VALUE("plugin_enzymes/max_hit_value");
const QString EnzymeSettings::MIN_HIT_VALUE("plugin_enzymes/min_hit_value");
const QString EnzymeSettings::SEARCH_REGION("plugin_enzymes/search_region");
const QString EnzymeSettings::EXCLUDED_REGION("plugin_enzymes/non_cut_region");
const QString EnzymeSettings::MAX_RESULTS("plugin_enzymes/max_results");
const QString EnzymeSettings::COMMON_ENZYMES( "ClaI,BamHI,BglII,DraI,EcoRI,EcoRV,HindIII,PstI,SalI,SmaI,XmaI");

EnzymeData::EnzymeData() {
    cutDirect = ENZYME_CUT_UNKNOWN;
    cutComplement = ENZYME_CUT_UNKNOWN;
    alphabet = NULL;
}

} // namespace
