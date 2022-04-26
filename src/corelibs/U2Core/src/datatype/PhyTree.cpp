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

PhyTreeData::PhyTreeData()
    : rootNode(nullptr), haveNodeLabels(false) {
}

PhyTreeData::PhyTreeData(const PhyTreeData& other)
    : QSharedData(other), haveNodeLabels(false) {
    rootNode = other.rootNode == nullptr ? nullptr : other.rootNode->clone();
}

PhyTreeData::~PhyTreeData() {
    if (rootNode != nullptr) {
        delete rootNode;
        rootNode = nullptr;
    }
}

PhyBranch* PhyTreeData::addBranch(PhyNode* node1, PhyNode* node2, double distance) {
    assert(!node1->isConnected(node2));

    PhyBranch* b = new PhyBranch();
    b->distance = distance;
    b->node1 = node1;
    b->node2 = node2;

    node1->branches.append(b);
    node2->branches.append(b);

    return b;
}

void PhyTreeData::removeBranch(PhyNode* node1, PhyNode* node2) {
    foreach (PhyBranch* b, node1->branches) {
        if (b->node1 == node1 && b->node2 == node2) {
            node1->branches.removeAll(b);
            node2->branches.removeAll(b);
            delete b;
            return;
        }
    }
    assert(0);
}

void PhyTreeData::setUsingNodeLabels(bool _haveNodeLabels) {
    haveNodeLabels = _haveNodeLabels;
}

void PhyTreeData::renameNodes(const QMap<QString, QString>& newNamesByOldNames) {
    SAFE_POINT(nullptr != rootNode, QObject::tr("UGENE internal error"), );

    QList<PhyNode*> treeNodes = rootNode->getChildrenNodes();
    treeNodes.append(rootNode);

    foreach (PhyNode* currentNode, treeNodes) {
        QString newName = newNamesByOldNames[currentNode->getName()];
        if (!newName.isEmpty()) {
            currentNode->setName(newName);
        }
    }
}

PhyNode::PhyNode() {
}

PhyBranch::PhyBranch()
    : node1(nullptr), node2(nullptr), distance(0), nodeValue(-1.0) {
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

const PhyNode* PhyNode::getSecondNodeOfBranch(int branchNumber) const {
    SAFE_POINT(branchNumber < branches.size() && branchNumber >= 0, "Invalid branch number", nullptr);
    return branches.at(branchNumber)->node2;
}
double PhyNode::getBranchesDistance(int branchNumber) const {
    SAFE_POINT(branchNumber < branches.size() && branchNumber >= 0, "Invalid branch number", -1.0);
    return branches.at(branchNumber)->distance;
}

double PhyNode::getBranchesNodeValue(int branchNumber) const {
    SAFE_POINT(branchNumber < branches.size() && branchNumber >= 0, "Invalid branch number", -1.0);
    return branches.at(branchNumber)->nodeValue;
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

PhyBranch* PhyNode::getBranchAt(int i) const {
    return branches[i];
}

PhyBranch* PhyNode::getBranch(int i) const {
    return getBranchAt(i);
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

    PhyBranch* b = new PhyBranch();
    b->distance = distance;
    b->node1 = newParent;
    b->node2 = this;

    newParent->branches.append(b);
    branches.append(b);
}

QList<PhyNode*> PhyNode::getChildrenNodes() const {
    QList<PhyNode*> childNodes;
    foreach (PhyBranch* branch, branches) {
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
    for (int i = 0, s = branches.size(); i < s; ++i) {
        PhyBranch* curBranch = branches[i];
        SAFE_POINT(nullptr != curBranch, "NULL pointer to PhyBranch", );
        PhyNode* childNode = curBranch->node2;
        SAFE_POINT(nullptr != childNode, "NULL pointer to PhyNode", );
        if (childNode != this) {
            childNode->branches.removeOne(branches[i]);
            delete curBranch;
            delete childNode;
        } else {
            PhyNode* parentNode = curBranch->node1;
            if (parentNode != nullptr) {
                parentNode->branches.removeOne(branches[i]);
            }
            delete curBranch;
        }
    }
}

PhyNode* PhyNode::clone() const {
    QList<const PhyNode*> nodesPreOrder;
    addIfNotPreset(nodesPreOrder);

    QList<PhyBranch*> allBranches;
    QMap<const PhyNode*, PhyNode*> nodeTable;
    for (const PhyNode* n : qAsConst(nodesPreOrder)) {
        PhyNode* n2 = new PhyNode();
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
        PhyTreeData::addBranch(node1, node2, b->distance);
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
    QList<PhyBranch*> blist = this->branches;
    int s = blist.size();
    for (int i = 0; i < s; i++) {
        if (blist[i]->node2 != 0) {
            int d = blist[i]->distance;
            blist[i]->node2->print(nodes, tab, d);
        }
    }
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
        QString str(node->getName());
        return names.contains(str.replace('_', ' ')) ? 1 : 0;
    }
}

void PhyTreeUtils::rerootPhyTree(PhyTree& phyTree, PhyNode* node) {
    PhyNode* curRoot = phyTree->getRootNode();
    SAFE_POINT(nullptr != curRoot, "Null pointer argument 'curRoot' were passed to PhyTreeUtils::rerootPhyTree(...)", );
    SAFE_POINT(nullptr != node, "Null pointer argument 'node' were passed to PhyTreeUtils::rerootPhyTree(...)", );

    double distance = 0.0;
    if (node == curRoot) {
        return;
    }
    PhyNode* centralNode = node->getParentNode();
    if (centralNode == curRoot) {
        if (centralNode->getChildrenNodes().at(0) != node) {
            centralNode->swapBranches(0, 1);
        }
        return;
    }
    PhyNode* newRoot = new PhyNode();
    PhyNode* newParentNode = newRoot;
    distance = node->getDistanceToRoot() - newRoot->getDistanceToRoot();
    node->setParentNode(newRoot, distance);
    PhyNode* oldParent = centralNode->getParentNode();
    if (nullptr != oldParent) {
        distance = centralNode->getDistanceToRoot() - newRoot->getDistanceToRoot();
        centralNode->setParentNode(newRoot, distance);
    }

    PhyNode* s = nullptr;
    const PhyNode* firstNode = oldParent;
    while (nullptr != oldParent) {
        s = oldParent->getParentNode();
        SAFE_POINT(s != firstNode, "There is cyclic graph in the phylogenetic tree", );
        distance = oldParent->getDistanceToRoot() - centralNode->getDistanceToRoot();
        oldParent->setParentNode(centralNode, distance);

        newParentNode = centralNode;
        centralNode = oldParent;
        oldParent = s;
    }

    if (centralNode->getChildrenNodes().size() == 1) {
        /* remove old root */
        oldParent = centralNode->getChildrenNodes().at(0);
        distance = oldParent->getDistanceToRoot() - newParentNode->getDistanceToRoot();
        oldParent->setParentNode(newParentNode, distance);
        delete centralNode;
    }

    phyTree->setRootNode(newRoot);
}

}  // namespace U2
