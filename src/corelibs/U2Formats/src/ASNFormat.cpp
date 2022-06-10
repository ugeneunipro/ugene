/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "ASNFormat.h"

#include <QDebug>
#include <QStringList>

#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "DocumentFormatUtils.h"
#include "PDBFormat.h"
#include "StdResidueDictionary.h"

/* TRANSLATOR U2::ASNFormat */

// `comment` is string like
// "Some title Mol_id: 1; Molecule: Molecule name; Chain: A, C; Some_specification: Specification value; Mol_id: 2; Molecule: Molecule name; Chain: B, D; Some_specification: Specification value"
// Returns list of
// "Mol_id: 1; Molecule: Molecule name; Chain: A, C; Some_specification: Specification value"
// "Mol_id: 2; Molecule: Molecule name; Chain: B, D; Some_specification: Specification value"
static QStringList parseMolecules(const QString& comment) {
    QStringList ans;
    const QString molIdStr("Mol_id");

    const int molIdInd = comment.indexOf(molIdStr, Qt::CaseInsensitive);
    if (molIdInd < 0) {
        return ans;
    }
    const QString molInfos = comment.mid(molIdInd);  // No title at the beginning

    int start = 0;
    int end = 0;
    while (start > -1) {
        end = molInfos.indexOf(molIdStr, start + 1, Qt::CaseInsensitive);
        QString str = molInfos.mid(start, end).trimmed();
        if (str.endsWith(';')) {
            str.remove(str.length() - 1, 1);
        }
        ans << str;
        start = end;
    }
    return ans;
}

// mol is string like
// "Mol_id: 1; Chain: A, C; Some_specification: Specification value"
// Returns {'A', 'C'}
static QList<char> parseChains(const QString& mol) {
    QList<char> ans;

    int start = mol.indexOf("Chain:", Qt::CaseInsensitive);
    if (start < 0) {
        return ans;
    }
    start += 6;  // "Chain:" length
    const int end = mol.indexOf(';', start);
    const QString chains = mol.mid(start, end > start ? end - start : -1);
    for (QString& str : chains.split(',', QString::SkipEmptyParts)) {
        str = str.trimmed();
        if (str.length() == 1) {
            ans << str.at(0).toLatin1();
        }
    }
    return ans;
}

// mol is string like
// "Mol_id: 1; Molecule: Molecule name; Some_specification: Specification value"
// Returns "Molecule name"
static QString parseMolName(const QString& mol) {
    QString ans;

    int start = mol.indexOf("Molecule:", Qt::CaseInsensitive);
    if (start < 0) {
        return ans;
    }
    start += 9;  // Length of "Molecule:"
    const int end = mol.indexOf(';', start);
    ans = mol.mid(start, end > start ? end - start : -1).trimmed();
    return ans;
}

namespace U2 {

ASNFormat::ASNFormat(QObject* p)
    : DocumentFormat(p, BaseDocumentFormats::PLAIN_ASN, DocumentFormatFlag(0), QStringList() << "prt") {
    formatName = tr("MMDB");
    formatDescription = tr("ASN is a format used my the Molecular Modeling Database (MMDB)");
    supportedObjectTypes += GObjectTypes::BIOSTRUCTURE_3D;
    supportedObjectTypes += GObjectTypes::SEQUENCE;
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
}

FormatCheckResult ASNFormat::checkRawData(const QByteArray& rawData, const GUrl&) const {
    static const char* formatTag = "Ncbi-mime-asn1 ::= strucseq";

    if (!rawData.startsWith(formatTag)) {
        return FormatDetection_NotMatched;
    }

    bool textOnly = !TextUtils::contains(TextUtils::BINARY, rawData.constData(), rawData.size());
    return textOnly ? FormatDetection_VeryHighSimilarity : FormatDetection_NotMatched;
}

Document* ASNFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    BioStruct3D bioStruct;

