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

#ifndef _U2_FASTA_FORMAT_H_
#define _U2_FASTA_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT FastaFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    FastaFormat(QObject* p);

    void storeSequence(const DNASequence& sequence, IOAdapter* io, U2OpStatus& os);
    void storeSequence(const U2SequenceObject *sequence, IOAdapter *io, U2OpStatus &os);

    virtual void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os);

    //name-sequence list
    static QList <QPair<QString, QString> > getSequencesAndNamesFromUserInput(const QString &userInput, U2OpStatus &os);

    virtual bool isStreamingSupport() {return true;}

    virtual void storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os);

    static const char FASTA_HEADER_START_SYMBOL;
    static const char FASTA_COMMENT_START_SYMBOL;

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;
    virtual DNASequence *loadTextSequence(IOAdapter* io, U2OpStatus& os);
    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);
};

}//namespace

#endif
