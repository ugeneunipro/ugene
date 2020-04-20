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

#include "PToolsAligner.h"

#include <U2Core/Log.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/U2Region.h>

#include "ptools/superpose.h"

#include <exception>

namespace U2 {

/* class PToolsAligner : public StructuralAlignmentAlgorithm */

static PTools::Rigidbody* createRigidBody(const BioStruct3DReference &subset) {
    PTools::Rigidbody *body = new PTools::Rigidbody();
    const BioStruct3D &biostruct = subset.obj->getBioStruct3D();

    foreach (int chainId, subset.chains) {
        const Molecule3DModel &model = biostruct.getModelByName(chainId, subset.modelId);

        U2Region region;
        if (subset.chains.size() == 1) {
            // take region associated with the single chain from the subset
            region = subset.chainRegion;
        }
        else {
            // take full chain
            region = U2Region(0, biostruct.moleculeMap.value(chainId)->residueMap.size());
        }

        // built in assumtion that order of atoms in BioStruct3D matches order of residues
        int i = 0;
        foreach (const SharedAtom &atom, model.atoms)
        {
            // take into account only CA atoms (backbone) because subsets may have different residues,
            // i.e. different number of atoms on the same nuber of residues
            if ( atom->name == "CA") {
                if (i >= region.startPos && i < region.endPos()) {
                    PTools::Atomproperty pproperty;
                    pproperty.SetType(atom->name.data());
                    pproperty.SetResidId(atom->residueIndex.toInt());
                    //pproperty.SetAtomId();

                    const Vector3D &coord = atom->coord3d;
                    PTools::Coord3D pcoord(coord.x, coord.y, coord.z);

                    PTools::Atom patom(pproperty, pcoord);
                    body->AddAtom(patom);
                }
                ++i;
            }
        }
    }

    return body;
}

/** @returns Number of residues (CA atoms) in subset */
static int getSubsetSize(const BioStruct3DReference &subset) {
    int res = 0;
    if (subset.chains.size() == 1) {
        // lenggth of region
        res = subset.chainRegion.length;
    }
    else {
        // lenghth of all chains
        foreach (int chainId, subset.chains) {
            int length = subset.obj->getBioStruct3D().moleculeMap.value(chainId)->residueMap.size();
            res += length;
        }
    }

    return res;
}

QString PToolsAligner::validate(const StructuralAlignmentTaskSettings &settings) {
    int refSize = getSubsetSize(settings.ref), altSize = getSubsetSize(settings.alt);
    if (refSize != altSize) {
        return QString("structure subsets has different size (number of residues)");
    }

    return QString();
}

StructuralAlignment PToolsAligner::align(const StructuralAlignmentTaskSettings &settings, TaskStateInfo &state) {
    algoLog.trace(QString("PToolsAligner started on %1 (reference) vs %2").arg(settings.ref.print(), settings.alt.print()));

    QString error;

    StructuralAlignment result;
    try {
        QScopedPointer<PTools::Rigidbody> prefBody(createRigidBody(settings.ref));
        QScopedPointer<PTools::Rigidbody> paltBody(createRigidBody(settings.alt));

        if (prefBody->Size() != paltBody->Size()) {
            error = QString("Failed to align, subsets turn to RigidBodies of a different size");
        }
        else {
            Superpose_t presult = PTools::superpose(*prefBody, *paltBody);

            result.rmsd = presult.rmsd;
            for (int i = 0; i < 16; ++i) {
                result.transform[i] = presult.matrix(i/4, i%4);
            }
        }
    }
    catch (const std::exception &e) {
        error = QString("Internal ptools error: %1").arg(e.what());
    }
    catch (...) {
        error = QString("Internal ptools error");
    }

    if (!error.isEmpty()) {
        algoLog.error(error);
        state.setError(error);
    }

    return result;
}

}   // namespace U2
