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

#include <U2Algorithm/SArrayBasedFindTask.h>
#include <U2Algorithm/SArrayIndex.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "RepeatFinderTests.h"
#include "FindRepeatsTask.h"
#include "RF_SArray_TandemFinder.h"

namespace U2 {

#define SEQ_ATTR    "seq"
#define SEQ2_ATTR   "seq2"
#define REG_ATTR    "reg"
#define W_ATTR      "w"
#define C_ATTR      "c"
#define INV_ATTR    "inverted"
#define MIND_ATTR   "mind"
#define MAXD_ATTR   "maxd"
#define RESULT_ATTR "expected_result"
#define REFL_ATTR   "reflect"
#define EXCL_ATTR   "exclude"
#define SEQUENCE    "sequence"
#define QUERY       "query"
#define USE_BITMASK "bit-mask"
#define MISMATCHES  "mismatches"
#define ALG_ATTR    "alg"


U2Region GTest_FindSingleSequenceRepeatsTask::parseRegion(const QString& n, const QDomElement& el) {
    U2Region res;
    QString v = el.attribute(n);
    if (v.isEmpty()) {
        return res;
    }
    int idx = v.indexOf("..");
    if (idx == -1 || idx+2 >= v.length()) {
        return res;
    }
    QString v1 = v.left(idx);
    QString v2 = v.mid(idx+2);
    int startPos = v1.toInt();
    int endPos = v2.toInt();
    if (startPos >= 0 && endPos > startPos) {
        res.startPos = startPos - 1;
        res.length = endPos - startPos + 1;
    }
    return res;
}

void GTest_FindSingleSequenceRepeatsTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    seq = el.attribute(SEQ_ATTR);
    if (seq.isEmpty()) {
        stateInfo.setError(QString("Value not found '%1'").arg(SEQ_ATTR));
        return;
    }
    seq2 = el.attribute(SEQ2_ATTR);
    if (seq2.isEmpty()) {
        seq2 = seq;
    }
    region = parseRegion(REG_ATTR, el);

    QString algStr = el.attribute(ALG_ATTR);

    if (algStr == "suffix") {
        alg = RFAlgorithm_Suffix;
    }
    else {
        if (algStr == "diagonal") {
            alg = RFAlgorithm_Diagonal;
        }
        else {
            alg = RFAlgorithm_Auto;
        }
    }

    minD = el.attribute(MIND_ATTR, "-1").toInt();
    maxD = el.attribute(MAXD_ATTR, "-1").toInt();

    QString wStr = el.attribute(W_ATTR);
    if (wStr.isEmpty()) {
        stateInfo.setError(QString("Value not found '%1'").arg(W_ATTR));
        return;
    }
    w = wStr.toInt();
    if (w < 2) {
        stateInfo.setError(QString("Illegal value for '%1': %2").arg(W_ATTR).arg(wStr));
        return;
    }

    QString cStr = el.attribute(C_ATTR, "0");
    c = cStr.toInt();
    if (c < 0  || c >= w) {
        stateInfo.setError(QString("Illegal value for '%1': %2").arg(C_ATTR).arg(cStr));
        return;
    }

    inverted = el.attribute("invert") == "true";
    reflect = el.attribute("reflect", "true") == "true";
    filterNested = el.attribute("filterNested", "false") == "true";
    filterUnique = el.attribute("filterUnique", "false") == "true";

    if (filterNested && filterUnique){
        stateInfo.setError(QString("Filter unique and filter nested cannot go together"));
        return;
    }

    resultFile = el.attribute(RESULT_ATTR);
    if (resultFile.isEmpty()) {
        stateInfo.setError(QString("Value not found '%1'").arg(RESULT_ATTR));
        return;
    }

    excludeList = el.attribute(EXCL_ATTR).split(',', QString::SkipEmptyParts);
}

static QString getAlgName(RFAlgorithm alg) {
    QString res;
    switch(alg) {
        case RFAlgorithm_Diagonal: res = "diagonal"; break;
        case RFAlgorithm_Suffix: res = "suffix"; break;
        default: res = "UNKNOWN"; break;
    }
    return res;
}

