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

#ifndef __ASSEMBLY_CONSENSUS_AREA_H__
#define __ASSEMBLY_CONSENSUS_AREA_H__

#include "AssemblyReferenceArea.h"
#include "AssemblyConsensusTask.h"

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Algorithm/AssemblyConsensusAlgorithm.h>

namespace U2 {

class AssemblyBrowserUi;

class AssemblyConsensusArea: public AssemblySequenceArea {
    Q_OBJECT
public:
    AssemblyConsensusArea(AssemblyBrowserUi * ui);

    QMenu * getConsensusAlgorithmMenu();
    QList<QAction*> getAlgorithmActions();
    QAction* getDiffAction() { return diffAction; }

public slots:
    void sl_exportConsensus();
    void sl_exportConsensusVariations();

protected:
    virtual QByteArray getSequenceRegion(U2OpStatus &os);
    virtual bool canDrawSequence();
    virtual void drawSequence(QPainter &p);
    virtual void mousePressEvent(QMouseEvent *e);

protected slots:
    virtual void sl_offsetsChanged();
    virtual void sl_zoomPerformed();

private slots:
    void sl_consensusAlgorithmChanged(QAction *a);
    void sl_drawDifferenceChanged(bool value);
    void sl_consensusReady();

private:
    void createContextMenu();
    void launchConsensusCalculation();
    void updateActions();

    QMenu * contextMenu;
    QMenu * consensusAlgorithmMenu;
    QList<QAction*> algorithmActions;
    QAction * diffAction;
    QAction * exportConsensusVariationsAction;
    QSharedPointer<AssemblyConsensusAlgorithm> consensusAlgorithm;

    ConsensusInfo cache;
    ConsensusInfo lastResult;
    U2Region previousRegion;
    bool canceled;
    BackgroundTaskRunner<ConsensusInfo> consensusTaskRunner;
};

} //ns

#endif
