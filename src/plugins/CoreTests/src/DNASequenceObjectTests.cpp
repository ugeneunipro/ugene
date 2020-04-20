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

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DNASequenceObjectTests.h"

namespace U2 {

/* TRANSLATOR U2::GTest */

#define DOC_ATTR   "doc"
#define VALUE_ATTR  "value"
#define SORT_ATTR   "sort"
#define START_ATTR  "seqstart"
#define OBJ_ATTR    "obj"
#define SEQNAME_ATTR  "seqname"
#define QUALITY_ATTR "quality"
#define POSITION_ATTR "pos"

void GTest_DNASequenceSize::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    bool ok = false;
    seqSize = v.toInt(&ok);
    if (!ok) {
        failMissingValue(VALUE_ATTR);
    }

}

Task::ReportResult GTest_DNASequenceSize::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    int tempLength=mySequence->getSequenceLength();
    if (tempLength != seqSize){
        stateInfo.setError(QString("sequence size not matched: %1, expected %2 ").arg(tempLength).arg(seqSize));
    }
    return ReportResult_Finished;
}

void GTest_DNASequenceAlphabet::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    alphabetId = el.attribute(VALUE_ATTR);
    if (alphabetId.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

}

Task::ReportResult GTest_DNASequenceAlphabet::report() {
    GObject *obj = getContext<GObject>(this,objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(name));
        return ReportResult_Finished;
    }

    const DNAAlphabet *tempAlphabet = mySequence->getAlphabet();
    assert(tempAlphabet!=NULL);

    if (tempAlphabet->getId() != alphabetId){
        stateInfo.setError(QString("Alphabet not matched: %1, expected %2 ").arg(tempAlphabet->getId()).arg(alphabetId));
    }

    return ReportResult_Finished;
}

void GTest_DNASequencePart::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    subseq = el.attribute(VALUE_ATTR).toLatin1();
    if (subseq.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

    QString p = el.attribute(START_ATTR);
    if (p.isEmpty()) {
        failMissingValue(START_ATTR);
        return;
    }
    bool ok = false;
    startPos= p.toInt(&ok);
    if (!ok) {
        failMissingValue(START_ATTR);
    }
}

