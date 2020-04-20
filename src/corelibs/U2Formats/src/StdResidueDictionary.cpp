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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>

#include "StdResidueDictionary.h"
#include "ASNFormat.h"
#include "PDBFormat.h"

namespace U2 {

QMutex StdResidueDictionary::standardDictionaryLock;
QScopedPointer<StdResidueDictionary> StdResidueDictionary::standardDictionary(NULL);

bool StdResidueDictionary::load( const QString& fileName ) {

    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);

    if (!iof) {
        return false;
    }

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());

    bool ok = io->open(fileName, IOAdapterMode_Read);
    if (!ok) {
        return false;
    }

    U2OpStatus2Log ti;
    ASNFormat::AsnParser asnParser(io.data(),ti);
    AsnNode* rootElem = asnParser.loadAsnTree();
    if (ti.hasError()) {
       return false;
    }

    buildDictionaryFromAsnTree(rootElem);

    return true;
}

#define STD_DICT_FILE_NAME ":format/datafiles/MMDBStdResidueDict"

StdResidueDictionary* StdResidueDictionary::createStandardDictionary()
{
    StdResidueDictionary* stdDictionary = new StdResidueDictionary();
    if (!stdDictionary->load(STD_DICT_FILE_NAME)) {
        return NULL;
    }

    if (!stdDictionary->validate()) {
        return NULL;
    }

    return stdDictionary;
}

void buildStdAtomFromNode(AsnNode* atomNode, StdAtom& atom)
{
    /*
        Atom ::= SEQUENCE {
            id			Atom-id,
            name			VisibleString OPTIONAL,
            iupac-code		SEQUENCE OF VisibleString OPTIONAL,
            element			ENUMERATED { ... },
            ionizable-proton	ENUMERATED {
                                true(1),
                                false(2),
                                unknown(255) } OPTIONAL
          }
    */

    // Load atom name
    atom.name = atomNode->getChildById(1)->value.trimmed();
    // Load atom number
    QByteArray elementName = atomNode->getChildById(3)->value;
    atom.atomicNum = PDBFormat::getElementNumberByName(elementName.toUpper());
}


void buildStdResidueFromNode(AsnNode* residueNode, StdResidue& residue)
{
    /*
        Residue-graph ::= SEQUENCE {
            id			Residue-graph-id,
            descr			SEQUENCE OF Biomol-descr OPTIONAL,
            residue-type	INTEGER {	deoxyribonucleotide(1),
                                        ribonucleotide(2),
                                        amino-acid(3),
                                        other(255) } OPTIONAL,
            iupac-code		SEQUENCE OF VisibleString OPTIONAL,
            atoms			SEQUENCE OF Atom,
            bonds			SEQUENCE OF Intra-residue-bond,
            chiral-centers	SEQUENCE OF Chiral-center OPTIONAL
        }
    */

    // we are making some assumptions about children indexes due to real file contents

    // Load residue name
    AsnNode* descrNode = residueNode->getChildById(1);
    residue.name = descrNode->getChildById(0)->value;

    // Load residue type
    QByteArray residueTypeName = residueNode->getChildById(2)->value;
    residue.type = StdResidueDictionary::getResidueTypeByName(residueTypeName);

    // Load residue code
    AsnNode* codeNode = residueNode->getChildById(3);
    residue.code = codeNode->getChildById(0)->value.at(0);

    // Load residue atoms
    AsnNode* atomsNode = residueNode->getChildById(4);
    foreach (AsnNode* node, atomsNode->children) {
        bool ok = false;
        int atomId = node->getChildById(0)->value.toInt(&ok);
        Q_ASSERT(ok == true);
        StdAtom atom;
        buildStdAtomFromNode(node, atom);
        residue.atoms.insert(atomId, atom);
    }

    // Load intra residue bonds
    AsnNode* bondsNode = residueNode->getChildById(5);
    foreach (AsnNode* node, bondsNode->children) {
        StdBond bond;
        bool id1OK = false, id2OK = false;
        bond.atom1Id = node->getChildById(0)->value.toInt(&id1OK);
        bond.atom2Id = node->getChildById(1)->value.toInt(&id2OK);
        Q_ASSERT(id1OK && id2OK);
        residue.bonds.append(bond);
    }

}

void StdResidueDictionary::buildDictionaryFromAsnTree( AsnNode* rootElem )
{
    /*
        Biostruc-residue-graph-set ::= SEQUENCE {
            id			    SEQUENCE OF Biostruc-id OPTIONAL,
            descr			SEQUENCE OF Biomol-descr OPTIONAL,
            residue-graphs	SEQUENCE OF Residue-graph
            }

        Residue-graph ::= SEQUENCE {
            id			Residue-graph-id,
            ...
        }
    */

    AsnNode* resGraphsNode = rootElem->findChildByName("residue-graphs");

    // Load residues
    foreach (AsnNode* child, resGraphsNode->children) {
        bool ok = false;
        int id = child->getChildById(0)->value.toInt(&ok);
        Q_ASSERT(ok == true);
        StdResidue residue;
        buildStdResidueFromNode(child, residue);
        residues.insert(id, residue);
    }


}

U2::ResidueType StdResidueDictionary::getResidueTypeByName( const QByteArray& name )
{
    if (name == "amino-acid") {
        return AMINO_ACID;
    } else if (name == "deoxyribonucleotide") {
        return DEOXYRIBONUCLEOTIDE;
    } else if (name == "ribonucleotide") {
        return RIBONUCLEOTIDE;
    } else {
        return OTHER;
    }
}



bool StdResidueDictionary::validate() const
{
    // TODO: replace with normal tests

    // simple validation tests
#ifdef DEBUG
    Q_ASSERT(residues.count() == 84);

    const StdResidue& asp = residues.value(10);
    Q_ASSERT(asp.name == "ASP");
    Q_ASSERT(asp.code == 'D');
    Q_ASSERT(asp.type == AMINO_ACID);
    Q_ASSERT(asp.atoms.count() == 14 );


    const QHash<int, StdAtom> aspAtoms = asp.atoms;
    const StdAtom& atom = aspAtoms.value(5);
    Q_ASSERT(atom.name == "H");
    Q_ASSERT(atom.atomicNum == 1);
#endif

    return true;
}



const StdResidueDictionary* StdResidueDictionary::getStandardDictionary()
{
    QMutexLocker locker( &standardDictionaryLock );
    if (standardDictionary.isNull()) {
        standardDictionary.reset( createStandardDictionary() );
    }

    return standardDictionary.data();
}

StdResidueDictionary::~StdResidueDictionary()
{

}

const StdResidue StdResidueDictionary::getResidueById( int id ) const
{
    return residues.value(id);
}

StdResidueDictionary* StdResidueDictionary::createFromAsnTree( AsnNode* rootElem )
{
    AsnNode* resGraphsNode = ASNFormat::findFirstNodeByName(rootElem, "residue-graphs");
    if (resGraphsNode == NULL) {
        return NULL;
    }

    StdResidueDictionary* localDictionary = new StdResidueDictionary();
    // Load residues
    foreach (AsnNode* child, resGraphsNode->children) {
        bool ok = false;
        int id = child->getChildById(0)->value.toInt(&ok);
        Q_ASSERT(ok == true);
        StdResidue residue;
        buildStdResidueFromNode(child, residue);
        localDictionary->residues.insert(id, residue);
    }

    return localDictionary;

}
} //namespace


