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
    QList<PhyNode*> childNodes = node->getChildNodes();
    return std::any_of(childNodes.begin(), childNodes.end(), [](auto childNode) { return hasNamedInnerNode(childNode); });
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
    QList<PhyNode*> nodes;
    int tab = 0;
    int distance = 0;
    rootNode->print(nodes, distance, tab);
}

QList<PhyNode*> PhyTreeData::getNodesPreOrder() const {
    QList<PhyNode*> nodes;
    if (rootNode != nullptr) {
        rootNode->addIfNotPreset(nodes);
    }
    return nodes;
}

const QList<PhyBranch*>& PhyNode::getBranches() const {
    return branches;
}

const PhyNode* PhyNode::getSecondNodeOfBranch(int branchNumber) const {
    SAFE_POINT(branchNumber < branches.size() && branchNumber >= 0, "Invalid branch number", nullptr);
    return branches.at(branchNumber)->node2;
}

void PhyNode::addIfNotPreset(QList<PhyNode*>& nodes) {
    CHECK(!nodes.contains(this), );
    nodes.append(this);
    for (PhyBranch* b : qAsConst(branches)) {
        assert(b->node1 != nullptr && b->node2 != nullptr);
        b->node1->addIfNotPreset(nodes);
        b->node2->addIfNotPreset(nodes);
    }
}

void PhyNode::addIfNotPreset(QList<const PhyNode*>& nodes) const {
    CHECK(!nodes.contains(this), );
    nodes.append(this);
    for (PhyBranch* b : qAsConst(branches)) {
        assert(b->node1 != nullptr && b->node2 != nullptr);
        b->node1->addIfNotPreset(nodes);
        b->node2->addIfNotPreset(nodes);
    }
}

bool PhyNode::isConnected(const PhyNode* node) const {
    foreach (PhyBranch* b, branches) {
        if (b->node1 == node || b->node2 == node) {
            return true;
        }
    }
    return false;
}

PhyNode* PhyNode::parent() const {
    foreach (PhyBranch* currentBrunch, branches) {
        if (currentBrunch->node2 == this)
            return currentBrunch->node1;
    }
    return nullptr;
}

const PhyNode* PhyNode::getParentNode() const {
    return parent();
}

PhyNode* PhyNode::getParentNode() {
    return parent();
}

bool PhyNode::isLeafNode() const {
    return branches.size() == 1 && branches[0] == getParentBranch();
}

void PhyNode::setParentNode(PhyNode* newParent, double distance) {
    int branchesNumber = branches.size();
    for (int i = 0; i < branchesNumber; i++) {
        PhyBranch* currentBrunch = branches.at(i);

        if (currentBrunch->node1 == newParent) {
            return;
        } else if (currentBrunch->node2 == newParent) {
            // Invert branch nodes if newParent currently is a child
            currentBrunch->node1 = newParent;
            currentBrunch->node2 = this;
            currentBrunch->distance = distance;
            return;
        } else if (currentBrunch->node2 == this) {
            // Remove link between the node and previous parent
            PhyNode* parentNode = currentBrunch->node1;
            if (nullptr != parentNode) {
                parentNode->branches.removeOne(currentBrunch);
            }
            // Link the node to the new parent
            if (nullptr != newParent) {
                currentBrunch->node1 = newParent;
                currentBrunch->distance = distance;
                newParent->branches.append(currentBrunch);
            }
            return;
        }
    }

    auto branch = new PhyBranch();
    branch->distance = distance;
    branch->node1 = newParent;
    branch->node2 = this;

    newParent->branches.append(branch);
    branches.append(branch);
}

QList<PhyNode*> PhyNode::getChildNodes() const {
    QList<PhyNode*> childNodes;
    for (PhyBranch* branch : qAsConst(branches)) {
        if (branch->node1 == this) {
            childNodes.append(branch->node2);
        }
    }
    return childNodes;
}

const PhyBranch* PhyNode::getParentBranch() const {
    foreach (PhyBranch* branch, branches) {
        if (branch->node2 == this) {
            return branch;
        }
    }

    return nullptr;
}

PhyNode::~PhyNode() {
    for (PhyBranch* branch : branches) {
        SAFE_POINT(branch != nullptr, "NULL pointer to PhyBranch", );
        PhyNode* childNode = branch->node2;
        SAFE_POINT(childNode != nullptr, "NULL pointer to PhyNode", );
        if (childNode != this) {
            childNode->branches.removeOne(branch);
            delete childNode;
        } else {
            PhyNode* parentNode = branch->node1;
            if (parentNode != nullptr) {
                parentNode->branches.removeOne(branch);
            }
        }
        delete branch;
    }
}

PhyNode* PhyNode::clone() const {
    QList<const PhyNode*> nodesPreOrder;
    addIfNotPreset(nodesPreOrder);

    QList<PhyBranch*> allBranches;
    QMap<const PhyNode*, PhyNode*> nodeTable;
    for (const PhyNode* n : qAsConst(nodesPreOrder)) {
        auto n2 = new PhyNode();
        n2->name = n->name;
        nodeTable[n] = n2;
        for (PhyBranch* b : qAsConst(n->branches)) {
            if (!allBranches.contains(b)) {
                allBranches.append(b);
            }
        }
    }
    for (PhyBranch* b : qAsConst(allBranches)) {
        PhyNode* node1 = nodeTable[b->node1];
        PhyNode* node2 = nodeTable[b->node2];
        assert(node1 != nullptr && node2 != nullptr);
        PhyTreeUtils::addBranch(node1, node2, b->distance);
    }
    PhyNode* myClone = nodeTable.value(this);
    assert(myClone != nullptr);
    return myClone;
}

