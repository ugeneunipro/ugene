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

#include "PhyTreeObject.h"

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GObjectTypes.h"

namespace U2 {

/////// U2PhyTree Implementation ///////////////////////////////////////////////////////////////////

U2PhyTree::U2PhyTree()
    : U2RawData() {
}

U2PhyTree::U2PhyTree(const U2DbiRef& dbiRef)
    : U2RawData(dbiRef) {
}

U2DataType U2PhyTree::getType() const {
    return U2Type::PhyTree;
}

/////// PhyTreeObject Implementation ///////////////////////////////////////////////////////////////////

PhyTreeObject* PhyTreeObject::createInstance(const PhyTree& tree, const QString& objectName, const U2DbiRef& dbiRef, U2OpStatus& os, const QVariantMap& hintsMap) {
    U2PhyTree object(dbiRef);
    object.visualName = objectName;
    object.serializer = NewickPhyTreeSerializer::ID;

    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    RawDataUdrSchema::createObject(dbiRef, folder, object, os);
    CHECK_OP(os, nullptr);

    U2EntityRef entRef(dbiRef, object.id);
    commit(tree, entRef, os);
    CHECK_OP(os, nullptr);

    return new PhyTreeObject(tree, objectName, entRef, hintsMap);
}

void PhyTreeObject::commit(const PhyTree& tree, const U2EntityRef& treeRef, U2OpStatus& os) {
    CHECK_EXT(tree.data() != nullptr, os.setError("NULL tree data"), );
    QString data = NewickPhyTreeSerializer::serialize(tree, os);
    CHECK_OP(os, );
    RawDataUdrSchema::writeContent(data.toLocal8Bit(), treeRef, os);
}

void PhyTreeObject::commit(const PhyTree& tree, const U2EntityRef& treeRef) {
    U2OpStatus2Log os;
    commit(tree, treeRef, os);
}

void PhyTreeObject::commit() {
    ensureDataLoaded();
    commit(tree, entityRef);
}

void PhyTreeObject::loadDataCore(U2OpStatus& os) {
    QString serializer = RawDataUdrSchema::getObject(entityRef, os).serializer;
    CHECK_OP(os, );
    SAFE_POINT(NewickPhyTreeSerializer::ID == serializer, "Unknown serializer id", );

    QByteArray data = RawDataUdrSchema::readAllContent(entityRef, os);
    CHECK_OP(os, );

    tree = NewickPhyTreeSerializer::deserialize(data, os);
}

PhyTreeObject::PhyTreeObject(const QString& objectName, const U2EntityRef& treeRef, const QVariantMap& hintsMap)
    : GObject(GObjectTypes::PHYLOGENETIC_TREE, objectName, hintsMap) {
    entityRef = treeRef;
}

PhyTreeObject::PhyTreeObject(const PhyTree& tree, const QString& objectName, const U2EntityRef& treeRef, const QVariantMap& hintsMap)
    : GObject(GObjectTypes::PHYLOGENETIC_TREE, objectName, hintsMap), tree(tree) {
    entityRef = treeRef;
    dataLoaded = true;
}

void PhyTreeObject::onTreeChanged() {
    commit();
    setModified(true);
}

const PhyTree& PhyTreeObject::getTree() const {
    ensureDataLoaded();
    return tree;
}

GObject* PhyTreeObject::clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints) const {
    DbiOperationsBlock opBlock(dstDbiRef, os);
    CHECK_OP(os, nullptr);

    ensureDataLoaded();

    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    PhyTreeObject* cln = createInstance(tree, getGObjectName(), dstDbiRef, os, gHints.getMap());
    CHECK_OP(os, nullptr);
    cln->setIndexInfo(getIndexInfo());
    return cln;
}

void PhyTreeObject::setTree(const PhyTree& _tree) {
    tree = _tree;
    onTreeChanged();
    emit si_phyTreeChanged();
}

void PhyTreeObject::rerootPhyTree(PhyNode* node) {
    ensureDataLoaded();
    PhyTreeUtils::rerootPhyTree(tree, node);
    onTreeChanged();
    emit si_phyTreeChanged();
}

bool PhyTreeObject::treesAreAlike(const PhyTree& tree1, const PhyTree& tree2) {
    QList<PhyNode*> track1 = tree1->getNodesPreOrder();
    QList<PhyNode*> track2 = tree2->getNodesPreOrder();
    CHECK(track1.size() == track2.size(), false);

    for (const PhyNode* n1 : qAsConst(track1)) {
        if (n1->name.isEmpty()) {
            continue;
        }
        for (const PhyNode* n2 : qAsConst(track2)) {
            if (n2->name != n1->name) {
                continue;
            }
            if (n1->getChildBranches().size() != n2->getChildBranches().size()) {
                return false;
            }
        }
    }

    return true;
}

const PhyNode* PhyTreeObject::findPhyNodeByName(const QString& name) {
    ensureDataLoaded();
    QList<PhyNode*> nodes = tree.constData()->getNodesPreOrder();
    for (const PhyNode* node : qAsConst(nodes)) {
        if (node->name == name) {
            return node;
        }
    }
    return nullptr;
}

}  // namespace U2
