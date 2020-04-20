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

#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include "FindAlgorithmTests.h"

namespace U2 {

//musthave attributes
#define DOC_ATTR                    "document"
#define SEQUENCE_ATTR               "sequence"
#define STRAND_ATTR                 "strand"
#define PATTERN_ATTR                "pattern"
#define RANGE_ATTR                  "range"
#define MAX_ERR_ATTR                "max_error"
#define ALGORITHM_ATTR              "algorithm"
#define EXPECTED_ATTR               "expected"
//flags
#define SOURCE_ATTR                 "source"
#define AMBIG_ATTR                  "ambig"
#define MAXLEN_ATTR                 "max_len"

#define CIRCULAR_LABEL              "circular"

U2Region stringToRegion(const QString& regionStr) {
    int region[2];
    QStringList regStrList = regionStr.split("..", QString::SkipEmptyParts);
    if (regStrList.size() != 2) {
        return U2Region();
    }
    bool ok;
    for (int i = 0; i < 2; i++) {
        region[i] = regStrList[i].toInt(&ok);
        if(!ok){
            return U2Region();
        }
    }
    return U2Region(region[0], region[1] - region[0]);
}

void GTest_FindAlgorithmTest::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);
    QString buf = el.attribute(STRAND_ATTR);
    if(buf.isEmpty()){
        stateInfo.setError(GTest::tr("value not set %1").arg(STRAND_ATTR));
        return;
    }
    if(buf == "direct"){
        settings.strand = FindAlgorithmStrand_Direct;
    }else if(buf == "both"){
        settings.strand = FindAlgorithmStrand_Both;
    }else if(buf == "complement"){
        settings.strand = FindAlgorithmStrand_Complement;
    }else{
        stateInfo.setError(GTest::tr("value for %1 is incorrect").arg(STRAND_ATTR));
        return;
    }

    buf = el.attribute(PATTERN_ATTR);
    if(buf.isEmpty()){
        stateInfo.setError(GTest::tr("value not set %1").arg(PATTERN_ATTR));
        return;
    }
    settings.pattern = buf.toLatin1();

    buf = el.attribute(SOURCE_ATTR);
    if(buf == "translation"){
        translatetoAmino = true;
    }else{
        translatetoAmino = false;
    }

    buf = el.attribute(DOC_ATTR);
    if(buf.isEmpty()){
        stateInfo.setError(GTest::tr("value not set %1").arg(DOC_ATTR));
        return;
    }
    docName = buf;

    buf = el.attribute(SEQUENCE_ATTR);
    if(buf.isEmpty()){
        stateInfo.setError(GTest::tr("value not set %1").arg(SEQUENCE_ATTR));
        return;
    }
    sequenceName = buf;


    buf = el.attribute(RANGE_ATTR);
    QStringList regionStr = buf.split("..", QString::SkipEmptyParts);
    bool ok;
    int region[2];
    if(regionStr.size() != 2){
        stateInfo.setError(GTest::tr("value incorrect for %1").arg(RANGE_ATTR));
        return;
    }
    for(int i = 0; i < 2;i++){
        region[i] = regionStr[i].toInt(&ok);
        if(!ok){
            stateInfo.setError(GTest::tr("value incorrect for %1").arg(RANGE_ATTR));
            return;
        }
    }

    settings.searchRegion = U2Region(region[0],region[1] - region[0]);

    buf = el.attribute(MAX_ERR_ATTR);
    settings.maxErr = buf.toInt(&ok);
    if(!ok){
        stateInfo.setError(GTest::tr("value incorrect for %1").arg(MAX_ERR_ATTR));
        return;
    }
    if(settings.maxErr >= settings.pattern.length()){
        stateInfo.setError(GTest::tr("%1 attribute value greater or equal pattern length").arg(MAX_ERR_ATTR));
        return;
    }


    buf = el.attribute(ALGORITHM_ATTR);
    if(buf.isEmpty()){
        stateInfo.setError(GTest::tr("value not set %1").arg(ALGORITHM_ATTR));
        return;
    }
    if (buf == "exact") {
        settings.patternSettings = FindAlgorithmPatternSettings_Exact;
    } else if(buf == "subst"){
        settings.patternSettings = FindAlgorithmPatternSettings_Subst;
    }else if(buf == "insdel"){
        settings.patternSettings = FindAlgorithmPatternSettings_InsDel;
    }
    else if(buf == "regexp"){
        settings.patternSettings = FindAlgorithmPatternSettings_RegExp;
        settings.maxRegExpResultLength =  el.attribute(MAXLEN_ATTR).toInt(&ok);
        if(!ok){
            stateInfo.setError(GTest::tr("value incorrect for %1").arg(MAXLEN_ATTR));
            return;
        }
    }
    else{
        stateInfo.setError(GTest::tr("value for %1 incorrect").arg(ALGORITHM_ATTR));
    }

    buf = el.attribute(AMBIG_ATTR);
    if(buf == "true"){
        settings.useAmbiguousBases = true;
    }else{
        settings.useAmbiguousBases = false;
    }

    buf = el.attribute(EXPECTED_ATTR);
    QStringList splittedToRegions = buf.split(";", QString::SkipEmptyParts);
    U2Region r;
    foreach(QString regStr, splittedToRegions){

        if (regStr.startsWith(QString(CIRCULAR_LABEL))) {
            regStr.chop(1);
            regStr.remove(QString(CIRCULAR_LABEL) + "(");

            QStringList regList = regStr.split(",", QString::SkipEmptyParts);
            r = U2Region();
            foreach (QString reg, regList) {
                U2Region regionPart = stringToRegion(reg);
                if (regionPart.startPos > r.startPos) {
                    r.startPos = regionPart.startPos;
                }
                r.length += regionPart.length;
            }
        } else {
            r = stringToRegion(regStr);
        }
        if (r.isEmpty()) {
            stateInfo.setError(GTest::tr("value incorrect for %1").arg(EXPECTED_ATTR));
        }
        expectedResults.append(r);
    }
}