    const StdResidueDictionary* stdResidueDict = StdResidueDictionary::getStandardDictionary();
    CHECK_EXT(stdResidueDict != nullptr, os.setError(tr("Standard residue dictionary not found")), nullptr);

    AsnParser asnParser(io, os);
    ioLog.trace("ASN: Parsing: " + io->toString());

    QScopedPointer<AsnNode> rootElem(asnParser.loadAsnTree());
    ioLog.trace(QString("ASN tree for %1 was built").arg(io->toString()));
    os.setProgress(30);

    if (!rootElem.isNull()) {
        BioStructLoader ldr;
        ldr.setStandardDictionary(stdResidueDict);
        ldr.loadBioStructFromAsnTree(rootElem.data(), bioStruct, os);
    }
    os.setProgress(80);

    CHECK_OP(os, nullptr);
    ioLog.trace(QString("BioStruct3D loaded from ASN tree (%1)").arg(io->toString()));

    bioStruct.calcCenterAndMaxDistance();
    os.setProgress(90);

    Document* doc = PDBFormat::createDocumentFromBioStruct3D(dbiRef, bioStruct, this, io->getFactory(), io->toString(), os, fs);

    ioLog.trace("ASN Parsing finished: " + io->toString());
    os.setProgress(100);

    return doc;
}

AsnNode* ASNFormat::findFirstNodeByName(AsnNode* rootElem, const QString& nodeName) {
    if (rootElem->name == nodeName) {
        return rootElem;
    }

    for (AsnNode* node : qAsConst(rootElem->getChildren())) {
        AsnNode* child = findFirstNodeByName(node, nodeName);
        if (child != nullptr) {
            return child;
        }
    }

    return nullptr;
}

AsnNodeList ASNFormat::findNodesByName(AsnNode* root, const QString& nodeName, AsnNodeList& nodes) {
    if (root->name == nodeName) {
        nodes.append(root);
    }

    for (AsnNode* child : qAsConst(root->getChildren())) {
        findNodesByName(child, nodeName, nodes);
    }

    return nodes;
}

QString ASNFormat::getAsnNodeTypeName(const AsnNode* node) {
    switch (node->kind) {
        case ASN_NO_KIND:
            return QString("ASN_NO_KIND");
        case ASN_ROOT:
            return QString("ASN_ROOT");
        case ASN_SEQ:
            return QString("ASN_SEQ");
        case ASN_VALUE:
            return QString("ASN_VALUE");
        default:
            Q_ASSERT(0);
    }
    return QString("");
}

void ASNFormat::BioStructLoader::loadBioStructPdbId(AsnNode* rootNode, BioStruct3D& struc) {
    AsnNode* nameNode = ASNFormat::findFirstNodeByName(rootNode, "name");
    SAFE_POINT(nameNode != nullptr, "nameNode == NULL?", );
    struc.pdbId = nameNode->value;
}

void ASNFormat::BioStructLoader::loadBioStructFromAsnTree(AsnNode* rootNode, BioStruct3D& struc, U2OpStatus& ti) {
    /*
        id              SEQUENCE OF Biostruc-id,
        descr           SEQUENCE OF Biostruc-descr OPTIONAL,
        chemical-graph  Biostruc-graph,
        features        SEQUENCE OF Biostruc-feature-set OPTIONAL,
        model           SEQUENCE OF Biostruc-model OPTIONAL }
    */

    try {
        localDictionary.reset(StdResidueDictionary::createFromAsnTree(rootNode));

        // Load pdb Id
        loadBioStructPdbId(rootNode, struc);

        // Load biostruct molecules
        AsnNode* graphNode = findFirstNodeByName(rootNode, "chemical-graph");
        if (graphNode == nullptr) {
            throw AsnBioStructError("models not found");
        }
        loadBioStructGraph(graphNode, struc);

        // Load secondary structure
        AsnNode* featureSetNode = findFirstNodeByName(rootNode, "features");
        if (featureSetNode != nullptr) {
            loadBioStructSecondaryStruct(featureSetNode, struc);
        }

        // Load biostruct models (coordinates)
        AsnNode* modelsNode = findFirstNodeByName(rootNode, "model");
        if (modelsNode == nullptr) {
            throw AsnBioStructError("models not found");
        }
        loadBioStructModels(modelsNode->getChildren(), struc);

        // TODO: implement loading bonds from file
        // loadIntraResidueBonds(struc);
        PDBFormat::calculateBonds(struc);

        stdResidueCache.clear();
        atomSetCache.clear();
    } catch (const AsnBaseException& ex) {
        ti.setError(ex.msg);
        return;
    } catch (...) {
        ti.setError(ASNFormat::tr("Unknown error occurred"));
        return;
    }
}

