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
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "SiteconAlgorithmTests.h"
#include "DIPropertiesSitecon.h"
#include "SiteconIO.h"

/* TRANSLATOR U2::GTest */

namespace U2 {

#define DOC_ATTR "doc"
#define DOC_URL "url"
#define DOC1_ATTR "model1"
#define DOC2_ATTR "model2"
#define SEQNAME_ATTR "sequence"
#define EXPECTED_RESULTS_ATTR  "expected_results"
#define DINUCLEOTIDE_POSITIONS "di_positions"
#define PROPERTIES_INDEXES "props_indexes"
#define OFFSET_ATTR "offset"
#define MODEL_ATTR "model"
#define STRAND_ATTR "strand"
#define TRESH_ATTR "treshhold"

void GTest_CalculateACGTContent::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString expected = el.attribute(EXPECTED_RESULTS_ATTR);
    QStringList expectedList = expected.split(QRegExp("\\,")); //may be QRegExp("\\,")
    if (expectedList.size() != 4) {
        stateInfo.setError(  QString("here must be 4 items in %1").arg(EXPECTED_RESULTS_ATTR) );
        return;
    }
    int i = 0, sum = 0;
    foreach(QString str, expectedList) {
        bool isOk;
        int m = str.toInt(&isOk);
        if(!isOk) {
            stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(EXPECTED_RESULTS_ATTR) );
            return;
        }
        expectedACGT[i++] = m;
        sum += m;
    }
    if(sum < 100 || sum > 102) {
        stateInfo.setError(  QString("Wrong %1 values").arg(EXPECTED_RESULTS_ATTR) );
        return;
    }
}

void GTest_CalculateACGTContent::prepare(){
    Document* doc = getContext<Document>(this, docName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(docName) );
        return;
    }
    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    MultipleSequenceAlignmentObject *mao = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(mao==NULL){
        stateInfo.setError(  QString("error can't cast to MultipleSequenceAlignmentObject from GObject") );
        return;
    }
    ma = mao->getMsaCopy();
}

void GTest_CalculateACGTContent::run() {
    SiteconAlgorithm::calculateACGTContent(ma, s);
}

