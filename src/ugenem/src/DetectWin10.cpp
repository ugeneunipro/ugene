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

#include "DetectWin10.h"

#include <QScopedArrayPointer>

#if defined (Q_OS_WIN)

bool DetectWindowsVersion::isWindows10() {
    OSVERSIONINFO osver;
    detectWinVersion(&osver);
    return osver.dwMajorVersion == 10 && osver.dwMinorVersion == 0;
}

QString DetectWindowsVersion::getVersionString() {
    OSVERSIONINFO osver;
    detectWinVersion(&osver);
    return QString::number(osver.dwMajorVersion) + "." + QString::number(osver.dwMinorVersion);
}

void DetectWindowsVersion::detectWinVersion(OSVERSIONINFO *osver) {
    if (!determineWinOsVersionPost8(osver)) {
        determineWinOsVersionFallbackPost8(osver);
    }
}

// Determine Windows versions >= 8 by querying the version of kernel32.dll.
bool DetectWindowsVersion::determineWinOsVersionPost8(OSVERSIONINFO *result) {
    typedef WORD(WINAPI* PtrGetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
    typedef BOOL(WINAPI* PtrVerQueryValueW)(LPCVOID, LPCWSTR, LPVOID, PUINT);
    typedef BOOL(WINAPI* PtrGetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);

    QSystemLibrary versionLib(QStringLiteral("version"));
    if (!versionLib.load())
        return false;
    PtrGetFileVersionInfoSizeW getFileVersionInfoSizeW = (PtrGetFileVersionInfoSizeW)versionLib.resolve("GetFileVersionInfoSizeW");
    PtrVerQueryValueW verQueryValueW = (PtrVerQueryValueW)versionLib.resolve("VerQueryValueW");
    PtrGetFileVersionInfoW getFileVersionInfoW = (PtrGetFileVersionInfoW)versionLib.resolve("GetFileVersionInfoW");
    if (!getFileVersionInfoSizeW || !verQueryValueW || !getFileVersionInfoW)
        return false;

    const wchar_t kernel32Dll[] = L"kernel32.dll";
    DWORD handle;
    const DWORD size = getFileVersionInfoSizeW(kernel32Dll, &handle);
    if (!size)
        return false;
    QScopedArrayPointer<BYTE> versionInfo(new BYTE[size]);
    if (!getFileVersionInfoW(kernel32Dll, handle, size, versionInfo.data()))
        return false;
    UINT uLen;
    VS_FIXEDFILEINFO *fileInfo = Q_NULLPTR;
    if (!verQueryValueW(versionInfo.data(), L"\\", (LPVOID *)&fileInfo, &uLen))
        return false;
    const DWORD fileVersionMS = fileInfo->dwFileVersionMS;
    const DWORD fileVersionLS = fileInfo->dwFileVersionLS;
    result->dwMajorVersion = HIWORD(fileVersionMS);
    result->dwMinorVersion = LOWORD(fileVersionMS);
    result->dwBuildNumber = HIWORD(fileVersionLS);
    return true;
}

// Fallback for determining Windows versions >= 8 by looping using the
// version check macros. Note that it will return build number=0 to avoid
// inefficient looping.
void DetectWindowsVersion::determineWinOsVersionFallbackPost8(OSVERSIONINFO *result) {
    result->dwBuildNumber = 0;
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);
    OSVERSIONINFOEX checkVersion = { sizeof(OSVERSIONINFOEX), result->dwMajorVersion, 0,
        result->dwBuildNumber, result->dwPlatformId, { '\0' }, 0, 0, 0, 0, 0 };
    for (; VerifyVersionInfo(&checkVersion, VER_MAJORVERSION | VER_PLATFORMID, conditionMask); ++checkVersion.dwMajorVersion)
        result->dwMajorVersion = checkVersion.dwMajorVersion;
    conditionMask = 0;
    checkVersion.dwMajorVersion = result->dwMajorVersion;
    checkVersion.dwMinorVersion = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);
    for (; VerifyVersionInfo(&checkVersion, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, conditionMask); ++checkVersion.dwMinorVersion)
        result->dwMinorVersion = checkVersion.dwMinorVersion;
}

#endif