inline quint64 calcStdResidueIndex(int chainId, int resId) {
    return (((quint64)chainId << 32) | (quint64)resId);
}

void ASNFormat::BioStructLoader::loadModelCoordsFromNode(AsnNode* modelNode, AtomCoordSet& coordSet, QMap<int, Molecule3DModel>& molModels, const BioStruct3D& struc) {
    /*
        Atomic-coordinates ::= SEQUENCE {
            number-of-points    INTEGER,
            atoms           Atom-pntrs,
            sites           Model-space-points,
            temperature-factors Atomic-temperature-factors OPTIONAL,
            occupancies     Atomic-occupancies OPTIONAL,
            alternate-conf-ids  Alternate-conformation-ids OPTIONAL,
            conf-ensembles      SEQUENCE OF Conformation-ensemble OPTIONAL }

        Model-space-points ::= SEQUENCE {
            scale-factor        INTEGER,
            x           SEQUENCE OF INTEGER,
            y           SEQUENCE OF INTEGER,
            z           SEQUENCE OF INTEGER }

        Atom-pntrs ::= SEQUENCE {
            number-of-ptrs      INTEGER,
            molecule-ids        SEQUENCE OF Molecule-id,
            residue-ids         SEQUENCE OF Residue-id,
            atom-ids            SEQUENCE OF Atom-id }

    */

    AsnNode* aCoordsNode = modelNode->findChildByName("coordinates literal atomic");

    bool ok = false;
    AsnNode* numAtomsNode = aCoordsNode->getChild(0);
    int numAtoms = numAtomsNode->value.toInt(&ok);
    Q_ASSERT(ok == true);

    AsnNode* atomPntrsNode = aCoordsNode->getChild(1);
    Q_ASSERT(atomPntrsNode->name == "atoms");
    AsnNode* chainIds = atomPntrsNode->getChild(1);
    AsnNode* resIds = atomPntrsNode->getChild(2);
    AsnNode* atomIds = atomPntrsNode->getChild(3);

    AsnNode* sitesNode = aCoordsNode->getChild(2);
    Q_ASSERT(sitesNode->name == "sites");
    AsnNode* sfNode = sitesNode->getChild(0);
    AsnNode* xCoords = sitesNode->getChild(1);
    AsnNode* yCoords = sitesNode->getChild(2);
    AsnNode* zCoords = sitesNode->getChild(3);
    double scaleFactor = sfNode->value.toInt();

    for (int i = 0; i < numAtoms; ++i) {
        int chainId = chainIds->getChild(i)->value.toInt();
        int resId = resIds->getChild(i)->value.toInt();
        int atomId = atomIds->getChild(i)->value.toInt();
        double x = xCoords->getChild(i)->value.toInt() / scaleFactor;
        double y = yCoords->getChild(i)->value.toInt() / scaleFactor;
        double z = zCoords->getChild(i)->value.toInt() / scaleFactor;
        auto a = new AtomData();
        a->chainIndex = chainId;
        a->residueIndex = ResidueIndex(resId, ' ');
        a->coord3d = Vector3D(x, y, z);

        quint64 index = calcStdResidueIndex(chainId, resId);
        if (stdResidueCache.contains(index)) {
            StdResidue stdResidue = stdResidueCache.value(index);
            const StdAtom stdAtom = stdResidue.atoms.value(atomId);
            a->atomicNumber = stdAtom.atomicNum;
            a->name = stdAtom.name;
            // TODO: add this
            // a->occupancy = occupancy;
            // a->temperature = temperature;
        }
        SharedAtom atom(a);
        coordSet.insert(i + 1, atom);
        if (struc.moleculeMap.contains(chainId)) {
            molModels[chainId].atoms.append(atom);
        }
        // atomSetCache[index].insert(atomId, atom);
    }
}

