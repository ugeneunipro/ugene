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

#ifndef _U2_COLOR_SCHEME_UTILS_H_
#define _U2_COLOR_SCHEME_UTILS_H_

#include <U2Core/global.h>

#include "MsaColorScheme.h"

namespace U2 {

class U2ALGORITHM_EXPORT ColorSchemeUtils {
public:
    static bool getSchemaColors(ColorSchemeData &customScheme);
    static QList<ColorSchemeData> getSchemas();
    static QString getColorsDir();
    static void getDefaultUgeneColors(DNAAlphabetType type, QMap<char, QColor>& alphColors);
    static QMap<char, QColor> getDefaultSchemaColors(DNAAlphabetType type, bool defaultAlpType);
    static void setColorsDir(const QString &colorsDir);
    static void fillEmptyColorScheme(QVector<QColor> &colorsPerChar);

    static const QString COLOR_SCHEME_AMINO_KEYWORD;
    static const QString COLOR_SCHEME_NUCL_KEYWORD;
    static const QString COLOR_SCHEME_NUCL_DEFAULT_KEYWORD;
    static const QString COLOR_SCHEME_NUCL_EXTENDED_KEYWORD;

    static const QString COLOR_SCHEME_NAME_FILTERS;
    static const QString COLOR_SCHEME_SETTINGS_ROOT;
    static const QString COLOR_SCHEME_SETTINGS_SUB_DIRECTORY;
    static const QString COLOR_SCHEME_COLOR_SCHEMA_DIR;
};

}   // namespace U2

#endif // _U2_COLOR_SCHEME_UTILS_H_
