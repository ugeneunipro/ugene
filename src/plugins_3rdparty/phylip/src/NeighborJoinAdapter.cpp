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

#include "NeighborJoinAdapter.h"

#include <QString>
#include <QTemporaryFile>
#include <QVector>

#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/Task.h>

#include "DistanceMatrix.h"
#include "NeighborJoinWidget.h"
#include "PhylipCmdlineTask.h"
#include "SeqBootAdapter.h"

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include "dnadist.h"
#include "neighbor.h"
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

namespace U2 {

QMutex NeighborJoinCalculateTreeTask::runLock;

void createPhyTreeFromPhylipTree(const MultipleSequenceAlignment& ma, node* p, double m, boolean njoin, node* start, PhyNode* root, int bootstrap_repl) {
    PhyNode* current = nullptr;

    if (p == start) {
        current = root;
    } else {
        current = new PhyNode;
    }
    if (p) {
        static int counter = 0;
        if (p->tip) {
            if (bootstrap_repl != 0) {
                current->setName(QString::fromLatin1(p->nayme));
            } else {
                assert(p->index - 1 < ma->getRowCount());
                current->setName(QString(ma->getMsaRow(p->index - 1)->getName()));
            }
        } else {
            current->setName(QString("node %1").arg(counter++));
            createPhyTreeFromPhylipTree(ma, p->next->back, m, njoin, start, current, bootstrap_repl);
            createPhyTreeFromPhylipTree(ma, p->next->next->back, m, njoin, start, current, bootstrap_repl);
            if (p == start && njoin) {
                createPhyTreeFromPhylipTree(ma, p->back, m, njoin, start, current, bootstrap_repl);
            }
        }

        if (p == start) {
            counter = 0;
        } else {
            if (bootstrap_repl != 0) {
                if (p->deltav == 0) {
                    PhyTreeData::addBranch(root, current, bootstrap_repl);
                } else {
                    PhyTreeData::addBranch(root, current, p->deltav);
                }
            } else {
                PhyTreeData::addBranch(root, current, p->v);
            }
        }
    }
}

void replacePhylipRestrictedSymbols(QByteArray& name) {
    static const char badSymbols[] = {',', ':', '[', ']', '(', ')', ';'};
    static int sz = sizeof(badSymbols) / sizeof(char);
    for (int i = 0; i < sz; ++i) {
        name.replace(badSymbols[i], ' ');
    }
}

Task* NeighborJoinAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s) {
    return new PhylipCmdlineTask(ma, s);
}

CreatePhyTreeWidget* NeighborJoinAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment& ma, QWidget* parent) {
    return new NeighborJoinWidget(ma, parent);
}

NeighborJoinCalculateTreeTask::NeighborJoinCalculateTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s)
    : PhyTreeGeneratorTask(ma, s, TaskFlag_FailOnSubtaskError), memLocker(stateInfo) {
    setTaskName("NeighborJoin algorithm");
}

