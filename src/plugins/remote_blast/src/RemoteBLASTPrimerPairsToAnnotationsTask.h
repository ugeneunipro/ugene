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

#pragma once

#include <U2Core/Task.h>

#include "RemoteBLASTTask.h"

#include <QPointer>

namespace U2 {

class Annotation;
class RemoteBLASTPrimerPairToAnnotationsTask;
class U2SequenceObject;

// Run @RemoteBLASTPrimerPairToAnnotationsTask for each received primer pair
class RemoteBLASTPrimerPairsToAnnotationsTask : public Task {
public:
    // \param seqObj Sequence object.
    // \param primerAnnotationPairs Primer pairs list.
    // \param cfg BLAST config.
    RemoteBLASTPrimerPairsToAnnotationsTask(const QPointer<U2SequenceObject>& seqObj,
                                            const QList<QPair<Annotation*, Annotation*>>& primerAnnotationPairs,
                                            const RemoteBLASTTaskSettings& cfg);

    void prepare() override;

private:
    QPointer<U2SequenceObject> seqObj;
    QList<QPair<Annotation*, Annotation*>> primerAnnotationPairs;
    RemoteBLASTTaskSettings cfg;

    QList<RemoteBLASTPrimerPairToAnnotationsTask*> tasks;
};

}