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

#pragma once

#include <QObject>
#include <QStyleFactory>
#include <QTimer>

namespace U2 {

class MainWindowImpl;

// Creates a style by given parameters
class StyleFactory : public QObject {
public:
    StyleFactory(MainWindowImpl* parent);

    // Supported color themes
    enum class ColorTheme {
        Light,
        Dark,
        Auto
    };

    // Create style by style name and color theme
    // styleName - name of the style, e.g. "Fusion", "Windows", "Macintosh"
    // colorThemeIndex - index of color theme, e.g. 0 - Light, 1 - Dark, 2 - Auto
    QStyle* createNewStyle(const QString& styleName, int colorThemeIndex);

    // Apply color scheme if it was changed by system
    // This function is called for macOS only because this is the only system
    // which triggers the event when system color scheme is changed
    // TODO: replace with QStyleHints::colorSchemeChanged signal when Qt is upgraded to 6.5+
    void applyAutomaticallyChangedColorSchemeForMacOs();

    // Returns true if current theme is dark
    bool isDarkTheme() const;

    // Returns the index of color theme which should be used now
    int getNewColorThemeIndex() const;

    // Returns the name of visual style which should be used now
    QString getNewVisualStyleName(int newColorThemeIndex) const;

    // True if auto style avaliable
    // Not avaliable on macOS early than 10.14 and
    // Windows early than 10 1809 10.0.17763
    static bool isAutoStyleAvaliable();

private:
    void applyAutomaticallyChangedColorScheme();

    // True if dark style enabled
    static bool isDarkStyleEnabled();

    // Create style by style name and color theme
    static QStyle* create(const QString& styleName, ColorTheme colorTheme);
    // Create style by style name and color theme
    static QStyle* create(const QString& styleName, int colorTheme);

    bool isDark = false;
#ifdef Q_OS_DARWIN
    bool colorIsChangedByUser = false;
#endif
#ifdef Q_OS_WIN
    QTimer colorThemeTimer;
#endif

};

}