void GTest_FindSingleSequenceRepeatsTask::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }
    U2SequenceObject * seq1IObj = getContext<U2SequenceObject>(this, seq);
    if (seq1IObj == NULL){
        stateInfo.setError("can't find sequence1");
        return;
    }

    if (region.isEmpty()) {
        region = U2Region(0, seq1IObj->getSequenceLength());
    }

    int maxLen = seq1IObj->getSequenceLength();
    if (minD == -1) {
        minD = -maxLen;
    }
    if (maxD == -1) {
        maxD = maxLen;
    }


    QList<RFAlgorithm> algos;
    if (alg == RFAlgorithm_Auto) {
        algos << RFAlgorithm_Diagonal << RFAlgorithm_Suffix;
    }
    else {
        algos << alg;
    }

    FindRepeatsTaskSettings s;
    s.minLen = w;
    s.mismatches = c;
    s.minDist = minD;
    s.maxDist = maxD;
    s.inverted = inverted;
    s.seqRegion = region;
    s.seq2Region = region;
    s.reportReflected = reflect;
    s.nThreads = 1;//todo: add to settings

    if (filterNested == true){
        s.filter = DisjointRepeats;
    }else if(filterUnique == true){
        s.filter = UniqueRepeats;
    }else{
        s.filter = NoFiltering;
    }

    U2OpStatusImpl os;
    foreach(RFAlgorithm algo, algos) {
        QString algName = getAlgName(algo);
        if (excludeList.contains(algName)) {
            continue;
        }
        s.algo = algo;
        DNASequence seqData = seq1IObj->getWholeSequence(os);
        CHECK_OP_EXT(os, setError(os.getError()), );
        Task* sub = new FindRepeatsTask(s, seqData, seqData);
        addSubTask(sub);
    }
}

void GTest_FindSingleSequenceRepeatsTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }
    QVector<RFResult> expectedResults;
    // load file with results
    QString fname = env->getVar("COMMON_DATA_DIR") + "/" + resultFile;
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stateInfo.setError(QString("Can't open results file %1").arg(fname));
        return;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList hit = line.split(' ', QString::SkipEmptyParts);
        if (!(hit.size()==3 || hit.size()==4)) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(line));
            return;
        }
        RFResult r;
        r.x = hit[0].toInt() - 1;
        r.y = hit[1].toInt() - 1;
        r.l = hit[2].toInt();
        r.c = hit.size()==4 ? hit[3].toInt() : -1;
        if (r.x < 0 || r.y < 0 || r.l < 0 || (hit.size()==4 && r.c < 0)) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(line));
            return;
        }
        expectedResults.append(r);

    }
    file.close();

    qSort(expectedResults);

    //check all subtasks
    FindRepeatsTask* sub = qobject_cast<FindRepeatsTask*>(getSubtasks()[0].data());
    QVector<RFResult> calcResults = sub->getResults();
    if (expectedResults.size()!=calcResults.size()) {
        stateInfo.setError(QString("Results count not matched, num = %1, expected = %2, alg = %3")
                        .arg(calcResults.size()).arg(expectedResults.size()).arg(getAlgName(sub->getSettings().algo)));
        return;
    }
    qSort(calcResults);

    for (int i=0, n = expectedResults.size(); i < n; i++) {
        RFResult re = expectedResults[i];
        RFResult rc = calcResults[i];
        if (re!=rc || ((re.c>=0) && (re.c != rc.c))) {
            QString errorString = QString("Results not matched, expected(%1, %2, %3), computed(%4, %5, %6), algo = %7")
                .arg(re.x).arg(re.y).arg(re.l).arg(rc.x).arg(rc.y).arg(rc.l).arg(getAlgName(sub->getSettings().algo));

            if (re.c>=0) {
                errorString = QString("Results not matched, expected(%1, %2, %3, %4), computed(%5, %6, %7, %8), algo = %9")
                    .arg(re.x).arg(re.y).arg(re.l).arg(re.c).arg(rc.x).arg(rc.y).arg(rc.l).arg(rc.c).arg(getAlgName(sub->getSettings().algo));
            }

            stateInfo.setError(errorString);
            return;
        }
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void GTest_FindTandemRepeatsTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    minD = el.attribute(MIND_ATTR, "-1").toInt();
    maxD = el.attribute(MAXD_ATTR, "-1").toInt();

    minSize = el.attribute("minSize", "3").toInt();
    repeatCount = el.attribute("repeatCount", "3").toInt();

    inverted = el.attribute("invert") == "true";
    reflect = el.attribute("reflect", "true") == "true";
    filterNested = el.attribute("filterNested", "false") == "true";
    filterUnique = el.attribute("filterUnique", "false") == "true";

    if (filterNested && filterUnique){
        stateInfo.setError(QString("Filter unique and filter nested cannot go together"));
        return;
    }

    results = el.attribute(RESULT_ATTR);
    sequence = el.attribute("sequence");
}

