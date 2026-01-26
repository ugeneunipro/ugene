/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <U2Core/global.h>

namespace U2 {

// This class contains policies related to file dialogs.
class U2GUI_EXPORT U2FileDialogPolicy {
public:
    // Check if we need to use non-native file dialog due to known issues with some 3rd party software.
    static void checkIfShouldUseNonNativeDialog();

    // Detects known malicious software that may interfere with native file dialogs.
    // @return map where key is the software name, value is the detected version.
    static QMap<QString, QString> detectMaliciousSoftwareInstalled();

private:
    // Get version of the library.
    // NOTE: implemented on Windows only now, add other OSs support if required
    // @param path - path to the library file.
    // @return version in format "major.minor.patch.build" or empty string if version cannot be detected.
    static QString getFileVersion(const QString& path);

    // Checks if the version of the library is less or equal than the target version.
    // @param considered - version to be compared.
    // @param target - target version.
    // @param orEqual - if true, the equality is considered.
    // @return true if considered version is less or equal than target version, false otherwise.
    static int versionLessThan(const QString& considered, const QString& target, bool orEqual = true);

    // Detects the version of Aspera Connect library.
    // @param outVersion - output parameter to store the detected version.
    // @return true if Aspera Connect library is found and version is detected, false otherwise.
    static bool detectAsperaVersion(QString& outVersion);

};

}  // namespace U2
