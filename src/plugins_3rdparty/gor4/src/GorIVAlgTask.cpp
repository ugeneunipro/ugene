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

#include <QFile>

#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>
#include <U2View/SecStructPredictUtils.h>

#include "GorIVAlgTask.h"
#include "gor.h"


namespace U2 {

const QString GorIVAlgTask::taskName = "GORIV";
QMutex GorIVAlgTask::runLock;

GorIVAlgTask::GorIVAlgTask(const QByteArray& inputSeq) : SecStructPredictTask(inputSeq)
{
    GCOUNTER( cvar, tvar, "GorIVAlgTask" );
}

void GorIVAlgTask::run() 
{
    output.resize(sequence.size());
    sequence.push_front(' ');


    QFile seqDb(":gor4//datafiles//New_KS.267.seq");
    if (!seqDb.open(QIODevice::ReadOnly)) {
        stateInfo.setError(SecStructPredictTask::tr("seq database not found"));
        return;
    }

    QFile strucDb(":gor4//datafiles//New_KS.267.obs");
    if (!strucDb.open(QIODevice::ReadOnly)) {
        stateInfo.setError(SecStructPredictTask::tr("observed structures database not found"));
        return;
    }

    //TODO: get rid of this limit
    const int MAXRES = 10000;
    if (sequence.size() > MAXRES) {
        stateInfo.setError(SecStructPredictTask::tr("sequence is too long, max seq size is 10000"));
        return;
    }

    //TODO: BUG-0000808
    QMutexLocker runLocker( &runLock );
    runGORIV(seqDb, strucDb, sequence.data(), sequence.size() - 1, output.data(), stateInfo);
    CHECK_OP(stateInfo, );
    results = SecStructPredictUtils::saveAlgorithmResultsAsAnnotations(output, GORIV_ANNOTATION_NAME);
}


} //namespace



