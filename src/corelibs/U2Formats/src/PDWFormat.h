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

#ifndef _U2_PDW_FORMAT_H_
#define _U2_PDW_FORMAT_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;
class Annotation;
class AnnotationTableObject;
class U2SequenceObject;

class U2FORMATS_EXPORT PDWFormat : public TextDocumentFormat {
    Q_OBJECT
public:
                                    PDWFormat(QObject *p);

protected:
    virtual FormatCheckResult       checkRawTextData(const QByteArray &rawData,
                                        const GUrl & = GUrl()) const;

    virtual Document *              loadTextDocument(IOAdapter *io, const U2DbiRef &dbiRef,
                                        const QVariantMap &fs, U2OpStatus &os);

private:
    static QByteArray               parseSequence(IOAdapter *io, U2OpStatus &ti);

    static SharedAnnotationData     parseAnnotation(IOAdapter *io, U2OpStatus &ti);

    static QByteArray               readPdwValue(const QByteArray &readBuf, const QByteArray &valueName);

    void                            load(IOAdapter *io, const U2DbiRef &ref, const QVariantMap &fs, const GUrl &docUrl, QList<GObject *> &objects,
                                        U2OpStatus &ti, U2SequenceObject *&dnaObj, AnnotationTableObject *&aObj);
};

}//namespace

#endif
