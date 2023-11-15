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

#include "RemoteBLASTPrimerPairsToAnnotationsTask.h"
#include "RemoteBLASTPrimerPairToAnnotationsTask.h"

#include <U2Core/Annotation.h>

#include <U2View/ADVSequenceObjectContext.h>

#include <QPointer>

namespace U2 {


RemoteBLASTPrimerPairsToAnnotationsTask::RemoteBLASTPrimerPairsToAnnotationsTask(const QPointer<ADVSequenceObjectContext>& _ctx,
                                                                                 const QList<QPair<Annotation*, Annotation*>>& _primerAnnotationPairs,
                                                                                 const RemoteBLASTTaskSettings& _cfg)
    : Task(tr("BLAST primer pairs"), TaskFlags_NR_FOSE_COSC),
    ctx(_ctx),
    primerAnnotationPairs(_primerAnnotationPairs),
    cfg(_cfg) {
    // We should not run more than one BLAST request per time,
    // because multiple requests looks very suspicious and BLAST could block them all
    setMaxParallelSubtasks(1);
}

void RemoteBLASTPrimerPairsToAnnotationsTask::prepare() {
    for (const auto& primerPair : qAsConst(primerAnnotationPairs)) {
        auto group = primerPair.first->getGroup();
        auto t = new RemoteBLASTPrimerPairToAnnotationsTask(group->getName(), ctx, primerPair.first->getData(), primerPair.second->getData(), cfg, group->getGroupPath());
        tasks << t;
        addSubTask(t);
    }
}


}  // namespace U2