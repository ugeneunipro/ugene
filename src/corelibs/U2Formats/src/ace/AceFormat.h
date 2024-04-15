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

#pragma once

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MsaObject.h>

#include "../TextDocumentFormat.h"

#include "AceImportUtils.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT ACEFormat : public TextDocumentFormatDeprecated {
    Q_OBJECT
public:
    ACEFormat(QObject* p);

protected:
    FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const override;
    Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;

private:
    /*
     * Parses the AF tag of the format. This tag has the following structure:
     * AF <read name> <C or U> <padded start consensus position>
     * C or U means complemented or uncomplemented (direct, in UGENE terms).
     * The <read name> is the true read name.
     *
     * \param io the input-ouput adapter instance.
     * \param ti the state information handler instance.
     * \param buff buffer, whuch contains text, read from the file.
     * \param count the number of reads, read from header of the file.
     * \param reads reads representation.All information, except sequence itself, should be filled in this function.
     * This value will be used further to check, that AF and RD tags has comparable reads.
     **/
    static void parseAFTag(U2::IOAdapter* io, U2OpStatus& ti, char* buff, int count, QList<Assembly::Sequence>& reads);
    /*
     * Parses RD and QA tags of the format. These tags have the following structures:
     * RD <read name> <# of padded bases> <# of whole read info items> <# of read tags>
     * QA <qual clipping start> <qual clipping end> <align clipping start> <align clipping end>
     *
     * \param io the input-ouput adapter instance.
     * \param ti the state information handler instance.
     * \param buff buffer, whuch contains text, read from the file.
     * \param name name of the considerable read.
     * \param sequence considerable read's sequence.
     **/
    static void parseRDandQATag(U2::IOAdapter* io, U2OpStatus& ti, char* buff, QString& name, QByteArray& sequence);

    /**
     * Offsets in an ACE file are specified relatively to the reference sequence,
     * so "pos" can be negative.
     */
    static qint64 getSmallestOffset(const QList<Assembly::Sequence>& reads);

    void load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& ti);

    static const QString CO;
    static const QString RD;
    static const QString QA;
    static const QString AS;
    static const QString AF;
    static const QString BQ;
};

}  // namespace U2