void PhyNode::print(QList<PhyNode*>& nodes, int tab, int distance) {
    if (nodes.contains(this)) {
        return;
    }
    nodes.append(this);
    for (int i = 0; i < tab; i++) {
        std::cout << " ";
    }
    tab++;
    std::cout << "name: " << this->name.toLatin1().constData() << " distance: " << distance << std::endl;
    QList<PhyBranch*> branchList = this->branches;
    int s = branchList.size();
    for (int i = 0; i < s; i++) {
        if (branchList[i]->node2 != nullptr) {
            int d = branchList[i]->distance;
            branchList[i]->node2->print(nodes, tab, d);
        }
    }
}

void PhyNode::swapBranches(int branchIndex1, int branchIndex2) {
    branches.swap(branchIndex1, branchIndex2);
}

double PhyNode::getDistanceToRoot() const {
    double distanceToRoot = 0.0;
    const PhyBranch* currentBranch = getParentBranch();
    const PhyNode* currentNode = getParentNode();
    while (nullptr != currentBranch) {
        SAFE_POINT(currentNode != this, "There is cyclic graph in the phylogenetic tree", 0.0);
        distanceToRoot += currentBranch->distance;
        if (nullptr == currentNode) {
            break;
        }
        currentBranch = currentNode->getParentBranch();
        currentNode = currentNode->getParentNode();
    }
    return distanceToRoot;
}

int PhyTreeUtils::getNumSeqsFromNode(const PhyNode* node, const QSet<QString>& names) {
    int size = node->branches.size();
    if (size > 1) {
        int s = 0;
        for (int i = 0; i < size; ++i) {
            if (node->branches[i]->node2 != node) {
                int num = getNumSeqsFromNode(node->branches[i]->node2, names);
                if (!num) {
                    return 0;
                }
                s += num;
            }
        }
        return s;
    } else {
        QString str = node->name;
        return names.contains(str.replace('_', ' ')) ? 1 : 0;
    }
}

void PhyTreeUtils::rerootPhyTree(PhyTree& phyTree, PhyNode* node) {
    PhyNode* curRoot = phyTree->getRootNode();
    SAFE_POINT(nullptr != curRoot, "Null pointer argument 'curRoot' were passed to PhyTreeUtils::rerootPhyTree(...)", );
    SAFE_POINT(nullptr != node, "Null pointer argument 'node' were passed to PhyTreeUtils::rerootPhyTree(...)", );
    CHECK(node != curRoot, );

    PhyNode* centralNode = node->getParentNode();
    if (centralNode == curRoot) {
        if (centralNode->getChildNodes().at(0) != node) {
            centralNode->swapBranches(0, 1);
        }
        return;
    }
    auto newRootNode = new PhyNode();
    PhyNode* newParentNode = newRootNode;
    double distance = node->getDistanceToRoot() - newRootNode->getDistanceToRoot();
    node->setParentNode(newRootNode, distance);
    PhyNode* oldParent = centralNode->getParentNode();
    if (oldParent != nullptr) {
        distance = centralNode->getDistanceToRoot() - newRootNode->getDistanceToRoot();
        centralNode->setParentNode(newRootNode, distance);
    }

    const PhyNode* firstNode = oldParent;
    while (oldParent != nullptr) {
        PhyNode* s = oldParent->getParentNode();
        SAFE_POINT(s != firstNode, "There is cyclic graph in the phylogenetic tree", );
        distance = oldParent->getDistanceToRoot() - centralNode->getDistanceToRoot();
        oldParent->setParentNode(centralNode, distance);

        newParentNode = centralNode;
        centralNode = oldParent;
        oldParent = s;
    }

    if (centralNode->getChildNodes().size() == 1) {
        /* remove old root */
        oldParent = centralNode->getChildNodes().at(0);
        distance = oldParent->getDistanceToRoot() - newParentNode->getDistanceToRoot();
        oldParent->setParentNode(newParentNode, distance);
        delete centralNode;
    }

    phyTree->setRootNode(newRootNode);
}

PhyBranch* PhyTreeUtils::addBranch(PhyNode* node1, PhyNode* node2, double distance) {
    SAFE_POINT(!node1->isConnected(node2), "node1 is not connected to node2", nullptr);

    auto newBranch = new PhyBranch();
    newBranch->distance = distance;
    newBranch->node1 = node1;
    newBranch->node2 = node2;

    node1->branches.append(newBranch);
    node2->branches.append(newBranch);

    return newBranch;
}

void PhyTreeUtils::removeBranch(PhyNode* node1, PhyNode* node2) {
    QList<PhyBranch*> node1BranchesBeforeRemove = node1->branches;
    for (PhyBranch* branch : qAsConst(node1BranchesBeforeRemove)) {
        if (branch->node1 == node1 && branch->node2 == node2) {
            node1->branches.removeAll(branch);
            node2->branches.removeAll(branch);
            delete branch;
            break;
        }
    }
    SAFE_POINT(node1BranchesBeforeRemove.size() == node1->branches.size() + 1, "Branch is not found", );
}

}  // namespace U2
