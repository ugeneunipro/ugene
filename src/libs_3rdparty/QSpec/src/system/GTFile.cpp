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

#include <system/GTClipboard.h>

#include <QDebug>
#include <QDir>

#include "system/GTFile.h"

#ifdef Q_OS_LINUX
// We using c++14, while <filesystem> is in c++17, so using experimental version.
// TODO: check on Windows & Mac.
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#ifdef Q_OS_WIN

// Microsoft's example of how to change file system permissions is used below.
// See http://msdn.microsoft.com/en-us/library/windows/desktop/aa379283%28v=vs.85%29.aspx for details.

#    include <Aclapi.h>

// Sets NTFS security rights of the current user for the specified file or directory,
// either to allow or deny writing to the file or creating files/subfolders in the directory.
// In case of a directory, rights to delete files/subfolders are not affected.
// NB: Only works for NTFS, has no effect on FAT objects.
static DWORD SetFileWriteACL(
    LPTSTR pszObjName,  // name of object
    BOOL allowWrite  // zero to deny file modification
) {
    DWORD dwRes = 0;
    PACL pNewDACL = NULL;
    EXPLICIT_ACCESS ea[2];
    SE_OBJECT_TYPE ObjectType = SE_FILE_OBJECT;  // type of object
    TRUSTEE_FORM TrusteeForm = TRUSTEE_IS_NAME;  // format of trustee structure

#    ifdef UNICODE
    LPWSTR pszTrustee = const_cast<LPWSTR>(L"CURRENT_USER");  // trustee for new ACE
#    else
    LPSTR pszTrustee = const_cast<LPSTR>("CURRENT_USER");
#    endif

    if (NULL == pszObjName) {
        return ERROR_INVALID_PARAMETER;
    }

    // Initialize an EXPLICIT_ACCESS structure for the new ACE.

    ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessMode = GRANT_ACCESS;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TrusteeForm;
    ea[0].Trustee.ptstrName = pszTrustee;

    ULONG aclCount;
    if (allowWrite) {
        aclCount = 1;
        ea[0].grfAccessPermissions = FILE_ALL_ACCESS;
    } else {
        aclCount = 2;
        ea[0].grfAccessPermissions = (FILE_GENERIC_READ | GENERIC_EXECUTE);

        // All generic rights have common bits (READ_CONTROL etc) and we do not want to deny them
        ea[1].grfAccessPermissions = (FILE_GENERIC_WRITE & ~FILE_GENERIC_READ);
        ea[1].grfAccessMode = DENY_ACCESS;
        ea[1].grfInheritance = NO_INHERITANCE;
        ea[1].Trustee.TrusteeForm = TrusteeForm;
        ea[1].Trustee.ptstrName = pszTrustee;
    }

    // Create a new ACL
    dwRes = SetEntriesInAcl(aclCount, ea, NULL, &pNewDACL);
    if (ERROR_SUCCESS != dwRes) {
        printf("SetEntriesInAcl Error %u\n", dwRes);
        goto Cleanup;
    }

    // Attach the new ACL as the object's DACL.
    dwRes = SetNamedSecurityInfo(pszObjName, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
    if (ERROR_SUCCESS != dwRes) {
        printf("SetNamedSecurityInfo Error %u\n", dwRes);
        goto Cleanup;
    }

Cleanup:

    if (pNewDACL != NULL) {
        LocalFree(static_cast<HLOCAL>(pNewDACL));
    }

    return dwRes;
}

#endif

namespace HI {

static const QFile::Permissions GenericReadPermissions = QFile::ReadOwner | QFile::ExeOwner |
                                                         QFile::ReadUser | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup |
                                                         QFile::ReadOther | QFile::ExeOther;

static const QFile::Permissions GenericWritePermissions = QFile::WriteOwner |
                                                          QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther;

static bool setFilePermissions(const QString& path, bool allowWrite, bool recursive) {
    QFileInfo fileInfo(path);
    if (!(fileInfo.exists())) {
        return false;
    }
    if (fileInfo.isSymLink()) {
        return false;
    }

    bool res = true;
    if (recursive && fileInfo.isDir()) {
        QDir dir(path);
        foreach (const QString& entryPath, dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks)) {
            res &= setFilePermissions(path + "/" + entryPath, allowWrite, recursive);
        }
    }

    QFile::Permissions perm = GenericReadPermissions;
    if (allowWrite) {
        perm |= GenericWritePermissions;
    }
    // On Windows, Qt permissions partially work (e.g. FAT respects read-only attribute for ordinary files),
    // so try it anyway.
    // And the case of FAT folders is hopeless, there's no way to control access to a folder content.
    bool qtRes = QFile(path).setPermissions(perm);

#ifdef Q_OS_WIN
    // Probably, could skip this NTFS-specific branch if the qtRes is OK, assuming it worked nicely on FAT
    // But Qt did not really documented this, so let's go for the most guarantee
    if (fileInfo.isRelative() && !fileInfo.makeAbsolute()) {
        return false;
    }
    const QString windowsPath = QDir::toNativeSeparators(fileInfo.filePath());

    const int pathLength = windowsPath.size();
    QScopedArrayPointer<wchar_t> pathString(new wchar_t[pathLength + 1]);

    windowsPath.toWCharArray(pathString.data());
    pathString[pathLength] = '\0';

    DWORD dwRes = SetFileWriteACL(pathString.data(), allowWrite);
    if (allowWrite) {
        // workaround Qt's quirk on NTFS, force clearing RO attr
        SetFileAttributesW(/*reinterpret_cast<LPCWSTR>*/ (pathString.data()), FILE_ATTRIBUTE_NORMAL);
    }

    qtRes = (ERROR_SUCCESS == dwRes);
#endif

    return res & qtRes;
}

#define GT_CLASS_NAME "GTFile"

#define GT_METHOD_NAME "setReadWrite"
void GTFile::setReadWrite(const QString& path, bool recursive) {
    waitForFile(path);
    bool set = setFilePermissions(path, true, recursive);
    GT_CHECK(set, "read-write permission could not be set")
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReadOnly"
void GTFile::setReadOnly(const QString& path, bool recursive) {
    waitForFile(path);
    bool set = setFilePermissions(path, false, recursive);
    GT_CHECK(set, "read-only permission could not be set")
}
#undef GT_METHOD_NAME

const QString GTFile::backupPostfix = "_GT_backup";

#define GT_METHOD_NAME "equals"
bool GTFile::equals(const QString& path1, const QString& path2, bool simplified) {
    waitForFile(path1);
    waitForFile(path2);

    QFile f1(path1);
    QFile f2(path2);

    bool ok = f1.open(QIODevice::ReadOnly) && f2.open(QIODevice::ReadOnly);
    GT_CHECK_RESULT(ok, f1.errorString() + " " + f2.errorString(), false);

    QByteArray byteArray1 = f1.readAll();
    QByteArray byteArray2 = f2.readAll();
    if (simplified) {
        byteArray1 = byteArray1.simplified();
        byteArray2 = byteArray2.simplified();
    }

    GT_CHECK_RESULT((f1.error() == QFile::NoError && f2.error() == QFile::NoError), f1.errorString() + " " + f2.errorString(), false);
    return byteArray1 == byteArray2;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "equals"
bool GTFile::equals(const QString& path1) {
    QFile f1(path1);

    bool ok = f1.open(QIODevice::ReadOnly);
    GT_CHECK_RESULT(ok, f1.errorString(), false);

    QByteArray byteArray1 = f1.readAll();
    QByteArray byteArray2 = GTClipboard::text().toLocal8Bit();

    GT_CHECK_RESULT(f1.error() == QFile::NoError, f1.errorString(), false);

    return byteArray1 == byteArray2;
}
#undef GT_METHOD_NAME
#define GT_METHOD_NAME "getSize"
qint64 GTFile::getSize(const QString& path) {
    QFile file(path);
    bool ok = file.open(QIODevice::ReadOnly);
    GT_CHECK_RESULT(ok, "file '" + path + "' is not found", -1);
    return file.size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copy"
void GTFile::copy(const QString& from, const QString& to) {
    QFile f2(to);
    bool ok = f2.open(QIODevice::ReadOnly);
    if (ok) {
        f2.remove();
    }

    bool copied = QFile::copy(from, to);
    GT_CHECK(copied == true, "can't copy <" + from + "> to <" + to + ">");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copyDir"
void GTFile::copyDir(const QString& dirToCopy, const QString& dirToPaste) {
#ifdef Q_OS_LINUX
    QByteArray from = dirToCopy.toLocal8Bit();
    QByteArray to = dirToPaste.toLocal8Bit();
    GT_CHECK(!fs::exists(to.constData()), "Target dir is already exists: " + dirToPaste);

    std::error_code errorCode;
    fs::copy_options options = fs::copy_options::recursive;
    fs::copy(from.constData(), to.constData(), options, errorCode);
    GT_CHECK(errorCode.value() == 0, "Failed to copy " + dirToCopy + " to " + dirToPaste);
#else
    QDir from;
    from.setFilter(QDir::Hidden | QDir::AllDirs | QDir::Files);
    from.setPath(dirToCopy);

    QString pastePath = dirToPaste;
    bool ok = QDir().mkpath(pastePath);
    GT_CHECK(ok, "could not create directory: " + pastePath);

    QFileInfoList list = from.entryInfoList();
    foreach (QFileInfo info, list) {
        if (info.fileName() == "." || info.fileName() == "..") {
            continue;
        }
        if (info.isFile()) {
            copy(info.filePath(), pastePath + '/' + info.fileName());
        } else if (info.isDir()) {
            copyDir(info.filePath(), pastePath + '/' + info.fileName());
        }
    }
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeDir"
void GTFile::removeDir(const QString& dirPath) {
    QDir dir(dirPath);
    qDebug("GT_DEBUG_MESSAGE removing dir: %s", dirPath.toLocal8Bit().constData());

    bool isOsWindows;
#ifdef Q_OS_WIN
    isOsWindows = true;
#else
    isOsWindows = false;
#endif
    if (isOsWindows) {
        dir.setFilter(QDir::Hidden | QDir::AllDirs | QDir::Files);
        foreach (QFileInfo fileInfo, dir.entryInfoList()) {
            QString fileName = fileInfo.fileName();
            QString filePath = fileInfo.filePath();
            if (fileName != "." && fileName != "..") {
                QFile file(filePath);
                file.setPermissions(QFile::ReadOther | QFile::WriteOther);
                if (!file.remove(filePath)) {
                    QDir fileDir(dirPath);
                    if (!fileDir.rmdir(filePath)) {
                        removeDir(filePath);
                    }
                }
            }
        }
    } else {
        foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden)) {
            QString fileName = fileInfo.fileName();
            QString filePath = fileInfo.filePath();
            if (fileName != "." && fileName != "..") {
                if (!QFile::remove(filePath)) {
                    QDir fileDir(filePath);
                    if (!fileDir.rmdir(filePath)) {
                        removeDir(filePath);
                    }
                }
            }
        }
    }
    dir.rmdir(dir.absoluteFilePath(dirPath));
    qDebug("GT_DEBUG_MESSAGE directory removed: %s", dirPath.toLocal8Bit().constData());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "backup"
void GTFile::backup(const QString& path) {
    qDebug("Backup file %s", path.toLocal8Bit().constData());
    if (QFile(path).exists()) {
        copy(path, path + backupPostfix);
    } else {
        qDebug("Failed to backup. Files does not exist: %s", path.toLocal8Bit().constData());
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "backupDir"
void GTFile::backupDir(const QString& path) {
    copyDir(path, path + backupPostfix);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "restore"
void GTFile::restore(const QString& path) {
    qDebug("Restoring file %s", path.toLocal8Bit().constData());
    QFile backupFile(path + backupPostfix);

    bool ok = backupFile.open(QIODevice::ReadOnly);
    GT_CHECK(ok, "There is no backup file for <" + path + ">");

    QFile file(path);
    ok = file.open(QIODevice::ReadOnly);
    if (ok) {
        file.remove();
    }

    bool renamed = backupFile.rename(path);
    GT_CHECK(renamed == true, "restore of <" + path + "> can't be done");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "restoreDir"
void GTFile::restoreDir(const QString& path) {
    QDir backupDir(path + backupPostfix);
    bool exists = backupDir.exists();
    if (!exists) {
        return;
    }

    QDir dir(path);
    exists = dir.exists();
    if (exists) {
        removeDir(dir.absolutePath());
    }

    bool renamed = backupDir.rename(path + backupPostfix, path);
    GT_CHECK(renamed == true, "restore of <" + path + "> can't be done");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "check"
bool GTFile::check(const QString& path) {
    QFile file(path);
    return file.exists();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "waitForFile"
void GTFile::waitForFile(const QString& path, int timeout) {
    for (int time = 0; time < timeout && !check(path); time += 500) {
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "create"
void GTFile::create(const QString& filePath) {
    bool success = QFile(filePath).open(QIODevice::WriteOnly);
    GT_CHECK(success, "Can't open file to write");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "readAll"
QByteArray GTFile::readAll(const QString& filePath) {
    QFile file(filePath);
    bool opened = file.open(QFile::ReadOnly);
    GT_CHECK_RESULT(opened, "Can't open file for read: '" + filePath + "'", "");

    return file.readAll();
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace HI