Task::ReportResult GTest_CalculateACGTContent::report(){
    for(int i = 0; i < 4; i++) {
        if (expectedACGT[i] != s.acgtContent[i]) {
            stateInfo.setError(  QString("Actual results not equal with expected") );
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

void GTest_CalculateDispersionAndAverage::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QStringList propsList = el.attribute(PROPERTIES_INDEXES).split(QRegExp("\\,")),
                diPosStrList = el.attribute(DINUCLEOTIDE_POSITIONS).split(QRegExp("\\,")),
                expectedStrList = el.attribute(EXPECTED_RESULTS_ATTR).split(QRegExp("\\,"));
    QStringList::Iterator expResIt;
    expResIt = expectedStrList.begin();
    foreach(QString posStr, diPosStrList) {
        bool isOk;
        int pos = posStr.toInt(&isOk);
        if(!isOk) {
            stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(DINUCLEOTIDE_POSITIONS) );
            return;
        }
        foreach(QString propStr, propsList) {
            int propIndex = propStr.toInt(&isOk);
            if(!isOk) {
                stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(PROPERTIES_INDEXES) );
                return;
            }
            ResultVector r;
            r.push_back(pos);
            r.push_back(propIndex);
            int exp = qRound((*expResIt).toFloat(&isOk) * 10000);
            if(!isOk) {
                stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            r.push_back(exp);
            if(expResIt == expectedStrList.end()){
                stateInfo.setError(  QString("Too less items in %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            expResIt++;
            exp = qRound((*expResIt).toFloat(&isOk) * 10000);
            if(!isOk) {
                stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            r.push_back(exp);
            expectedResults.push_back(ResultVector(r));
            if(expResIt == expectedStrList.end()){
                stateInfo.setError(  QString("Too less items in %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            expResIt++;
        }
    }

    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }
}

void GTest_CalculateDispersionAndAverage::prepare() {
    Document* doc = getContext<Document>(this, docName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(docName) );
        return;
    }
    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    MultipleSequenceAlignmentObject *mao = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(mao==NULL){
        stateInfo.setError(  QString("error can't cast to MultipleSequenceAlignmentObject from GObject") );
        return;
    }
    ma = mao->getMsaCopy();
}

void GTest_CalculateDispersionAndAverage::run() {
    DinucleotitePropertyRegistry di;
    s.props = di.getProperties();
    SiteconAlgorithm::calculateACGTContent(ma, s);
    s.numSequencesInAlignment = ma->getNumRows();
    TaskStateInfo stub;
    result = SiteconAlgorithm::calculateDispersionAndAverage(ma, s, stub);
}

Task::ReportResult GTest_CalculateDispersionAndAverage::report() {
    foreach(ResultVector rv, expectedResults){
        int i = rv[0];
        int j = rv[1];
        PositionStats vec = result[i];
        DiStat stat = vec[j];
        int sdev = qRound(stat.sdeviation * 10000),
            average = qRound(stat.average * 10000),
            expAve = rv[2],
            expSdev = rv[3];
        if (sdev != expSdev) {
            stateInfo.setError(  QString("Expected and Actual 'SDev' values are different: %1 %2").arg(expSdev/10000).arg(sdev/10000) );
            return ReportResult_Finished;
        }
        if (average != expAve) {
            stateInfo.setError(  QString("Expected and Actual 'Average' values are different: %1 %2").arg(expAve/10000).arg(average/10000) );
            return ReportResult_Finished;
        }

    }
    return ReportResult_Finished;
}

void GTest_CalculateFirstTypeError::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString windowSizeStr = el.attribute(OFFSET_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(OFFSET_ATTR);
        return;
    }
    bool isOk;
    offset = windowSizeStr.toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(OFFSET_ATTR) );
        return;
    }

    QStringList expectedStrList = el.attribute(EXPECTED_RESULTS_ATTR).split(QRegExp("\\,"));
    foreach(QString str, expectedStrList){
        int exp = qRound(str.toFloat(&isOk) * 10000);
        if(!isOk) {
            stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(EXPECTED_RESULTS_ATTR) );
            return;
        }
        expectedResult.push_back(exp);
    }
}

void GTest_CalculateFirstTypeError::prepare() {
    Document* doc = getContext<Document>(this, docName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(docName) );
        return;
    }
    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    MultipleSequenceAlignmentObject *mao = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(mao==NULL){
        stateInfo.setError(  QString("error can't cast to MultipleSequenceAlignmentObject from GObject") );
        return;
    }
    ma = mao->getMsaCopy();
}

void GTest_CalculateFirstTypeError::run() {
    DinucleotitePropertyRegistry di;
    s.props = di.getProperties();
    SiteconAlgorithm::calculateACGTContent(ma, s);
    s.numSequencesInAlignment = ma->getNumRows();
    s.windowSize = ma->getLength();
    TaskStateInfo stub;
    result = SiteconAlgorithm::calculateFirstTypeError(ma, s, stub);
}

Task::ReportResult GTest_CalculateFirstTypeError::report() {
    int i = offset + 1;
    foreach(int exp, expectedResult) {
        int act = qRound(result[i] * 10000);
        //printf("Expected: %i", exp);
        //printf(" Actual: %i \r\n", act);
        if(act != exp){
            stateInfo.setError(  QString("Expected and Actual values are different: %1 %2").arg(exp).arg(act) );
            return ReportResult_Finished;
        }
        i++;
    }
    return ReportResult_Finished;
}

void GTest_CalculateSecondTypeError::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString windowSizeStr = el.attribute(OFFSET_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(OFFSET_ATTR);
        return;
    }
    bool isOk;
    offset = windowSizeStr.toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(OFFSET_ATTR) );
        return;
    }

    QStringList expectedStrList = el.attribute(EXPECTED_RESULTS_ATTR).split(QRegExp("\\,"));
    foreach(QString str, expectedStrList){
        int exp = str.toInt(&isOk);
        if(!isOk) {
            stateInfo.setError(  QString("Wrong conversion to the integer for one of the %1").arg(EXPECTED_RESULTS_ATTR) );
            return;
        }
        expectedResult.push_back(exp);
    }
}

