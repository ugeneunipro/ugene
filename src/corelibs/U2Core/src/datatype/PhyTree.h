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

    /** Returns true if the tree has at least 1 non-leaf node with a label. */
    bool hasNamedInnerNodes() const;

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

    PhyNode* parentNode = nullptr;
    PhyNode* childNode = nullptr;

    /** Distance between child and parent nodes. */
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

    PhyBranch* getParentBranch() const;

    PhyNode* getParentNode() const;

    const QList<PhyBranch*>& getChildBranches() const;

    /** Adds current node and all node children into the collection using pre-order algorithm. */
    void collectNodesPreOrder(QList<PhyNode*>& nodes);
    void collectNodesPreOrder(QList<const PhyNode*>& nodes) const;

    bool isConnected(const PhyNode* node) const;

    /** Clones current node & sub-tree. */
    PhyNode* clone() const;

    /** Prints sub-tree to stdout. */
    void print(int distance = 0, int tabSize = 0) const;

    /**Inverts order of the child node branches. */
    void invertOrderOrChildBranches();

    /** Returns true if the node is a leaf (tip) node in the tree. */
    bool isLeafNode() const;

    /** Returns true if the node is a root node in the tree. */
    bool isRootNode() const;

    QString name;

    int countLeafNodesInSubtree() const;

    /** Unlinks current node from the parent node: destroys branch between this node and parent node. */
    void unlinkFromParent();

private:
    /** Make this node a root node in the hierarchy. Moves parent node to the children of the current node. */
    void makeRoot();

    /**
     * Branch to the parent node.
     * Null for the root node.
     * Current node owns this branch: the branch is destroyed by the current node.
     */
    PhyBranch* parentBranch = nullptr;

    /**
     * Branches to child nodes.
     * Empty for leaf nodes.
     * Current node does not own these branches: branches are removed as a part of child node removal.
     */
    QList<PhyBranch*> childBranches;
};

class U2CORE_EXPORT PhyTreeUtils {
public:
    /** Re-roots tree in-place: all nodes & branches are preserved, but some branches are inverted. */
    static void rerootPhyTree(PhyTree& phyTree, PhyNode* node);

    /** Adds new branch between 2 nodes. The child node must have no parent. */
    static PhyBranch* addBranch(PhyNode* parentNode, PhyNode* childNode, double distance);
};

}  // namespace U2

#endif