void GTest_FindTandemRepeatsTask::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }
    //this->getContext(this,"")
    //new DNAAlphabetRegistryImpl(
//    TaskResourceUsage* tru = AppContext::getTaskScheduler()->getTaskResources(NULL).constData();
    const DNAAlphabet* alph = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    seqObj = new DNASequence(QString("sequence"), sequence.toLatin1(), alph );
    if (seqObj == NULL){
        stateInfo.setError("can't find sequence1");
        return;
    }
    string = (char*)(seqObj->constData());

    int maxLen = sequence.length();
    if (minD == -1) {
        minD = -maxLen;
    }
    if (maxD == -1) {
        maxD = maxLen;
    }
    if (maxSize == 0) {
        maxSize = maxLen;
    }


    FindTandemsTaskSettings s;
    s.minPeriod = minSize;
    s.minRepeatCount = repeatCount;
    s.seqRegion = region;
    s.nThreads = 1;//todo: add to settings

    addSubTask( new TandemFinder(s, *seqObj) );
}

void GTest_FindTandemRepeatsTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }
    QList<Tandem> expectedResults;
    // load file with results
    QStringList resList = results.split(';', QString::SkipEmptyParts);
    foreach(const QString& result, resList){
        QStringList hit = result.split(',', QString::SkipEmptyParts);
        if (hit.size()!=3) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(result));
            return;
        }
        bool offsetConverted;
        Tandem tnd(hit[0].toInt(&offsetConverted), hit[2].toInt(), hit[1].toInt());
        if ( !offsetConverted || tnd.size == 0 || tnd.repeatLen==0) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(result));
            return;
        }
        expectedResults.append(tnd);
    }

    //check all subtasks
    TandemFinder* sub = qobject_cast<TandemFinder*>(this->getSubtasks()[0].data());
    QList<Tandem> calcResults = sub->getResults();
    if (expectedResults.size()!=calcResults.size()) {
        QString results("First results are:\n");
        for (int i=0, n = qMin(calcResults.size(),3); i < n; i++) {
            Tandem rc = calcResults[i];
            results.append(QString("%1 %2 %3\n").arg(rc.offset).arg(rc.size).arg(rc.repeatLen));
        }
        stateInfo.setError(QString("Results count not matched, num = %1, expected = %2\n%3").arg(calcResults.size()).arg(expectedResults.size()).arg(results));
        return;
    }
    qSort(expectedResults);
    qSort(calcResults);

    for (int i=0, n = expectedResults.size(); i < n; i++) {
        Tandem re = expectedResults[i];
        Tandem rc = calcResults[i];
        if (re.offset!=rc.offset || re.size != rc.size || re.repeatLen!=rc.repeatLen) {
            stateInfo.setError(QString("Results not matched, expected(%1, %2, %3), computed(%4, %5, %6)")
                .arg(re.offset).arg(re.size).arg(re.repeatLen)
                .arg(rc.offset).arg(rc.size).arg(rc.repeatLen) );
            return;
        }
    }

    delete seqObj;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


U2Region GTest_FindRealTandemRepeatsTask::parseRegion(const QString& n, const QDomElement& el) {
    U2Region res;
    QString v = el.attribute(n);
    if (v.isEmpty()) {
        return res;
    }
    int idx = v.indexOf("..");
    if (idx == -1 || idx+2 >= v.length()) {
        return res;
    }
    QString v1 = v.left(idx);
    QString v2 = v.mid(idx+2);
    int startPos = v1.toInt();
    int endPos = v2.toInt();
    if (startPos >= 0 && endPos > startPos) {
        res.startPos = startPos - 1;
        res.length = endPos - startPos + 1;
    }
    return res;
}

void GTest_FindRealTandemRepeatsTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    minD = el.attribute(MIND_ATTR, "-1").toInt();
    maxD = el.attribute(MAXD_ATTR, "-1").toInt();

    minSize = el.attribute("minSize", "1").toInt();
    repeatCount = el.attribute("repeatCount", "3").toInt();

    inverted = el.attribute("invert") == "true";
    reflect = el.attribute("reflect", "true") == "true";
    filterNested = el.attribute("filterNested", "false") == "true";
    filterUnique = el.attribute("filterUnique", "false") == "true";

    if (filterNested && filterUnique){
        stateInfo.setError(QString("Filter unique and filter nested cannot go together"));
        return;
    }

    results = el.attribute(RESULT_ATTR);
    if (results.isEmpty()) {
        stateInfo.setError(QString("Value not found '%1'").arg(RESULT_ATTR));
        return;
    }

    sequence = el.attribute("sequence");
    if (sequence.isEmpty()) {
        stateInfo.setError(QString("Value not found '%1'").arg("sequence"));
        return;
    }
}

void GTest_FindRealTandemRepeatsTask::prepare() {
    CHECK_OP(stateInfo, );
    U2SequenceObject * seqObj = getContext<U2SequenceObject>(this, sequence);
    if (seqObj == NULL){
        stateInfo.setError("can't find sequence1");
        return;
    }
    if (region.isEmpty()) {
        region = U2Region(0, seqObj->getSequenceLength());
    }

    int maxLen = seqObj->getSequenceLength();
    if (minD == -1) {
        minD = -maxLen;
    }
    if (maxD == -1) {
        maxD = maxLen;
    }

    FindTandemsTaskSettings s;
    s.minPeriod = minSize;
    s.minRepeatCount = repeatCount;
    s.seqRegion = region;
    s.nThreads = 1;//todo: add to settings

    U2OpStatusImpl os;
    DNASequence dna = seqObj->getWholeSequence(os);
    CHECK_OP_EXT(os, setError(os.getError()), );
    addSubTask( new TandemFinder(s, dna) );
}

void GTest_FindRealTandemRepeatsTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }
    QList<Tandem> expectedResults;
    // load file with results
    QString fname = env->getVar("COMMON_DATA_DIR") + "/" + results;
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stateInfo.setError(QString("Can't open results file %1").arg(fname));
        return;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList hit = line.split(' ', QString::SkipEmptyParts);
        if (hit.size()!=5) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(line));
            return;
        }
        bool h1ok, h2ok, h3ok;
        Tandem t(hit[0].toInt(&h1ok)-1, hit[3].toInt(&h2ok), hit[2].toInt(&h3ok));
        if (!h1ok || !h2ok || !h3ok) {
            stateInfo.setError(QString("Can't parse results line: %1").arg(line));
            return;
        }
        expectedResults.append(t);
    }
    file.close();

    qSort(expectedResults);

    //check all subtasks
    TandemFinder* sub = qobject_cast<TandemFinder*>(this->getSubtasks()[0].data());
    QList<Tandem> calcResults = sub->getResults();
    QMutableListIterator<Tandem> cIt(calcResults);
    QMutableListIterator<Tandem> eIt(expectedResults);
    while (cIt.hasNext() && eIt.hasNext()){
        Tandem er = eIt.peekNext();
        Tandem cr = cIt.peekNext();
        if (er<cr){
            eIt.next();
//            eIt.remove();
        }else if(cr<er){
            cIt.next();
            cIt.remove();
        }else{
            eIt.next();
            eIt.remove();
            cIt.next();
            cIt.remove();
        }
    }
    if (!expectedResults.isEmpty()){
        QString result("First of them:\n");
        Tandem rc = expectedResults[0];
        result.append(QString("%1 %2 %3\n").arg(rc.offset).arg(rc.size).arg(rc.repeatLen));
        stateInfo.setError(QString("Not all expected tandems found: total %1\n%2").arg(expectedResults.size()).arg(result));
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void GTest_SArrayBasedFindTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);


    QString buf = el.attribute(RESULT_ATTR);
    if (buf.isEmpty()) {
        stateInfo.setError(QString("Value not found: '%1'").arg(RESULT_ATTR));
        return;
    }

    QStringList results = buf.split(",");
    foreach (const QString& str, results ) {
        bool ok = false;
        int pos = str.toInt(&ok);
        if (!ok) {
            stateInfo.setError("Can't parse expected results");
            return;
        } else {
            expectedResults.append(pos);
        }
    }

    seqObjName = el.attribute(SEQUENCE);
    if (seqObjName.isEmpty()) {
        stateInfo.setError(QString("Value not found: '%1'").arg(SEQUENCE));
        return;
    }

    buf = el.attribute(MISMATCHES);
    bool ok = false;
    nMismatches = buf.toInt(&ok);
    if (!ok) {
        nMismatches = 0;
    }


    useBitMask = el.attribute(USE_BITMASK) == "true";

    query = el.attribute(QUERY);
    if (query.isEmpty()) {
        stateInfo.setError(QString("Value not found: '%1'").arg(QUERY));
        return;
    }



}

