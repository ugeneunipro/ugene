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

#ifndef _U2_FORMAT_UTILS_H_
#define _U2_FORMAT_UTILS_H_

#include <U2Core/global.h>

#include <QStringList>

namespace U2 {

class DocumentFormatConstraints;

class U2CORE_EXPORT FormatUtils {

public:
    static QString splitThousands(qint64 num);

    static QString formatNumber(qint64 num);
    static QString formatNumberWithSeparators(int num);
    static QString insertSeparators(int num);

    // QDate::getShortMonthName() returns date in local translation
    // This is English version of this function, required by EMBL and Genbank
    // for correct date formatting
    static QString getShortMonthName(int num);

    static QString prepareFileFilter(const QMap<QString, QStringList>& formatNamesWithExtensions, bool allowAnyFiles = true, const QStringList& extraExtensions = QStringList(".gz"));
    static QString prepareFileFilter(const QString& name, const QStringList& exts, bool any = true, const QStringList& extraExts = QStringList(".gz"));
    static QString prepareDocumentsFileFilter(const DocumentFormatId &fid, bool any, const QStringList &extra = QStringList(".gz"));
    //returns filter for all formats supported. All-docs filter is returned first if any==true
    static QString prepareDocumentsFileFilter(bool any, const QStringList& extraExts = QStringList(".gz"));
    static QString prepareDocumentsFileFilter(const DocumentFormatConstraints& c, bool any);
    static QString prepareDocumentsFileFilterByObjType(const GObjectType& t, bool any);
};

}//namespace

#endif

