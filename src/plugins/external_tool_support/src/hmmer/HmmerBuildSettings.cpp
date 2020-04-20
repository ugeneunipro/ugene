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

#include <U2Core/U2SafePoints.h>

#include "HmmerBuildSettings.h"

namespace U2 {

HmmerBuildSettings::HmmerBuildSettings()
    : modelConstructionStrategy(p7_ARCH_FAST),
      relativeSequenceWeightingStrategy(p7_WGT_PB),
      effectiveSequenceWeightingStrategy(p7_EFFN_ENTROPY),
      eset(-1.0),
      seed(42),
      symfrac(0.5),
      fragtresh(0.5),
      wid(0.62),
      ere(-1.0),
      esigma(45.0),
      eid(0.62),
      eml(200),
      emn(200),
      evl(200),
      evn(200),
      efl(100),
      efn(200),
      eft(0.04)
{

}

bool HmmerBuildSettings::validate() const {
    CHECK(0 <= symfrac && symfrac <= 1, false);
    CHECK(0 <= wid && wid <= 1, false);
    CHECK(0 < eset || effectiveSequenceWeightingStrategy != p7_EFFN_SET, false);
    CHECK(-1 == ere || 0 < ere, false);
    CHECK(0 <= fragtresh && fragtresh <= 1, false);
    CHECK(0 < esigma, false);
    CHECK(0 <= eid && eid <= 1, false);
    CHECK(0 < eml, false);
    CHECK(0 < emn, false);
    CHECK(0 < evl, false);
    CHECK(0 < evn, false);
    CHECK(0 < efl, false);
    CHECK(0 < efn, false);
    CHECK(0 < wid && wid < 1, false);
    CHECK(0 <= seed, false);

    return true;
}

}   // namespace U2