void NeighborJoinCalculateTreeTask::run() {
    QMutexLocker runLocker(&runLock);

    GCOUNTER(cvar, "PhylipNeigborJoin");

    PhyTree phyTree(nullptr);

    if (inputMA->getRowCount() < 3) {
        setError("Neighbor-Joining runs must have at least 3 species");
        result = phyTree;
        return;
    }

    try {
        if (settings.bootstrap) {  // bootstrapping and creating a consensus tree
            setTaskInfo(&stateInfo);
            setBootstr(true);
            stateInfo.setDescription("Generating sequences");

            QScopedPointer<SeqBoot> seqBoot(new SeqBoot);

            QTemporaryFile tmpFile;
            QString path = seqBoot->getTmpFileTemplate();
            if (!path.isEmpty()) {
                tmpFile.setFileTemplate(path);
            }
            if (!tmpFile.open()) {
                setError("Can't create temporary file");
                result = phyTree;
                return;
            }

            seqBoot->generateSequencesFromAlignment(inputMA, settings);

            stateInfo.setDescription("Calculating trees");

            for (int replicateIndex = 0; replicateIndex < settings.replicates; replicateIndex++) {
                stateInfo.progress = (int)(replicateIndex / (float)settings.replicates * 100);

                const MultipleSequenceAlignment& curMSA = seqBoot->getMSA(replicateIndex);
                QScopedPointer<DistanceMatrix> distanceMatrix(new DistanceMatrix());
                distanceMatrix->calculateOutOfAlignment(curMSA, settings);

                if (!distanceMatrix->getErrorMessage().isEmpty()) {
                    stateInfo.setError(distanceMatrix->getErrorMessage());
                    result = phyTree;
                    return;
                }
                QString distanceMatrixValidationError = distanceMatrix->validate();
                if (!distanceMatrixValidationError.isEmpty()) {
                    setError(tr("Failed to compute distance matrix: %1. Try to update run parameters.").arg(distanceMatrixValidationError));
                    result = phyTree;
                    return;
                }

                int sz = distanceMatrix->rawMatrix.count();

                // Allocate memory resources
                neighbour_init(sz, memLocker, tmpFile.fileName());
                if (memLocker.hasError()) {
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
                for (int rowIndex = 0; rowIndex < sz; ++rowIndex) {
                    const MultipleSequenceAlignmentRow row = inputMA->getMsaRow(rowIndex);
                    QByteArray name = row->getName().toLatin1();
                    replacePhylipRestrictedSymbols(name);
                    qstrncpy(nayme[rowIndex], name.constData(), sizeof(naym));

                    for (int j = name.length(); j < nmlngth; j++) {
                        nayme[rowIndex][j] = ' ';
                    }
                }

                // Calculate tree
                neighbour_calc_tree();

                neighbour_free_resources();
            }
            progress = 99;
            stateInfo.setDescription("Calculating consensus tree");

            QByteArray tmpFileName = tmpFile.fileName().toLocal8Bit();
            if (settings.consensusID == ConsensusModelTypes::Strict) {
                consens_starter(tmpFileName, settings.fraction, true, false, false, false);
            } else if (settings.consensusID == ConsensusModelTypes::MajorityRuleExt) {
                consens_starter(tmpFileName, settings.fraction, false, true, false, false);
            } else if (settings.consensusID == ConsensusModelTypes::MajorityRule) {
                consens_starter(tmpFileName, settings.fraction, false, false, true, false);
            } else if (settings.consensusID == ConsensusModelTypes::M1) {
                consens_starter(tmpFileName, settings.fraction, false, false, false, true);
            } else {
                QString message = "Unsupported consensus type: " + settings.consensusID;
                setError(L10N::internalError(message));
                FAIL(message, );
            }

            auto rootPhy = new PhyNode();
            bool njoin = true;

            createPhyTreeFromPhylipTree(inputMA, root, 0.43429448222, njoin, root, rootPhy, settings.replicates);

            consens_free_res();

            PhyTreeData* data = new PhyTreeData();
            data->setRootNode(rootPhy);

            phyTree = data;
        } else {
            // Exceptions are used to avoid phylip exit(-1) error handling and canceling task
            setTaskInfo(&stateInfo);
            setBootstr(false);

            QScopedPointer<DistanceMatrix> distanceMatrix(new DistanceMatrix);
            distanceMatrix->calculateOutOfAlignment(inputMA, settings);

            if (!distanceMatrix->getErrorMessage().isEmpty()) {
                stateInfo.setError(distanceMatrix->getErrorMessage());
                result = phyTree;
                return;
            }

            QString distanceMatrixValidationError = distanceMatrix->validate();
            if (!distanceMatrixValidationError.isEmpty()) {
                setError(tr("Failed to compute distance matrix: %1. Try to update run parameters.").arg(distanceMatrixValidationError));
                result = phyTree;
                return;
            }

            int sz = distanceMatrix->rawMatrix.count();

            // Allocate memory resources
            neighbour_init(sz, memLocker);
            if (memLocker.hasError()) {
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
    } catch (const std::bad_alloc&) {
        setError(QString("Not enough memory to calculate tree for alignment \"%1\"").arg(inputMA->getName()));
    } catch (const char* message) {
        stateInfo.setError(QString("Phylip error %1").arg(message));
    }

    result = phyTree;
}

}  // namespace U2
