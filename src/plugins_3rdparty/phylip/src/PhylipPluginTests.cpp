/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PhylipPluginTests.h"

#include <QDir>

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/SaveDocumentTask.h>

#include "DistanceMatrix.h"
#include "PhylipPlugin.h"

namespace U2 {

QList<XMLTestFactory*> PhylipPluginTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_NeighborJoin::createFactory());
    return res;
}

void GTest_NeighborJoin::init(XMLTestFormat*, const QDomElement& el) {
    treeObjFromDoc = nullptr;
    task = nullptr;
    input = nullptr;
    inputDocCtxName = el.attribute("in");
    if (inputDocCtxName.isEmpty()) {
        failMissingValue("in");
        return;
    }
    resultCtxName = el.attribute("sample");
    QString bootstrSeed;
    bootstrSeed = el.attribute("bootstrapSeed");
    if (bootstrSeed == nullptr) {
        bootStrapSeed = -1;
    } else {
        bootStrapSeed = bootstrSeed.toInt();
    }
}

void GTest_NeighborJoin::prepare() {
    maDoc = getContext<Document>(this, inputDocCtxName);
    if (maDoc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(inputDocCtxName));
        return;
    }

    QList<GObject*> list = maDoc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.isEmpty()) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }

    GObject* obj = list.first();
    if (obj == nullptr) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }
    assert(obj != nullptr);
    auto ma = qobject_cast<MultipleAlignmentObject*>(obj);
    if (ma == nullptr) {
        stateInfo.setError(QString("error can't cast to multiple alignment from GObject"));
        return;
    }

    input = ma;

    treeDoc = getContext<Document>(this, resultCtxName);
    if (treeDoc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(resultCtxName));
        return;
    }

    QList<GObject*> list2 = treeDoc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE);
    if (list2.isEmpty()) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }

    GObject* obj2 = list2.first();
    if (obj2 == nullptr) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::PHYLOGENETIC_TREE));
        return;
    }

    treeObjFromDoc = qobject_cast<PhyTreeObject*>(obj2);

    if (treeObjFromDoc == nullptr) {
        stateInfo.setError(QString("error can't cast to phylogenetic tree from GObject"));
        return;
    }
    assert(obj != nullptr);

    CreatePhyTreeSettings settings;
    settings.algorithm = PhylipPlugin::PHYLIP_NEIGHBOUR_JOIN_ALGORITHM_NAME_AND_KEY;

    if (bootStrapSeed != -1) {
        settings.bootstrap = true;
        settings.seed = bootStrapSeed;
        settings.consensusID = "Majority Rule (extended)";
        settings.replicates = 100;
    }

    task = new PhyTreeGeneratorLauncherTask(input->getAlignment(), settings);
    addSubTask(task);
}

Task::ReportResult GTest_NeighborJoin::report() {
    if (!task->hasError()) {
        const PhyTree computedTree = task->getResult();
        if (computedTree == nullptr) {
            stateInfo.setError("Result tree is NULL");
            return ReportResult_Finished;
        }
        const PhyTree& treeFromDoc = treeObjFromDoc->getTree();
        bool same = PhyTreeObject::treesAreAlike(computedTree, treeFromDoc);
        if (!same) {
            stateInfo.setError("Trees are not equal");
        }
    }

    return ReportResult_Finished;
}

}  // namespace U2
