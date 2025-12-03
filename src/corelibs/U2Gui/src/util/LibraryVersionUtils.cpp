/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "LibraryVersionUtils.h"

#ifdef Q_OS_WIN
#    include <windows.h>

#endif  // Q_OS_WIN

#include <QStringList>
#include <QFileInfo>

#include <U2Core/U2SafePoints.h>

namespace U2 {

QString LibraryVersionUtils::getFileVersion(const QString& path) {
#ifdef Q_OS_WIN
    std::wstring wpath = path.toStdWString();

    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(wpath.c_str(), &handle);
    if (size == 0)
        return QString();

    QByteArray buffer;
    buffer.resize(int(size));

    if (!GetFileVersionInfoW(wpath.c_str(), 0, size, buffer.data()))
        return QString();

    VS_FIXEDFILEINFO* info = nullptr;
    UINT len = 0;

    if (!VerQueryValueW(buffer.data(), L"\\", (LPVOID*)&info, &len) || !info)
        return QString();

    DWORD ms = info->dwFileVersionMS;
    DWORD ls = info->dwFileVersionLS;

    return QString("%1.%2.%3.%4")
        .arg(HIWORD(ms))
        .arg(LOWORD(ms))
        .arg(HIWORD(ls))
        .arg(LOWORD(ls));
#else
    Q_UNUSED(path);
    FAIL("File version detection is not implemented for this OS", );
#endif
}

int LibraryVersionUtils::versionLessThan(const QString& considered, const QString& target, bool orEqual) {
    QStringList consideredSplit = considered.split('.');
    QStringList targetSplit = target.split('.');

    while (consideredSplit.size() < 4) {
        consideredSplit << "0";
    }
    while (targetSplit.size() < 4) {
        targetSplit << "0";
    }

    for (int i = 0; i < 4; ++i) {
        int consideredI = consideredSplit[i].toInt();
        int targetI = targetSplit[i].toInt();
        if (consideredI < targetI) {
            return true;
        }
        if (consideredI > targetI) {
            return false;
        }
    }
    return orEqual;
}

bool LibraryVersionUtils::detectAsperaVersion(QString& outVersion) {
    QString userProfile = qEnvironmentVariable("USERPROFILE");

    QList<QString> candidates = {
        "C:/Program Files/IBM/Aspera Connect/bin/asdrive64.dll",
        "C:/Program Files (x86)/IBM/Aspera Connect/bin/asdrive64.dll",
        "C:/Program Files/IBM/Aspera Connect/bin/asdrive.dll",
        "C:/Program Files (x86)/IBM/Aspera Connect/bin/asdrive.dll",
    };

    if (!userProfile.isEmpty()) {
        candidates << userProfile + "/AppData/Local/Programs/IBM/Aspera Connect/bin/asdrive64.dll";
        candidates << userProfile + "/AppData/Local/Programs/IBM/Aspera Connect/bin/asdrive.dll";
    }

    for (const QString& path : candidates) {
        QFileInfo fi(path);
        if (fi.exists() && fi.isFile()) {
            QString ver = getFileVersion(path);
            if (!ver.isEmpty()) {
                outVersion = ver;
                return true;
            }
        }
    }

    return false;
}

}  // namespace U2
