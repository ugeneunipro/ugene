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

#ifndef _DETECT_WIN_10_H_
#define _DETECT_WIN_10_H_

#include <QString>

#if defined (Q_OS_WIN)

#include <windows.h>
#include <winnt.h>

class DetectWindowsVersion {
public:
    static bool isWindows10();
    static QString getVersionString();
private:
    static void detectWinVersion(OSVERSIONINFO *osver);
    static bool determineWinOsVersionPost8(OSVERSIONINFO *result);
    static void determineWinOsVersionFallbackPost8(OSVERSIONINFO *result);
};

//QT class copied from 5.4.0 for WIN 10 correct detection
class QSystemLibrary {
public:
    explicit QSystemLibrary(const QString &libraryName) {
        m_libraryName = libraryName;
        m_handle = 0;
        m_didLoad = false;
    }

    explicit QSystemLibrary(const wchar_t *libraryName) {
        m_libraryName = QString::fromWCharArray(libraryName);
        m_handle = 0;
        m_didLoad = false;
    }

    bool load(bool onlySystemDirectory = true) {
        m_handle = load((const wchar_t *)m_libraryName.utf16(), onlySystemDirectory);
        m_didLoad = true;
        return (m_handle != 0);
    }

    bool isLoaded() {
        return (m_handle != 0);
    }

    QFunctionPointer resolve(const char *symbol) {
        if (!m_didLoad)
            load();
        if (!m_handle)
            return 0;
#ifdef Q_OS_WINCE
        return QFunctionPointer(GetProcAddress(m_handle, (const wchar_t*)QString::fromLatin1(symbol).utf16()));
#else
        return QFunctionPointer(GetProcAddress(m_handle, symbol));
#endif
    }

    static QFunctionPointer resolve(const QString &libraryName, const char *symbol) {
        return QSystemLibrary(libraryName).resolve(symbol);
    }

    static Q_CORE_EXPORT HINSTANCE load(const wchar_t *lpFileName, bool onlySystemDirectory = true);
private:
    HINSTANCE m_handle;
    QString m_libraryName;
    bool m_didLoad;
};

#endif

#endif
