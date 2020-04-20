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

#ifndef _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_H_
#define _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_H_

#include <U2Core/global.h>
#include <U2Core/Matrix44.h>
#include <U2Core/Task.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/BioStruct3DObject.h>

namespace U2 {

/** Reference to a subset of BioStruct3D */
// Maybe this class must be merged with BioStruct3DChainSelection
class U2ALGORITHM_EXPORT BioStruct3DReference {
public:
    BioStruct3DReference(const BioStruct3DObject *_obj, int _chainId, const U2Region &_chainRegion, int _modelId)
            : obj(_obj), chains(), chainRegion(_chainRegion), modelId(_modelId)
    {
        chains << _chainId;
    }

    BioStruct3DReference(const BioStruct3DObject *_obj, const QList<int> &_chains, int _modelId)
            : obj(_obj), chains(_chains), chainRegion(), modelId(_modelId)
    {
        assert(obj);
        // if one chain selected set region from start to end
        if (chains.size() == 1) {
            int chainId = chains.first();
            int length = obj->getBioStruct3D().moleculeMap.value(chainId)->residueMap.size();
            chainRegion = U2Region(0, length);
        }
    }

    /** Pretty print structure reference description */
    QString print() const;

    const BioStruct3DObject *obj;
    QList<int> chains;

    // when more than one chain selected, region ignored
    U2Region chainRegion;

    int modelId;
};  // class BioStruct3DReference

class U2ALGORITHM_EXPORT StructuralAlignmentTaskSettings {
public:
    StructuralAlignmentTaskSettings(const BioStruct3DReference &_ref, const BioStruct3DReference &_alt) : ref(_ref), alt(_alt) {}
    BioStruct3DReference ref, alt;
};  // struct StructuralAlignmentTaskSettings

/** Structural alignment algorithm result */
class U2ALGORITHM_EXPORT StructuralAlignment {
public:
    StructuralAlignment() : rmsd(0.0), transform() {}
    double rmsd;
    Matrix44 transform;
};  // class StructuralAlignment

/** Structural alignment algorithm abstract interface */
class U2ALGORITHM_EXPORT StructuralAlignmentAlgorithm {
public:
    virtual ~StructuralAlignmentAlgorithm() {}

    /** Test settings for algorithm specific constraints.
      * @returns "" on ok and error descripton on fail
      */
    virtual QString validate(const StructuralAlignmentTaskSettings &settings) = 0;
    virtual StructuralAlignment align(const StructuralAlignmentTaskSettings &settings, TaskStateInfo &state) = 0;
};  // class StructuralAlignmentAlgorithm

/** Task wrapper for structural alignment algorithm */
class U2ALGORITHM_EXPORT StructuralAlignmentTask : public Task {
    Q_OBJECT

public:
    StructuralAlignmentTask(StructuralAlignmentAlgorithm *algorithm, const StructuralAlignmentTaskSettings &settings);

    virtual void run();
    Task::ReportResult report();
    QString generateReport() const;

    StructuralAlignment getResult() const { return result; }
    StructuralAlignmentTaskSettings getSettings() const { return settings; }

private:
    QScopedPointer<StructuralAlignmentAlgorithm> algorithm;
    StructuralAlignmentTaskSettings settings;

    StructuralAlignment result;
};  // class StructuralAlignmentTask

}   // namespace U2

#endif  // #ifndef _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_H_
