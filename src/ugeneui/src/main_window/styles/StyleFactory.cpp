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
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "main_window/MainWindowImpl.h"

#ifdef Q_OS_WIN
#    include <QOperatingSystemVersion>
#    include <QSettings>
#endif


namespace U2 {

StyleFactory::StyleFactory(MainWindowImpl* parent)
    : QObject(parent) {

#ifdef Q_OS_WIN
    // TODO: replace with QStyleHints::colorSchemeChanged signal when Qt is upgraded to 6.5+
    // Now the only way to track system color theme changed on Windows is
    // to periodically check the corresponding registry value.
    connect(&colorThemeTimer, &QTimer::timeout, this, [this]() {
        auto s = AppContext::getAppSettings()->getUserAppsSettings();
        auto cm = static_cast<StyleFactory::ColorTheme>(s->getColorThemeId());
        CHECK(cm == StyleFactory::ColorTheme::Auto, );

        auto mwi = qobject_cast<MainWindowImpl*>(this->parent());
        SAFE_POINT_NN(mwi, );

        mwi->setNewStyle(s->getVisualStyle(), (int)cm);
    });
    colorThemeTimer.start(5000);
#endif
}

//QStringList StyleFactory::keys() {
//    return QStyleFactory::keys();
//}

QStyle* StyleFactory::createNewStyle(const QString& styleName, int colorThemeIndex) {
    auto cm = static_cast<StyleFactory::ColorTheme>(colorThemeIndex);
#ifdef Q_OS_DARWIN
    colorIsChangedByUser = true;
    switch (cm) {
        case StyleFactory::ColorTheme::Light:
            MacStyleFactory::macSetToLightTheme();
            isDark = false;
            break;
        case StyleFactory::ColorTheme::Dark:
            MacStyleFactory::macSetToDarkTheme();
            isDark = true;
            break;
        case StyleFactory::ColorTheme::Auto:
            MacStyleFactory::macSetToAutoTheme();
            isDark = StyleFactory::isDarkStyleEnabled();
            break;
    }
    colorIsChangedByUser = false;
#else
    switch (cm) {
        case StyleFactory::ColorTheme::Light:
            isDark = false;
            break;
        case StyleFactory::ColorTheme::Dark:
            isDark = true;
            break;
        case StyleFactory::ColorTheme::Auto:
            CHECK(isDark != StyleFactory::isDarkStyleEnabled(), nullptr);

            isDark = StyleFactory::isDarkStyleEnabled();
            break;
    }
#endif
    return StyleFactory::create(styleName, cm);
    //QApplication::setStyle(newStyle);
}

void StyleFactory::applyNewColorScheme() {
#ifdef Q_OS_DARWIN
    CHECK(!colorIsChangedByUser, );

    auto s = AppContext::getAppSettings()->getUserAppsSettings();
    auto cm = static_cast<StyleFactory::ColorTheme>(s->getColorThemeId());
    CHECK(cm == StyleFactory::ColorTheme::Auto, );

    auto mwi = qobject_cast<MainWindowImpl*>(this->parent());
    SAFE_POINT_NN(mwi, );

    mwi->setNewStyle(s->getVisualStyle(), (int)cm);
#endif
}

bool StyleFactory::isDarkTheme() const {
    return isDark;
}

int StyleFactory::getNewColorThemeIndex() const {
    int colorThemeIndex = 0;
    if (!isDark) {
        colorThemeIndex = 1;
    }

    return colorThemeIndex;
}

QString StyleFactory::getNewVisualStyleName(int newColorThemeIndex) const {
    auto s = AppContext::getAppSettings()->getUserAppsSettings();
    auto visualStyleName = s->getVisualStyle();
#ifdef Q_OS_WIN
    // On Windows Dark theme + fusion is better, than Dark mode + windowsvista
    if (newColorThemeIndex == 1) {
        visualStyleName = "fusion";
    }
#endif
    return visualStyleName;
}

#ifdef Q_OS_WIN32
namespace {
bool windowsDarkThemeAvailable() {
    // dark theme supported Windows 10 1809 10.0.17763 onward
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

bool StyleFactory::isDarkStyleAvaliable() {
#ifdef Q_OS_DARWIN
    return MacStyleFactory::macDarkThemeAvailable();
#elif defined(Q_OS_WIN32)
    return windowsDarkThemeAvailable();
#else
    return false;
#endif
}

bool StyleFactory::isDarkStyleEnabled() {
#ifdef Q_OS_DARWIN
    return MacStyleFactory::macIsInDarkTheme();
#elif defined(Q_OS_WIN32)
    return windowsIsInDarkTheme();
#else
    return false;
#endif
}

QStyle* StyleFactory::create(const QString& styleName, ColorTheme colorTheme) {
    QStyle* result = nullptr;
    QStyle* qtStyle = QStyleFactory::create(styleName);
    auto proxyStyle = new ProxyStyle(qtStyle);

#ifdef Q_OS_DARWIN
    result = proxyStyle;
#else
    switch (colorTheme) {
        case ColorTheme::Light:
            result = proxyStyle;
            break;
        case ColorTheme::Dark:
            result = new DarkStyle(proxyStyle);
            break;
        case ColorTheme::Auto:
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

QStyle* StyleFactory::create(const QString& styleName, int colorTheme) {
    auto colorThemeEnum = static_cast<ColorTheme>(colorTheme);
    return create(styleName, colorThemeEnum);
}



}  // namespace U2
