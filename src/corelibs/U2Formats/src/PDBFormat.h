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

#ifndef _U2_PDB_FORMAT_H_
#define _U2_PDB_FORMAT_H_

#include <QSharedDataPointer>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/BioStruct3D.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;
class AnnotationTableObject;
class U2SequenceObject;
class AtomData;
typedef QSharedDataPointer<AtomData> SharedAtom;


class U2FORMATS_EXPORT  PDBFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    PDBFormat(QObject* p);

    static int getElementNumberByName(const QByteArray& elementName);
    static char getAcronymByName(const QByteArray& name);
    static QHash<QByteArray, int> createAtomNumMap();
    static void calculateBonds(BioStruct3D& bioStruct);
    static Document* createDocumentFromBioStruct3D(const U2DbiRef& dbi, BioStruct3D &bioStruct, DocumentFormat* format, IOAdapterFactory* iof, const GUrl& url, U2OpStatus& ti, const QVariantMap& fs );

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;
    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

private:
    static QHash<QByteArray,int> atomNumMap;
    static QHash<QByteArray, char> acronymNameMap;

    void initUtilityMaps();
    void updateSecStructChainIndexes(BioStruct3D& biostruc);


    class PDBParser {
    private:
        // Data
        IOAdapter *io;
        QString currentPDBLine;
        ResidueIndex currentResidueIndex;
        int currentChainIndex;
        int currentMoleculeIndex;
        int currentModelIndex;
        int residueOrder;
        char currentChainIndentifier;
        QString currentMoleculeName;
        bool flagMultipleModels, flagAtomRecordPresent;
        QHash<char, int> chainIndexMap;
        QHash<char, QByteArray> seqResMap;
        QSet<QByteArray> resIndSet;
        QMap <QString, QString> chainToMoleculeMap;

        // Methods
        QByteArray getSpecValue(const QByteArray& specLine, const QByteArray& valueName);
        void parseHeader(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseMacromolecularContent(bool firstCompndLine, U2OpStatus& ti);
        void parseCompound(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseDBRef(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseSequence(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseSecondaryStructure(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseHet(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseAtomConnections(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseAtom(BioStruct3D& biostruct, U2OpStatus& ti);
        void parseSplitSection(U2OpStatus& ti);

        void createMolecule( char chainIdentifier, BioStruct3D &biostruct, int chainIndex );

        void parseModel(BioStruct3D& biostruct, U2OpStatus& ti);
        void updateSecStructChainIndexes( BioStruct3D& biostruc );
        void updateResidueIndexes( BioStruct3D& biostruc );
        bool seqResContains(char chainIdentier, int residueIndex, char acronym);
        QByteArray getNextSpecLine();
    public:
        PDBParser(IOAdapter* io);
        void parseBioStruct3D(BioStruct3D& biostruct, U2OpStatus& ts);
    };

};

}//namespace

#endif