StdResidue ASNFormat::BioStructLoader::loadResidueFromNode(AsnNode* resNode, ResidueData* residue) {
    /*
        Residue ::= SEQUENCE {
            id          Residue-id,
            name            VisibleString OPTIONAL,
            residue-graph       Residue-graph-pntr }

        Residue-graph-pntr ::= CHOICE {
            local           Residue-graph-id,
            biostruc        Biostruc-graph-pntr,
            standard        Biostruc-residue-graph-set-pntr }

        Biostruc-residue-graph-set-pntr ::= SEQUENCE {
            biostruc-residue-graph-set-id   Biostruc-id,
            residue-graph-id        Residue-graph-id }

    */

    AsnNode* resGraphPntrNode = resNode->getChild(2);
    const StdResidueDictionary* dictionary = nullptr;
    int stdResidueId = 0;
    bool ok = false;
    if ((resGraphPntrNode->kind == ASN_VALUE) && (resGraphPntrNode->value.contains("local"))) {
        dictionary = localDictionary.data();
        stdResidueId = resGraphPntrNode->value.split(' ').at(1).toInt(&ok);
    } else if (resGraphPntrNode->name.contains("standard")) {
        dictionary = standardDictionary;
        stdResidueId = resGraphPntrNode->getChild(1)->value.toInt(&ok);
    } else {
        Q_ASSERT(0);
    }

    Q_ASSERT(ok == true);
    StdResidue stdResidue = dictionary->getResidueById(stdResidueId);
    residue->name = stdResidue.name;
    residue->acronym = stdResidue.code;
    return stdResidue;
}

void ASNFormat::BioStructLoader::loadMoleculeFromNode(AsnNode* moleculeNode, MoleculeData* molecule) {
    /*
        Molecule-graph ::= SEQUENCE {
            id          Molecule-id,
            descr           SEQUENCE OF Biomol-descr OPTIONAL,
            seq-id          Seq-id OPTIONAL,
            residue-sequence    SEQUENCE OF Residue,
            inter-residue-bonds SEQUENCE OF Inter-residue-bond OPTIONAL,
            sid                     PCSubstance-id OPTIONAL }

        Residue ::= SEQUENCE {
            id          Residue-id,
            ... }

    */

    int chainId = moleculeNode->getChild(0)->value.toInt();

    AsnNode* resideusNode = moleculeNode->findChildByName("residue-sequence");
    for (AsnNode* resNode : qAsConst(resideusNode->getChildren())) {
        // Load residue id
        AsnNode* idNode = resNode->getChild(0);
        int resId = idNode->value.toInt();
        // Load residue
        ResidueData* resData = new ResidueData();
        resData->chainIndex = chainId;
        StdResidue stdResidue = loadResidueFromNode(resNode, resData);
        molecule->residueMap.insert(ResidueIndex(resId, ' '), SharedResidue(resData));
        stdResidueCache.insert(calcStdResidueIndex(chainId, resId), stdResidue);
    }
}

