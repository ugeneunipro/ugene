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

#ifdef Q_OS_DARWIN
#    include "MacStyleFactory.h"
#else
#    ifdef Q_OS_WIN
#        include "WinStyleFactory.h"
#        include <windows.h>
#    endif
#    include "DarkStyle.h"
#endif

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

#include "main_window/MainWindowImpl.h"

namespace U2 {

StyleFactory::StyleFactory(MainWindowImpl* parent)
    : QObject(parent), QAbstractNativeEventFilter() {

    if (isOsWindows()) {
        // Now the only way to track system color theme changed on Windows is
        // to periodically check the corresponding registry value.
        // TODO: replace with QStyleHints::colorSchemeChanged signal when Qt is upgraded to 6.5+
        qApp->installNativeEventFilter(this);
    }
}

StyleFactory::~StyleFactory() {
    if (isOsWindows()) {
        qApp->removeNativeEventFilter(this);
    }
}

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
}

void StyleFactory::syncColorSchemeWithSystem() {
#ifdef Q_OS_DARWIN
    // On macOS this function is triggered on changing color scheme by user too
    CHECK(!colorIsChangedByUser, );
#endif
    auto s = AppContext::getAppSettings()->getUserAppsSettings();
    auto cm = static_cast<StyleFactory::ColorTheme>(s->getColorThemeId());
    CHECK(cm == StyleFactory::ColorTheme::Auto, );
    CHECK(isDark != StyleFactory::isDarkStyleEnabled(), );

    auto mwi = qobject_cast<MainWindowImpl*>(this->parent());
    SAFE_POINT_NN(mwi, );

    mwi->setNewStyle(s->getVisualStyle(), (int)cm);
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
    if (isOsWindows()) {
        // On Windows Dark theme + fusion is better, than Dark mode + windowsvista
        if (newColorThemeIndex == 1) {
            visualStyleName = "fusion";
        }
    }
    return visualStyleName;
}

bool StyleFactory::isAutoStyleAvaliable() {
#ifdef Q_OS_DARWIN
    return MacStyleFactory::macDarkThemeAvailable();
#elif defined(Q_OS_WIN32)
    return WinStyleFactory::windowsDarkThemeAvailable();
#else
    return false;
#endif
}

bool StyleFactory::nativeEventFilter(const QByteArray& eventType, void* message, long* result) {
    // Used for Windows only to detect system color scheme changes
    SAFE_POINT(isOsWindows(), "Should be called on Windows only", false);

    // This function is called periodically by Qt.
    // On windows eventType is "windows_generic_MSG" or "windows_dispatcher_MSG"
    CHECK(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG", false);

    MSG* msg = static_cast<MSG*>(message);
    CHECK(msg != nullptr, false);

    if (msg->message == WM_SETTINGCHANGE && msg->lParam != 0) {
        LPCWSTR param = reinterpret_cast<LPCWSTR>(msg->lParam);
        if (wcscmp(param, L"ImmersiveColorSet") == 0) {
            syncColorSchemeWithSystem();
        }
    }
    return false;
}

bool StyleFactory::isDarkStyleEnabled() {
#ifdef Q_OS_DARWIN
    return MacStyleFactory::macIsInDarkTheme();
#elif defined(Q_OS_WIN32)
    return WinStyleFactory::windowsIsInDarkTheme();
#else
    return false;
#endif
}

QStyle* StyleFactory::create(const QString& styleName, ColorTheme colorTheme) {
    QStyle* result = nullptr;
    QStyle* qtStyle = QStyleFactory::create(styleName);
    auto proxyStyle = new ProxyStyle(qtStyle);

    if (isOsMac()) {
        result = proxyStyle;
    } else {
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
    }
    return result;
}


}  // namespace U2
