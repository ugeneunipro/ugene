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

#include "StyleFactory.h"

#include "ProxyStyle.h"

#ifndef Q_OS_DARWIN
#include "DarkStyle.h"
#else
#include "MacStyleFactory.h"
#endif

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#ifdef Q_OS_WIN
#    include <QOperatingSystemVersion>
#    include <QSettings>
#endif


namespace U2 {


QStringList StyleFactory::keys() {
    return QStyleFactory::keys();
}

QStyle* StyleFactory::create(const QString& styleName, ColorMode colorMode) {
    QStyle* result = nullptr;
    QStyle* qtStyle = QStyleFactory::create(styleName);
    auto proxyStyle = new ProxyStyle(qtStyle);

#ifdef Q_OS_DARWIN
    result = proxyStyle;
#else
    switch (colorMode) {
        case ColorMode::Light:
            result = proxyStyle;
            break;
        case ColorMode::Dark:
            result = new DarkStyle(proxyStyle);
            break;
        case ColorMode::Auto:
            if (isDarkStyleEnabled()) {
                result = new DarkStyle(proxyStyle);
            } else {
                result = proxyStyle;
            }
            break;
    }
    // Re-use the original style object name, because it is saved in the settings as a part of 'User preferences'.
    if (qtStyle != nullptr) {
        result->setObjectName(qtStyle->objectName());
    }
#endif
    return result;
}

QStyle* StyleFactory::create(const QString& styleName, int colorMode) {
    auto colorModeEnum = static_cast<ColorMode>(colorMode);
    return create(styleName, colorModeEnum);
}

#ifdef Q_OS_WIN32
namespace {
bool windowsDarkThemeAvailable() {
    // dark mode supported Windows 10 1809 10.0.17763 onward
    // https://stackoverflow.com/questions/53501268/win10-dark-theme-how-to-use-in-winapi
    if (QOperatingSystemVersion::current().majorVersion() == 10) {
        return QOperatingSystemVersion::current().microVersion() >= 17763;
    } else if (QOperatingSystemVersion::current().majorVersion() > 10) {
        return true;
    } else {
        return false;
    }
}

bool windowsIsInDarkTheme() {
    CHECK(windowsDarkThemeAvailable(), false);

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}

}  // namespace
#endif

bool StyleFactory::isDarkStyleEnabled() {
#ifdef Q_OS_DARWIN
    return MacStyleFactory::macIsInDarkTheme();
#elif defined(Q_OS_WIN32)
    return windowsIsInDarkTheme();
#else
    FAIL("Auto color mode is not avalible on Linux", false);
#endif
}



}  // namespace U2