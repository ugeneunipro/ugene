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

#ifndef __ASSEMBLY_BROWSER_SETTINGS_H__
#define __ASSEMBLY_BROWSER_SETTINGS_H__

namespace U2 {

class AssemblyBrowserSettings {
public:
    enum OverviewScaleType {
        Scale_Linear,
        Scale_Logarithmic
    };
    static OverviewScaleType getOverviewScaleType();
    static void setOverviewScaleType(OverviewScaleType t);
    
    static bool getShowCoordsOnRuler();
    static void setShowCoordsOnRuler(bool what);

    static bool getShowCoverageOnRuler();
    static void setShowCoverageOnRuler(bool what);

    static bool getReadHintEnabled();
    static void setReadHintEnabled(bool what);

    static bool getOptimizeRenderOnScroll();
    static void setOptimizeRenderOnScroll(bool what);
    
}; // AssemblyBrowserSettings

} // U2

#endif // __ASSEMBLY_BROWSER_SETTINGS_H__
