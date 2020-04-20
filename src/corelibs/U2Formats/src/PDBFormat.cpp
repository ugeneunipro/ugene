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

#include <time.h>

#include <QStringList>

#include <U2Algorithm/MolecularSurface.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "PDBFormat.h"

namespace U2 {

QHash<QByteArray,int> PDBFormat::atomNumMap = createAtomNumMap();
QHash<QByteArray, char> PDBFormat::acronymNameMap;

PDBFormat::PDBFormat(QObject* p) : TextDocumentFormat(p, BaseDocumentFormats::PLAIN_PDB, DocumentFormatFlag(0), QStringList("pdb"))
{
    formatName = tr("PDB");
    formatDescription = tr("The Protein Data Bank (PDB) format provides a standard representation for macromolecular structure data derived from X-ray diffraction and NMR studies.");

    initUtilityMaps();
    AtomConstants::init();
    supportedObjectTypes+=GObjectTypes::BIOSTRUCTURE_3D;
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    supportedObjectTypes+=GObjectTypes::ANNOTATION_TABLE;
}

FormatCheckResult PDBFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    static QList<QByteArray> tags = QList<QByteArray>()
        << "HEADER" << "ATOM" << "MODEL" << "REMARK" << "OBSLTE"
        << "TITLE" << "SPLIT" << "CAVEAT" << "COMPND" << "SOURCE"
        << "KEYWDS" << "EXPDTA" << "AUTHOR" << "REVDAT" << "SPRSDE" << "JRNL";

    bool ok = false;
    foreach (const QByteArray &tag, tags) {
        if (rawData.startsWith(tag)) {
            ok = true;
            break;
        }
    }

    if (!ok) {
        return FormatDetection_NotMatched;
    }

    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, rawData.constData(), rawData.size());
    return hasBinaryData ? FormatDetection_NotMatched : FormatDetection_HighSimilarity;
}


Document* PDBFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    GUrl url = io->getURL();
    ioLog.trace("Start PDB parsing: " +url.getURLString());

    BioStruct3D bioStruct;
    PDBParser pdbParser(io);
    clock_t t1 =  clock();
    pdbParser.parseBioStruct3D(bioStruct, os);
    clock_t t2 =  clock();
    perfLog.trace("PDB file parsing time: " + QString::number((float)(t2 - t1)/ CLOCKS_PER_SEC));

    CHECK_OP(os, NULL);
    io->close();
    os.setProgress(80);

    algoLog.trace("Calculating center and max distance...");
    bioStruct.calcCenterAndMaxDistance();
    os.setProgress(85);

    algoLog.trace("Generating chains annotations...");
    os.setProgress(90);
    calculateBonds(bioStruct);

    algoLog.trace("Calculating bonds...");
    Document* doc = createDocumentFromBioStruct3D(dbiRef, bioStruct, this,  io->getFactory(), url, os, fs);

    ioLog.trace("PDB parsing finished: " + url.getURLString());
    os.setProgress(100);
    return doc;
}

void PDBFormat::initUtilityMaps()
{
    static bool initialized = false;

    if (!initialized) {

        // init acronymNameMap

        // amino acid residues
        acronymNameMap.insert("ALA", 'A');
        acronymNameMap.insert("VAL", 'V');
        acronymNameMap.insert("PHE", 'F');
        acronymNameMap.insert("PRO", 'P');
        acronymNameMap.insert("MET", 'M');
        acronymNameMap.insert("ILE", 'I');
        acronymNameMap.insert("LEU", 'L');
        acronymNameMap.insert("ASP", 'D');
        acronymNameMap.insert("GLU", 'E');
        acronymNameMap.insert("GLY", 'G');
        acronymNameMap.insert("LYS", 'K');
        acronymNameMap.insert("ARG", 'R');
        acronymNameMap.insert("SER", 'S');
        acronymNameMap.insert("THR", 'T');
        acronymNameMap.insert("TYR", 'Y');
        acronymNameMap.insert("HIS", 'H');
        acronymNameMap.insert("CYS", 'C');
        acronymNameMap.insert("ASN", 'N');
        acronymNameMap.insert("GLN", 'Q');
        acronymNameMap.insert("TRP", 'W');
        // dna
        acronymNameMap.insert("DA", 'A');
        acronymNameMap.insert("DT", 'T');
        acronymNameMap.insert("DG", 'G');
        acronymNameMap.insert("DC", 'C');
        acronymNameMap.insert("A", 'A');
        acronymNameMap.insert("T", 'T');
        acronymNameMap.insert("G", 'G');
        acronymNameMap.insert("C", 'C');
        acronymNameMap.insert("U", 'U');

        // now we've initialized utility maps
        initialized = true;
    }
}

