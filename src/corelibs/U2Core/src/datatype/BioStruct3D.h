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

#ifndef _U2_BIOSTRUCT3D_H_
#define _U2_BIOSTRUCT3D_H_

#include <U2Core/Vector3D.h>
#include <U2Core/global.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/Matrix44.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QSharedData>

namespace U2 {

class U2Region;

class U2CORE_EXPORT ResidueIndex {
    int resId;
    int order; // to keep order of residues in the chain
    char insCode;
public:
    ResidueIndex() : resId(0), order(0), insCode(' ') {}
    ResidueIndex(int residueIdx, char insertionCode) : resId(residueIdx), order(0), insCode(insertionCode) {}
    bool operator<(const ResidueIndex& other) const;
    bool operator==(const ResidueIndex& other) const;
    bool operator!=(const ResidueIndex& other) const;
    int toInt() const { return resId; }
    void setOrder(int ord) { order = ord; }

    int getOrder() const;
    char getInsCode() const;
};


//! Represents biopolimer residue
class ResidueData : public QSharedData {
public:
    enum Type {
        TYPE_UNKNOWN, TYPE_PROTEIN, TYPE_DNA, TYPE_RNA
    };
    Type type;
    QByteArray name;
    char acronym;
    int chainIndex;
    ResidueData() : type(TYPE_UNKNOWN), acronym(' '), chainIndex(0) {}
};

typedef QSharedDataPointer<ResidueData> SharedResidue;

//! Represents atom
class AtomData : public QSharedData {
public:
    int atomicNumber;
    int chainIndex;
    ResidueIndex residueIndex;
    QByteArray name;
    Vector3D coord3d;
    float occupancy, temperature;

public:
    AtomData() {
        chainIndex = 0;
        atomicNumber = 0;
        temperature = -1.0;
        occupancy = 0;
    }

};

typedef QSharedDataPointer<AtomData> SharedAtom;

//! Represents chemical bond between atoms
class Bond {
    SharedAtom atom1;
    SharedAtom atom2;
public:
    Bond(const SharedAtom& a1, const SharedAtom& a2) : atom1(a1), atom2(a2) {}
    const SharedAtom getAtom1() const { return atom1; }
    const SharedAtom getAtom2() const { return atom2; }
};

//! Represents protein secondary structure: alpha-helix, beta-strand, turn
class SecondaryStructure : public QSharedData {
public:
    enum Type {
        Type_None = -1, Type_AlphaHelix, Type_PiHelix, Type_310Helix,
        Type_BetaStrand, Type_BetaBridge, Type_Turn, Type_BendRegion
    };

    Type type;
    char chainIdentifier;
    int chainIndex;
    int startSequenceNumber;
    int endSequenceNumber;

public:
    SecondaryStructure() : type(Type_None) {
        startSequenceNumber = 0;
        endSequenceNumber = 0;
        chainIndex = 0;
        chainIdentifier = 0;
    }
};

typedef QSharedDataPointer<SecondaryStructure> SharedSecondaryStructure;

//! Model 3D
/*!
    Represents collection of molecule atomic coordinates and bonds
    */
class Molecule3DModel {
public:
    QList<SharedAtom> atoms;
    QList<Bond> bonds;
};

//! Molecule
/*!
    Represents molecule chain (biopolymer or any other)
    */

class MoleculeData : public QSharedData {
public:
    MoleculeData() : chainId(0), engineered(false) {}
    MoleculeData(const QString& molName) : name(molName), chainId(0), engineered(false) {}
    QMap<ResidueIndex, SharedResidue> residueMap;

    // this list used by biostrct renderers
    // its indexes are NOT model ids taken from PDB
    QMap <int, Molecule3DModel> models;

    QString name;
    char chainId;
    bool engineered;
};

typedef QSharedDataPointer<MoleculeData> SharedMolecule;
typedef QHash<int, SharedAtom> AtomCoordSet;

//! Biological 3D structure
class U2CORE_EXPORT BioStruct3D {
public:
    static QString SecStructAnnotationTag;
    static QString MoleculeAnnotationTag;
    static QString AlphaHelixAnnotationTag;
    static QString BetaStrandAnnotationTag;
    static QString TurnAnnotationTag;
    static QString ChainIdQualifierName;
    static QString SecStructTypeQualifierName;

public:
    BioStruct3D();
    /** This is not deep copy constructor */
    BioStruct3D(const BioStruct3D &other);

