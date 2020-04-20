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

#include <QColor>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>

#include "ColorSchemeUtils.h"

namespace U2 {

const QString ColorSchemeUtils::COLOR_SCHEME_AMINO_KEYWORD = "AMINO";
const QString ColorSchemeUtils::COLOR_SCHEME_NUCL_KEYWORD = "NUCL";
const QString ColorSchemeUtils::COLOR_SCHEME_NUCL_DEFAULT_KEYWORD = "NUCL_DEFAULT";
const QString ColorSchemeUtils::COLOR_SCHEME_NUCL_EXTENDED_KEYWORD = "NUCL_EXTENDED";

const QString ColorSchemeUtils::COLOR_SCHEME_NAME_FILTERS = ".csmsa";
const QString ColorSchemeUtils::COLOR_SCHEME_SETTINGS_ROOT = "/color_schema_settings/";
const QString ColorSchemeUtils::COLOR_SCHEME_SETTINGS_SUB_DIRECTORY = "MSA_schemes";
const QString ColorSchemeUtils::COLOR_SCHEME_COLOR_SCHEMA_DIR = "colors_scheme_dir";

namespace {

bool lineValid(const QStringList &properties, const QMap<char, QColor> &alphColors) {
    if (properties.size() != 2) {
        return false;
    }

    if (properties[0].size() != 1 || (!alphColors.contains(properties[0][0].toLatin1()))) {
        return false;
    }

    if (!QColor(properties[1]).isValid()) {
        return false;
    }

    return true;
}

QByteArray uniteAlphabetChars(const QByteArray &firstAlphabetChars, const QByteArray &secondAlphabetChars) {
    QByteArray unitedAlphabetChars = firstAlphabetChars;
    for (int i = 0; i < secondAlphabetChars.size(); ++i) {
        if (!unitedAlphabetChars.contains(secondAlphabetChars[i])) {
            unitedAlphabetChars.append(secondAlphabetChars[i]);
        }
    }
    qSort(unitedAlphabetChars.begin(), unitedAlphabetChars.end());
    return unitedAlphabetChars;
}

}

bool ColorSchemeUtils::getSchemaColors(ColorSchemeData &customScheme) {
    QMap<char, QColor> &alphColors = customScheme.alpColors;
    const QString &file = customScheme.name + COLOR_SCHEME_NAME_FILTERS;
    DNAAlphabetType &type = customScheme.type;
    bool &defaultAlpType = customScheme.defaultAlpType = true;

    QString dirPath = getColorsDir();
    QDir dir(dirPath);
    if (!dir.exists()) {
        coreLog.info(QString("%1: no such folder").arg(dirPath));
        return false;
    }

    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);

    QScopedPointer<IOAdapter> io(factory->createIOAdapter());
    if (!io->open(dir.filePath(file), IOAdapterMode_Read)) {
        coreLog.info(QString("%1: no such scheme").arg(customScheme.name));
        return false;
    }

    while (!io->isEof()) {
        const int max_size = 1024;

        QByteArray byteLine(max_size + 1, 0);
        int lineLength = io->readLine(byteLine.data(), max_size);
        QString line(byteLine);
        line.remove(lineLength, line.size() - lineLength);
        if (line.isEmpty()) {
            continue;

        }

        if (line == COLOR_SCHEME_AMINO_KEYWORD) {
            type = DNAAlphabet_AMINO;
        } else if (line.contains(COLOR_SCHEME_NUCL_KEYWORD)) {
            type = DNAAlphabet_NUCL;
            if (line == COLOR_SCHEME_NUCL_DEFAULT_KEYWORD) {
                defaultAlpType = true;
            } else if(line == COLOR_SCHEME_NUCL_EXTENDED_KEYWORD) {
                defaultAlpType = false;
            } else {
                coreLog.info(QString("%1: mode of nucleic alphabet of scheme not defined, use default mode").arg(customScheme.name));
            }
        } else {
            coreLog.info(QString("%1: alphabet of scheme not defined").arg(customScheme.name));
            return false;
        }

        alphColors = getDefaultSchemaColors(type, defaultAlpType);
        break;
    }

    QMap<char, QColor> tmpHelper;
    while (!io->isEof()) {
        const int max_size = 1024;

        QByteArray byteLine(max_size + 1, 0);
        int lineLength = io->readLine(byteLine.data(), max_size);
        QString line(byteLine);
        line.remove(lineLength, line.size() - lineLength);
        if (line.isEmpty()) {
            continue;

        }
        QStringList properties = line.split(QString("="), QString::SkipEmptyParts);

        if (!lineValid(properties, alphColors)) {
            coreLog.info(QString("%1: scheme is not valid").arg(customScheme.name));
            return false;
        }

        tmpHelper[properties.first().at(0).toLatin1()] = QColor(properties[1]);
    }

    QMapIterator<char, QColor> it(tmpHelper);
    while (it.hasNext()) {
        it.next();
        alphColors[it.key()] = it.value();
    }

