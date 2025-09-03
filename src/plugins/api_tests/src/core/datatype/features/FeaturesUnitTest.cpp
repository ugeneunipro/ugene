/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "FeaturesUnitTest.h"

#include <QFile>

#include <U2Core/FeatureColors.h>
#include <U2Core/U2FeatureType.h>

namespace U2 {

IMPLEMENT_TEST(FeaturesUnitTest, checkFeaturesColors) {
    for (int i = 0; i <= (int)U2FeatureType::RestrictionSite; i++) {
        auto featureType = static_cast<U2FeatureType>(i);
        auto name = U2FeatureTypes::getVisualName(featureType);
        auto generatedColor = FeatureColors::genLightColor(name).name();
        auto color = U2FeatureTypes::getColor(featureType).name();
        CHECK_EQUAL(generatedColor, color, "feature color");
    }
}

}  // namespace U2
