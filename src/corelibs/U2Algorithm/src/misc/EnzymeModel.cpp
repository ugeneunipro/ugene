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

#include "EnzymeModel.h"

namespace U2 {

const QString EnzymeSettings::DATA_DIR_KEY("enzymes");
const QString EnzymeSettings::DATA_FILE_KEY("plugin_enzymes/lastFile");
const QString EnzymeSettings::LAST_SELECTION("plugin_enzymes/selection");
const QString EnzymeSettings::CHECKED_SUPPLIERS("plugin_enzymes/checkedSuppliers");
const QString EnzymeSettings::MIN_ENZYME_LENGTH("plugin_enzymes/min_enzyme_length");
const QString EnzymeSettings::MAX_ENZYME_LENGTH("plugin_enzymes/max_enzyme_length");
const QString EnzymeSettings::OVERHANG_TYPE("plugin_enzymes/overhang_type");
const QString EnzymeSettings::SHOW_PALINDROMIC("plugin_enzymes/show_palindromic");
const QString EnzymeSettings::SHOW_UNINTERRUPTED("plugin_enzymes/show_uninterrupted");
const QString EnzymeSettings::SHOW_NONDEGENERATE("plugin_enzymes/show_nondegenerate");
const QString EnzymeSettings::ENABLE_HIT_COUNT("plugin_enzymes/enable_hit_count");
const QString EnzymeSettings::MAX_HIT_VALUE("plugin_enzymes/max_hit_value");
const QString EnzymeSettings::MIN_HIT_VALUE("plugin_enzymes/min_hit_value");
const QString EnzymeSettings::MAX_RESULTS("plugin_enzymes/max_results");
const QString EnzymeSettings::COMMON_ENZYMES("ClaI,BamHI,BglII,DraI,EcoRI,EcoRV,HindIII,PstI,SalI,SmaI,XmaI");

int EnzymeData::getFullLength() const {
    int leadingNsNumber = 0;
    int trailingNsNumber = 0;
    int seqSize = seq.size();
    if (cutDirect != ENZYME_CUT_UNKNOWN) {
        if (cutDirect < 0) {
            leadingNsNumber = qAbs(cutDirect);
        } else if (cutDirect > seqSize) {
            trailingNsNumber = cutDirect - seqSize;
        }
    }
    if (cutComplement != ENZYME_CUT_UNKNOWN) {
        if (cutComplement < 0) {
            trailingNsNumber = qMax(trailingNsNumber, qAbs(cutComplement));
        } else if (cutComplement > seqSize) {
            leadingNsNumber = qMax(leadingNsNumber, cutComplement - seqSize);
        }
    }

    return leadingNsNumber + seqSize + trailingNsNumber;
}


}  // namespace U2
