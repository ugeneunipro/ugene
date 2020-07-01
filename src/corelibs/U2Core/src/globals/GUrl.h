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

#ifndef _U2_GURL_H_
#define _U2_GURL_H_

#include <QMetaType>

#include <U2Core/global.h>

namespace U2 {

/** GUrl is designed to provide an easy to use and error prone implementation or URL for
DocumentModel in UGENE.
The core of GUrl is string based representation of an URL: urlString.
GUrl class enforces urlString to be always in correct canonical form.
All oter fields are supplementary.

Note: that 2 GUrls are considered equals if the urlStrings are equal.

Warning: in the current state GUrl canonical form can contain symlinks.
         2 GUrls with different symlinks to the same target are not equal!

* Why not QUrl - QUrl is too error prone and requires a lot of external code to support valid state.
* Why not QString - requires external utils to represent all urls in canonical form. Can't be to extended.
*/

/** Type of the URL */
enum GUrlType {
    GUrl_File,    // local file or default URL type for unknown files
    GUrl_Http,    // both http and https protocols
    GUrl_Ftp,
    GUrl_VFSFile,    // memory block
    GUrl_Network    // an abstract network url (e.g. shared database url)
};

class U2CORE_EXPORT GUrl {
public:
    // constructs empty url. The default type -> file
    GUrl() {
        type = GUrl_File;
    }

    // constructs url specified by string. The type is parsed
    GUrl(const QString &urlString);

    // constructs url specified by string. The type provided as param
    GUrl(const QString &urlString, const GUrlType type);

    GUrl(const GUrl &anotherUrl);

    bool operator==(const GUrl &url) const;

    bool operator!=(const GUrl &url) const;

    const QString &getURLString() const {
        return urlString;
    }

    // The function converts url string to multibyte form
    // default code page is CP_THREAD_ACP
    // must use "delete" to delete returned value
    QByteArray getURLStringAnsi(int codePage = -1) const;

    GUrlType getType() const {
        return type;
    }

    bool isEmpty() const {
        return urlString.isEmpty();
    }

    bool isLocalFile() const {
        return type == GUrl_File;
    }

    bool isHyperLink() const {
        return type == GUrl_Http;
    }

    bool isNetworkSource() const {
        return type == GUrl_Network;
    }

    bool isVFSFile() const {
        return type == GUrl_VFSFile;
    }

    QString dirPath() const;

    QString fileName() const;

    QString baseFileName() const;

    QString lastFileSuffix() const;

    QString completeFileSuffix() const;

    static GUrlType getURLType(const QString &rawUrl);

private:
    static bool registerMeta;
    QString urlString;
    GUrlType type;
};

QDataStream &operator<<(QDataStream &out, const GUrl &myObj);
QDataStream &operator>>(QDataStream &in, GUrl &myObj);

}    // namespace U2

Q_DECLARE_METATYPE(U2::GUrl);

#endif
