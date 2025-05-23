/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QSharedDataPointer>
#include <QStack>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/DocumentModel.h>

#include "StdResidueDictionary.h"

namespace U2 {

class IOAdapter;
class BioStruct3D;
class MoleculeData;
class ResidueData;
class U2SequenceObject;
class AtomData;
typedef QSharedDataPointer<AtomData> SharedAtom;
typedef QHash<int, SharedAtom> AtomCoordSet;

enum AsnElementKind {
    ASN_NO_KIND,
    ASN_SEQ,
    ASN_VALUE,
    ASN_ROOT
};

class U2FORMATS_EXPORT AsnNode {
public:
    AsnNode(const QByteArray& name, const AsnElementKind& kind, AsnNode* parent);
    ~AsnNode();
    const QByteArray name;
    QByteArray value;
    AsnElementKind kind = ASN_NO_KIND;

    AsnNode* findChildByName(const QByteArray& childName) const;
    AsnNode* getChild(int index) const;

    const QList<AsnNode*>& getChildren() const;

private:
    AsnNode* parent = nullptr;
    QList<AsnNode*> children;
};

typedef QList<AsnNode*> AsnNodeList;

class U2FORMATS_EXPORT ASNFormat : public DocumentFormat {
    Q_OBJECT
public:
    ASNFormat(QObject* p);
    ~ASNFormat();
    FormatCheckResult checkRawData(const QByteArray& data, const GUrl& = GUrl()) const override;

protected:
    Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;

public:
    class U2FORMATS_EXPORT AsnParser {
        static const char* filetypeTag;

        // Parsing state
        struct ParseState {
            ParseState()
                : atEnd(false), numOpenedTags(0) {
            }

            QByteArray parentName;
            bool atEnd;
            int numOpenedTags;
        };

        // Data
        IOAdapter* io;
        U2OpStatus& ts;
        QByteArray buffer;
        char prev;
        ParseState curState;
        QByteArray curElementName, curElementValue;
        AsnElementKind curElementKind;
        bool haveErrors;
        bool insideRoot;
        bool fileAtEnd;
        QStack<ParseState> states;

        // Core
        bool readRootElement();
        bool readNextElement();
        void processValue();
        void parseNextElement(AsnNode* node);
        void saveState();
        void restoreState();
        void dbgPrintCurrentState();
        void initState(const QByteArray& parentName);

        // Utility
        static bool isQuoted(const QByteArray& str);
        static void removeQuotes(QByteArray& str);
        static void dbgPrintAsnTree(const AsnNode* rootElem, int level = 0);

    public:
        AsnParser(IOAdapter* _io, U2OpStatus& _ts)
            : io(_io), ts(_ts), prev(' '), curElementKind(ASN_NO_KIND),
              haveErrors(false), insideRoot(false), fileAtEnd(false) {
        }
        ~AsnParser() {
        }
        AsnNode* loadAsnTree();
    };

    static QString getAsnNodeTypeName(const AsnNode* node);
    static AsnNode* findFirstNodeByName(AsnNode* rootElem, const QString& nodeName);
    static AsnNodeList findNodesByName(AsnNode* root, const QString& nodeName, AsnNodeList& lst);

private:
    struct AsnBaseException : public std::exception {
        QString msg;
        AsnBaseException(const QString& what)
            : msg(what) {
        }
    };

    struct AsnReadError : AsnBaseException {
        AsnReadError()
            : AsnBaseException(ASNFormat::tr("read error occurred")) {
        }
    };

    struct AsnParserError : public AsnBaseException {
        AsnParserError(const QString& what)
            : AsnBaseException(what) {
        }
    };

    struct AsnBioStructError : public AsnBaseException {
        AsnBioStructError(const QString& what)
            : AsnBaseException(ASNFormat::tr("biostruct3d obj loading error: %1").arg(what)) {
        }
    };

    class BioStructLoader {
    public:
        BioStructLoader()
            : standardDictionary(0), localDictionary(0) {
        }
        void loadBioStructFromAsnTree(AsnNode* rootNode, BioStruct3D& struc, U2OpStatus& os);
        const StdResidueDictionary* getStandardDictionary() const {
            return standardDictionary;
        }
        void setStandardDictionary(const StdResidueDictionary* _standardDictionary) {
            standardDictionary = _standardDictionary;
        }

    private:
        void loadBioStructModels(QList<AsnNode*> models, BioStruct3D& struc);
        void loadBioStructGraph(AsnNode* graphNode, BioStruct3D& struc);
        /* Find the Id node in @struc and assign to @rootNode. Set error to @os if not found.  */
        void loadBioStructPdbId(AsnNode* rootNode, BioStruct3D& struc, U2OpStatus& os);
        void loadBioStructSecondaryStruct(AsnNode* rootNode, BioStruct3D& struc);
        void loadBioStructFeature(AsnNode* featureNode, BioStruct3D& struc);
        void loadModelCoordsFromNode(AsnNode* node, AtomCoordSet& coordSet, QMap<int, Molecule3DModel>& molModels, const BioStruct3D& struc);
        void loadMoleculeFromNode(AsnNode* moleculeNode, MoleculeData* molecule);
        void loadIntraResidueBonds(BioStruct3D& struc);
        StdResidue loadResidueFromNode(AsnNode* resNode, ResidueData* residue);

        // chain_id -> molecule name
        static QMap<char, QString> loadMoleculeNames(AsnNode* biostructGraphDescr);

    private:
        const StdResidueDictionary* standardDictionary;
        QScopedPointer<StdResidueDictionary> localDictionary;
        QHash<quint64, StdResidue> stdResidueCache;
        QMap<quint64, AtomCoordSet> atomSetCache;
    };
};

}  // namespace U2
