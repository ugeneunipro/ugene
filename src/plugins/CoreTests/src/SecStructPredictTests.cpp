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

#include <QDomElement>

#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Algorithm/SecStructPredictTask.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "SecStructPredictTests.h"

namespace U2 {

#define SEQ_ATTR "seq"
#define SEQ_NAME_ATTR "seq-name"
#define INDEX_ATTR "index"
#define OUTPUT_SEQ_ATTR "output-seq"
#define ALG_NAME_ATTR "algorithm-name"


void GTest_SecStructPredictAlgorithm::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    inputSeq = el.attribute(SEQ_ATTR);
    if (inputSeq.isEmpty()) {
        failMissingValue(SEQ_ATTR);
        return;
    }

    outputSeq = el.attribute(OUTPUT_SEQ_ATTR);
    if (outputSeq.isEmpty()) {
        failMissingValue(OUTPUT_SEQ_ATTR);
        return;
    }

    algName = el.attribute(ALG_NAME_ATTR);
    if (algName.isEmpty()) {
        failMissingValue(ALG_NAME_ATTR);
        return;
    }

}


void GTest_SecStructPredictAlgorithm::prepare() {

    SecStructPredictAlgRegistry* sspr = AppContext::getSecStructPredictAlgRegistry();
    if (!sspr->hadRegistered(algName)) {
        stateInfo.setError( QString(tr("Algorithm named %1 not found")).arg(algName));
        return;
    }
    SecStructPredictTaskFactory* factory = sspr->getAlgorithm(algName);
    task = factory->createTaskInstance(inputSeq.toLatin1());
    addSubTask(task);

}


Task::ReportResult GTest_SecStructPredictAlgorithm::report() {

    const QByteArray& output = task->getSSFormatResults();

    if (output != outputSeq) {
        stateInfo.setError(tr("Output sec struct sequence is incorrect"));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

///////////////////////////////////////////////////////////////////////////////////////////

void GTest_SecStructPredictTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    seqName = el.attribute(SEQ_NAME_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQ_NAME_ATTR);
        return;
    }

    algName = el.attribute(ALG_NAME_ATTR);
    if (algName.isEmpty()) {
        failMissingValue(ALG_NAME_ATTR);
        return;
    }

    resultsTableContextName = el.attribute(INDEX_ATTR);
    if (resultsTableContextName.isEmpty()) {
        failMissingValue(INDEX_ATTR);
        return;
    }
}

void GTest_SecStructPredictTask::prepare()
{
    U2SequenceObject * mySequence = getContext<U2SequenceObject>(this, seqName);
    if(mySequence==NULL){
        stateInfo.setError( QString("error can't cast to sequence from GObject"));
        return;
    }
    SecStructPredictAlgRegistry* sspr = AppContext::getSecStructPredictAlgRegistry();
    if (!sspr->hadRegistered(algName)) {
        stateInfo.setError( QString(tr("Algorithm named %1 not found")).arg(algName));
        return;
    }
    SecStructPredictTaskFactory* factory = sspr->getAlgorithm(algName);
    QByteArray seqData = mySequence->getWholeSequenceData(stateInfo);
    task = factory->createTaskInstance(seqData);
    CHECK_OP(stateInfo, );
    addSubTask(task);
}

Task::ReportResult GTest_SecStructPredictTask::report()
{
    if (task!=NULL && task->hasError()) {
        stateInfo.setError(task->getError());
    } else if (!resultsTableContextName.isEmpty()) {
        QList<SharedAnnotationData> results = task->getResults();
        U2OpStatusImpl os;
        const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        SAFE_POINT_OP(os, ReportResult_Finished);
        aObj = new AnnotationTableObject(resultsTableContextName, dbiRef);
        aObj->addAnnotations(results);
        addContext(resultsTableContextName, aObj);
        contextAdded = true;
    }

    return ReportResult_Finished;
}

void GTest_SecStructPredictTask::cleanup()
{
    if (contextAdded) {
        removeContext(resultsTableContextName);
    }

    XmlTest::cleanup();
}
///////////////////////////////////////////////////////////////////////////////////////////


QList<XMLTestFactory*> SecStructPredictTests::createTestFactories()
{
    QList<XMLTestFactory*> res;
    res.append(GTest_SecStructPredictAlgorithm::createFactory());
    res.append(GTest_SecStructPredictTask::createFactory());
    return res;
}

} //namespace

