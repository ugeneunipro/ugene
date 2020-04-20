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

#include "DistanceMatrix.h"
#include "U2Core/global.h"
#include <U2Core/DNAAlphabet.h>

#include <QSharedData>

#include "dnadist.h"
#include "protdist.h"

#include <iostream>
#include <float.h>

namespace U2{

void DistanceMatrix::calculateOutOfAlignment( const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& settings ) {
    try {
        malignment = &ma;
        int index = 0;
        int size = ma->getNumRows();
        this->size = size;
        printdata = false;

        foreach(const MultipleSequenceAlignmentRow& r, ma->getMsaRows()) {
            const QString& str = r->getName();
            index_map.insert(str, index);
            index++;
            unprocessed_taxa.append(str);
        }
        if(!memoryLocker.tryAcquire(size*(sizeof(matrixrow) + sizeof(float)*size))) {
            errorMessage = memoryLocker.getError();
            return;
        }

        for(int i=0; i<size; i++){
            matrixrow row;

            for(int j=0; j<size; j++){
                row.append(0);
            }

            rawMatrix.append(row);
        }
        spp = ma->getNumRows();
        sites = ma->getLength();
        chars = sites;
        nonodes = 2*sites - 1;
        DNAAlphabetType alphabetType = ma->getAlphabet()->getType();

        ibmpc = IBMCRT;
        ansi = ANSICRT;
        mulsets = false;
        datasets = 1;
        firstset = true;

        if ((alphabetType == DNAAlphabet_RAW) || (alphabetType == DNAAlphabet_NUCL)){

            setDNADistSettings(settings);
            doinit(memoryLocker);
            if(memoryLocker.hasError()) {
                errorMessage = memoryLocker.getError();
                return;
            } 
            //ttratio = ttratio0;
            inputoptions();

            for (int k=0; k<spp; k++){
                for(int j=0; j<sites; j++) {
                    y[k][j] = ma->getMsaRow(k)->charAt(j);
                }
            }
            makeweights();
            dnadist_makevalues();
            dnadist_empiricalfreqs();

            getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange, freqsfrom, printdata);
            makedists();

        } else {

            prot_doinit(settings, memoryLocker);
            if(memoryLocker.hasError()) {
                errorMessage = memoryLocker.getError();
                return;
            } 
            if (!(kimura || similarity))
                code();
            if (!(usejtt || usepmb || usepam ||  kimura || similarity)) {
                protdist_cats();
                maketrans();
                qreigen(prob, 20L);
            } else {
                if (kimura || similarity)
                    fracchange = 1.0;
                else {
                    if (usejtt)
                        jtteigen();
                    else {
                        if (usepmb)
                            pmbeigen();
                        else
                            pameigen();
                    }
                }
            }

            doinput();
            Phylip_Char charstate;
            aas aa = (aas)0;

            for (int k=0; k<spp; k++){
                for(int j=0; j<sites; j++){
                    charstate = ma->getMsaRow(k)->charAt(j);
                    switch (charstate) {
                        case 'A':
                            aa = ala;
                            break;
                        case 'B':
                            aa = asx;
                            break;

                        case 'C':
                            aa = cys;
                            break;

                        case 'D':
                            aa = asp;
                            break;

                        case 'E':
                            aa = glu;
                            break;

                        case 'F':
                            aa = phe;
                            break;

                        case 'G':
                            aa = gly;
                            break;

                        case 'H':
                            aa = his;
                            break;

                        case 'I':
                            aa = ileu;
                            break;

                        case 'K':
                            aa = lys;
                            break;

                        case 'L':
                            aa = leu;
                            break;

                        case 'M':
                            aa = met;
                            break;

                        case 'N':
                            aa = asn;
                            break;

                        case 'P':
                            aa = pro;
                            break;

                        case 'Q':
                            aa = gln;
                            break;

                        case 'R':
                            aa = arg;
                            break;

                        case 'S':
                            aa = ser;
                            break;

                        case 'T':
                            aa = thr;
                            break;

                        case 'V':
                            aa = val;
                            break;

                        case 'W':
                            aa = trp;
                            break;

                        case 'X':
                            aa = unk;
                            break;

                        case 'Y':
                            aa = tyr;
                            break;

                        case 'Z':
                            aa = glx;
                            break;

                        case '*':
                            aa = stop;
                            break;

                        case '?':
                            aa = quest;
                            break;

                        case '-':
                            aa = del;
                            break;
                    }
                    gnode[k][j] = aa;
                }
            }

            if (ith == 1)
                firstset = false;
            prot_makedists();

            for (int i = 0; i < spp; i++) {
                free(gnode[i]);
            }
        }
        for (int i=0; i<spp; i++){
            for(int j=0; j<spp; j++){
                rawMatrix[i][j] = d[i][j];
            }
        }
    } catch(const std::bad_alloc &) {
        errorMessage = QString("Not enough memory to calculate distance matrix for alignment \"%1\"").arg(ma->getName());
        if(NULL != gnode) {
            for (int i = 0; i < spp; i++) {
                free(gnode[i]);
            }
            free(gnode);
        }
    }
}

