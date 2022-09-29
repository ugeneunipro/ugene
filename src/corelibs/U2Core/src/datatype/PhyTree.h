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

#ifndef _U2_PHYTREE_H_
#define _U2_PHYTREE_H_

#include <QMap>
#include <QObject>
#include <QSharedData>
#include <QVector>

#include <U2Core/global.h>

namespace U2 {

class PhyNode;
class PhyBranch;

class U2CORE_EXPORT PhyTreeData : public QSharedData {
public:
    PhyTreeData() = default;

    /** TODO: class has no copy operator! */
    PhyTreeData(const PhyTreeData& other);

    // TODO: this destructor is not virtual!
    ~PhyTreeData();

    /** Returns all nodes in the tree using pre-order tree traversal algorithm. */
    QList<PhyNode*> getNodesPreOrder() const;

    /** Dumps tree into std output. */
    void print() const;

    void setRootNode(PhyNode* rootNode);

    PhyNode* getRootNode() const;

    bool hasNodeLabels = false;

private:
    PhyTreeData& operator=(const PhyTreeData&);
    PhyNode* rootNode = nullptr;
};
typedef QSharedDataPointer<PhyTreeData> PhyTree;

class U2CORE_EXPORT PhyBranch : public QObject {
    Q_OBJECT
public:
    PhyBranch() = default;

    void movingToAnotherAddress(PhyBranch* newAddress);

    PhyNode* node1 = nullptr;
    PhyNode* node2 = nullptr;
    double distance = 0;
    double nodeValue = -1.0;
};

class U2CORE_EXPORT PhyNode {
    Q_DISABLE_COPY(PhyNode)
    friend class PhyTreeUtils;
    friend class PhyTreeData;

public:
    PhyNode() = default;
    ~PhyNode();

    const QList<PhyBranch*>& getBranches() const;

    const PhyNode* getSecondNodeOfBranch(int branchNumber) const;

    /** Adds current node and all node children into the collection using pre-order algorithm. */
    void addIfNotPreset(QList<PhyNode*>& nodes);
    void addIfNotPreset(QList<const PhyNode*>& nodes) const;

    bool isConnected(const PhyNode* node) const;

    PhyNode* clone() const;

    /* For distance matrix */
    const PhyNode* getParentNode() const;

    PhyNode* getParentNode();

    void print(QList<PhyNode*>& nodes, int distance, int tabSize);

    /* For reroot */
    void setParentNode(PhyNode* newParent, double distance);

    QList<PhyNode*> getChildrenNodes() const;

    void swapBranches(int branchIndex1, int branchIndex2);

    double getDistanceToRoot() const;

    const PhyBranch* getParentBranch() const;

    QString name;

private:
    PhyNode* parent() const;

    QList<PhyBranch*> branches;
};

class U2CORE_EXPORT PhyTreeUtils {
public:
    static int getNumSeqsFromNode(const PhyNode* node, const QSet<QString>& names);

    static void rerootPhyTree(PhyTree& phyTree, PhyNode* node);

    static PhyBranch* addBranch(PhyNode* node1, PhyNode* node2, double distance);

    static void removeBranch(PhyNode* node1, PhyNode* node2);
};

}  // namespace U2

#endif