    QMap <int, SharedMolecule> moleculeMap;
    QMap <int, AtomCoordSet> modelMap;
    QList<SharedSecondaryStructure> secondaryStructures;
    QList<Bond> interMolecularBonds;
    QString descr;
    QByteArray pdbId;

    // Const member functions
    double getRadius()  const { return radius; }
    const Vector3D& getCenter() const { return rotationCenter; }

    void setRadius(double value);
    void setCenter(const Vector3D &value);

    QByteArray getRawSequenceByChainIndex(int id) const;
    char getChainIdByIndex(int id) const;
    int getIndexByChainId(char chainId) const;
    int getNumberOfAtoms() const;
    QList<SharedAtom> getAllAtoms() const;
    int getNumberOfResidues() const;
    static int residueIndexToInt(const ResidueIndex& idx);
    const SharedAtom getAtomById(int index, int modelIndex) const;
    const SharedResidue getResidueById(int chainIndex, ResidueIndex residueIndex) const;

    // Theese three methods need to solve confusion between model ids (same as in PDB)
    // and internal model indexes, both  are used in UGENE
    // model ids should be used for user interface, and one fine day may become a strings
    // model index numbers used by internal UGENE parts, and should be consequent int values from 0
    /** @returns list of model ids (same as in PDB), that can be not consequent */
    const QList<int> getModelsNames() const;

    /** @returns molecule model by it's model id (same as in PDB) */
    const Molecule3DModel getModelByName(int moleculeId, int name) const;

    /** @returns model by it's index number in our data structure */
    const Molecule3DModel getModelByIndex(int moleculeId, int index) const;

    // Modifier
    void calcCenterAndMaxDistance();

    /** Generates the map: chainId <-> annotations */
    QMap<int, QList<SharedAnnotationData> > generateAnnotations() const;

    /** Biostruct 3D model should be transforemd with this matrix */
    void setTransform(const Matrix44 &m) { transform = m; }
    const Matrix44& getTransform() const { return transform; }

public:
    // Static methods
    static const QString getSecStructTypeName(SecondaryStructure::Type type);

private:
    QMap<int, QList<SharedAnnotationData> > generateChainAnnotations() const;
    void generateSecStructureAnnotations(QMap<int, QList<SharedAnnotationData> > &result) const;

private:
    double radius;
    Vector3D rotationCenter;
    Matrix44 transform;
};

//! Shared data for BioStruct3DSelection
class BioStruct3DChainSelectionData : public QSharedData {
public:
    BioStruct3DChainSelectionData() : QSharedData() {}
    BioStruct3DChainSelectionData(const BioStruct3DChainSelectionData &other) : QSharedData(), selection(other.selection) {}

    ~BioStruct3DChainSelectionData() {}

    // <chainId, residueId> presented in map believed to be selected
    // can be optimized on assumption that residue ids are continual
    QMultiMap<int, int> selection;

};  // class BioStruct3DChainSelectionData

//! Represents residue chain selection on BioStruct3D.
class U2CORE_EXPORT BioStruct3DChainSelection {
public:
    BioStruct3DChainSelection(const BioStruct3D &biostruct);
    BioStruct3DChainSelection(const BioStruct3DChainSelection &other);

    const BioStruct3D &getBioStruct3D() const { return biostruct; }

    bool inSelection(int chainId, int residueId) const;
    bool isEmpty() const { return data->selection.isEmpty(); }

    void add(int chain, const U2Region &region);
    void add(int chain, const QVector<U2Region> &regions);

    void remove(int chain, const U2Region &region);
    void remove(int chain, const QVector<U2Region> &regions);

    void update(int chain, const U2Region &add, const U2Region &remove);
    void update(int chain, const QVector<U2Region> &adds, const QVector<U2Region> &removes);

private:
    const BioStruct3D &biostruct;
    QSharedDataPointer<BioStruct3DChainSelectionData> data;
};  // class U2CORE_EXPORT BioStruct3DChainSelection

} // namespace U2



#endif // _U2_BIOSTRUCT3D_H_