DistanceMatrix::DistanceMatrix()
    : rawdata(NULL),
      size(),
      malignment(NULL),
      treedata(NULL)
{

}

DistanceMatrix::~DistanceMatrix(){
    if(NULL != y) {
        for (int i = 0; i < spp; i++) {
            free(y[i]); 
        }
        free(y);
        y = NULL;
    }

    if(NULL != nodep) {
        for (int i = 0; i < spp; i++) {
            for (int j = 0; j < endsite; j++) {
                free(nodep[i]->x[j]);
            }
            free(nodep[i]->x);
            free(nodep[i]);
        }
        free(nodep);
        nodep = NULL;
    }
    free(category);
    category = NULL;

    free(oldweight);
    oldweight = NULL;

    free(weight);
    weight = NULL;

    free(alias);
    alias = NULL;

    free(ally);
    ally = NULL;

    free(location);
    location = NULL;

    free(weightrat);
    weightrat = NULL;

    if(NULL != d) {
        for (int i = 0; i < spp; i++) {
            free(d[i]);
        }
        free(d);
        d = NULL;
    }
}

void DistanceMatrix::printPhyTree(){
    DistanceMatrix::printPhyTree(treedata);

}
void DistanceMatrix::calculateQMatrix(){
    for(int i=0; i<size; i++){
        matrixrow row;

        for(int j=0; j<size; j++){
            row.append(0);
        }

        qdata.append(row);
    }

    for(int i=0; i<size; i++){
        qdata[i].reserve(size);

    }
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(i==j){
                qdata[i][j] = 0.0;
            }else{
                float dij = rawMatrix[i][j];
                float ri = calculateRawDivergence(i);
                float rj = calculateRawDivergence(j);
                float q = dij - (ri + rj)/(size-2);
                qdata[i][j] = q;
            }
        }
    }
}


void DistanceMatrix::dumpRawData(matrix& m){
    std::cout<<"Distance Matrix "<<std::endl;
    for (int i=0; i< size; i++){
        for(int j=0; j<size; j++){
            float distance = m[i][j];
            std::cout<<distance<<" ";
        }
        std::cout<<std::endl;
    }
}

void DistanceMatrix::dumpQData(){
    std::cout<<"Q Matrix "<<std::endl;
    for (int i=0; i< size; i++){
        for(int j=0; j<size; j++){
            float distance = qdata[i][j];
            std::cout<<distance<<" ";
        }
        std::cout<<std::endl;
    }
}
QString DistanceMatrix::generateNodeName(QString name1, QString name2){
    QString r = "___";
    r.append(name1);
    r = r.append("___");
    r = r.append(name2);
    return r;
}
void DistanceMatrix::switchName(PhyNode* node){
    QString str = node->getName();
    if(str.startsWith("ROOT")){
        node->setName("");
    }
    if(str.startsWith("___")){
        node->setName("");
    }
    for(int i=0; i<node->branchCount(); i++){
        node->setBranchesDistance(i, abs(node->getBranchesDistance(i)));
        if(node->getBranchesDistance(i) != node->getBranchesDistance(i)){
            node->setBranchesDistance(i, 1.0);
        }
    }
}