void GTest_FindAlgorithmTest::prepare(){
    Document* doc = getContext<Document>(this, docName);
    if (doc == NULL) {
        stateInfo.setError(GTest::tr("context not found %1").arg(docName));
        return;
    }
    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (list.size() == 0) {
        stateInfo.setError(GTest::tr("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE));
        return;
    }

    foreach(GObject *go, list){
       if(go->getGObjectName() == sequenceName){
           se = qobject_cast<U2SequenceObject *>(go);
           break;
       }
    }

    U2OpStatusImpl os;
    settings.sequence = se->getWholeSequenceData(os);
    SAFE_POINT_OP(os, );
    settings.searchIsCircular = se->isCircular();
    settings.complementTT = GObjectUtils::findComplementTT(se->getAlphabet());
    if(translatetoAmino){
        settings.proteinTT = GObjectUtils::findAminoTT(se, false);
    }
    t = new FindAlgorithmTask(settings);
    addSubTask(t);
}


Task::ReportResult GTest_FindAlgorithmTest::report(){
    QList<FindAlgorithmResult> actualResults = t->popResults();
    if(actualResults.size() != expectedResults.size()){
        stateInfo.setError(GTest::tr("Expected and actual result sizes are different: %1 , %2")
            .arg(expectedResults.size())
            .arg(actualResults.size()));
        return ReportResult_Finished;
    }

    for(int i = 0; i < actualResults.size(); i++){
        if(!expectedResults.contains(actualResults[i].region)){
            stateInfo.setError(GTest::tr("Expected and actual regions are different: %1..%2 , %3..%4")
                .arg(expectedResults[i].startPos)
                .arg(expectedResults[i].endPos())
                .arg(actualResults[i].region.startPos)
                .arg(actualResults[i].region.endPos()));
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

///////////////////////////////////////////////////////////////////////////////////////////
QList<XMLTestFactory*> FindAlgorithmTests::createTestFactories(){
    QList<XMLTestFactory*> res;
    res.append(GTest_FindAlgorithmTest::createFactory());

    return res;
}

}
