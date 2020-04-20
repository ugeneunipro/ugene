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
    PhyTreeData();
    PhyTreeData(const PhyTreeData& other);
    ~PhyTreeData();
    QList<const PhyNode*> collectNodes() const;
    void print() const;

    void setRootNode(PhyNode* _rootNode) { rootNode = _rootNode; }
    PhyNode* getRootNode() const { return rootNode; }

    static PhyBranch* addBranch(PhyNode* node1, PhyNode* node2, double distance);
    static void removeBranch(PhyNode* node1, PhyNode* node2);

    void setUsingNodeLabels(bool haveNodeLabels);
    bool usingNodeLabels() const { return haveNodeLabels; }

    void renameNodes(const QMap<QString, QString>& newNamesByOldNames);
private:
    PhyTreeData & operator= (const PhyTreeData &);
    PhyNode* rootNode;
    bool haveNodeLabels;
};
typedef QSharedDataPointer<PhyTreeData> PhyTree;

class U2CORE_EXPORT PhyBranch : public QObject {
    Q_OBJECT
public:
    PhyBranch();

    void movingToAnotherAddress(PhyBranch* newAdress);

    PhyNode* node1;
    PhyNode* node2;
    double   distance;
    double      nodeValue;
};

class U2CORE_EXPORT PhyNode {
    Q_DISABLE_COPY(PhyNode)
        friend class PhyTreeUtils;
    friend class PhyTreeData;
public:
    PhyNode();
    ~PhyNode();

    /* const */
    const QString & getName() const { return name; }
    int branchCount() const { return branches.size(); }
    const PhyNode * getSecondNodeOfBranch(int branchNumber) const;
    double getBranchesDistance(int branchNumber) const;
    double getBranchesNodeValue(int branchNumber) const;
    void validate(QList<const PhyNode*>& track) const;
    bool isConnected(const PhyNode* node) const;
    PhyNode * clone() const;

    PhyBranch * getBranch(int i) const;
    void setName(const QString& _name) { name = _name; }

    /* For distance matrix */
    const PhyNode * getParentNode() const;
    PhyNode * getParentNode();
    void setBranchesDistance(int branchNumber, double distance) { branches.at(branchNumber)->distance = distance; }
    void print(QList<PhyNode*>& nodes, int distance, int tab);

    /* For reroot */
    void setParentNode(PhyNode* newParent, double distance);
    QList<PhyNode*> getChildrenNodes() const;
    void swapBranches(int firstBrunch, int secondBranch) { branches.swap(firstBrunch, secondBranch); }
    double getDistanceToRoot() const;

    const PhyBranch * getParentBranch() const;

private:
    QString             name;
    QList<PhyBranch*>   branches;

private:
    PhyBranch * getBranchAt(int i) const;
    PhyNode * parent() const;
    void addToTrack(QSet<const PhyNode*>& track) const;
};

class U2CORE_EXPORT PhyTreeUtils {
public:
    static int getNumSeqsFromNode(const PhyNode *node, const QSet<QString>& names);
    static void rerootPhyTree(PhyTree& phyTree, PhyNode* node);
};

}//namespace

#endif

