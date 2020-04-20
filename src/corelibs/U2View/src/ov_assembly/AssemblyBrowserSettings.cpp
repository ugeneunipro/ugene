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

#include "AssemblyBrowserSettings.h"

#include <QString>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

namespace U2 {

static const QString SETTINGS_PREFIX("assembly_browser/");
static const QString SCALE_TYPE(SETTINGS_PREFIX + "scale_type");
static const QString SHOW_RULER_COORDS(SETTINGS_PREFIX + "show_coords_on_ruler");
static const QString SHOW_RULER_COVERAGE(SETTINGS_PREFIX + "show_coverage_on_ruler");
static const QString READ_HINT(SETTINGS_PREFIX + "read_hint");
static const QString OPTIMIZE_SCROLL(SETTINGS_PREFIX + "optimize_scroll");

AssemblyBrowserSettings::OverviewScaleType AssemblyBrowserSettings::getOverviewScaleType() {
    return OverviewScaleType(AppContext::getSettings()->getValue(SCALE_TYPE, Scale_Linear).value<int>());
}

void AssemblyBrowserSettings::setOverviewScaleType(OverviewScaleType t) {
    AppContext::getSettings()->setValue(SCALE_TYPE, t);
}

bool AssemblyBrowserSettings::getShowCoordsOnRuler() {
    return AppContext::getSettings()->getValue(SHOW_RULER_COORDS, true).value<bool>();
}

void AssemblyBrowserSettings::setShowCoordsOnRuler(bool what) {
    AppContext::getSettings()->setValue(SHOW_RULER_COORDS, what);
}

bool AssemblyBrowserSettings::getShowCoverageOnRuler() {
    return AppContext::getSettings()->getValue(SHOW_RULER_COVERAGE, true).value<bool>();
}

void AssemblyBrowserSettings::setShowCoverageOnRuler(bool what) {
    AppContext::getSettings()->setValue(SHOW_RULER_COVERAGE, what);
}

bool AssemblyBrowserSettings::getReadHintEnabled() {
    return AppContext::getSettings()->getValue(READ_HINT, true).value<bool>();
}

void AssemblyBrowserSettings::setReadHintEnabled(bool what) {
    AppContext::getSettings()->setValue(READ_HINT, what);
}

bool AssemblyBrowserSettings::getOptimizeRenderOnScroll() {
    return AppContext::getSettings()->getValue(OPTIMIZE_SCROLL, true).value<bool>();
}

void AssemblyBrowserSettings::setOptimizeRenderOnScroll(bool what) {
    AppContext::getSettings()->setValue(OPTIMIZE_SCROLL, what);
}

} // U2
