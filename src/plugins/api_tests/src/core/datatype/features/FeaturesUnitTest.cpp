/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
    for (int i = 1; i <= (int)U2FeatureType::RestrictionSite; i++) {
        auto featureType = static_cast<U2FeatureType>(i);
        auto color = U2FeatureTypes::getLightColor(featureType).name();
        QString generatedColor;
        switch (featureType) {
            case U2FeatureType::Cds:
                generatedColor = "#9bffff";
                break;
            case U2FeatureType::Gene:
                generatedColor = "#00ffc8";
                break;
            case U2FeatureType::MiscFeature:
                generatedColor = "#ffff99";
                break;
            case U2FeatureType::RepeatRegion:
                generatedColor = "#ccccff";
                break;
            case U2FeatureType::Source:
                generatedColor = "#cccccc";
                break;
            case U2FeatureType::Sts:
                generatedColor = "#00dcdc";
                break;
            case U2FeatureType::TRna:
                generatedColor = "#c8fac8";
                break;
            case U2FeatureType::Variation:
                generatedColor = "#e32636";
                break;
            case U2FeatureType::ThreePrimeUtr:
                generatedColor = "#ffcde6";
                break;
            case U2FeatureType::FivePrimeUtr:
                generatedColor = "#ffc8c8";
                break;
            case U2FeatureType::RepeatUnit:
                generatedColor = "#ccccff";
                break;
            default:
                auto name = U2FeatureTypes::getVisualName(featureType);
                generatedColor = FeatureColors::genLightColor(name).name();
                break;
        }
        CHECK_EQUAL(color, generatedColor, "CDS");
    }
}

}  // namespace U2
