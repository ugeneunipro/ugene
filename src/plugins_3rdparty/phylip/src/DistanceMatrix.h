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

#include <QMap>
#include <QString>
#include <U2Algorithm/SubstMatrixRegistry.h>
#include <U2Core/AppResources.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/PhyTree.h>
#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <QVector>
#include <iostream>

namespace U2{

typedef QVector<float> matrixrow;
typedef QVector<matrixrow> matrix;

class DistanceMatrix{
private:
    float* rawdata;
    int size;
    QMap<QString, int> index_map;
    const MultipleSequenceAlignment* malignment;
    PhyTreeData* treedata;
    matrix qdata;
    matrix middlematrix;
    QList<QString> visited_list;
    QList<QString> unprocessed_taxa;
    QList<PhyNode*> printed_nodes;
    QString errorMessage;

public:
    DistanceMatrix();
    ~DistanceMatrix();

    matrix rawMatrix;
    bool isValid();
    void calculateOutOfAlignment(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& settings);
    const QString& getErrorMessage() {return errorMessage;}
    void freeMemory(void*& allocatedMemory){free(allocatedMemory);}

private:
    
    //TODO: this stuff is obsolete. Consider deleting this.
    void  calculateQMatrix();
    float calculateRawDivergence (int i);
    float getDistance (QString seq1, QString seq2);
    void  dumpRawData(matrix& m);
    void  dumpQData();
    QString getTaxaName(int index);
    void addPairToTree(QPair<int, int> *location);
    int getSize(){return size;}
    PhyNode* getNodeByName(QString name);
    PhyNode* findNode(PhyNode* startnode, QString name);
    float calculateRootDistance(int i, int j);
    float calculateAdjacentDistance(int i, int j, float rootDistance);
    QString generateNodeName(QString name1, QString name2);
    void recalculateDistanceMatrix(int i, int j, PhyNode* newnode);
    float calculateNewDistance(QPair<int, int>* location, int current);
    bool hasUnprocessedTaxa(){
        return !unprocessed_taxa.empty();
    }
    static void printPhyTree(PhyTreeData* treeData);
    void printPhyTree();
    static void printPhyNode(PhyNode* node, int tab, QList<PhyNode*>& nodes);
    void printIndex();
    int getNewIndex(QString name, QPair<int, int> loc, QMap<QString, int>& old_map);
    void dumpRawData();
    static bool areTreesEqual(PhyTree* tree1, PhyTree* tree2);
    static void addNodeToList(QList<PhyNode*>& nodelist, QMap<QString, int>& nodemap, QList<PhyBranch*>& branches,  PhyNode* node);
    void switchName(PhyNode* node);
    void switchNamesToAllNodes();

    MemoryLocker memoryLocker;
};



}
