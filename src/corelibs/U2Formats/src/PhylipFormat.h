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

#ifndef _U2_PHYLIP_FORMAT_H_
#define _U2_PHYLIP_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

#include "TextDocumentFormat.h"

namespace U2 {

/**
 * Phylip multiple sequence alignment format.
 * See rosalind.info/glossary/phylip-format or https://evolution.genetics.washington.edu/phylip/doc/sequence.html .
 */
class U2FORMATS_EXPORT PhylipFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    PhylipFormat(QObject* p, const DocumentFormatId& id);

    void storeTextDocument(IOAdapterWriter& writer, Document* doc, U2OpStatus& os) override;

protected:
    MultipleSequenceAlignmentObject* load(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

    /** Parses header line and saves sequenceCount and columnCount values. Returns true if the header was parsed succesfully. */
    bool parseHeader(const QString& data, int& sequenceCount, int& columnCount) const;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

    virtual MultipleSequenceAlignment parse(IOAdapterReader& reader, U2OpStatus& os) const = 0;
};

/** Sequential variant of Phylip format. Example:
 * 3 30
 * Taxon1     ACCGTTTCCACAGCATTATGG
 * GCTCGATGA
 * Taxon2     CACTTCACAAATCAATATTGA
 * GCTAGTGCA
 * Taxon3     TAAGGTATTGGGCTTGGTTCG
 * CAGGGGACT
 */
class U2FORMATS_EXPORT PhylipSequentialFormat : public PhylipFormat {
    Q_OBJECT
public:
    PhylipSequentialFormat(QObject* p);

    void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

protected:
    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    MultipleSequenceAlignment parse(IOAdapterReader& reader, U2OpStatus& os) const override;
};

/** Interleaved variant of Phylip format. Example:
 * 3 30
 * Taxon1     ACCGTTTCCACAGCATTATGG
 * Taxon2     CACTTCACAAATCAATATTGA
 * Taxon3     TAAGGTATTGGGCTTGGTTCG
 * GCTCGATGA
 * GCTAGTGCA
 * CAGGGGACT
 */
class U2FORMATS_EXPORT PhylipInterleavedFormat : public PhylipFormat {
    Q_OBJECT
public:
    PhylipInterleavedFormat(QObject* p);

    void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

protected:
    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    MultipleSequenceAlignment parse(IOAdapterReader& reader, U2OpStatus& os) const override;
};

}  // namespace U2

#endif  // _U2_PHYLIP_FORMAT_H_