void DistanceMatrix::addPairToTree(QPair<int, int>* location){
    //        treedata->rootNode->dumpBranches();
    QString name1 = getTaxaName(location->first);
    if(unprocessed_taxa.contains(name1)){
        unprocessed_taxa.removeAll(name1);
    }
    PhyNode* oldnode1 = getNodeByName(name1);
    if(oldnode1==0){
        return;
    }
    PhyNode* rootnode1 = oldnode1->getParentNode();

    treedata->removeBranch(rootnode1, oldnode1);

    //printPhyTree();

    QString name2 = getTaxaName(location->second);   
    if(unprocessed_taxa.contains(name2)){
        unprocessed_taxa.removeAll(name2);
    }
    PhyNode* oldnode2 = getNodeByName(name2);
    PhyNode* rootnode2 = oldnode2->getParentNode();
    treedata->removeBranch(rootnode2, oldnode2);
    if (oldnode1==oldnode2){
        return;
    }
    //printPhyTree();

    float distance1 = calculateRootDistance(location->first, location->second);
    float distance2 = calculateAdjacentDistance(location->first, location->second, distance1);

    PhyNode* new_node = new PhyNode();
    QString new_name = generateNodeName(name1, name2);
    new_node->setName(new_name);
    treedata->addBranch(new_node, oldnode1, distance1);
    if(!new_node->isConnected(oldnode2)){
        treedata->addBranch(new_node, oldnode2, distance2);
    }
    if(!rootnode1->isConnected(new_node)){
        treedata->addBranch(rootnode1, new_node, 1);
    }
    //printPhyTree();

    for(int j=0; j<this->size; j++){
        middlematrix.append(rawMatrix[j]);
    }
    //dumpRawData();
    //        dumpRawData(middlematrix);

    for(int j=0; j<size; j++){
        rawMatrix[j].remove(location->first);
        rawMatrix[j].remove(location->second);

    }
    rawMatrix.remove(location->first);
    rawMatrix.remove(location->second);
    this->size-=2;

    //        dumpRawData(rawMatrix);
    QMap<QString, int> old_map;
    QList<QString> namelist = index_map.uniqueKeys();
    for(int j = 0; j<index_map.size(); j++){
        int index = index_map[namelist[j]];
        old_map.insert(namelist[j], index);
    }



    QList<QString> names = index_map.uniqueKeys();
    index_map.clear();
    for(int j =0; j<names.size(); j++){
        QString cur_name = names[j];
        if(cur_name!=name1 && cur_name!=name2){
            int index = getNewIndex(cur_name, *location, old_map);
            index_map.insert(cur_name, index);
        }
    }
    //        int new_index = index_map.size();

    index_map.insert(new_name, index_map.size());
    //printIndex();

    for(int j =0; j<size; j++){
        QString name  = getTaxaName(j);
        int old_index = old_map[name];
        float distance = calculateNewDistance(location, old_index);
        rawMatrix[j].append(distance);
    }

    this->size++;

    matrixrow row;
    for(int j=0; j<size-1; j++){
        float distance = rawMatrix[j][size-1];
        row.append(distance);

    }
    row.append(0);


    rawMatrix.append(row);
    middlematrix.clear();

    //dumpRawData();
    //treedata->rootNode->dumpBranches();
}

PhyNode *DistanceMatrix::getNodeByName(QString name){
    visited_list.clear();
    PhyNode* rootNode = treedata->getRootNode();
    return findNode(rootNode, name);
}

PhyNode* DistanceMatrix::findNode(PhyNode* startnode, QString name){
    visited_list.append(startnode->getName());
    if(startnode->getName() == name){
        return startnode;
    }else{
        for (int i=0; i<startnode->branchCount(); i++) {
            PhyBranch *branch = startnode->getBranch(i);
            if(!visited_list.contains(branch->node2->getName())){
                PhyNode* node2 = findNode(branch->node2, name);
                if(node2!=0){
                    return node2;
                }
            }
        }
    }
    return 0;
}

float DistanceMatrix::calculateRootDistance(int i, int j){
    //S(AU) =d(AB) / 2 + [r(A)-r(B)] / 2(N-2) = 1 
    float distance = rawMatrix[i][j]/2;

    float num1 = calculateRawDivergence(i);
    float num2 = calculateRawDivergence(j);

    float div = (num1 - num2)/(2*(size-2));
    float result = distance+div;
    return result;
}

float DistanceMatrix::calculateAdjacentDistance(int i, int j, float rootDistance){
    float result = rawMatrix[i][j] - rootDistance;
    return result;
}

float DistanceMatrix::calculateNewDistance(QPair<int, int>* location, int current){
    float a = middlematrix[current][location->first];
    float b = middlematrix[current][location->second];
    float c = middlematrix[location->first][location->second]/2; 
    return a + b - c; 
}

float DistanceMatrix::calculateRawDivergence(int index){
    float divergence = 0;
    for (int i=0; i<index; i++){
        divergence+=rawMatrix[i][index];
    }
    for(int j=index; j<size; j++){
        divergence+=rawMatrix[index][j];
    }
    return divergence;
}

QString DistanceMatrix::getTaxaName(int index){
    QMap<QString, int>::iterator i;
    for (i = index_map.begin(); i != index_map.end(); ++i){
        if(i.value()==index){
            return i.key();
        }

    }
    return " ";
}