PDBFormat::PDBParser::PDBParser(IOAdapter* _io)
    : io(_io),
      currentPDBLine(""),
      currentChainIndex(1),
      currentMoleculeIndex(0),
      residueOrder(0)
{
    currentModelIndex = 0;
    currentChainIndentifier = ' ';
    flagMultipleModels = false;
    flagAtomRecordPresent = false;
}

void PDBFormat::PDBParser::parseBioStruct3D(BioStruct3D& biostruct, U2OpStatus& ti) {

    QByteArray readBuff(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buf = readBuff.data();
    qint64 len = 0;
    bool firstCompndLine = true;
    while (!ti.isCoR()) {

        bool lineOk = true;

        len = io->readUntil(buf, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        if (len == 0) {
            break;
        }

        // there could be no terminator if this is end of file, so we have to check for this
        if (!lineOk && !io->isEof()) {
            ti.setError(U2::PDBFormat::tr("Line is too long"));
            return;
        }
        currentPDBLine = QString(QByteArray(buf, len));

        ti.setProgress(io->getProgress() * 0.8);

        if (currentPDBLine.startsWith("HEADER")) {
            parseHeader(biostruct, ti);
            continue;
        }

        if (currentPDBLine.startsWith("COMPND")) {
            parseMacromolecularContent(firstCompndLine, ti);
            firstCompndLine = false;
            continue;
        }

        if (currentPDBLine.startsWith("SEQRES")) {
            parseSequence(biostruct, ti);
            continue;
        }


        if (currentPDBLine.startsWith("HELIX ") || currentPDBLine.startsWith("SHEET ")
            || currentPDBLine.startsWith("TURN  ")) {
                parseSecondaryStructure(biostruct, ti);
                continue;
        }


        if (currentPDBLine.startsWith("ATOM  ") || currentPDBLine.startsWith("HETATM")) {
            parseAtom(biostruct, ti);
            continue;
        }

        if (currentPDBLine.startsWith("TER")) {
            ++currentChainIndex;
            continue;
        }

        if (currentPDBLine.startsWith("SPLIT ")) {
            parseSplitSection(ti);
            continue;
        }

        if (currentPDBLine.startsWith("MODEL")) {
            currentChainIndex = 1;
            parseModel(biostruct, ti);
            continue;
        }

        if (currentPDBLine.startsWith("ENDMDL")) {
            flagMultipleModels = true;
            ++currentModelIndex;
            continue;
        }
    }

    CHECK_OP(ti,);

    if (!flagAtomRecordPresent) {
        ti.setError(U2::PDBFormat::tr("Some mandatory records are absent"));
    }

    updateSecStructChainIndexes(biostruct);

}


namespace {
const QString MOLECULE_TAG = "MOLECULE";
const QString CHAIN_TAG = "CHAIN";
}

void PDBFormat::PDBParser::parseMacromolecularContent(bool firstCompndLine, U2OpStatus&) {
    /*
    Record Format
    COLUMNS       DATA TYPE       FIELD         DEFINITION
    ----------------------------------------------------------------------------------
    1 -  6       Record name     "COMPND"
    8 - 10       Continuation    continuation  Allows concatenation of multiple records.
    11 - 80      Specification   compound      Description of the molecular components.
    list
    Details
    */
    if (!firstCompndLine) {
        QString specification = currentPDBLine.mid(10, currentPDBLine.size() - 11).trimmed().toLatin1();
        if (specification.startsWith(MOLECULE_TAG)) {
            QRegExp end(";\\s*$");
            int index = end.indexIn(specification);
            index = (index > 0) ? index : specification.size();
            currentMoleculeName = specification.mid(MOLECULE_TAG.size() + 1, index - MOLECULE_TAG.size() - 1).trimmed();
        } else if (specification.startsWith(CHAIN_TAG)) {
            QStringList idetifiers = specification.split(QRegExp(",|:|;"));
            for (int i = 1; i < idetifiers.size(); i++) {
                QString identifier = idetifiers.at(i).trimmed();
                if (identifier.size() > 0 && !currentMoleculeName.isEmpty()) {
                    chainToMoleculeMap[identifier] = currentMoleculeName;
                }
            }
        }
    }
}

void PDBFormat::PDBParser::parseHeader(BioStruct3D& biostruct, U2OpStatus&)
{
    /*
    Record Format
    COLUMNS DATA TYPE FIELD DEFINITION
    ------------------------------------------------------------------------------------
    1 - 6       Record name "HEADER"
    11 - 50     Classifies the molecule(s).
    51 - 59     Deposition date. This is the date the coordinates were received at the PDB.
    63 - 66     This identifier is unique within the PDB.
    */

    QString desc = currentPDBLine.mid(10,40).trimmed();
    QByteArray pdbId = currentPDBLine.mid(62,4).toLatin1();
    biostruct.descr = desc;
    biostruct.pdbId = pdbId;
}

bool PDBFormat::PDBParser::seqResContains(char chainIdentifier, int residueIndex, char acronym)
{
    if (seqResMap.isEmpty()) {
        // the PDB file doesn't have seq res record -> we cannot use this method
        return true;
    }

    if (!seqResMap.contains(chainIdentifier) || residueIndex == 0) {
        return false;
    }
    QByteArray sequence = seqResMap.value(chainIdentifier);
    int numChars  = sequence.size();
    if (residueIndex <= numChars) {
        return sequence[residueIndex - 1] == acronym;
    } else {
        return false;
    }
}

void PDBFormat::PDBParser::parseAtom(BioStruct3D& biostruct, U2OpStatus&)
{
    /*
    Record Format

    COLUMNS     DEFINITION
    1 - 6       Record name "ATOM "
    7 - 11      Atom serial number.
    13 - 16     Atom name.
    17          Alternate location indicator.
    18 - 20     Residue name.
    22          Chain identifier.
    23 - 26     Residue sequence number.
    27          Code for insertion of residues.
    31 - 38     Orthogonal coordinates for X in Angstroms.
    39 - 46     Orthogonal coordinates for Y in Angstroms.
    47 - 54     Orthogonal coordinates for Z in Angstroms.
    55 - 60     Occupancy.
    61 - 66     Temperature factor.
    77 - 78     Element symbol, right-justified.
    79 - 80     Charge on the atom.
    */

    if (!flagAtomRecordPresent)
        flagAtomRecordPresent = true;

    bool isHetero = false;
    if (currentPDBLine.startsWith("HETATM")) {
        isHetero = true;
    }

    int id = currentPDBLine.mid(6,5).toInt();
    QByteArray atomName = currentPDBLine.mid(12,4).toLatin1().trimmed();
    QByteArray residueName = currentPDBLine.mid(17,3).toLatin1().trimmed();
    int resId = currentPDBLine.mid(22,4).toLatin1().toInt();
    char insCode = currentPDBLine.at(26).toLatin1();
    char residueAcronym = PDBFormat::getAcronymByName(residueName);
    char chainIdentifier = currentPDBLine.at(21).toLatin1();

    ResidueIndex residueIndex(resId,insCode);
    bool atomIsInChain = !isHetero || seqResContains(chainIdentifier, residueIndex.toInt(), residueAcronym);

    QByteArray elementName = currentPDBLine.mid(76,2).toLatin1().trimmed();

    QByteArray element = elementName.isEmpty() ? atomName.mid(0,1) : elementName;
    int atomicNumber = PDBFormat::getElementNumberByName(element);

    int chainIndex = chainIndexMap.contains(chainIdentifier) ? chainIndexMap.value(chainIdentifier) : currentChainIndex;

    if (currentModelIndex == 0 && atomIsInChain) {

        // Process residue

        if (!biostruct.moleculeMap.contains(chainIndex)) {
            createMolecule(chainIdentifier, biostruct, chainIndex);
        }

        SharedMolecule& mol = biostruct.moleculeMap[chainIndex];

        if (currentResidueIndex != residueIndex)  {
            SharedResidue residue(new ResidueData);
            residue->name = residueName;
            residue->acronym = residueAcronym;
            if (residue->acronym == 'X') {
                ioLog.details(tr("PDB warning: unknown residue name: %1").arg(residue->name.constData()));
            }
            residue->chainIndex = chainIndex;
            currentResidueIndex = residueIndex;
            residueOrder++;
            residueIndex.setOrder(residueOrder);
            mol->residueMap.insert(residueIndex, residue);
        }

    }

    // Process atom
    double x,y,z;
    x = currentPDBLine.mid(30,8).toDouble();
    y = currentPDBLine.mid(38,8).toDouble();
    z = currentPDBLine.mid(46,8).toDouble();
    double occupancy = currentPDBLine.mid(54,6).toDouble();
    double temperature = currentPDBLine.mid(60,6).toDouble();

    SharedAtom a(new AtomData);
    a->chainIndex = chainIndex;
    a->residueIndex = residueIndex;
    a->atomicNumber = atomicNumber;
    a->name = atomName;
    a->coord3d = Vector3D(x,y,z);
    a->occupancy = occupancy;
    a->temperature = temperature;

    biostruct.modelMap[currentModelIndex + 1].insert(id, a);

    if (atomIsInChain) {
        SharedMolecule& mol = biostruct.moleculeMap[chainIndex];
        Molecule3DModel& model3D = mol->models[currentModelIndex];
        model3D.atoms.insert(id, a);
    }

}

void PDBFormat::PDBParser::parseSecondaryStructure(BioStruct3D& biostruct, U2OpStatus& ti)
{
    /*

    Record Format Examples

    1         2         3
    0123456789012345678901234567890123456
    HELIX    1  H1 ILE      7  LEU     18
    HELIX    2  H2 PRO     19  PRO     19
    HELIX    3  H3 GLU     23  TYR     29
    HELIX    4  H4 THR     30  THR     30
    SHEET    1  S1 2 THR     2  CYS     4
    SHEET    2  S2 2 CYS    32  ILE    35
    SHEET    3  S3 2 THR    39  PRO    41
    TURN     1  T1 GLY    42  TYR    44

    */

    SecondaryStructure::Type structureType = SecondaryStructure::Type_None;
    int startIndex;
    int endIndex;
    int startChainIDIndex;
//    int endChainIDIndex;


    if (currentPDBLine.startsWith("HELIX ")) {
        structureType = SecondaryStructure::Type_AlphaHelix;
        startChainIDIndex = 19;
//        endChainIDIndex = 31;
        startIndex = 21;
        endIndex = 33;
    } else if (currentPDBLine.startsWith("SHEET ")) {
        structureType = SecondaryStructure::Type_BetaStrand;
        startChainIDIndex = 21;
//        endChainIDIndex = 32;
        startIndex = 22;
        endIndex = 33;
    } else if (currentPDBLine.startsWith("TURN  ")) {
        // TODO: is this is obsolete?
        structureType = SecondaryStructure::Type_Turn;
        startChainIDIndex = 19;
//        endChainIDIndex = 30;
        startIndex = 20;
        endIndex = 31;
    } else {
        Q_ASSERT(0);
        return;
    }

    if (currentPDBLine.length() < endIndex + 4)
    {
        ti.setError(U2::PDBFormat::tr("Invalid secondary structure record"));
        return;
    }

    char startChainID = currentPDBLine.at(startChainIDIndex).toLatin1();

    int startSequenceNumber = currentPDBLine.mid(startIndex, 4).toInt();
    int endSequenceNumber = currentPDBLine.mid(endIndex, 4).toInt();

    SharedSecondaryStructure secStruct(new SecondaryStructure);
    secStruct->type = structureType;
    secStruct->chainIdentifier = startChainID;
    secStruct->startSequenceNumber = startSequenceNumber;
    secStruct->endSequenceNumber = endSequenceNumber;
    biostruct.secondaryStructures.append(secStruct);
}

void PDBFormat::PDBParser::parseSequence(BioStruct3D& biostruct, U2OpStatus& ti)
{
    Q_UNUSED(biostruct);
    /*
    Record Format

    COLUMNS     DEFINITION
    1 - 6       Record name "SEQRES"
    8 - 10      Integer serNum Serial number of the SEQRES record for the current chain.
                Starts at 1 and increments by one each line. Reset to 1 for each chain.
    12          Character chainID Chain identifier. This may be any single legal character,
                including a blank which is is used if there is only one chain.
    14 - 17     Integer numRes Number of residues in the chain.
                This value is repeated on every record.
    20 - ...    Residues
    */


    if (currentPDBLine.length() < 24 /* at least one residue */)
    {
        ti.setError(U2::PDBFormat::tr("Invalid SEQRES: less then 24 characters"));
        return;
    }

    char chainIdentifier = currentPDBLine.at(11).toLatin1();
    if (!seqResMap.contains(chainIdentifier)) {
        seqResMap.insert(chainIdentifier, QByteArray());
    }

    QStringList residues = currentPDBLine.mid(19).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QByteArray sequencePart;
    foreach (QString name, residues) {
        SharedResidue residue(new ResidueData);
        char acronym = PDBFormat::getAcronymByName(name.toLatin1());
        sequencePart.append(acronym);
    }
    seqResMap[chainIdentifier].append(sequencePart);

}


QByteArray PDBFormat::PDBParser::getSpecValue(const QByteArray& specLine, const QByteArray& valueName)
{
    const char* delimTag(";");
    int startIndex = specLine.indexOf(valueName) + valueName.length() + 1;
    int lastIndex = specLine.indexOf(delimTag);
    return specLine.mid(startIndex, lastIndex - startIndex).trimmed();
}

int PDBFormat::getElementNumberByName(const QByteArray& elementName)
{
    if (atomNumMap.contains(elementName))
        return atomNumMap.value(elementName);
    else
        return 0;
}

QByteArray PDBFormat::PDBParser::getNextSpecLine()
{
    QByteArray readBuf(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buf = readBuf.data();
    bool lineOk;
    int len = io->readUntil(buf, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    // retrieve back ioAdapter position
    io->skip(-len);
    return readBuf;

}

void PDBFormat::PDBParser::parseSplitSection(U2OpStatus& /*ti*/)
{
   /* COLUMNS DATA TYPE FIELD DEFINITION
    1 - 6 Record name "SPLIT "
    9 - 10 Continuation continuation Allows concatenation of multiple records.
    12 - 15 IDcode idCode ID code of related entry.
    .. every 2 spaces IDcode*/
    QStringList ids = currentPDBLine.mid(11).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    ioLog.trace(QString("The list of SPLIT ids is %1").arg(ids.join(",")));


}


void PDBFormat::PDBParser::parseModel(BioStruct3D& biostruct, U2OpStatus& ti)
{
    /*
    COLUMNS        DATA  TYPE    FIELD          DEFINITION
    1 -  6        Record name   "MODEL "
    11 - 14        Integer       serial         Model serial number.
    */

    //TODO: int modelIndex = currentPDBLine.mid(10,4).toInt();
    Q_UNUSED(biostruct);
    Q_UNUSED(ti);
}

void PDBFormat::PDBParser::updateSecStructChainIndexes(BioStruct3D& biostruc)
{
    QMutableListIterator<SharedSecondaryStructure> i(biostruc.secondaryStructures);
    while (i.hasNext()) {
        SharedSecondaryStructure& secStruc = i.next();
        char chainIdentifier = secStruc->chainIdentifier;
        if (!chainIndexMap.contains(chainIdentifier)) {
            i.remove();
            continue;
        }
        secStruc->chainIndex = chainIndexMap.value(chainIdentifier);

    }
}

void PDBFormat::PDBParser::createMolecule(char chainIdentifier, BioStruct3D &biostruct, int chainIndex) {
    SharedMolecule newMol(new MoleculeData);
    newMol->chainId = chainIdentifier;
    if (chainToMoleculeMap.contains(QString(chainIdentifier))) {
        newMol->name = chainToMoleculeMap[QString(chainIdentifier)];
    }
    biostruct.moleculeMap.insert(chainIndex,newMol);
    chainIndexMap.insert(chainIdentifier, chainIndex);
}

void PDBFormat::PDBParser::updateResidueIndexes(BioStruct3D& /*biostruc*/)
{
}


char PDBFormat::getAcronymByName(const QByteArray& name)
{
    if (acronymNameMap.contains(name))
        return acronymNameMap.value(name);
    else
        return char('X');

}

static inline bool existsCovalentBond(double r1, double r2, double distance) {
    static const double tolerance = 0.45;

    if (distance <= r1 + r2 + tolerance) {
        return true;
    }

    return false;
}

void PDBFormat::calculateBonds(BioStruct3D& bioStruct)
{
    // maxDisance = 2*MaxCovalentRadius + tolerance
    static const double maxRadius = 2.0;
    static const double maxDistance = 2*maxRadius + 0.45;

    //quint64 num_comps = 0, firstFilter = 0, secondFilter = 0;
    clock_t t1 =  clock();
    QMap<int, SharedMolecule>::iterator molIter;
    QMap<int, Molecule3DModel>::iterator modelIter;
    for (molIter = bioStruct.moleculeMap.begin(); molIter != bioStruct.moleculeMap.end(); ++molIter) {
        SharedMolecule& mol = molIter.value();
        for (modelIter = mol->models.begin(); modelIter != mol->models.end(); modelIter++) {
            Molecule3DModel& model = modelIter.value();
            QList<SharedAtom>::const_iterator i1,i2;
            QList<SharedAtom>::const_iterator constEnd = model.atoms.constEnd();
            for (i1 = model.atoms.constBegin(); i1 != constEnd; ++i1) {
                 const SharedAtom& a1 = (*i1);
                 double r1 = AtomConstants::getAtomCovalentRadius(a1->atomicNumber);
                 i2 = i1;
                 for (++i2; i2 != constEnd; ++i2) {
                    //++num_comps;
                    const SharedAtom& a2 = (*i2);
                    if ((qAbs(a2->coord3d.x - a1->coord3d.x) > maxDistance) ||
                        (qAbs(a2->coord3d.y - a1->coord3d.y) > maxDistance) ||
                        (qAbs(a2->coord3d.z - a1->coord3d.z) > maxDistance) ) {
                        continue;
                    }
                    //++firstFilter;
                    if ((qAbs(a2->coord3d.x - a1->coord3d.x) + qAbs(a2->coord3d.y - a1->coord3d.y) +
                            qAbs(a2->coord3d.z - a1->coord3d.z)) > maxDistance) {
                        continue;
                    }
                    //++secondFilter;
                    double distance = (a1->coord3d - a2->coord3d).length();
                    double r2 = AtomConstants::getAtomCovalentRadius(a2->atomicNumber);
                    if (existsCovalentBond(r1, r2, distance)) {
                        model.bonds.append(Bond(a1, a2));
                    }
                 }
            }
        }
    }
    clock_t t2 = clock();
    perfLog.trace("PDB bonds calculation time: " + QString::number((float)(t2 - t1)/ CLOCKS_PER_SEC));
}

QHash<QByteArray, int> PDBFormat::createAtomNumMap()
{
    QHash<QByteArray, int> atomNumMap;

    // Init atomNumMap
    atomNumMap.insert("H",1);
    atomNumMap.insert("HE",2);
    atomNumMap.insert("LI",3);
    atomNumMap.insert("BE",4);
    atomNumMap.insert("B",5);
    atomNumMap.insert("C",6);
    atomNumMap.insert("N",7);
    atomNumMap.insert("O",8);
    atomNumMap.insert("F",9);
    atomNumMap.insert("NE",10);
    atomNumMap.insert("NA",11);
    atomNumMap.insert("MG",12);
    atomNumMap.insert("AL",13);
    atomNumMap.insert("SI",14);
    atomNumMap.insert("P",15);
    atomNumMap.insert("S",16);
    atomNumMap.insert("CL",17);
    atomNumMap.insert("AR",18);
    atomNumMap.insert("K",19);
    atomNumMap.insert("CA",20);
    atomNumMap.insert("SC",21);
    atomNumMap.insert("TI",22);
    atomNumMap.insert("V",23);
    atomNumMap.insert("CR",24);
    atomNumMap.insert("MN",25);
    atomNumMap.insert("FE",26);
    atomNumMap.insert("CO",27);
    atomNumMap.insert("NI",28);
    atomNumMap.insert("CU",29);
    atomNumMap.insert("ZN",30);
    atomNumMap.insert("GA",31);
    atomNumMap.insert("GE",32);
    atomNumMap.insert("AS",33);
    atomNumMap.insert("SE",34);
    atomNumMap.insert("BR",35);
    atomNumMap.insert("KR",36);
    atomNumMap.insert("RB",37);
    atomNumMap.insert("SR",38);
    atomNumMap.insert("Y",39);
    atomNumMap.insert("ZR",40);
    atomNumMap.insert("NB",41);
    atomNumMap.insert("MO",42);
    atomNumMap.insert("TC",43);
    atomNumMap.insert("RU",44);
    atomNumMap.insert("RH",45);
    atomNumMap.insert("PD",46);
    atomNumMap.insert("AG",47);
    atomNumMap.insert("CD",48);
    atomNumMap.insert("IN",49);
    atomNumMap.insert("SN",50);
    atomNumMap.insert("SB",51);
    atomNumMap.insert("TE",52);
    atomNumMap.insert("I",53);
    atomNumMap.insert("XE",54);
    atomNumMap.insert("CS",55);
    atomNumMap.insert("BA",56);
    atomNumMap.insert("LA",57);
    atomNumMap.insert("CE",58);
    atomNumMap.insert("PR",59);
    atomNumMap.insert("ND",60);
    atomNumMap.insert("PM",61);
    atomNumMap.insert("SM",62);
    atomNumMap.insert("EU",63);
    atomNumMap.insert("GD",64);
    atomNumMap.insert("TB",65);
    atomNumMap.insert("DY",66);
    atomNumMap.insert("HO",67);
    atomNumMap.insert("ER",68);
    atomNumMap.insert("TM",69);
    atomNumMap.insert("YB",70);
    atomNumMap.insert("LU",71);
    atomNumMap.insert("HF",72);
    atomNumMap.insert("TA",73);
    atomNumMap.insert("W",74);
    atomNumMap.insert("RE",75);
    atomNumMap.insert("OS",76);
    atomNumMap.insert("IR",77);
    atomNumMap.insert("PT",78);
    atomNumMap.insert("AU",79);
    atomNumMap.insert("HG",80);
    atomNumMap.insert("TL",81);
    atomNumMap.insert("PB",82);
    atomNumMap.insert("BI",83);
    atomNumMap.insert("PO",84);
    atomNumMap.insert("AT",85);
    atomNumMap.insert("RN",86);
    atomNumMap.insert("FR",87);
    atomNumMap.insert("RA",88);
    atomNumMap.insert("AC",89);
    atomNumMap.insert("TH",90);
    atomNumMap.insert("PA",91);
    atomNumMap.insert("U",92);
    atomNumMap.insert("NP",93);
    atomNumMap.insert("PU",94);
    atomNumMap.insert("AM",95);
    atomNumMap.insert("CM",96);
    atomNumMap.insert("BK",97);
    atomNumMap.insert("CF",98);
    atomNumMap.insert("ES",99);
    atomNumMap.insert("FM",100);
    atomNumMap.insert("MD",101);
    atomNumMap.insert("NO",102);
    atomNumMap.insert("LR",103);

    return atomNumMap;
}

Document * PDBFormat::createDocumentFromBioStruct3D(const U2DbiRef &dbiRef, BioStruct3D &bioStruct,
    DocumentFormat *format, IOAdapterFactory *iof, const GUrl &url, U2OpStatus &os, const QVariantMap &fs)
{
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    QList<GObject*> objects;
    QSet<QString> uniqueNames;
    QMap<AnnotationTableObject *, U2SequenceObject *> relationsMap;
    QString objectName = bioStruct.pdbId.isEmpty() ? url.baseFileName() : bioStruct.pdbId;
    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    QVariantMap hints;
    hints.insert(DBI_FOLDER_HINT, folder);
    BioStruct3DObject* biostrucObj = BioStruct3DObject::createInstance(bioStruct, objectName, dbiRef, os, hints);
    CHECK_OP(os, NULL);
    QMap<int, QList<SharedAnnotationData> > anns = bioStruct.generateAnnotations();
    TmpDbiObjects dbiObjects(dbiRef, os);
    foreach(int key, bioStruct.moleculeMap.keys()) {
        // Create dna sequence object
        QByteArray sequence = bioStruct.getRawSequenceByChainIndex(key);
        char chainId = bioStruct.getChainIdByIndex(key);
        QString sequenceName;
        if (chainId > 0) {
            sequenceName = QString(bioStruct.pdbId) + QString(" chain %1 sequence").arg(chainId);
        } else {
            sequenceName = QString(bioStruct.pdbId) + QString(" chain %1 sequence").arg(key);
        }
        if (sequenceName.isEmpty()){
            sequenceName = "Sequence";
        }
        sequenceName = TextUtils::variate(sequenceName, "_", uniqueNames);
        sequenceName.squeeze();
        uniqueNames.insert(sequenceName);
        const DNAAlphabet* al = U2AlphabetUtils::findBestAlphabet(sequence);
        DNASequence dnaSeq(sequenceName, sequence, al);
        dnaSeq.info.insert(DNAInfo::DEFINITION, sequenceName);
        dnaSeq.info.insert(DNAInfo::COMMENT, bioStruct.descr);
        dnaSeq.info.insert(DNAInfo::CHAIN_ID, key);
        U2EntityRef ref = U2SequenceUtils::import(os, dbiRef, folder, dnaSeq, dnaSeq.alphabet->getId());
        U2SequenceObject* seqObj = new U2SequenceObject(dnaSeq.getName(), ref);
        SAFE_POINT(seqObj, QString("Got NULL object from DocumentFormatUtils addSequenceObjectDeprecated, os.error = %1").arg(os.getError()), NULL);
        objects.append(seqObj);
        dbiObjects.objects << seqObj->getSequenceRef().entityId;
        if (os.isCoR()) {
            break;
        }

        // create AnnnotationTableObject
        QString annotationTableName;
        if (chainId > 0) {
            annotationTableName = QString(bioStruct.pdbId) + QString(" chain %1 annotation").arg(chainId);
        } else {
            annotationTableName = QString(bioStruct.pdbId) + QString(" chain %1 annotation").arg(key);
        }
        AnnotationTableObject *aObj = new AnnotationTableObject(annotationTableName, dbiRef, hints);
        aObj->addAnnotations(anns.value(key));

        objects.append(aObj);
        relationsMap.insert(aObj, seqObj);
    }
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    objects.append(biostrucObj);
    Document* doc = new Document(format, iof, url, dbiRef, objects, fs);

    // set object relations
    QMap<AnnotationTableObject *, U2SequenceObject*>::const_iterator i;
    for (i = relationsMap.constBegin(); i != relationsMap.constEnd(); ++i) {
        U2SequenceObject* dnao = i.value();
        AnnotationTableObject *ao = i.key();
        ao->addObjectRelation(dnao, ObjectRole_Sequence);
        biostrucObj->addObjectRelation(dnao, ObjectRole_Sequence);
        biostrucObj->addObjectRelation(ao, ObjectRole_AnnotationTable);
    }

    return doc;
}

} // namespace U2
