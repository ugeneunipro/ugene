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

#include "DNATranslationImplTests.h"
#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/DNAAlphabetRegistryImpl.h>

namespace U2 {

/* TRANSLATOR U2::GTest */

#define VALUE_ATTR   "value"
#define START_ATTR   "seqstart"
#define END_ATTR     "seqend"
#define OBJ_ATTR     "obj"
//---------------------------------------------------------------------
void GTest_DNATranslation3to1Test::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(START_ATTR);
    if (v.isEmpty()) {
        failMissingValue(START_ATTR);
        return;
    }
    bool ok = false;
    strFrom = v.toInt(&ok);
    if (!ok) {
        failMissingValue(START_ATTR);
    }

    QString e = el.attribute(END_ATTR);
    if (e.isEmpty()) {
        failMissingValue(END_ATTR);
        return;
    }
    ok = false;
    strTo = e.toInt(&ok);
    if (!ok) {
        failMissingValue(END_ATTR);
    }

    stringValue = el.attribute(VALUE_ATTR);
    if (stringValue.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DNATranslation3to1Test::report() {
    GObject *obj = getContext<GObject>(this, objContextName);
    if (obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(QString("error can't cast to sequence from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }
    if(!(mySequence->getAlphabet()->isNucleic())){
        stateInfo.setError(QString("error Alphabet is not Nucleic: %1").arg(mySequence->getAlphabet()->getId()));
        return ReportResult_Finished;
    }

    DNATranslation* aminoTransl = 0;

    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();

    QList<DNATranslation*> aminoTs = tr->lookupTranslation(mySequence->getAlphabet(), DNATranslationType_NUCL_2_AMINO);
    if (!aminoTs.empty()) {
        aminoTransl = tr->getStandardGeneticCodeTranslation(mySequence->getAlphabet());
    }
    int tempValue;
    if(strTo == -1){
        tempValue=-1;
    } else{
        tempValue=(strTo-strFrom+1);
    }
    QByteArray myByteArray = mySequence->getSequenceData(U2Region(strFrom,tempValue));
    QByteArray rezult(myByteArray.length() / 3, 0);
    int n = aminoTransl->translate(myByteArray, myByteArray.length(), rezult.data(), rezult.length());
    assert(n == rezult.length()); Q_UNUSED(n);

    if(rezult != stringValue.toLatin1()){
        stateInfo.setError(QString("translated sequence not matched: %1, expected %2 ").arg(rezult.data()).arg(stringValue));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

//---------------------------------------------------------------------
QList<XMLTestFactory*> DNATranslationImplTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_DNATranslation3to1Test::createFactory());

    return res;
}

}