void ASNFormat::BioStructLoader::loadBioStructModels(QList<AsnNode*> modelNodes, BioStruct3D& struc) {
    /*
        Biostruc-model ::= SEQUENCE {
            id          Model-id,
            type            Model-type,
            descr           SEQUENCE OF Model-descr OPTIONAL,
            model-space     Model-space OPTIONAL,
            model-coordinates   SEQUENCE OF Model-coordinate-set OPTIONAL }
    */

    Q_ASSERT(modelNodes.count() != 0);

    for (AsnNode* modelNode : qAsConst(modelNodes)) {
        // Load model id
        AsnNode* idNode = modelNode->getChild(0);
        int modelId = idNode->value.toInt();
        // Load model
        AtomCoordSet atomCoords;
        AsnNode* modelCoordsNode = modelNode->findChildByName("model-coordinates");
        QMap<int, Molecule3DModel> molModels;
        loadModelCoordsFromNode(modelCoordsNode->getChild(0), atomCoords, molModels, struc);
        struc.modelMap.insert(modelId, atomCoords);
        foreach (int chainId, struc.moleculeMap.keys()) {
            Q_ASSERT(molModels.contains(chainId));
            struc.moleculeMap[chainId]->models.insert(modelId, molModels.value(chainId));
        }
    }
}

void ASNFormat::BioStructLoader::loadBioStructGraph(AsnNode* graphNode, BioStruct3D& struc) {
    /*
        Biostruc-graph ::= SEQUENCE {
            descr           SEQUENCE OF Biomol-descr OPTIONAL,
            molecule-graphs     SEQUENCE OF Molecule-graph,
            inter-molecule-bonds    SEQUENCE OF Inter-residue-bond OPTIONAL,
            residue-graphs      SEQUENCE OF Residue-graph OPTIONAL }

        Molecule-graph ::= SEQUENCE {
            id          Molecule-id,
            descr           SEQUENCE OF Biomol-descr OPTIONAL,
            ...

       Biomol-descr ::= CHOICE {
            name            VisibleString,
            molecule-type   INTEGER {   dna(1),
            rna(2),
            protein(3),
            other-biopolymer(4),
            solvent(5),
            other-nonpolymer(6),
            other(255) }
            ...
    */

    QMap<char, QString> names = loadMoleculeNames(graphNode->findChildByName("descr"));

    AsnNode* moleculesNode = graphNode->findChildByName("molecule-graphs");

    for (AsnNode* molNode : qAsConst(moleculesNode->getChildren())) {
        // Load molecule id
        bool ok = false;
        int molId = molNode->getChild(0)->value.toInt(&ok);
        SAFE_POINT(ok, "Invalid type conversion", );
        // Load molecule data
        AsnNode* const descrNode = molNode->findChildByName("descr");
        QByteArray molTypeName = descrNode->findChildByName("molecule-type")->value;
        QByteArray molChainId = descrNode->findChildByName("name")->value;
        if (molTypeName == "protein" || molTypeName == "dna" || molTypeName == "rna") {
            MoleculeData* mol = new MoleculeData();
            if (molChainId.length() == 1) {
                mol->chainId = molChainId[0];
                if (names.contains(mol->chainId)) {
                    mol->name = names[mol->chainId];
                }
            }

            loadMoleculeFromNode(molNode, mol);
            struc.moleculeMap.insert(molId, SharedMolecule(mol));
        }
    }
}

ASNFormat::~ASNFormat() {
}

void ASNFormat::BioStructLoader::loadBioStructSecondaryStruct(AsnNode* setsNode, BioStruct3D& struc) {
    /*
        Biostruc-feature-set ::= SEQUENCE {
            id      Biostruc-feature-set-id,
            descr       SEQUENCE OF Biostruc-feature-set-descr OPTIONAL,
            features    SEQUENCE OF Biostruc-feature }

        Biostruc-feature-set-descr ::= CHOICE {
            name            VisibleString,
            pdb-comment     VisibleString,
            other-comment       VisibleString,
            attribution     Pub }

   */

    for (AsnNode* featureSet : qAsConst(setsNode->getChildren())) {
        QByteArray descr = featureSet->findChildByName("descr")->getChild(0)->value;
        if (descr != "PDB secondary structure") {
            continue;
        }

        AsnNode* features = featureSet->getChild(2);
        Q_ASSERT(features->name == "features");
        for (AsnNode* featureNode : qAsConst(features->getChildren())) {
            loadBioStructFeature(featureNode, struc);
        }
    }
}

