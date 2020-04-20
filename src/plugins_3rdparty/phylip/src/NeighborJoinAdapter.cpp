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

#include <QString>
#include <QTemporaryFile>
#include <QVector>

#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/Task.h>

#include "DistanceMatrix.h"
#include "NeighborJoinAdapter.h"
#include "NeighborJoinWidget.h"
#include "PhylipCmdlineTask.h"
#include "SeqBootAdapter.h"

#include "dnadist.h"
#include "neighbor.h"
#include "protdist.h"

namespace U2 {

QMutex NeighborJoinCalculateTreeTask::runLock;

void createPhyTreeFromPhylipTree(const MultipleSequenceAlignment &ma, node *p, double m, boolean njoin, node *start, PhyNode* root, int bootstrap_repl)
{
    PhyNode* current = NULL;

    if (p == start) {
        current = root;
    } else {
        current = new PhyNode;
    }
    if(p){
        static int counter = 0;
        if (p->tip) {
            if(bootstrap_repl != 0){
                current->setName(QString::fromLatin1(p->nayme));
            }else{
                assert(p->index - 1 < ma->getNumRows());
                current->setName(QString(ma->getMsaRow(p->index - 1)->getName()));
            }
        } else {
            current->setName(QString("node %1").arg(counter++));
            createPhyTreeFromPhylipTree(ma, p->next->back,  m, njoin, start, current, bootstrap_repl);
            createPhyTreeFromPhylipTree(ma, p->next->next->back, m, njoin, start, current, bootstrap_repl);
            if (p == start && njoin) {
                createPhyTreeFromPhylipTree(ma, p->back, m, njoin, start, current, bootstrap_repl);
            }
        }

        if (p == start) {
            counter = 0;
        } else {
            if(bootstrap_repl != 0){
                if(p->deltav == 0){
                    PhyTreeData::addBranch(root, current, bootstrap_repl);
                }else{
                    PhyTreeData::addBranch(root, current, p->deltav );
                }
            }else{
                PhyTreeData::addBranch(root, current, p->v);
            }
            
        }
    }
}  

void replacePhylipRestrictedSymbols(QByteArray& name) {
    static const char badSymbols[] = {',',':','[',']','(',')',';' };
    static int sz = sizeof (badSymbols) / sizeof(char);
    for (int i = 0; i < sz; ++i) {
        name.replace(badSymbols[i], ' ');
    }
}

Task* NeighborJoinAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s){
    return new PhylipCmdlineTask(ma, s);
}

CreatePhyTreeWidget * NeighborJoinAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment &ma, QWidget *parent) {
    return new NeighborJoinWidget(ma, parent);
}

NeighborJoinCalculateTreeTask::NeighborJoinCalculateTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s)
:PhyTreeGeneratorTask(ma, s), memLocker(stateInfo){
    setTaskName("NeighborJoin algorithm");
}

