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

#include "BioStruct3D.h"

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Log.h>

namespace U2 {

/* class BioStruct3D */

QString BioStruct3D::MoleculeAnnotationTag("chain_info");
QString BioStruct3D::AlphaHelixAnnotationTag("alpha_helix");
QString BioStruct3D::BetaStrandAnnotationTag("beta_strand");
QString BioStruct3D::TurnAnnotationTag("turn");
QString BioStruct3D::ChainIdQualifierName("chain_id");
QString BioStruct3D::SecStructAnnotationTag("sec_struct");
QString BioStruct3D::SecStructTypeQualifierName("sec_struct_type");

BioStruct3D::BioStruct3D()
        : moleculeMap(), modelMap(),
        secondaryStructures(),
        interMolecularBonds(),
        descr(), pdbId(),
        radius(0), rotationCenter(),
        transform()
{
    transform.loadIdentity();
}

BioStruct3D::BioStruct3D(const BioStruct3D &other)
    : moleculeMap(other.moleculeMap), modelMap(other.modelMap),
    secondaryStructures(other.secondaryStructures),
    interMolecularBonds(other.interMolecularBonds),
    descr(other.descr), pdbId(other.pdbId),
    radius(other.radius), rotationCenter(other.rotationCenter),
    transform(other.transform)
{
}

void BioStruct3D::calcCenterAndMaxDistance() {

    Vector3D siteSum;
    Vector3D center;
    double dist;
    radius = 0.0;
    int numberOfAtoms = 0;
    // loop trough all atoms twice - once to get average center, then once to
    // find max distance from this center
    for (int i = 0; i < 2; ++i) {
        foreach (SharedMolecule molecule, moleculeMap) {
            foreach(Molecule3DModel model3d, molecule->models.values()) {
                foreach (const AtomData* atom, model3d.atoms) {
                    Vector3D site = atom->coord3d;
                    if (i==0) {
                        siteSum += atom->coord3d;
                        ++numberOfAtoms;
                    }
                    else {
                        dist = (site - center).length();
                        if (dist > radius){
                            radius = dist;
                        }
                    }
                }
            }
        }

        if (i == 0) {
            if (numberOfAtoms == 0) {
                algoLog.trace("Number of atoms is 0!");
            } else {
                center = siteSum / numberOfAtoms;
            }
        }

    }
    algoLog.trace(QString("center: (%1,%2,%3)\n maxDistFromCenter: %4").arg(center.x).arg(center.y).arg(center.z).arg(radius) );

    rotationCenter = center;

}

int BioStruct3D::getNumberOfAtoms() const
{
    // get first coordinates set
    const AtomCoordSet& coordSet = modelMap.begin().value();
    return coordSet.count();
}

QList<SharedAtom> BioStruct3D::getAllAtoms() const {
    const AtomCoordSet& coordSet = modelMap.begin().value();
    return coordSet.values();
}

QMap<int, QList<SharedAnnotationData> > BioStruct3D::generateAnnotations() const
{
    QMap<int, QList<SharedAnnotationData> > result = generateChainAnnotations();
    generateSecStructureAnnotations(result);
    return result;
}

QMap<int, QList<SharedAnnotationData> > BioStruct3D::generateChainAnnotations() const
{
    QMap<int, QList<SharedAnnotationData> > result;
    const char* molNameQualifier = "molecule_name";
    //const char* pdbChainIdQualifier = "pdb_id";

    QMap<int, SharedMolecule>::ConstIterator iter = moleculeMap.constBegin();
    while (iter != moleculeMap.end()) {
        int length = iter.value()->residueMap.size();
        SharedAnnotationData sd( new AnnotationData);
        sd->location->regions << U2Region(0,length);
        sd->name = BioStruct3D::MoleculeAnnotationTag;
        if ((*iter)->chainId > 0) {
            sd->qualifiers.append(U2Qualifier(ChainIdQualifierName, QString("%1").arg((*iter)->chainId)));
        }
        if (!(*iter)->name.isEmpty()) {
            sd->qualifiers.append(U2Qualifier(molNameQualifier, (*iter)->name));
        }

        result[iter.key()].append(sd);
        ++iter;
    }
    return result;
}

int BioStruct3D::getNumberOfResidues() const
{
    int numResidues = 0;

    foreach (SharedMolecule mol, moleculeMap) {
        numResidues += mol->residueMap.size();
    }

    return numResidues;
}

const SharedAtom BioStruct3D::getAtomById( int atomIndex, int modelIndex ) const
{
    if (modelMap.contains(modelIndex)) {
        const AtomCoordSet& coordSet = modelMap.value(modelIndex);
        if (coordSet.contains(atomIndex)) {
            return coordSet.value(atomIndex);
        }
    }

    return SharedAtom(NULL);
}

const SharedResidue BioStruct3D::getResidueById( int chainIndex, ResidueIndex residueIndex ) const
{
    const SharedMolecule mol =  moleculeMap.value(chainIndex);
    foreach (const ResidueIndex& id, mol->residueMap.keys() ){
        if ( id.toInt() == residueIndex.toInt() ) {
            return mol->residueMap.value(id);
        }
    }

    return SharedResidue(NULL);
}


const QString BioStruct3D::getSecStructTypeName( SecondaryStructure::Type type )
{
    switch(type) {
    case SecondaryStructure::Type_AlphaHelix:
        return BioStruct3D::AlphaHelixAnnotationTag;
    case SecondaryStructure::Type_PiHelix:
        return "pi_helix";
    case SecondaryStructure::Type_310Helix:
        return "310_helix";
    case SecondaryStructure::Type_BetaStrand:
        return BioStruct3D::BetaStrandAnnotationTag;
    case SecondaryStructure::Type_BetaBridge:
        return "beta_bridge";
    case SecondaryStructure::Type_Turn:
        return BioStruct3D::TurnAnnotationTag;
    case SecondaryStructure::Type_BendRegion:
        return "bend_region";
    default:
        return QString("unknown");
    }

}

void BioStruct3D::generateSecStructureAnnotations(QMap<int, QList<SharedAnnotationData> > &result) const
{
    // TODO: issue 0000637
    if (moleculeMap.isEmpty())
        return;


    foreach (const SharedSecondaryStructure& struc, secondaryStructures) {
        SharedAnnotationData sd(NULL);
        int chainId = struc->chainIndex;
        assert(chainId != 0);
        int initResidueId = moleculeMap.value(chainId)->residueMap.constBegin().key().toInt();
        sd = new AnnotationData;
        sd->name = BioStruct3D::SecStructAnnotationTag;
        U2Qualifier qual(SecStructTypeQualifierName, getSecStructTypeName(struc->type));
        sd->qualifiers.append(qual);
        int numResidues = struc->endSequenceNumber - struc->startSequenceNumber + 1;
        // TODO: determine why can it happen and fix if it's a bug
        if (numResidues < 0) {
            continue;
        }
        int startIndex = struc->startSequenceNumber - initResidueId;
        U2Region chainRegion(startIndex, numResidues);
        sd->location->regions << chainRegion;
        Q_ASSERT(moleculeMap.contains(chainId));
        result[chainId].append(sd);
    }

}

QByteArray BioStruct3D::getRawSequenceByChainIndex(int id) const {
    QByteArray sequence("");

    SAFE_POINT(moleculeMap.contains(id), QString("Can't find chain identifier for index: %1").arg(id), sequence);
    const SharedMolecule molecule = moleculeMap.value(id);
    foreach (const SharedResidue residue, molecule->residueMap) {
       QChar c = residue->acronym;
       sequence.append(c);
    }

    return sequence;
}

char BioStruct3D::getChainIdByIndex(int index) const {
    SAFE_POINT(moleculeMap.contains(index), QString("Can't find chain identifier for index: %1").arg(index), 0);
    return moleculeMap.value(index)->chainId;
}

int BioStruct3D::getIndexByChainId(char chainId) const {
    foreach(int curIndex, moleculeMap.keys()) {
        const SharedMolecule molecule = moleculeMap.value(curIndex);
        if (molecule->chainId == chainId) {
            return curIndex;
        }
    }
    return -1;
}

const QList<int> BioStruct3D::getModelsNames() const {
    return modelMap.keys();
}

const Molecule3DModel BioStruct3D::getModelByName(int moleculeId, int name) const {
    int index = modelMap.keys().indexOf(name);
    assert(index != -1 && "invalid model id");
    return getModelByIndex(moleculeId, index);
}

const Molecule3DModel BioStruct3D::getModelByIndex(int moleculeId, int index) const {
    return moleculeMap[moleculeId]->models[index];
}

void BioStruct3D::setRadius(double value) {
    radius = value;
}

void BioStruct3D::setCenter(const Vector3D &value) {
    rotationCenter = value;
}

/* class U2CORE_EXPORT BioStruct3DChainSelection */

BioStruct3DChainSelection::BioStruct3DChainSelection(const BioStruct3D &biostruct_)
        : biostruct(biostruct_), data(new BioStruct3DChainSelectionData())
{}

BioStruct3DChainSelection::BioStruct3DChainSelection(const BioStruct3DChainSelection &other)
        : biostruct(other.biostruct), data(other.data)
{}

bool BioStruct3DChainSelection::inSelection(int chainId, int residueId) const {
    return data->selection.contains(chainId, residueId);
}

void BioStruct3DChainSelection::add(int chain, const U2Region &region) {
    int start = biostruct.moleculeMap[chain]->residueMap.begin().key().toInt();
    for (int i = region.startPos; i < region.endPos(); ++i) {
        if (!data->selection.contains(chain, start + i)) {
            data->selection.insert(chain, start + i);
        }

    }
}

void BioStruct3DChainSelection::add(int chain, const QVector<U2Region> &regions) {
    foreach (const U2Region &region, regions) {
        add(chain, region);
    }
}

void BioStruct3DChainSelection::remove(int chain, const U2Region &region) {
    int start = biostruct.moleculeMap[chain]->residueMap.begin().key().toInt();
    for (int i = region.startPos; i < region.endPos(); ++i) {
        data->selection.remove(chain, start + i);
    }
}

void BioStruct3DChainSelection::remove(int chain, const QVector<U2Region> &regions) {
    foreach (const U2Region &region, regions) {
        remove(chain, region);
    }
}

void BioStruct3DChainSelection::update(int chain, const U2Region &add, const U2Region &remove) {
    this->remove(chain, remove);
    this->add(chain, add);
}

void BioStruct3DChainSelection::update(int chain, const QVector<U2Region> &adds, const QVector<U2Region> &removes) {
    remove(chain, removes);
    add(chain, adds);
}

bool ResidueIndex::operator< ( const ResidueIndex& other ) const
{
    if (order == other.order) {
        if (resId == other.resId) {
            return insCode < other.insCode;
        } else {
            return resId < other.resId;
        }
    } else {
        return order < other.order;
    }
}

bool ResidueIndex::operator==( const ResidueIndex& other ) const
{
    return (other.insCode == insCode) && (other.resId == resId);
}

bool ResidueIndex::operator!=( const ResidueIndex& other ) const
{
    return !(*this == other);
}

int ResidueIndex::getOrder() const {
    return order;
}

char ResidueIndex::getInsCode() const {
    return insCode;
}

} // namespace U2
