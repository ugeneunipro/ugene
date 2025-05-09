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

#include "GUrl.h"

#include <QDataStream>
#include <QDir>

#include "U2SafePoints.h"

#ifdef Q_OS_WIN
#    include <windows.h>
#endif

namespace U2 {

static QString makeFilePathCanonical(const QString& originalUrl) {
    // ensure that name is canonical
    QString result = originalUrl.trimmed();

    QString fileUrlPrefix = "file://";
    if (result.startsWith(fileUrlPrefix)) {
        result = result.mid(fileUrlPrefix.length());
#ifdef Q_OS_WIN
        // on Windows, all slashes after "file:" can be trimmed, on Unix/Mac one must be kept to specify that it's an absolute path
        if (result.startsWith("/")) {
            while (result.startsWith("/")) {
                result = result.mid(1);
            }
        } else {
            result.prepend("//");
        }
#endif
    }

    // Windows drive letter, Qt resource designation or Samba share designation and name
    QString prefix;

    if (originalUrl.startsWith(':')) {  // is a Qt resource
        prefix = ":";
        result = result.mid(1);
    } else {
        // https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats#:~:text=%5C%5C.%5CC%3A%5CTest%5CFoo.txt%20%5C%5C%3F%5CC%3A%5CTest%5CFoo.txt
        // Paths starting with "\\.\" and "\\?\" appear to be the same, but Qt treats them differently.
        if (result.startsWith("\\\\?\\")) {
            result[2] = '.';
        }
        result = QFileInfo(result).absoluteFilePath();
    }

    bool isSambaPath = false;
    if (isOsWindows() && result.startsWith("//") && prefix.isEmpty()) {
        // keep Samba share designation
        prefix = "//";
        isSambaPath = true;
    }

    QStringList parts = result.split('/', Qt::SkipEmptyParts);
    if (!parts.empty()) {
        QStringList canonicalParts;
        if (isOsWindows()) {
            // append drive spec letter or Samba server name to the prefix
            if (isSambaPath) {
                prefix += parts.takeFirst();
            } else if (parts.at(0).endsWith(':') && parts.at(0).length() == 2 && prefix.isEmpty()) {  // Windows drive letter designation
                prefix = parts.takeFirst();
            }
        }
        // get rid of redundant '.' and '..' now
        QStringListIterator it(parts);
        while (it.hasNext()) {
            QString part = it.next();
            if (part == ".") {
                continue;
            } else if (part == "..") {
                if (!canonicalParts.isEmpty()) {
                    canonicalParts.removeLast();
                }
            } else if (!part.isEmpty()) {
                canonicalParts.append(part);
            }
        }
        result = prefix + "/" + canonicalParts.join("/");
    }

    return result;
}

GUrlType GUrl::getURLType(const QString& rawUrl) {
    GUrlType result = GUrl_File;
    if (rawUrl.startsWith("http://") || rawUrl.startsWith("https://")) {
        result = GUrl_Http;
    } else if (rawUrl.startsWith("ftp://")) {
        result = GUrl_Ftp;
    } else if (!rawUrl.startsWith("file://") && rawUrl.contains(QRegExp("^([\\.\\w-]+@)?[\\.\\w-]+:\\d*(/[\\w-]*)?$"))) {
        return GUrl_Network;
    } else if (rawUrl.startsWith(U2_VFS_URL_PREFIX)) {
        result = GUrl_VFSFile;
    }
    return result;
}

// constructs url specified by string. The type is parsed
GUrl::GUrl(const QString& _urlString) {
    urlString = _urlString;
    type = getURLType(urlString);
    if (type == GUrl_File) {
        urlString = makeFilePathCanonical(urlString);
    }
}

// constructs url specified by string. The type provided as param
GUrl::GUrl(const QString& _urlString, const GUrlType& _type) {
    urlString = _urlString;
    type = _type;
    if (type == GUrl_File) {
        urlString = makeFilePathCanonical(urlString);
    }
}

bool GUrl::operator==(const GUrl& url) const {
    if(isLocalFile() && url.isLocalFile()) {
        return QFileInfo(urlString) == QFileInfo(url.getURLString());
    } else if(type == GUrl_Http && url.type == GUrl_Http) {
        return urlString.toLower() == url.getURLString().toLower();
    } else {
        return urlString == url.getURLString();
    }    
}

bool GUrl::operator!=(const GUrl& url) const {
    return !(*this == url);
}

static QString path(const GUrl* url) {
    // TODO: parse HTTP and other formats for path part
    QString result;
    if (url->isVFSFile()) {
        return result;
    }
    result = url->getURLString();
    return result;
}

QString GUrl::dirPath() const {
    QString result;
    if (isVFSFile()) {
        return result;
    }
    CHECK(!isNetworkSource(), result);

    result = QFileInfo(path(this)).absoluteDir().absolutePath();
    return result;
}

QString GUrl::fileName() const {
    QString result;
    if (isVFSFile()) {
        return result;
    }
    CHECK(!isNetworkSource(), result);

    result = QFileInfo(path(this)).fileName();
    return result;
}

QString GUrl::baseFileName() const {
    QString result;
    CHECK(!isNetworkSource(), result);

    if (isVFSFile()) {
        QStringList args = urlString.split(U2_VFS_FILE_SEPARATOR, Qt::SkipEmptyParts);
        if (args.size() == 2) {
            result = QFileInfo(args.at(1)).baseName();
            if (!result.length()) {
                result = QFileInfo(args.at(1)).fileName();
            }
        }
    } else {
        result = QFileInfo(path(this)).baseName();
        if (!result.length()) {
            result = QFileInfo(path(this)).fileName();
        }
    }
    return result;
}

QString GUrl::lastFileSuffix() const {
    QString result;
    if (isVFSFile()) {
        return result;
    }
    CHECK(!isNetworkSource(), result);

    result = QFileInfo(path(this)).suffix();
    return result;
}

QString GUrl::completeFileSuffix() const {
    QString result;
    if (isVFSFile()) {
        return result;
    }
    CHECK(!isNetworkSource(), result);

    result = QFileInfo(path(this)).completeSuffix();
    return result;
}

static bool registerMetas() {
    qRegisterMetaType<GUrl>("GUrl");
    qRegisterMetaTypeStreamOperators<GUrl>("U2::GUrl");

    return true;
}

QDataStream& operator<<(QDataStream& out, const GUrl& myObj) {
    out << myObj.getURLString() << myObj.getType();
    return out;
}

QDataStream& operator>>(QDataStream& in, GUrl& myObj) {
    QString urlString;
    in >> urlString;
    int t;
    in >> t;
    GUrlType type = (GUrlType)t;
    myObj = GUrl(urlString, type);
    return in;
}

bool GUrl::registerMeta = registerMetas();

}  // namespace U2