    return true;
}

QList<ColorSchemeData> ColorSchemeUtils::getSchemas() {
    QList<ColorSchemeData> customSchemas;

    QDir dir(getColorsDir());
    if (!dir.exists()) {
        return QList<ColorSchemeData>();
    }

    QStringList filters;
    filters.append(QString("*%1").arg(COLOR_SCHEME_NAME_FILTERS));

    QStringList schemaFiles = dir.entryList(filters);
    foreach (const QString &schemaName, schemaFiles) {
        ColorSchemeData schema;
        schema.name = schemaName.split(".").first();
        bool ok = getSchemaColors(schema);
        if (!ok) {
            continue;
        }
        customSchemas.append(schema);
    }
    return customSchemas;
}

QString ColorSchemeUtils::getColorsDir() {
    QString settingsFile = AppContext::getSettings()->fileName();
    QString settingsDir = QDir(QFileInfo(settingsFile).absolutePath()).filePath(COLOR_SCHEME_SETTINGS_SUB_DIRECTORY);

    QString res = AppContext::getSettings()->getValue(COLOR_SCHEME_SETTINGS_ROOT + COLOR_SCHEME_COLOR_SCHEMA_DIR, settingsDir, true).toString();

    return res;
}

void ColorSchemeUtils::getDefaultUgeneColors(DNAAlphabetType type, QMap<char, QColor> &alphColors) {
    if (type == DNAAlphabet_AMINO) {
        alphColors['I'] = "#ff0000";
        alphColors['V'] = "#f60009";
        alphColors['L'] = "#ea0015";
        alphColors['F'] = "#cb0034";
        alphColors['C'] = "#c2003d";
        alphColors['M'] = "#b0004f";
        alphColors['A'] = "#ad0052";
        alphColors['G'] = "#6a0095";
        alphColors['X'] = "#680097";
        alphColors['T'] = "#61009e";
        alphColors['S'] = "#5e00a1";
        alphColors['W'] = "#5b00a4";
        alphColors['Y'] = "#4f00b0";
        alphColors['P'] = "#4600b9";
        alphColors['H'] = "#1500ea";
        alphColors['E'] = "#0c00f3";
        alphColors['Z'] = "#0c00f3";
        alphColors['Q'] = "#0c00f3";
        alphColors['D'] = "#0c00f3";
        alphColors['B'] = "#0c00f3";
        alphColors['N'] = "#0c00f3";
        alphColors['K'] = "#0000ff";
        alphColors['R'] = "#0000ff";
    } else if (type == DNAAlphabet_NUCL) {
        alphColors['A'] = "#FCFF92"; // yellow
        alphColors['C'] = "#70F970"; // green
        alphColors['T'] = "#FF99B1"; // light red
        alphColors['G'] = "#4EADE1"; // light blue
        alphColors['U'] = alphColors['T'].lighter(120);
        alphColors['N'] = "#FCFCFC";
    }
}

QMap<char, QColor> ColorSchemeUtils::getDefaultSchemaColors(DNAAlphabetType type, bool defaultAlpType) {
    QList<const DNAAlphabet *> alphabets = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets();
    QMap<DNAAlphabetType, QByteArray > alphabetChars;
    foreach (const DNAAlphabet* alphabet, alphabets){ // default initialization
        if (defaultAlpType == alphabet->isDefault()) {
            alphabetChars[alphabet->getType()] = uniteAlphabetChars(alphabetChars.value(alphabet->getType()) ,alphabet->getAlphabetChars());
        }
    }

    QMapIterator<DNAAlphabetType, QByteArray> it(alphabetChars);
    QByteArray alphabet;
    while (it.hasNext()) {
        it.next();
        if (it.key() == type) {
            alphabet = it.value();
            break;
        }
    }

    QMap<char, QColor> alphColors;
    for (int i = 0; i < alphabet.size(); ++i) {
        alphColors[alphabet[i]] = QColor(Qt::white);
    }

    getDefaultUgeneColors(type, alphColors);
    return alphColors;
}

void ColorSchemeUtils::setColorsDir(const QString &colorsDir) {
    QString settingsFile = AppContext::getSettings()->fileName();
    QString settingsDir = QFileInfo(settingsFile).absolutePath();
    QString finalColorDir = colorsDir;
    QFileInfo info(colorsDir);
    if (!info.isDir()) {
        finalColorDir = info.dir().absolutePath();
        coreLog.trace(QString("%1: the file location was trimmed to the file directory.").arg(colorsDir));
    }
    if (settingsDir != finalColorDir) {
        AppContext::getSettings()->setValue(COLOR_SCHEME_SETTINGS_ROOT + COLOR_SCHEME_COLOR_SCHEMA_DIR, finalColorDir, true);
    }
}

void ColorSchemeUtils::fillEmptyColorScheme(QVector<QColor> &colorsPerChar) {
    colorsPerChar.fill(QColor(), 256);
}


}   // namespace U2
