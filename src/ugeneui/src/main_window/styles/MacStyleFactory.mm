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

#include "MacStyleFactory.h"
#import <Cocoa/Cocoa.h>

#include <U2Core/Log.h>


namespace U2 {

bool MacStyleFactory::macDarkThemeAvailable() {
    if (__builtin_available(macOS 10.14, *)) {
        return true;
    } else {
        return false;
    }
}

bool MacStyleFactory::macIsInDarkTheme() {
    if (!macDarkThemeAvailable()) {
        return false;
    }

    auto appearance = [NSApp.effectiveAppearance bestMatchFromAppearancesWithNames:
            @[ NSAppearanceNameAqua, NSAppearanceNameDarkAqua ]];
    coreLog.info(appearance.UTF8String);
    return [appearance isEqualToString:NSAppearanceNameDarkAqua];
}

void MacStyleFactory::macSetToDarkTheme() {
    if (!macDarkThemeAvailable()) {
        return;
    }

    // https://stackoverflow.com/questions/55925862/how-can-i-set-my-os-x-application-theme-in-code
    [NSApp setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameDarkAqua]];
}

void MacStyleFactory::macSetToLightTheme() {
    if (!macDarkThemeAvailable()) {
        return;
    }

    [NSApp setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameAqua]];
}

void MacStyleFactory::macSetToAutoTheme() {
    if (!macDarkThemeAvailable()) {
        return;
    }

    [NSApp setAppearance:nil];
}

}