void GTest_CalculateSecondTypeError::prepare() {
    Document* doc = getContext<Document>(this, docName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(docName) );
        return;
    }
    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    MultipleSequenceAlignmentObject *mao = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(mao==NULL){
        stateInfo.setError(  QString("error can't cast to MultipleSequenceAlignmentObject from GObject") );
        return;
    }
    ma = mao->getMsaCopy();
}

void GTest_CalculateSecondTypeError::run() {
    DinucleotitePropertyRegistry di;
    s.props = di.getProperties();
    SiteconAlgorithm::calculateACGTContent(ma, s);
    s.numSequencesInAlignment = ma->getNumRows();
    s.windowSize = ma->getLength();
    SiteconModel m;
    m.aliURL = (getContext<Document>(this, docName))->getURLString();
    m.modelName = QFileInfo(m.aliURL).baseName();
    m.settings = s;
    m.matrix = SiteconAlgorithm::calculateDispersionAndAverage(ma, s, stateInfo);
    SiteconAlgorithm::calculateWeights(ma, m.matrix, m.settings, false, stateInfo);
    TaskStateInfo stub1;
    m.err1 = SiteconAlgorithm::calculateFirstTypeError(ma, s, stub1);
    TaskStateInfo stub2;
    result = SiteconAlgorithm::calculateSecondTypeError(m.matrix, s, stub2);
}

Task::ReportResult GTest_CalculateSecondTypeError::report() {
    int i = offset + 1;
    foreach(int exp, expectedResult) {
        int act = qRound(1/result[i]);
        printf("Expected: %i", exp);
        printf(" Actual: %i \r\n", act);
        if(act != exp){
            stateInfo.setError(  QString("Expected and Actual values are different: %1 %2").arg(exp).arg(act) );
            return ReportResult_Finished;
        }
        i++;
    }
    return ReportResult_Finished;
}