void GTest_SArrayBasedFindTask::cleanup() {
    wholeSeq = QByteArray();

    XmlTest::cleanup();
}

void GTest_SArrayBasedFindTask::prepare() {
    CHECK_OP(stateInfo, );

    U2SequenceObject * seqObj = getContext<U2SequenceObject>(this, seqObjName);
    if (seqObj == NULL){
        stateInfo.setError(QString("Can't find index sequence %1").arg(seqObjName));
        return;
    }
    DNAAlphabetType seqType = seqObj->getAlphabet()->getType();
    char unknownChar = seqType == DNAAlphabet_AMINO ? 'X' : seqType==DNAAlphabet_NUCL ? 'N' : '\0';

    const quint32* bitMask = NULL;
    int bitCharLen = 0;

    if (useBitMask) {
        bitCharLen = bt.getBitMaskCharBitsNum(seqType);
        bitMask = bt.getBitMaskCharBits(seqType);
    }

    int prefixSize = query.size();
    if (nMismatches > 0) {
        prefixSize = prefixSize / (nMismatches + 1);
    }

    wholeSeq = seqObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );
    index = new SArrayIndex(wholeSeq.constData(), seqObj->getSequenceLength(), prefixSize, stateInfo, unknownChar, bitMask, bitCharLen);

    if (hasError()) {
        return;
    }

    SArrayBasedSearchSettings s;
    s.query = query.toLatin1();
    s.useBitMask = useBitMask;
    s.bitMask = bitMask;
    s.nMismatches = nMismatches;
    s.bitMaskCharBitsNum = bitCharLen;
    s.unknownChar = unknownChar;
    findTask = new SArrayBasedFindTask(index, s);
    addSubTask( findTask );
}

void GTest_SArrayBasedFindTask::run()
{
    if (hasError() || isCanceled()) {
        return;
    }

    qSort(expectedResults);

    QList<int> calcResults = findTask->getResults();
    if ( expectedResults.size() != calcResults.size() ) {
        stateInfo.setError(QString("Results count do not match, num = %1, expected = %2")
            .arg(calcResults.size()).arg(expectedResults.size() ) );
        return;
    }

    qSort(calcResults);
    for (int i = 0, n = expectedResults.size(); i < n; i++) {
        int re = expectedResults[i];
        int rc = calcResults[i];
        if (re != rc) {
            stateInfo.setError(QString("Results not matched, expected %1, computed %2")
                .arg(re).arg(rc) );
            return;
        }
    }

}



//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

QList<XMLTestFactory*> RepeatFinderTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_FindSingleSequenceRepeatsTask::createFactory());
    res.append(GTest_FindTandemRepeatsTask::createFactory());
    res.append(GTest_FindRealTandemRepeatsTask::createFactory());
    res.append( GTest_SArrayBasedFindTask::createFactory() );
    return res;
}


} //namespace