void NeighborJoinCalculateTreeTask::run(){
    QMutexLocker runLocker( &runLock );

    GCOUNTER(cvar,tvar, "PhylipNeigborJoin" );


    PhyTree phyTree(NULL);

    if (inputMA->getNumRows() < 3) {
        setError("Neighbor-Joining runs must have at least 3 species");
        result = phyTree;
        return;
    }

    try {
        if(settings.bootstrap){ //bootstrapping and creating a consensus tree
            setTaskInfo(&stateInfo);
            setBootstr(true);
            stateInfo.setDescription("Generating sequences");

            QScopedPointer<SeqBoot> seqBoot(new SeqBoot);

            QTemporaryFile tmpFile;
            QString path = seqBoot->getTmpFileTemplate();
            if(!path.isEmpty()){
                tmpFile.setFileTemplate(path);
            }
            if(!tmpFile.open()){
                setError("Can't create temporary file");
                result = phyTree;
                return;
            }

            seqBoot->generateSequencesFromAlignment(inputMA,settings);

            stateInfo.setDescription("Calculating trees");

            for (int i = 0; i < settings.replicates; i++){
                stateInfo.progress = (int)(i/(float)settings.replicates * 100);

                const MultipleSequenceAlignment& curMSA = seqBoot->getMSA(i);
                QScopedPointer<DistanceMatrix> distanceMatrix(new DistanceMatrix);
                distanceMatrix->calculateOutOfAlignment(curMSA,settings);

                if(!distanceMatrix->getErrorMessage().isEmpty()) {
                    stateInfo.setError(distanceMatrix->getErrorMessage());
                    result = phyTree;
                    return;
                }
                if (!distanceMatrix->isValid()) {
                    setError("Calculated distance matrix is invalid");
                    result = phyTree;
                    return;
                }

                int sz = distanceMatrix->rawMatrix.count();

                // Allocate memory resources
                neighbour_init(sz, memLocker, tmpFile.fileName());
                if(memLocker.hasError()) {
                    stateInfo.setError(memLocker.getError());
                    return;
                }

                // Fill data
                vector* m = getMtx();
                for (int i = 0; i < sz; ++i) {
                    for (int j = 0; j < sz; ++j) {
                        m[i][j] = distanceMatrix->rawMatrix[i][j];
                    }
                }

                naym* nayme = getNayme();
                for (int i = 0; i < sz; ++i) {
                    const MultipleSequenceAlignmentRow row = inputMA->getMsaRow(i);
                    QByteArray name = row->getName().toLatin1();
                    replacePhylipRestrictedSymbols(name);
                    qstrncpy(nayme[i], name.constData(), sizeof(naym));

                    for(int j = name.length(); j < nmlngth; j++){
                        nayme[i][j] = ' ';
                    }
                }

                // Calculate tree
                const tree* curTree = neighbour_calc_tree(); 

                neighbour_free_resources();
            }
            progress = 99;
            stateInfo.setDescription("Calculating consensus tree");

            if(settings.consensusID == ConsensusModelTypes::Strict){
                consens_starter(tmpFile.fileName().toStdString().c_str(), settings.fraction, true, false, false, false);
            }else if(settings.consensusID == ConsensusModelTypes::MajorityRuleExt){
                consens_starter(tmpFile.fileName().toStdString().c_str(), settings.fraction, false, true, false, false);
            }else if(settings.consensusID == ConsensusModelTypes::MajorityRule){
                consens_starter(tmpFile.fileName().toStdString().c_str(), settings.fraction, false, false, true, false);
            }else if(settings.consensusID == ConsensusModelTypes::M1){
                consens_starter(tmpFile.fileName().toStdString().c_str(), settings.fraction, false, false, false, true);
            }else{
                assert(0);
            }

            PhyNode* rootPhy = new PhyNode();
            bool njoin = true;

            createPhyTreeFromPhylipTree(inputMA, root, 0.43429448222, njoin, root, rootPhy, settings.replicates);

            consens_free_res();

            PhyTreeData* data = new PhyTreeData();
            data->setRootNode(rootPhy);

            phyTree = data;
        }else{

        // Exceptions are used to avoid phylip exit(-1) error handling and canceling task 
            setTaskInfo(&stateInfo);
            setBootstr(false);

            QScopedPointer<DistanceMatrix> distanceMatrix(new DistanceMatrix);
            distanceMatrix->calculateOutOfAlignment(inputMA,settings);

            if(!distanceMatrix->getErrorMessage().isEmpty()) {
                stateInfo.setError(distanceMatrix->getErrorMessage());
                result = phyTree;
                return;
            }
            if (!distanceMatrix->isValid()) {
                stateInfo.setError("Calculated distance matrix is invalid");
                result = phyTree;
                return;
            }

            int sz = distanceMatrix->rawMatrix.count();

            // Allocate memory resources
            neighbour_init(sz, memLocker);
            if(memLocker.hasError()) {
                stateInfo.setError(memLocker.getError());
                return;
            }

            // Fill data
            vector* m = getMtx();
            for (int i = 0; i < sz; ++i) {
                for (int j = 0; j < sz; ++j) {
                    m[i][j] = distanceMatrix->rawMatrix[i][j];
                }
            }

            naym* nayme = getNayme();
            for (int i = 0; i < sz; ++i) {
                const MultipleSequenceAlignmentRow row = inputMA->getMsaRow(i);
                QByteArray name = row->getName().toLatin1();
                replacePhylipRestrictedSymbols(name);
                qstrncpy(nayme[i], name.constData(), sizeof(naym));
            }

            // Calculate tree
            const tree* curTree = neighbour_calc_tree();


            PhyNode* root = new PhyNode();
            bool njoin = true;

            stateInfo.progress = 99;
            createPhyTreeFromPhylipTree(inputMA, curTree->start, 0.43429448222, njoin, curTree->start, root, 0);

            neighbour_free_resources();

            PhyTreeData* data = new PhyTreeData();
            data->setRootNode(root);

            phyTree = data;
        }
    }
    catch (const std::bad_alloc &) {
        setError(QString("Not enough memory to calculate tree for alignment \"%1\"").arg(inputMA->getName()));
    }
    catch (const char* message) {
        stateInfo.setError(QString("Phylip error %1").arg(message));
    }

    result = phyTree;
}

} 