void GTest_SiteconSearchTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    seqName = el.attribute(SEQNAME_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQNAME_ATTR);
        return;
    }

    QString modelPath = el.attribute(MODEL_ATTR);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QString url = env->getVar("COMMON_DATA_DIR") + "/" + modelPath;
    model = SiteconIO::readModel(iof, url, stateInfo);

    QString strandStr = el.attribute(STRAND_ATTR);
    if (strandStr.isEmpty()) {
        failMissingValue(STRAND_ATTR);
        return;
    }
    if (strandStr == "direct") {
        complOnly = false;
        isNeedCompliment = false;
    }else if(strandStr == "compliment") {
        complOnly = true;
        isNeedCompliment = true;
    }else if(strandStr == "both"){
        complOnly = false;
        isNeedCompliment = true;
    }else {
        stateInfo.setError(  QString("%1 has incorrect value").arg(STRAND_ATTR) );
        return;
    }

    bool isOk;
    QString tre = el.attribute(TRESH_ATTR);
    if (tre.isEmpty()) {
        failMissingValue(TRESH_ATTR);
        return;
    }
    tresh = tre.toInt(&isOk);
    if(!isOk){
        stateInfo.setError(  QString("unable to convert %1 to integer").arg(TRESH_ATTR) );
        return;
    }

    QString expected = el.attribute(EXPECTED_RESULTS_ATTR);
    if (!expected.isEmpty()) {
        QStringList expectedList = expected.split(QRegExp("\\;"));
        foreach(QString propsArray, expectedList) {
            QStringList props = propsArray.split(QRegExp("\\,"));
            QString middleStr = props[0], scoreStr = props[1], strStr = props[2];
            int middle = middleStr.toInt(&isOk);
            if(!isOk){
                stateInfo.setError(  QString("unable to convert %1 to integer").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            U2Strand strand;
            U2Region reg;
            reg.length = model.settings.windowSize;
            if (strStr == "direct") {
                strand = U2Strand::Direct;
                reg.startPos = middle - (int)(model.settings.windowSize/2);
            } else if(strStr == "compliment") {
                strand = U2Strand::Complementary;
                reg.startPos = middle - (int)(model.settings.windowSize/2) + 1;
                /*
                if(model.settings.weightAlg == SiteconWeightAlg_Alg2){
                    reg.startPos++;
                }
                */
            } else {
                stateInfo.setError(  QString("%1 has incorrect value").arg(STRAND_ATTR) );
                return;
            }
            float psum = scoreStr.toFloat(&isOk);
            if(!isOk){
                stateInfo.setError(  QString("unable to convert %1 to float").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            SiteconSearchResult ssr;
            ssr.psum = psum;
            ssr.region = reg;
            ssr.strand = strand;
            expectedResults.append(ssr);
        }
    }
}

void GTest_SiteconSearchTask::prepare() {
    U2SequenceObject * mySequence = getContext<U2SequenceObject>(this, seqName);
    CHECK_EXT(mySequence != NULL, setError( QString("error can't cast to sequence from GObject")), );

    SiteconSearchCfg cfg;
    cfg.complOnly = complOnly;
    cfg.minPSUM = tresh;
    if (isNeedCompliment){
        cfg.complTT = GObjectUtils::findComplementTT(mySequence->getAlphabet());
    }
    QByteArray seqData = mySequence->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );
    task = new SiteconSearchTask(model, seqData, cfg, 0);
    addSubTask(task);
}

Task::ReportResult GTest_SiteconSearchTask::report() {
    results = task->takeResults();
    int matchesCount = 0;
    /*
    printf("Actual: \r\n");
    foreach(SiteconSearchResult r, results) {
        printf("%i ", r.region.startPos + (int)(model.settings.windowSize/2));
        if(r.complement){
            printf("C \r\n");
        }else{
            printf("D \r\n");
        }
    }
    printf("Expected: \r\n");
    foreach(SiteconSearchResult r, expectedResults) {
        printf("%i ", r.region.startPos + (int)(model.settings.windowSize/2));
        if(r.complement){
            printf("C \r\n");
        }else{
            printf("D \r\n");
        }
    }
    */
    if(results.size() != expectedResults.size()){
        stateInfo.setError(  QString("expected and equal result lists not equal by size, expected: %1, actual: %2").arg(expectedResults.size()).arg(results.size()) );
        return ReportResult_Finished;
    }
    /**/
    foreach(SiteconSearchResult exp, expectedResults) {
        foreach(SiteconSearchResult act, results) {
            int ePsum = qRound(exp.psum * 10), aPsum = qRound(act.psum * 10);
            if (exp.region == act.region && aPsum == ePsum && exp.strand == act.strand) {
                matchesCount++;
            }
        }
    }
    if(matchesCount != expectedResults.size()){
        stateInfo.setError(  QString("expected and equal result lists not equal") );
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}



void GTest_CompareSiteconModels::init(XMLTestFormat *, const QDomElement& el) {
    doc1ContextName = el.attribute(DOC1_ATTR);
    if (doc1ContextName.isEmpty()) {
        failMissingValue(DOC1_ATTR);
        return;
    }

    doc2ContextName = el.attribute(DOC2_ATTR);
    if (doc2ContextName.isEmpty()) {
        failMissingValue(DOC2_ATTR);
        return;
    }
}

Task::ReportResult GTest_CompareSiteconModels::report() {
    //SiteconModel model1 = getContext<SiteconModel>(doc1ContextName);
    //SiteconModel model2 = getContext<SiteconModel>(doc2ContextName);
    Document *doc1 = getContext<Document>(this, doc1ContextName);
    if (doc1 == NULL) {
        stateInfo.setError(  QString("document not found %1").arg(doc1ContextName) );
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, doc2ContextName);
    if (doc2 == NULL) {
        stateInfo.setError(  QString("document not found %1").arg(doc2ContextName) );
        return ReportResult_Finished;
    }
    SiteconModel model1 = SiteconIO::readModel(doc1->getIOAdapterFactory(), doc1->getURLString(), stateInfo);
    SiteconModel model2 = SiteconIO::readModel(doc2->getIOAdapterFactory(), doc2->getURLString(), stateInfo);
    if(model1 != model2) {
        stateInfo.setError(tr("Models not equal"));
    }
    return ReportResult_Finished;
}

}//namespace
