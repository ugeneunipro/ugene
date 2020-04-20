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

#ifndef _U2_BAM_FORMAT_H_
#define _U2_BAM_FORMAT_H_

#include <U2Core/DbiDocumentFormat.h>
#include <U2Core/GUrl.h>
#include <U2Core/U2FormatCheckResult.h>

#include <QStringList>


namespace U2 {

class BAMFormat : public DbiDocumentFormat {
    Q_OBJECT
public:
    BAMFormat();

    void storeDocument(Document *d, IOAdapter *io, U2OpStatus &os);
};

namespace BAM {

class BAMFormatUtils : public QObject
{
    Q_OBJECT
public:
    BAMFormatUtils(QObject *parent = NULL);

    FormatCheckResult checkRawData(const QByteArray &rawData, const GUrl& url = GUrl()) const;

    QStringList getSupportedDocumentFileExtensions() const { return fileExtensions; }

private:
    QStringList fileExtensions;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_FORMAT_H_