Task::ReportResult GTest_DNASequencePart::report() {
    GObject *obj = getContext<GObject>(this,objContextName);
    if (obj==NULL) {
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * objSequence = qobject_cast<U2SequenceObject*>(obj);
    if (objSequence==NULL) {
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    if (objSequence->getSequenceLength() < startPos + subseq.length()) {
        stateInfo.setError(QString("sequence size is less that region end: size=%1, region-end=%2, objectName=%3").arg(objSequence->getSequenceLength()).arg(startPos + subseq.length()).arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    QByteArray objSubSeq = objSequence->getSequenceData(U2Region(startPos, subseq.length()));
    if (!objSequence->getAlphabet()->isCaseSensitive()) {
        subseq = subseq.toUpper();
    }
    if (objSubSeq != subseq){
        stateInfo.setError(QString("region not matched: %1, expected %2").arg(objSubSeq.constData()).arg(subseq.constData()));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}
void GTest_DNASequenceAlphabetType::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    if (v == "DNAAlphabet_RAW"){
        alphabetType = DNAAlphabet_RAW;
        return;
    }
    if (v == "DNAAlphabet_NUCL"){
        alphabetType = DNAAlphabet_NUCL;
        return;
    }
    if (v == "DNAAlphabet_AMINO"){
        alphabetType = DNAAlphabet_AMINO;
        return;
    }
    stateInfo.setError(QString("alphabetType not set %1").arg(VALUE_ATTR));
    return;

}
Task::ReportResult GTest_DNASequenceAlphabetType::report() {
    GObject *obj = getContext<GObject>(this,objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    const DNAAlphabet *tempAlphabet = mySequence->getAlphabet();
    if( tempAlphabet->getType() != alphabetType){
        stateInfo.setError(QString("Alphabet type not matched: %1, expected %2").arg(tempAlphabet->getType()).arg(alphabetType));
    }
    return ReportResult_Finished;
}
void GTest_DNASequenceAlphabetId::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    alpId = el.attribute(VALUE_ATTR);
    if (alpId.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    return;

}
Task::ReportResult GTest_DNASequenceAlphabetId::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    const DNAAlphabet *tempAlphabet = mySequence->getAlphabet();
    if( tempAlphabet->getId() != alpId){
        stateInfo.setError(QString("Alphabet id not matched: %1 expected %2").arg(tempAlphabet->getId()).arg(alpId));
    }
    return ReportResult_Finished;
}

//----------------------------------------------------------
void GTest_DNAcompareSequencesNamesInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNAcompareSequencesNamesInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    GObject*obj=NULL;
    U2SequenceObject * mySequence = NULL;
    U2SequenceObject * mySequence2 = NULL;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        obj = objs.at(i);
        GObject *obj2 = objs2.at(i);

        if((obj->getGObjectType()== GObjectTypes::SEQUENCE)&&(obj2->getGObjectType() == GObjectTypes::SEQUENCE)){
            mySequence = qobject_cast<U2SequenceObject*>(obj);
            if(mySequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            mySequence2 = qobject_cast<U2SequenceObject*>(obj2);
            if(mySequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            if(mySequence->getGObjectName()!=mySequence2->getGObjectName()){
                stateInfo.setError(QString("Name of object in position %1 not matched: '%2' vs '%3'").arg(i)
                    .arg(mySequence->getGObjectName()).arg(mySequence2->getGObjectName()));
                return ReportResult_Finished;
            }
        }

    }

    if (objs.size() != objs2.size()) {
        QString error("Number of objects in doc mismatches: [%1=%2] vs [%3=%4]");
        error = error.arg(docContextName).arg(objs.size())
            .arg(secondDocContextName).arg(objs2.size());
        if (obj) {
            error += QString("\nLast good object: %1").arg(obj->getGObjectName());
        }
        stateInfo.setError(error);
    }

    return ReportResult_Finished;
}

//----------------------------------------------------------
void GTest_DNAcompareSequencesInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNAcompareSequencesInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    GObject*obj=NULL;
    U2SequenceObject * mySequence = NULL;
    U2SequenceObject * mySequence2 = NULL;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        obj = objs.at(i);
        GObject *obj2 = objs2.at(i);

        if((obj->getGObjectType()== GObjectTypes::SEQUENCE)&&(obj2->getGObjectType()== GObjectTypes::SEQUENCE)){
            mySequence = qobject_cast<U2SequenceObject*>(obj);
            if(mySequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            mySequence2 = qobject_cast<U2SequenceObject*>(obj2);
            if(mySequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            if(mySequence->getWholeSequenceData(stateInfo) != mySequence2->getWholeSequenceData(stateInfo)){
                CHECK_OP(stateInfo, ReportResult_Finished);
                stateInfo.setError(QString("Sequences of object in position %1 not matched").arg(i));
                return ReportResult_Finished;
            }
            CHECK_OP(stateInfo, ReportResult_Finished);
        }

    }

    if (objs.size() != objs2.size()) {
        QString error("Number of objects in doc mismatches: [%1=%2] vs [%3=%4]");
        error = error.arg(docContextName).arg(objs.size())
            .arg(secondDocContextName).arg(objs2.size());
        if (obj) {
            error += QString("\nLast good object: %1").arg(obj->getGObjectName());
        }
        stateInfo.setError(error);
    }

    return ReportResult_Finished;
}

//----------------------------------------------------------
void GTest_DNAcompareSequencesAlphabetsInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNAcompareSequencesAlphabetsInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    GObject*obj=NULL;
    U2SequenceObject * mySequence = NULL;
    U2SequenceObject * mySequence2 = NULL;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        obj = objs.at(i);
        GObject *obj2 = objs2.at(i);

        if((obj->getGObjectType()== GObjectTypes::SEQUENCE)&&(obj2->getGObjectType()== GObjectTypes::SEQUENCE)){
            mySequence = qobject_cast<U2SequenceObject*>(obj);
            if(mySequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            mySequence2 = qobject_cast<U2SequenceObject*>(obj2);
            if(mySequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
            if(mySequence->getAlphabet()!=mySequence2->getAlphabet()){
                stateInfo.setError(QString("Alphabets of object in position %1 not matched").arg(i));
                return ReportResult_Finished;
            }

        }

    }

    if (objs.size() != objs2.size()) {
        QString error("Number of objects in doc mismatches: [%1=%2] vs [%3=%4]");
        error = error.arg(docContextName).arg(objs.size())
            .arg(secondDocContextName).arg(objs2.size());
        if (obj) {
            error += QString("\nLast good object: %1").arg(obj->getGObjectName());
        }
        stateInfo.setError(error);
    }

    return ReportResult_Finished;
}

//-----------------------------------------------------------------------------
void GTest_DNAMulSequenceAlphabetId::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    alpId = el.attribute(VALUE_ATTR);
    if (alpId.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    return;

}
Task::ReportResult GTest_DNAMulSequenceAlphabetId::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence= qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(myMSequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    const DNAAlphabet *tempAlphabet = myMSequence->getAlphabet();
    if( tempAlphabet->getId() != alpId){
        stateInfo.setError(QString("Alphabet id not matched: %1 expected %2").arg(tempAlphabet->getId()).arg(alpId));
    }
    return ReportResult_Finished;
}

//-----------------------------------------------------------------------------

void GTest_DNAMulSequenceSize::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    bool ok = false;
    seqSize = v.toInt(&ok);
    if (!ok) {
        failMissingValue(VALUE_ATTR);
    }

}

Task::ReportResult GTest_DNAMulSequenceSize::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence= qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(myMSequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    int tempLength=myMSequence->getLength();
    if(tempLength != seqSize){
        stateInfo.setError(QString("sequence size not matched: %1, expected %2 ").arg(tempLength).arg(seqSize));
    }
    return ReportResult_Finished;
}

//-----------------------------------------------------------------------------

void GTest_DNAMulSequencePart::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    subseq = el.attribute(VALUE_ATTR).toLatin1();
    if (subseq.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

    QString p = el.attribute(START_ATTR);
    if (p.isEmpty()) {
        failMissingValue(START_ATTR);
        return;
    }
    bool ok = false;
    startPos= p.toInt(&ok);
    if (!ok) {
        failMissingValue(START_ATTR);
    }

    seqName = el.attribute(SEQNAME_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQNAME_ATTR);
        return;
    }

}

Task::ReportResult GTest_DNAMulSequencePart::report() {
    GObject *obj = getContext<GObject>(this,objContextName);
    if (obj==NULL) {
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence= qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(myMSequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    if (myMSequence->getLength() < startPos + subseq.length()) {
        stateInfo.setError(QString("sequence size is less that region end: size=%1, region-end=%").arg(myMSequence->getLength(), startPos + subseq.length()));
        return ReportResult_Finished;
    }

    if (!myMSequence->getAlphabet()->isCaseSensitive()) {
        subseq = subseq.toUpper();
    }
    bool ok_flag=false;
    U2OpStatus2Log os;
    const MultipleSequenceAlignment ma = myMSequence->getMultipleAlignment();
    foreach(const MultipleSequenceAlignmentRow& myItem , ma->getMsaRows()){
        if (myItem->getName() == seqName){
            ok_flag=true;
            QByteArray objSubSeq = myItem->mid(startPos, subseq.length(), os)->toByteArray(os, subseq.length());
            if (objSubSeq != subseq){
                stateInfo.setError(QString("region not matched: %1, expected %2").arg(objSubSeq.constData()).arg(subseq.constData()));
                return ReportResult_Finished;
            }
        }
    }
    if(!ok_flag){
        stateInfo.setError(QString("no Sequence name: %1").arg(seqName));
    }
    return ReportResult_Finished;

}

//-----------------------------------------------------------------------------

void GTest_DNAMulSequenceName::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    seqName = el.attribute(SEQNAME_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQNAME_ATTR);
        return;
    }

}

Task::ReportResult GTest_DNAMulSequenceName::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if (obj == NULL) {
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    bool ok_flag = false;
    const MultipleSequenceAlignment ma = myMSequence->getMultipleAlignment();
    foreach(const MultipleSequenceAlignmentRow& myItem, ma->getMsaRows()){
        if (myItem->getName() == seqName){
            ok_flag = true;
            break;
        }
    }
    if (!ok_flag){
        stateInfo.setError(QString("no Sequence name: %1").arg(seqName));
    }
    return ReportResult_Finished;

}


//-----------------------------------------------------------------------------

void GTest_DNAMulSequenceQuality::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    seqName = el.attribute(SEQNAME_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQNAME_ATTR);
        return;
    }

    expectedQuality = el.attribute(QUALITY_ATTR).toLatin1();
    if (expectedQuality.isEmpty()) {
        failMissingValue(QUALITY_ATTR);
        return;
    }


}

