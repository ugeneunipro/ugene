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
#include <U2Formats/DatabaseConnectionFormat.h>
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
#include <U2Formats/MysqlDbi.h>
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

#include "DocumentFormatRegistryImpl.h"

namespace U2 {

DocumentFormatRegistryImpl::~DocumentFormatRegistryImpl() {
    qDeleteAll(formats);
}

bool DocumentFormatRegistryImpl::registerFormat(DocumentFormat* f) {
    SAFE_POINT(getFormatById(f->getFormatId()) == NULL, "Existing format", false);
    formats.push_back(f);
    emit si_documentFormatRegistered(f);
    if (f->getFormatDescription().isEmpty()) {
        coreLog.trace("Warn! Format has no description: " + f->getFormatName());
    }
    return true;
}

QList<DocumentFormatId> DocumentFormatRegistryImpl::getRegisteredFormats() const {
    QList<DocumentFormatId> ids;
    foreach(DocumentFormat* df, formats) {
        ids.append(df->getFormatId());
    }
    return ids;
}

DocumentFormat* DocumentFormatRegistryImpl::selectFormatByFileExtension(const QString& fileExt) const {
    foreach(DocumentFormat* df, formats) {
        if (df->getSupportedDocumentFileExtensions().contains(fileExt)) {
            return df;
        }
    }
    return NULL;
}

QList<DocumentFormatId> DocumentFormatRegistryImpl::selectFormats(const DocumentFormatConstraints& c) const
{
    QList<DocumentFormatId> ids;
    foreach(DocumentFormat* df, formats) {
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
    return NULL;
}


void DocumentFormatRegistryImpl::init() {
    U2OpStatusImpl os;
    RawDataUdrSchema::init(os);
    SAFE_POINT_OP(os, );

    PlainTextFormat* text = new PlainTextFormat(this);
    registerFormat(text);

    FastaFormat* fasta = new FastaFormat(this);
    registerFormat(fasta);

    GenbankPlainTextFormat* gb = new GenbankPlainTextFormat(this);
    registerFormat(gb);

    EMBLPlainTextFormat* em = new EMBLPlainTextFormat(this);
    registerFormat(em);

    SwissProtPlainTextFormat* sp = new SwissProtPlainTextFormat(this);
    registerFormat(sp);

    ABIFormat* abi = new ABIFormat(this);
    registerFormat(abi);

    SCFFormat* scf = new SCFFormat(this);
    registerFormat(scf);

    RawDNASequenceFormat* rsf = new RawDNASequenceFormat(this);
    registerFormat(rsf);

    ClustalWAlnFormat* aln = new ClustalWAlnFormat(this);
    registerFormat(aln);

    StockholmFormat* stf = new StockholmFormat(this);
    registerFormat(stf);

    NewickFormat* nwf = new NewickFormat(this);
    registerFormat(nwf);

    PDBFormat* pdb = new PDBFormat(this);
    registerFormat(pdb);

    FastqFormat* ftq = new FastqFormat(this);
    registerFormat(ftq);

    ASNFormat* asn = new ASNFormat(this);
    registerFormat(asn);

    MSFFormat* msf = new MSFFormat(this);
    registerFormat(msf);

    BedFormat* bed = new BedFormat(this);
    registerFormat(bed);

    GFFFormat *gff = new GFFFormat(this);
    registerFormat(gff);

    GTFFormat* gtf = new GTFFormat(this);
    registerFormat(gtf);

    FpkmTrackingFormat* fpkmTr = new FpkmTrackingFormat(this);
    registerFormat(fpkmTr);

    NEXUSFormat* nexus = new NEXUSFormat(this);
    registerFormat(nexus);

    SAMFormat *sam = new SAMFormat(this);
    registerFormat(sam);

    MegaFormat *meg = new MegaFormat(this);
    registerFormat(meg);

    ACEFormat *aceFormat = new ACEFormat(this);
    registerFormat(aceFormat);

    AprFormat *apr = new AprFormat(this);
    registerFormat(apr);

    AceImporter *aceImporter = new AceImporter();
    importSupport.addDocumentImporter(aceImporter);

    AprImporter* aprImporter = new AprImporter();
    importSupport.addDocumentImporter(aprImporter);

    PDWFormat *pdw = new PDWFormat(this);
    registerFormat(pdw);

    SimpleSNPVariationFormat *snp = new SimpleSNPVariationFormat(this);
    registerFormat(snp);

    VCF4VariationFormat *vcf4 = new VCF4VariationFormat(this);
    registerFormat(vcf4);

    DifferentialFormat *diff = new DifferentialFormat(this);
    registerFormat(diff);

    PhylipInterleavedFormat *phIn = new PhylipInterleavedFormat(this);
    registerFormat(phIn);

    PhylipSequentialFormat *phSeq = new PhylipSequentialFormat(this);
    registerFormat(phSeq);

    registerFormat(new DatabaseConnectionFormat(this));
    registerFormat(new VectorNtiSequenceFormat(this));

    AppContext::getDbiRegistry()->registerDbiFactory(new SQLiteDbiFactory());
    AppContext::getDbiRegistry()->registerDbiFactory(new MysqlDbiFactory());

    DocumentFormatFlags flags(DocumentFormatFlag_SupportWriting | DocumentFormatFlag_CannotBeCompressed);
    DbiDocumentFormat* sdbi = new DbiDocumentFormat(SQLiteDbiFactory::ID, BaseDocumentFormats::UGENEDB, tr("UGENE Database"), QStringList()<<"ugenedb", flags);
    registerFormat(sdbi);
}

}//namespace
