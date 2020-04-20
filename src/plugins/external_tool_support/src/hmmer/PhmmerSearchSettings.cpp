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

#include "PhmmerSearchSettings.h"

namespace U2 {

const double PhmmerSearchSettings::OPTION_NOT_SET = -1.0;

PhmmerSearchSettings::PhmmerSearchSettings()
    : e(10.0),
      t(OPTION_NOT_SET),
      z(OPTION_NOT_SET),
      domE(10.0),
      domT(OPTION_NOT_SET),
      domZ(OPTION_NOT_SET),
      f1(0.02),
      f2(1e-3),
      f3(1e-5),
      doMax(false),
      noBiasFilter(false),
      noNull2(false),
      eml(200),
      emn(200),
      evl(200),
      evn(200),
      efl(100),
      efn(200),
      eft(0.04),
      popen(0.02),
      pextend(0.4),
      seed(42),
      annotationTable(NULL)
{

}

bool PhmmerSearchSettings::validate() const {
    CHECK(0 < e, false);
    CHECK(0 < t || OPTION_NOT_SET == t, false);
    CHECK(0 < z || OPTION_NOT_SET == z, false);
    CHECK(0 < domE, false);
    CHECK(0 < domT || OPTION_NOT_SET == domT, false);
    CHECK(0 < domZ || OPTION_NOT_SET == domZ, false);
    CHECK(0 < eml, false);
    CHECK(0 < emn, false);
    CHECK(0 < evl, false);
    CHECK(0 < evn, false);
    CHECK(0 < efl, false);
    CHECK(0 < efn, false);
    CHECK(0 < eft && eft < 1, false);
    CHECK(0 <= popen && popen < 0.5, false);
    CHECK(0 <= pextend && pextend < 1, false);
    CHECK(0 <= seed, false);
    CHECK(!querySequenceUrl.isEmpty(), false);
    CHECK(!targetSequenceUrl.isEmpty() || NULL != targetSequence, false);

    return true;
}

}   // namespace U2