void ASNFormat::BioStructLoader::loadBioStructFeature(AsnNode* featureNode, BioStruct3D& struc) {
    /*
        Biostruc-feature ::= SEQUENCE {
            id      Biostruc-feature-id OPTIONAL,
            name        VisibleString OPTIONAL,
            type    INTEGER {   helix(1),
                                strand(2),
                                sheet(3),
                                turn(4),
                                site(5),
                                ...
            other(255) } OPTIONAL,
            property    CHOICE {
                                ... } OPTIONAL,
            location    CHOICE {subgraph    Chem-graph-pntrs,
                                region      Region-pntrs,
                                alignment   Chem-graph-alignment,
                                similarity  Region-similarity,
                                indirect    Other-feature } OPTIONAL }

        Residue-interval-pntr ::= SEQUENCE {
            molecule-id     Molecule-id,
            from            Residue-id,
            to              Residue-id }

    */

    AsnNode* typeNode = featureNode->findChildByName("type");
    SecondaryStructure::Type ssType(SecondaryStructure::Type_None);
    if (typeNode->value == "helix") {
        ssType = SecondaryStructure::Type_AlphaHelix;
    } else if (typeNode->value == "strand" || typeNode->value == "sheet") {
        ssType = SecondaryStructure::Type_BetaStrand;
    } else if (typeNode->value == "turn") {
        ssType = SecondaryStructure::Type_Turn;
    } else {
        return;
    }

    AsnNode* locatioNode = featureNode->findChildByName("location subgraph residues interval")->getChild(0);
    bool idOK = false, fromOK = false, toOK = false;
    int chainId = locatioNode->getChild(0)->value.toInt(&idOK);
    int from = locatioNode->getChild(1)->value.toInt(&fromOK);
    int to = locatioNode->getChild(2)->value.toInt(&toOK);
    Q_ASSERT(idOK && fromOK && toOK);
    SecondaryStructure* ssData = new SecondaryStructure();
    ssData->chainIndex = chainId;
    ssData->type = ssType;
    ssData->startSequenceNumber = from;
    ssData->endSequenceNumber = to;

    struc.secondaryStructures.append(SharedSecondaryStructure(ssData));
}

void ASNFormat::BioStructLoader::loadIntraResidueBonds(BioStruct3D& struc) {
    Q_ASSERT(!stdResidueCache.isEmpty());
    foreach (int chainId, struc.moleculeMap.keys()) {
        SharedMolecule& mol = struc.moleculeMap[chainId];
        int numModels = mol->models.count();
        for (int modelId = 0; modelId < numModels; ++modelId) {
            QList<ResidueIndex> residueKeys = mol->residueMap.keys();
            for (const ResidueIndex& resId : qAsConst(residueKeys)) {
                quint64 index = calcStdResidueIndex(chainId, resId.toInt());
                if (!stdResidueCache.contains(index)) {
                    continue;
                }
                StdResidue residue = stdResidueCache.value(index);
                if (!atomSetCache.contains(index)) {
                    continue;
                }
                const AtomCoordSet& atomSet = atomSetCache.value(index);
                Q_ASSERT(!atomSet.isEmpty());
                for (const StdBond& bond : qAsConst(residue.bonds)) {
                    if (atomSet.contains(bond.atom1Id) && atomSet.contains(bond.atom2Id)) {
                        const SharedAtom a1 = atomSet.value(bond.atom1Id);
                        const SharedAtom a2 = atomSet.value(bond.atom2Id);
                        mol->models[modelId].bonds.append(Bond(a1, a2));
                    }
                }
            }
        }
    }
}