void DistanceMatrix::printPhyTree(PhyTreeData* data){
    data->print();
}

void DistanceMatrix::printPhyNode(PhyNode* node, int tab, QList<PhyNode*>& nodes){
    if(node==0 || nodes.contains(node)){
        return;
    }
    nodes.append(node);
    for(int i=0; i<tab; i++){
        std::cout<<" ";
    }
    tab++;
    std::cout<<"name: "<<node->getName().toLatin1().constData()<<std::endl;
    for (int i=0; i<node->branchCount(); i++) {
        PhyBranch *branch = node->getBranch(i);
        printPhyNode(branch->node2, tab, nodes);
    }
}
bool DistanceMatrix::areTreesEqual(PhyTree* tree1, PhyTree* tree2){
    QMap<QString, int> nodes1;
    QMap<QString, int> nodes2;

    QList<PhyNode* > list1;
    QList<PhyNode* > list2;

    QList<PhyBranch* > branches1;
    QList<PhyBranch* > branches2;

    addNodeToList(list1, nodes1, branches1, tree1->data()->getRootNode());
    addNodeToList(list2, nodes2, branches2, tree2->data()->getRootNode());

    QList<QString> keys1 = nodes1.keys();

    if(nodes1.count()!=nodes2.count())
        return false;

    for(int i=0; i<keys1.size(); i++){
        QString name = keys1[i];
        //int d1 = nodes1[name];
        if(!nodes2.contains(name)){
            return false;
        }

        if(!nodes2.contains(name)){
            return false;
        }
        /*int d2 = nodes2[name];
        if(d1!=d2){
        return false;
        }*/

    }

    return true;
}

void DistanceMatrix::addNodeToList(QList<PhyNode*>& nodelist, QMap<QString, int>& nodemap, QList<PhyBranch*>& branches,  PhyNode* node){
    if ((node==0) ||(nodelist.contains(node))){
        return;
    }else{
        nodelist.append(node);
        if(node->getName()!="ROOT" && node->getName()!="" && !node->getName().startsWith("___")){
            for (int i=0; i<node->branchCount(); i++) {
                PhyBranch *branch = node->getBranch(i);
                if(branch->node2 == node){
                    int d = branch->distance;
                    nodemap.insert(node->getName(), d);
                }
            }
        }
        for (int i=0; i<node->branchCount(); i++) {
            PhyBranch *branch = node->getBranch(i);
            if(!branches.contains(branch)){
                branches.append(branch);
                addNodeToList(nodelist, nodemap, branches, branch->node2);
            }
        }
    }

}

void DistanceMatrix::switchNamesToAllNodes(){
    QList<PhyNode* > nodes;
    QList<PhyBranch* > branches;
    QMap <QString, int> mmap;
    addNodeToList(nodes,mmap, branches, treedata->getRootNode());
    int size = nodes.size();

    for(int i=0; i<size; i++){
        switchName(nodes[i]);
    }
}


int DistanceMatrix::getNewIndex(QString name, QPair<int, int> loc, QMap<QString, int>& old_map){
    int old_index = old_map[name];
    if(old_index > loc.first && old_index > loc.second){
        int new_index = old_index-2;
        return new_index;
    }else if(old_index > loc.first || old_index>loc.second){
        int new_index = old_index-1;
        return new_index;
    }
    return old_index;
}
void DistanceMatrix::printIndex(){

    for(int i=0; i<index_map.size(); i++){
        QList<QString> names = index_map.keys(i);
        std::cout<<"Value :"<<i<<" Keys:";
        for(int j=0; j<names.size(); j++){
            std::cout<<" "<<names[j].toLatin1().constData();
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;

}
void DistanceMatrix::dumpRawData(){
    dumpRawData(rawMatrix);
}

inline bool isFiniteNumber(double x) 
{
    return (x <= DBL_MAX && x >= -DBL_MAX); 
}    

bool DistanceMatrix::isValid() {
    int sz = rawMatrix.count();
    int zeroCounter = 0;
    // The matrix must be square and must not have infinite numbers
    for (int i = 0; i < sz; ++i) {
        int sz2 = rawMatrix[i].count();
        if (sz2 != sz) {
            return false;
        }
        for (int j = 0; j < sz; ++j) {
            double value = rawMatrix[i][j];
            if (!isFiniteNumber(value)) {
                return false;
            }
            if (value == 0) {
                ++zeroCounter;
            }

        }
    }

    if (zeroCounter == sz*sz) {
        // All numbers are zeroes!
        return false;
    }

    return true;
}

} //namespace