Task::ReportResult GTest_DNAMulSequenceQuality::report() {
    GObject *obj = getContext<GObject>(this,objContextName);
    if (obj==NULL) {
        stateInfo.setError(QString("wrong object name: %1").arg(objContextName));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence= qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(myMSequence==NULL){
        stateInfo.setError(QString("Can not cast to alignment from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    bool ok_flag=false;
    foreach(const MultipleSequenceAlignmentRow& myItem , myMSequence->getMsa()->getMsaRows()){
        if (myItem->getName() == seqName){
            ok_flag = true;
            //QByteArray qualityCodes = myItem.getCoreQuality().qualCodes;
            //if (qualityCodes != expectedQuality){
            //    stateInfo.setError(
            //        QString("Quality scores are not valid! The score is %1, expected %2").arg(qualityCodes.constData()).arg(expectedQuality.constData())
            //        );
            //    return ReportResult_Finished;
            //}
        }
    }
    if(!ok_flag){
        stateInfo.setError(QString("Sequence %1 is not found in the alignment").arg(seqName));
    }
    return ReportResult_Finished;

}

//-----------------------------------------------------------------------------

void GTest_DNASequencInMulSequence::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    bool ok = false;
    seqInMSeq = v.toInt(&ok);
    if (!ok) {
        failMissingValue(VALUE_ATTR);
    }

}

Task::ReportResult GTest_DNASequencInMulSequence::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    MultipleSequenceAlignmentObject* myMSequence= qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(myMSequence==NULL){
        stateInfo.setError(QString("can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    int tempSize=myMSequence->getNumRows();
    if (tempSize != seqInMSeq){
        stateInfo.setError(QString("numbers of Sequence not match: %1, expected %2 ").arg(tempSize).arg(seqInMSeq));
    }
    return ReportResult_Finished;
}
//----------------------------------------------------------
void GTest_DNAcompareMulSequencesInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

    sortValue = el.attribute(SORT_ATTR);
}

Task::ReportResult GTest_DNAcompareMulSequencesInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    MultipleSequenceAlignmentObject * myMSequence = 0;
    MultipleSequenceAlignmentObject * myMSequence2 = 0;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        GObject *obj = objs.at(i);
        GObject *obj2 = objs2.at(i);

        if(obj->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
            if(myMSequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
        if(obj2->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence2 = qobject_cast<MultipleSequenceAlignmentObject*>(obj2);
            if(myMSequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
        if (myMSequence->getLength() != myMSequence2->getLength()) {
           stateInfo.setError(QString("sequences size not matched: size1=%1, size2=%2").arg(myMSequence->getLength()).arg(myMSequence2->getMultipleAlignment()->getLength()));
           return ReportResult_Finished;
        }

        MultipleSequenceAlignment one = myMSequence->getMultipleAlignment();
        MultipleSequenceAlignment two = myMSequence2->getMultipleAlignment();
        const QList <MultipleSequenceAlignmentRow> alignedSeqs1 = one->getMsaRows();
        const QList <MultipleSequenceAlignmentRow> alignedSeqs2 = two->getMsaRows();

        if( alignedSeqs1.size() != alignedSeqs2.size() ) {
            stateInfo.setError(QString( "different_sequences_number_in_msa_in_%1_object" ).arg( i ));
            return ReportResult_Finished;
        }

        if (sortValue == "true") {
            one->sortRowsByName();
            two->sortRowsByName();
        }

        int seqSz = alignedSeqs1.size();
        for(int n = 0; n < seqSz; n++ ) {
            const MultipleSequenceAlignmentRow &myItem1 = alignedSeqs1.at(i);
            const MultipleSequenceAlignmentRow &myItem2 = alignedSeqs2.at(i);
            if (myItem1->getName() != myItem2->getName()){
                stateInfo.setError(QString("names of regions in position %1 not matched: %2, expected %3").arg(n).arg(myItem2->getName()).arg(myItem1->getName()));
                return ReportResult_Finished;
            }
            if (*myItem1 != *myItem2) {
                stateInfo.setError(QString( "sequence_#%1_in_msa_in_object_#%2_not_matched" ).arg( n ).arg( i ));
            }
        }
    }

    return ReportResult_Finished;
}


//----------------------------------------------------------
void GTest_DNAcompareMulSequencesNamesInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNAcompareMulSequencesNamesInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    GObject*obj=NULL;
    GObject*obj2=NULL;
    MultipleSequenceAlignmentObject * myMSequence = 0;
    MultipleSequenceAlignmentObject * myMSequence2 = 0;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        obj = objs.at(i);
        obj2 = objs2.at(i);

        if(obj->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
            if(myMSequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
        if(obj2->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence2 = qobject_cast<MultipleSequenceAlignmentObject*>(obj2);
            if(myMSequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
////////////////////////////////////////
     if (myMSequence->getLength() != myMSequence2->getLength()) {
        stateInfo.setError(QString("sequences size not matched: size1=%1, size2=%").arg(myMSequence->getLength(), myMSequence2->getMultipleAlignment()->getLength()));
        return ReportResult_Finished;
    }
    const MultipleSequenceAlignment one = myMSequence->getMultipleAlignment();
    const MultipleSequenceAlignment two = myMSequence2->getMultipleAlignment();
    const QList<MultipleSequenceAlignmentRow> &myQList1 = one->getMsaRows();
    const QList<MultipleSequenceAlignmentRow> &myQList2 = two->getMsaRows();

    for (int n = 0; (n != myQList1.size()) || (n != myQList2.size()); n++) {
        const MultipleSequenceAlignmentRow &myItem1 = myQList1.at(i);
        const MultipleSequenceAlignmentRow &myItem2 = myQList2.at(i);
        if (myItem1->getName() != myItem2->getName()) {
            stateInfo.setError(QString("names of regions in position %1 not matched: %2, expected %3").arg(n).arg(myItem2->getName()).arg(myItem1->getName()));
            return ReportResult_Finished;
        }
    }
 //////////////////////////////////////////////////////////
    }

    if(obj!=objs.last() ){
        stateInfo.setError(QString("number of objects in document not matches: %1").arg(obj2->getGObjectName()));
        return ReportResult_Finished;
    }
    if(obj2!=objs2.last()){
        stateInfo.setError(QString("number of objects in document not matches: %1").arg(obj2->getGObjectName()));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}
//----------------------------------------------------------
void GTest_DNASequenceQualityScores::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    qualityScores = el.attribute(QUALITY_ATTR).toLatin1();
    if (qualityScores.isEmpty()) {
        failMissingValue(QUALITY_ATTR);

    }
}

Task::ReportResult GTest_DNASequenceQualityScores::report() {

    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("Can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    const DNAQuality& quality = mySequence->getQuality();
    if (quality.isEmpty()) {
        stateInfo.setError("Sequence doesn't have quality scores");
        return ReportResult_Finished;
    }

    if (quality.qualCodes != qualityScores) {
        stateInfo.setError( QString("Quality scores are not valid! The score is %1, expected %2").arg(quality.qualCodes.constData()).arg(qualityScores.constData()));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

//----------------------------------------------------------

void GTest_DNASequenceQualityValue::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString buf;
    bool ok = false;
    buf = el.attribute(POSITION_ATTR).toLatin1();
    if (buf.isEmpty()) {
        failMissingValue(POSITION_ATTR);
    }

    pos = buf.toInt(&ok);
    if (!ok) {
        setError("Failed to parse sequence position");
        return;
    }

    ok = false;
    buf = el.attribute(VALUE_ATTR).toLatin1();
    if (buf.isEmpty()) {
        failMissingValue(VALUE_ATTR);
    }

    expectedVal = buf.toInt(&ok);
    if (!ok) {
        setError("Failed to parse expected value");
        return;
    }

}

Task::ReportResult GTest_DNASequenceQualityValue::report() {

    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("Can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    const DNAQuality& quality = mySequence->getQuality();
    if ( (pos < 0) || (pos > quality.qualCodes.count() - 1 )  ) {
        stateInfo.setError(QString("Quality scores doesn't have position %1").arg(pos));
        return ReportResult_Finished;
    }

    int val = quality.getValue(pos);

    if (val != expectedVal) {
        stateInfo.setError( QString("Quality score values do not match! The score is %1, expected %2").arg(val).arg(expectedVal));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

//----------------------------------------------------------
void GTest_CompareDNASequenceQualityInTwoObjects::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    doc1CtxName = el.attribute("doc1");
    if (doc1CtxName.isEmpty()) {
        failMissingValue("doc1");
        return;
    }

    doc2CtxName = el.attribute("doc2");
    if (doc2CtxName.isEmpty()) {
        failMissingValue("doc2");
        return;
    }
}

static U2SequenceObject * getSeqObj( Document * doc ) {
    if( doc == NULL ) {
        return NULL;
    }

    QList<GObject*> seqObjs = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if( seqObjs.isEmpty() ) {
        return NULL;
    }
    return qobject_cast<U2SequenceObject*>(seqObjs.first());
}

Task::ReportResult GTest_CompareDNASequenceQualityInTwoObjects::report() {
    U2SequenceObject * seq1Obj = getSeqObj(getContext<Document>(this, doc1CtxName));
    if(seq1Obj == NULL) {
        setError(QString("Cannot find sequence object at '%1' document").arg(doc1CtxName));
        return ReportResult_Finished;
    }

    U2SequenceObject * seq2Obj = getSeqObj(getContext<Document>(this, doc2CtxName));
    if(seq2Obj == NULL) {
        setError(QString("Cannot find sequence object at '%1' document").arg(doc2CtxName));
        return ReportResult_Finished;
    }

    const DNAQuality& quality1 = seq1Obj->getQuality();
    const DNAQuality& quality2 = seq2Obj->getQuality();

    if( quality1.type != quality2.type ) {
        setError(QString("quality types not matched"));
        return ReportResult_Finished;
    }
    if( quality1.qualCodes != quality2.qualCodes ) {
        setError(QString("quality codes not matched"));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

//----------------------------------------------------------
void GTest_DNAcompareMulSequencesAlphabetIdInTwoObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    secondDocContextName = el.attribute(VALUE_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNAcompareMulSequencesAlphabetIdInTwoObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, secondDocContextName);
    if (doc2 == NULL) {
        stateInfo.setError(QString("document not found %1").arg(secondDocContextName));
        return ReportResult_Finished;
    }

    const QList<GObject*>& objs = doc->getObjects();
    const QList<GObject*>& objs2 = doc2->getObjects();
    GObject*obj=NULL;
    MultipleSequenceAlignmentObject * myMSequence = 0;
    MultipleSequenceAlignmentObject * myMSequence2 = 0;

    for(int i=0;(i!=objs.size())&&(i!=objs2.size());i++){
        obj = objs.at(i);
        GObject *obj2 = objs2.at(i);

        if(obj->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
            if(myMSequence==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
        if(obj2->getGObjectType()== GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT){
            myMSequence2 = qobject_cast<MultipleSequenceAlignmentObject*>(obj2);
            if(myMSequence2==NULL){
                stateInfo.setError(QString("can't cast to sequence from: %1 in position %2").arg(obj2->getGObjectName()).arg(i));
                return ReportResult_Finished;
            }
        }
////////////////////////////////////////
        const DNAAlphabet *tempAlphabet = myMSequence->getAlphabet();
        const DNAAlphabet *tempAlphabet2 = myMSequence2->getAlphabet();
        if (tempAlphabet->getId() != tempAlphabet2->getId()) {
            stateInfo.setError(QString("sequences alphabets not matched: alphabet1=%1, alphabet2=%").arg(tempAlphabet->getId(),tempAlphabet2->getId()));
            return ReportResult_Finished;
        }
//////////////////////////////////////////////////////////
    }

    if (objs.size() != objs2.size()) {
        QString error("Number of objects in doc mismatches: [%1=%2] vs [%3=%4]");
        error = error.arg(docContextName).arg(objs.size())
            .arg(secondDocContextName).arg(objs2.size());
        if (obj) {
            error += QString("\nLast good object: %1").arg(obj->getGObjectName());
        }
        stateInfo.setError(error);
    }

    return ReportResult_Finished;
}

//-----------------------------------------------------------------------------
QList<XMLTestFactory*> DNASequenceObjectTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_DNASequenceSize::createFactory());
    res.append(GTest_DNASequenceAlphabet::createFactory());
    res.append(GTest_DNASequencePart::createFactory());
    res.append(GTest_DNASequenceAlphabetType::createFactory());
    res.append(GTest_DNASequenceAlphabetId::createFactory());
    res.append(GTest_DNASequenceQualityScores::createFactory());
    res.append(GTest_CompareDNASequenceQualityInTwoObjects::createFactory());
    res.append(GTest_DNASequenceQualityValue::createFactory());
    res.append(GTest_DNAMulSequenceAlphabetId::createFactory());
    res.append(GTest_DNAMulSequenceSize::createFactory());
    res.append(GTest_DNAMulSequencePart::createFactory());
    res.append(GTest_DNASequencInMulSequence::createFactory());
    res.append(GTest_DNAcompareSequencesNamesInTwoObjects::createFactory());
    res.append(GTest_DNAcompareSequencesInTwoObjects::createFactory());
    res.append(GTest_DNAcompareSequencesAlphabetsInTwoObjects::createFactory());
    res.append(GTest_DNAcompareMulSequencesInTwoObjects::createFactory());
    res.append(GTest_DNAMulSequenceQuality::createFactory());
    res.append(GTest_DNAMulSequenceName::createFactory());
    res.append(GTest_DNAcompareMulSequencesNamesInTwoObjects::createFactory());
    res.append(GTest_DNAcompareMulSequencesAlphabetIdInTwoObjects::createFactory());
    return res;
}



}//namespace
