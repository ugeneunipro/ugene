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

#include "DocumentFormatRegistryImpl.h"

#include <U2Core/AppContext.h>
#include <U2Core/DbiDocumentFormat.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ABIFormat.h>
#include <U2Formats/ASNFormat.h>
#include <U2Formats/AceFormat.h>
#include <U2Formats/AceImporter.h>
#include <U2Formats/AprFormat.h>
#include <U2Formats/AprImporter.h>
#include <U2Formats/BedFormat.h>
#include <U2Formats/ClustalWAlnFormat.h>
#include <U2Formats/DifferentialFormat.h>
#include <U2Formats/EMBLPlainTextFormat.h>
#include <U2Formats/FastaFormat.h>
#include <U2Formats/FastqFormat.h>
#include <U2Formats/FpkmTrackingFormat.h>
#include <U2Formats/GFFFormat.h>
#include <U2Formats/GTFFormat.h>
#include <U2Formats/GenbankPlainTextFormat.h>
#include <U2Formats/MSFFormat.h>
#include <U2Formats/MegaFormat.h>
#include <U2Formats/NEXUSFormat.h>
#include <U2Formats/NewickFormat.h>
#include <U2Formats/PDBFormat.h>
#include <U2Formats/PDWFormat.h>
#include <U2Formats/PhylipFormat.h>
#include <U2Formats/PlainTextFormat.h>
#include <U2Formats/RawDNASequenceFormat.h>
#include <U2Formats/SAMFormat.h>
#include <U2Formats/SCFFormat.h>
#include <U2Formats/SQLiteDbi.h>
#include <U2Formats/SimpleSNPVariationFormat.h>
#include <U2Formats/StockholmFormat.h>
#include <U2Formats/SwissProtPlainTextFormat.h>
#include <U2Formats/VCF4VariationFormat.h>
#include <U2Formats/VectorNtiSequenceFormat.h>

namespace U2 {

DocumentFormatRegistryImpl::~DocumentFormatRegistryImpl() {
    qDeleteAll(formats);
}

bool DocumentFormatRegistryImpl::registerFormat(DocumentFormat* f) {
    SAFE_POINT(getFormatById(f->getFormatId()) == nullptr, "Existing format", false);
    formats.push_back(f);
    emit si_documentFormatRegistered(f);
    if (f->getFormatDescription().isEmpty()) {
        coreLog.trace("Warn! Format has no description: " + f->getFormatName());
    }
    return true;
}

QList<DocumentFormatId> DocumentFormatRegistryImpl::getRegisteredFormats() const {
    QList<DocumentFormatId> ids;
    foreach (DocumentFormat* df, formats) {
        ids.append(df->getFormatId());
    }
    return ids;
}

DocumentFormat* DocumentFormatRegistryImpl::selectFormatByFileExtension(const QString& fileExt) const {
    foreach (DocumentFormat* df, formats) {
        if (df->getSupportedDocumentFileExtensions().contains(fileExt)) {
            return df;
        }
    }
    return nullptr;
}

QList<DocumentFormatId> DocumentFormatRegistryImpl::selectFormats(const DocumentFormatConstraints& c) const {
    QList<DocumentFormatId> ids;
    foreach (DocumentFormat* df, formats) {
        if (df->checkConstraints(c)) {
            ids.append(df->getFormatId());
        }
    }
    return ids;
}

bool DocumentFormatRegistryImpl::unregisterFormat(DocumentFormat* f) {
    int n = formats.removeAll(f);
    bool res = n > 0;
    if (res) {
        emit si_documentFormatUnregistered(f);
    }
    return res;
}

DocumentFormat* DocumentFormatRegistryImpl::getFormatById(DocumentFormatId id) const {
    foreach (DocumentFormat* f, formats) {
        if (BaseDocumentFormats::equal(f->getFormatId(), id)) {
            return f;
        }
    }
    return nullptr;
}

void DocumentFormatRegistryImpl::init() {
    U2OpStatusImpl os;
    RawDataUdrSchema::init(os);
    SAFE_POINT_OP(os, );

    registerFormat(new PlainTextFormat(this));
    registerFormat(new FastaFormat(this));
    registerFormat(new GenbankPlainTextFormat(this));
    registerFormat(new EMBLPlainTextFormat(this));
    registerFormat(new SwissProtPlainTextFormat(this));
    registerFormat(new ABIFormat(this));
    registerFormat(new SCFFormat(this));
    registerFormat(new RawDNASequenceFormat(this));
    registerFormat(new ClustalWAlnFormat(this));
    registerFormat(new StockholmFormat(this));
    registerFormat(new NewickFormat(this));
    registerFormat(new PDBFormat(this));
    registerFormat(new FastqFormat(this));
    registerFormat(new ASNFormat(this));
    registerFormat(new MSFFormat(this));
    registerFormat(new BedFormat(this));
    registerFormat(new GFFFormat(this));
    registerFormat(new GTFFormat(this));
    registerFormat(new FpkmTrackingFormat(this));
    registerFormat(new NEXUSFormat(this));
    registerFormat(new SAMFormat(this));
    registerFormat(new MegaFormat(this));
    registerFormat(new ACEFormat(this));
    registerFormat(new AprFormat(this));

    importSupport.addDocumentImporter(new AceImporter());
    importSupport.addDocumentImporter(new AprImporter());

    registerFormat(new PDWFormat(this));
    registerFormat(new SimpleSNPVariationFormat(this));
    registerFormat(new VCF4VariationFormat(this));
    registerFormat(new DifferentialFormat(this));
    registerFormat(new PhylipInterleavedFormat(this));
    registerFormat(new PhylipSequentialFormat(this));
    registerFormat(new VectorNtiSequenceFormat(this));

    AppContext::getDbiRegistry()->registerDbiFactory(new SQLiteDbiFactory());

    DocumentFormatFlags flags(DocumentFormatFlag_SupportWriting | DocumentFormatFlag_CannotBeCompressed);
    registerFormat(new DbiDocumentFormat(SQLiteDbiFactory::ID, BaseDocumentFormats::UGENEDB, tr("UGENE Database"), QStringList() << "ugenedb", flags));
}

}  // namespace U2