QMap<char, QString> ASNFormat::BioStructLoader::loadMoleculeNames(AsnNode* biostructGraphDescr) {
    QMap<char, QString> ans;
    if (biostructGraphDescr == nullptr) {
        return ans;
    }

    if (AsnNode* const comment = biostructGraphDescr->findChildByName("pdb-comment")) {
        QStringList molecules = parseMolecules(comment->value);

        for (const QString& mol : qAsConst(molecules)) {
            QString name = parseMolName(mol);
            if (name.isEmpty()) {
                continue;
            }

            QList<char> chains = parseChains(mol);
            for (char chain : qAsConst(chains)) {
                ans.insert(chain, name);
            }
        }
    }
    return ans;
}

///////////////////////////////////////////////////////////////////////////////////////////

AsnNode* ASNFormat::AsnParser::loadAsnTree() {
    try {
        if (!readRootElement()) {
            throw AsnParserError(ASNFormat::tr("no root element"));
        }

        auto rootElem = new AsnNode(curElementName, ASN_ROOT, nullptr);
        parseNextElement(rootElem);
        if (states.count() != 0) {
            throw AsnParserError(ASNFormat::tr("states stack is not empty"));
        }
        return rootElem;

    } catch (const AsnBaseException& ex) {
        ts.setError(ex.msg);
        return nullptr;
    } catch (...) {
        ts.setError(ASNFormat::tr("Unknown error occurred"));
        return nullptr;
    }
}

