/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
 * https://ugene.net
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

#ifndef _U2_OPTIONAL_PARAMETERS_TO_EXCLUDE_H_
#define _U2_OPTIONAL_PARAMETERS_TO_EXCLUDE_H_

#include <QVariant>

namespace U2 {

// Parameters to exclude in whole primers. Fields must be null or convertible to int/double. Field is null if the user
// hasn't set parameter, which means it shouldn't be taken into account.
struct OptionalParametersToExclude {
    // How many conditions must be held to disable primer:
    enum class ExclusionCriteria {
        Any,  // at least one
        All  // all.
    };  // Order matters and depends on ui file.

    QVariant gibbsFreeEnergy;
    QVariant meltingPoint;
    QVariant complementLength;

    // Is at least one optional parameter set?
    bool isAnyParameterSet() const;
};

}  // namespace U2

#endif
