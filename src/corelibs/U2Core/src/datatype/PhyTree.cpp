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

#include <iostream>

#include <QSet>

#include <U2Core/U2SafePoints.h>

#include "PhyTree.h"

namespace U2 {

PhyTreeData::PhyTreeData(const PhyTreeData& other)
    : QSharedData(other) {
    rootNode = other.rootNode == nullptr ? nullptr : other.rootNode->clone();
}

PhyTreeData::~PhyTreeData() {
    if (rootNode != nullptr) {
        delete rootNode;
        rootNode = nullptr;
    }
}

static bool hasNamedInnerNode(const PhyNode* node) {
    CHECK(node != nullptr && !node->isLeafNode(), false);
    CHECK(node->name.isEmpty(), true);
    const QList<PhyBranch*>& branches = node->getChildBranches();
    return std::any_of(branches.begin(), branches.end(), [](auto branch) { return hasNamedInnerNode(branch->childNode); });
}

bool PhyTreeData::hasNamedInnerNodes() const {
    return hasNamedInnerNode(rootNode);
}

void PhyTreeData::setRootNode(PhyNode* newRootNode) {
    // TODO: delete old node?
    rootNode = newRootNode;
}

PhyNode* PhyTreeData::getRootNode() const {
    return rootNode;
}

void PhyTreeData::print() const {
    rootNode->print();
}

QList<PhyNode*> PhyTreeData::getNodesPreOrder() const {
    CHECK(rootNode != nullptr, {});
    QList<PhyNode*> nodes;
    rootNode->collectNodesPreOrder(nodes);
    return nodes;
}

const QList<PhyBranch*>& PhyNode::getChildBranches() const {
    return childBranches;
}

PhyBranch* PhyNode::getParentBranch() const {
    return parentBranch;
}

void PhyNode::collectNodesPreOrder(QList<PhyNode*>& nodes) {
    nodes.append(this);
    for (PhyBranch* branch : qAsConst(childBranches)) {
        branch->childNode->collectNodesPreOrder(nodes);
    }
}

void PhyNode::collectNodesPreOrder(QList<const PhyNode*>& nodes) const {
    nodes.append(this);
    for (PhyBranch* branch : qAsConst(childBranches)) {
        branch->childNode->collectNodesPreOrder(nodes);
    }
}

bool PhyNode::isConnected(const PhyNode* node) const {
    CHECK(node != getParentNode(), true);
    return std::any_of(childBranches.begin(), childBranches.end(), [node](auto branch) { return branch->childNode == node; });
}

PhyNode* PhyNode::getParentNode() const {
    return parentBranch == nullptr ? nullptr : parentBranch->parentNode;
}

bool PhyNode::isLeafNode() const {
    return childBranches.isEmpty();
}

bool PhyNode::isRootNode() const {
    return parentBranch == nullptr;
}

PhyNode::~PhyNode() {
    // Delete branch connected to parent.
    unlinkFromParent();

    // Delete all child nodes. Child nodes delete their parent (our child) branches.
    for (PhyBranch* childBranch : childBranches) {
        PhyNode* childNode = childBranch->childNode;
        SAFE_POINT(childNode->getParentNode() == this, "Child node has incorrect parent!", );
        delete childNode;
    }
}

PhyNode* PhyNode::clone() const {
    QList<const PhyNode*> allNodesInTree;
    collectNodesPreOrder(allNodesInTree);

    QList<PhyBranch*> allBranchesInTree;
    QHash<const PhyNode*, PhyNode*> clonedNodeByOriginalNode;
    for (const PhyNode* originalNode : qAsConst(allNodesInTree)) {
        auto clonedNode = new PhyNode();
        clonedNode->name = originalNode->name;
        clonedNodeByOriginalNode[originalNode] = clonedNode;
        if (originalNode->parentBranch) {
            allBranchesInTree.append(originalNode->parentBranch);
        }
    }
    for (PhyBranch* originalBranch : qAsConst(allBranchesInTree)) {
        PhyNode* parentClonedNode = clonedNodeByOriginalNode[originalBranch->parentNode];
        PhyNode* childClonedNode = clonedNodeByOriginalNode[originalBranch->childNode];
        SAFE_POINT(parentClonedNode != nullptr && childClonedNode != nullptr, "Cloned node not found!", nullptr);
        PhyTreeUtils::addBranch(parentClonedNode, childClonedNode, originalBranch->distance);
    }
    PhyNode* clonedNode = clonedNodeByOriginalNode.value(this);
    SAFE_POINT(clonedNode != nullptr, "Cloned node not found for the current node", nullptr);
    return clonedNode;
}

void PhyNode::print(int tab, int distance) const {
    for (int i = 0; i < tab; i++) {
        std::cout << " ";
    }
    tab++;
    std::cout << "name: " << this->name.toLatin1().constData() << " distance: " << distance << std::endl;
    QList<PhyBranch*> branchList = this->childBranches;
    int s = branchList.size();
    for (int i = 0; i < s; i++) {
        if (branchList[i]->childNode != nullptr) {
            int d = branchList[i]->distance;
            branchList[i]->childNode->print(tab, d);
        }
    }
}

int PhyNode::countLeafNodesInSubtree() const {
    int leafNodes = 0;
    for (auto childBranch : qAsConst(childBranches)) {
        leafNodes += childBranch->childNode->countLeafNodesInSubtree();
    }
    return qMax(1, leafNodes);  // Count self (1) if there are no children.
}

void PhyNode::invertOrderOrChildBranches() {
    std::reverse(childBranches.begin(), childBranches.end());
}

void PhyNode::makeRoot() {
    CHECK(!isRootNode(), );
    parentBranch->parentNode->makeRoot();
    SAFE_POINT(parentBranch->parentNode->isRootNode(), "Parent branch must be a root branch.", );

    parentBranch->parentNode->childBranches.removeOne(parentBranch);
    parentBranch->parentNode->parentBranch = parentBranch;
    parentBranch->childNode = parentBranch->parentNode;
    parentBranch->parentNode = this;
    childBranches.append(parentBranch);
    parentBranch = nullptr;
}

void PhyTreeUtils::rerootPhyTree(PhyTree& phyTree, PhyNode* node) {
    CHECK(!node->isRootNode(), );
    node->makeRoot();
    phyTree->setRootNode(node);
}

PhyBranch* PhyTreeUtils::addBranch(PhyNode* parentNode, PhyNode* childNode, double distance) {
    SAFE_POINT(childNode->parentBranch == nullptr, "PhyTreeUtils::addBranch child branch must have no parent.", nullptr);
    SAFE_POINT(!childNode->isConnected(parentNode), "PhyTreeUtils::addBranch nodes are already connected", nullptr);

    auto newBranch = new PhyBranch();
    newBranch->distance = distance;
    newBranch->parentNode = parentNode;
    newBranch->childNode = childNode;

    parentNode->childBranches.append(newBranch);
    childNode->parentBranch = newBranch;

    return newBranch;
}

void PhyNode::unlinkFromParent() {
    CHECK(parentBranch != nullptr, );
    parentBranch->parentNode->childBranches.removeOne(parentBranch);
    delete parentBranch;
    parentBranch = nullptr;
}

}  // namespace U2