bool ASNFormat::AsnParser::readRootElement() {
    Q_ASSERT(io->getProgress() == 0);

    QByteArray readBuff(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buf = readBuff.data();
    quint64 len = 0;

    bool lineOk = true;
    len = io->readUntil(buf, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    if (!lineOk) {
        throw AsnParserError(ASNFormat::tr("First line is too long"));
    }

    QString line = QString(QByteArray(buf, len));
    if (line.contains("::=")) {
        // get mime type
        int start = line.indexOf("::=") + 4;
        int finish = line.indexOf("{", start) - 1;
        QString mimeType = line.mid(start, finish - start);
        curElementName = mimeType.toLatin1();
        // initial state
        insideRoot = true;
        fileAtEnd = false;
        haveErrors = false;
        initState(mimeType.toLatin1());

        return true;
    } else {
        return false;
    }
}

void ASNFormat::AsnParser::initState(const QByteArray& parentName) {
    curState.parentName = parentName;
    curState.numOpenedTags = 1;
    curState.atEnd = false;
}

void ASNFormat::AsnParser::parseNextElement(AsnNode* node) {
    Q_ASSERT(insideRoot);

    while (!curState.atEnd) {
        if (readNextElement()) {
            if (curElementKind == ASN_VALUE) {
                auto child = new AsnNode(curElementName, curElementKind, node);
                child->value = curElementValue;
            } else if (curElementKind == ASN_SEQ) {
                saveState();
                auto child = new AsnNode(curElementName, curElementKind, node);
                parseNextElement(child);
                restoreState();
            }
        } else if (fileAtEnd) {
            break;
        }
    }
}

bool ASNFormat::AsnParser::readNextElement() {
    Q_ASSERT(insideRoot);
    buffer.clear();

    char ch;
    bool insideQuotes = false;
    while (io->getChar(&ch)) {
        if (ch == '\"') {
            insideQuotes = !insideQuotes;
        }

        if ((ch == '{') && (!insideQuotes)) {
            ++curState.numOpenedTags;
            if (curState.numOpenedTags == 2) {
                // load current Element
                curElementName = buffer.trimmed();
                curElementKind = ASN_SEQ;
                return true;
            }
        }

        if ((ch == ',') && (!insideQuotes)) {
            if (curState.numOpenedTags == 2) {
                --curState.numOpenedTags;
                // sequence is finished, clear the buffer
                buffer.clear();
                continue;
            } else {
                // read value
                processValue();
                return true;
            }
        }

        if ((ch == '}') && (!insideQuotes)) {
            --curState.numOpenedTags;
            curState.atEnd = true;
            if (!buffer.trimmed().isEmpty()) {
                // read value
                processValue();
                return true;
            }
            return false;
        }

        buffer += ch;
        prev = ch;
    }
    fileAtEnd = true;
    return false;
}

void ASNFormat::AsnParser::saveState() {
    states.push(curState);
    // init new state
    initState(curElementName);
}

void ASNFormat::AsnParser::restoreState() {
    Q_ASSERT(!states.empty());
    curState = states.pop();
}

void ASNFormat::AsnParser::processValue() {
    curElementValue.clear();
    QByteArray valBuffer = buffer.trimmed();
    QList<QByteArray> valueList = valBuffer.split(' ');
    int numElements = valueList.count();
    Q_ASSERT(numElements != 0);
    if (numElements == 1) {
        // there is only value
        curElementName.clear();
    } else {
        curElementName = valueList.first().trimmed();
        int numChars = curElementName.length() + 1;
        valBuffer.remove(0, numChars);
    }
    curElementValue = valBuffer;
    if (isQuoted(curElementValue)) {
        removeQuotes(curElementValue);
    }
    curElementKind = ASN_VALUE;
}

void ASNFormat::AsnParser::dbgPrintAsnTree(const AsnNode* rootElem, int deepness) {
    ++deepness;
    for (const AsnNode* node: qAsConst(rootElem->getChildren())) {
        QString str;
        for (int i = 0; i < deepness; ++i) {
            str += "  ";
        }
        str += QString("%1 (node)").arg(QString(node->name));
        if (node->kind == ASN_VALUE) {
            str += QString(" value = %1").arg(QString(node->value));
        }
        ioLog.trace(str);
        if (node->getChildren().count() != 0) {
            dbgPrintAsnTree(node, deepness);
        }
    }
    --deepness;
}

bool ASNFormat::AsnParser::isQuoted(const QByteArray& str) {
    int length = str.length();
    static const char quoteChar('\"');
    if ((str.at(0) == quoteChar) && (str.at(length - 1) == quoteChar)) {
        return true;
    }
    return false;
}

void ASNFormat::AsnParser::removeQuotes(QByteArray& str) {
    int len = str.length();
    str.remove(len - 1, 1);
    str.remove(0, 1);
}

void ASNFormat::AsnParser::dbgPrintCurrentState() {
    qDebug("current element");
    qDebug("\tname: %s", curElementName.constData());
    qDebug("\tbuffer: %s", buffer.trimmed().constData());
    qDebug("\tvalue: %s", curElementValue.constData());
    switch (curElementKind) {
        case ASN_ROOT:
            qDebug("\tkind: ASN_ROOT");
            break;
        case ASN_SEQ:
            qDebug("\tkind: ASN_SEQ");
            break;
        case ASN_VALUE:
            qDebug("\tkind: ASN_VALUE");
            break;
        default:
            Q_ASSERT(0);
    }
    if (haveErrors)
        qDebug("\terrors: yes");
    else
        qDebug("\terrors: no");

    qDebug();
}
///////////////////////////////////////////////////////////////////////////////////////////

AsnNode::AsnNode(const QByteArray& _name, const AsnElementKind& _kind, AsnNode* _parent)
    : name(_name), kind(_kind), parent(_parent) {
    if (parent != nullptr) {
        parent->children.append(this);
    }
}

AsnNode::~AsnNode() {
    if (parent != nullptr) {
        parent->children.removeOne(this);
    }
    QList<AsnNode*> childrenCopy = children;
    children.clear();
    for (AsnNode* child : qAsConst(childrenCopy)) {
        delete child;
    }
}

const QList<AsnNode*>& AsnNode::getChildren() const {
    return children;
}

AsnNode* AsnNode::findChildByName(const QByteArray& childName) const {
    for (AsnNode* child : qAsConst(children)) {
        if (child->name == childName) {
            return child;
        }
    }
    return nullptr;
}

AsnNode* AsnNode::getChild(int index) const {
    SAFE_POINT(index >= 0 && index < children.size(), "Invalid child node index: " + QString::number(index) + ", count: " + children.size(), nullptr);
    return children.at(index);
}

}  // namespace U2
