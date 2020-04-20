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

#ifndef _U2_FEATURE_KEYS_H_
#define _U2_FEATURE_KEYS_H_

#include <QtCore/QString>

namespace U2 {

const QString U2FeatureGeneName =                   "Gene";
const QString U2FeatureExonName =                   "Exon";
const QString U2FeatureCDSName =                    "CDS";

const QString U2FeatureSuperDupName =               "SD";
const QString U2FeatureConservativeName =           "CR";

const QString U2FeatureKey_Name =                   "name";
const QString U2FeatureKey_AltName =                "alt_name";
const QString U2FeatureKey_Note =                   "note";
const QString U2FeatureKey_Accession =              "accession";
const QString U2FeatureKey_KnownType =              "known";
const QString U2FeatureKey_Disease =                "disease";

//Since feature keys are used to store operations and region types, the most rare symbols combination is used
//to avoid adding qualifier with the same name
//Also it is short to reduce required space in the db

const QString U2FeatureKeyOperation =               "#~o";
const QString U2FeatureKeyOperationOrder =          "#~oo";
const QString U2FeatureKeyOperationJoin =           "#~oj";
const QString U2FeatureKeyOperationBond =           "#~ob";

const QString U2FeatureKeyCase =                    "#~c";

} //namespace

#endif //_U2_FEATURE_KEYS_H_
