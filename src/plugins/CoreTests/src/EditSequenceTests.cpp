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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2AlphabetUtils.h>

#include "EditSequenceTests.h"

namespace U2{
#define DOC_NAME_ATTR "doc_name"
#define SEQ_NAME_ATTR "seq_name"
#define START_POS_ATTR "start_pos"
#define INSERTED_SEQ_ATTR "inserted_sequence"
#define ANNOTATION_NAME_ATTR "annotation_name"
#define EXPECTED_SEQ_ATTR "expected_sequence"
#define EXPECTED_REGIONS_ATTR "expected_regions"
#define EXPECTED_ANNOTATION_STRATEGY_ATTR "annotation_processing"
#define LENGTH_ATTR "length"


void GTest_AddPartToSequenceTask::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);
    QString buf;
    buf = el.attribute(DOC_NAME_ATTR);
    if (!buf.isEmpty()){
        docName=buf;
    }
    buf = el.attribute(SEQ_NAME_ATTR);
    if (!buf.isEmpty()){
        seqName=buf;
    }
    buf = el.attribute(START_POS_ATTR);
    if (!buf.isEmpty()){
        startPos=buf.toInt();
    }
    buf = el.attribute(INSERTED_SEQ_ATTR);
    if (!buf.isEmpty()){
        insertedSequence=buf;
    }
    buf = el.attribute(ANNOTATION_NAME_ATTR);
    if (!buf.isEmpty()){
        annotationName=buf;
    }
    buf = el.attribute(EXPECTED_SEQ_ATTR);
    if (!buf.isEmpty()){
        expectedSequence=buf;
    }
    buf = el.attribute(EXPECTED_REGIONS_ATTR);
    if (!buf.isEmpty()){
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts)){
            expectedRegions.append(U2Region(str.split(',')[0].toInt(),str.split(',')[1].toInt()-str.split(',')[0].toInt()));
        }
        //expectedRegions=buf;
    }else{
        expectedRegions.clear();
    }

    buf = el.attribute(EXPECTED_ANNOTATION_STRATEGY_ATTR);
    if(buf.toLower() == "remove"){
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }else if(buf.toLower() == "split_joined"){
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Joined;
    }else if(buf.toLower() == "split_separate"){
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate;
    }else{
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    }
}

void GTest_AddPartToSequenceTask::prepare(){
    Document* loadedDocument = getContext<Document>(this, docName);
    if (loadedDocument == NULL) {
        stateInfo.setError(GTest::tr("context not found %1").arg(docName));
        return;
    }
    dnaso=(U2SequenceObject*)loadedDocument->findGObjectByName(seqName);
    if (dnaso==NULL){
        stateInfo.setError(GTest::tr("Sequence %1 not found").arg(seqName));
    }else{
        QList<Document*> docList;
        docList.append(loadedDocument);
        DNASequence seqToIns("test", insertedSequence.toLatin1(), U2AlphabetUtils::findBestAlphabet(insertedSequence.toLatin1()));
        Task* t = new ModifySequenceContentTask(loadedDocument->getDocumentFormatId(), dnaso, U2Region(startPos, 0), seqToIns, false, strat);
        addSubTask(t);
    }
}

Task::ReportResult GTest_AddPartToSequenceTask::report(){
    if (dnaso->getSequenceLength()!=expectedSequence.size()) {
        stateInfo.setError(GTest::tr("Length of sequence is incorrect. Expected:%2, but Actual:%1")
            .arg(dnaso->getSequenceLength())
            .arg(expectedSequence.length()));
        return ReportResult_Finished;
    }
    if (QString::compare(dnaso->getWholeSequenceData(stateInfo), expectedSequence, Qt::CaseInsensitive)!=0)//may be refactor this place
    {
        CHECK_OP(stateInfo, ReportResult_Finished);
        stateInfo.setError(GTest::tr("Sequence is incorrect. Expected:%1, but Actual:%2")
            .arg((QString)(dnaso->getWholeSequenceData(stateInfo)))
            .arg(expectedSequence));
        return ReportResult_Finished;
    }
    CHECK_OP(stateInfo, ReportResult_Finished);
    if (annotationName.length()!=0)
    {
        if(strat != U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate){
            Document* loadedDocument = getContext<Document>(this, docName);
            QList<GObject*> annotationTablesList = loadedDocument->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
            foreach(GObject *table, annotationTablesList){
                AnnotationTableObject *ato = dynamic_cast<AnnotationTableObject *>(table);
                foreach (Annotation *curentAnnotation, ato->getAnnotations()) {
                    if (curentAnnotation->getName() == annotationName) {
                        int i = 0;
                        if (curentAnnotation->getRegions().size() != expectedRegions.size()) {
                            stateInfo.setError(GTest::tr("Regions is incorrect. Expected size:%1 Actual size:%2")
                                .arg(expectedRegions.size()).arg(curentAnnotation->getRegions().size()));
                            break;
                        }
                        foreach (const U2Region &curRegion, curentAnnotation->getRegions()) {
                            if (curRegion != expectedRegions.at(i)) {
                                stateInfo.setError(GTest::tr("Regions is incorrect. Expected:%3,%4, but Actual:%1,%2")
                                    .arg(curRegion.startPos).arg(curRegion.endPos())
                                    .arg(expectedRegions.at(i).startPos).arg(expectedRegions.at(i).endPos()));
                            }
                            i++;
                        }
                        return ReportResult_Finished;
                    }
                }
            }
        } else {
            Document *loadedDocument = getContext<Document>(this, docName);
            QList<GObject *> annotationTablesList = loadedDocument->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
            foreach (GObject *table, annotationTablesList) {
                AnnotationTableObject *ato = dynamic_cast<AnnotationTableObject *>(table);
                foreach (Annotation *curentAnnotation, ato->getAnnotations()) {
                    if (curentAnnotation->getName() == annotationName){
                        foreach (const U2Region &curRegion, curentAnnotation->getRegions()) {
                            if (!expectedRegions.contains(curRegion)) {
                                stateInfo.setError(GTest::tr("Regions is incorrect. actual region didn't found in expected region list"));
                            }
                        }
                    }
                    return ReportResult_Finished;
                }
            }
        }
    }

    return ReportResult_Finished;
}


GTest_AddPartToSequenceTask::~GTest_AddPartToSequenceTask() {

}

//////////////////////////////////////////////////////////////////////////////////////////////


void GTest_RemovePartFromSequenceTask::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);
    QString buf;
    buf = el.attribute(DOC_NAME_ATTR);
    if (!buf.isEmpty()){
        docName=buf;
    }
    buf = el.attribute(SEQ_NAME_ATTR);
    if (!buf.isEmpty()){
        seqName=buf;
    }
    buf = el.attribute(START_POS_ATTR);
    if (!buf.isEmpty()){
        startPos=buf.toInt();
    }
    buf = el.attribute(LENGTH_ATTR);
    if (!buf.isEmpty()){
        length=buf.toInt();
    }
    buf = el.attribute(ANNOTATION_NAME_ATTR);
    if (!buf.isEmpty()){
        annotationName=buf;
    }
    buf = el.attribute(EXPECTED_SEQ_ATTR);
    if (!buf.isEmpty()){
        expectedSequence=buf;
    }
    buf = el.attribute(EXPECTED_REGIONS_ATTR);
    if (!buf.isEmpty()){
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts)){
            expectedRegions.append(U2Region(str.split(',')[0].toInt(),str.split(',')[1].toInt()-str.split(',')[0].toInt()));
        }
        //expectedRegions=buf;
    }else{
        expectedRegions.clear();
    }

    buf = el.attribute(EXPECTED_ANNOTATION_STRATEGY_ATTR);
    if (buf.toLower() == "remove"){
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    } else {
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    }
}

void GTest_RemovePartFromSequenceTask::prepare(){
    Document* loadedDocument = getContext<Document>(this, docName);
    if (loadedDocument == NULL) {
        stateInfo.setError(GTest::tr("context not found %1").arg(docName));
        return;
    }
    dnaso=(U2SequenceObject*)loadedDocument->findGObjectByName(seqName);
    if (dnaso==NULL){
        stateInfo.setError(GTest::tr("Sequence %1 not found").arg(seqName));
    }else{
        QList<Document*> docList;
        docList.append(loadedDocument);
        addSubTask(new ModifySequenceContentTask(loadedDocument->getDocumentFormatId(), dnaso, U2Region(startPos, length), DNASequence(), false, strat));
    }
}

Task::ReportResult GTest_RemovePartFromSequenceTask::report(){
    if (dnaso->getSequenceLength()!=expectedSequence.size()) {
        stateInfo.setError(GTest::tr("Length of sequence is incorrect. Expected:%2, but Actual:%1")
            .arg(dnaso->getSequenceLength())
            .arg(expectedSequence.length()));
        return ReportResult_Finished;
    }
    if (QString::compare (dnaso->getWholeSequenceData(stateInfo), expectedSequence, Qt::CaseInsensitive)!=0)//may be refactor this place
    {
        CHECK_OP(stateInfo, ReportResult_Finished);
        stateInfo.setError(GTest::tr("Sequence is incorrect. Expected:%1, but Actual:%2")
            .arg((QString)(dnaso->getWholeSequenceData(stateInfo)))
            .arg(expectedSequence));
        return ReportResult_Finished;
    }
    CHECK_OP(stateInfo, ReportResult_Finished);
    if (annotationName.length() != 0) {
        Document* loadedDocument = getContext<Document>(this, docName);
        QList<GObject*> annotationTablesList = loadedDocument->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
        foreach(GObject *table, annotationTablesList){
            AnnotationTableObject *ato = dynamic_cast<AnnotationTableObject *>(table);
            foreach (Annotation *curentAnnotation, ato->getAnnotations()) {
                if (curentAnnotation->getName() == annotationName) {
                    int i = 0;
                    if (curentAnnotation->getRegions().size() != expectedRegions.size()){
                        stateInfo.setError(GTest::tr("Regions is incorrect. Expected size:%1 Actual size:%2")
                            .arg(expectedRegions.size()).arg(curentAnnotation->getRegions().size()));
                        break;
                    }
                    foreach (const U2Region &curRegion, curentAnnotation->getRegions()){
                        if (curRegion != expectedRegions.at(i)) {
                            stateInfo.setError(GTest::tr("Regions is incorrect. Expected:%3,%4, but Actual:%1,%2")
                                .arg(curRegion.startPos).arg(curRegion.endPos()).arg(expectedRegions.at(i).startPos).arg(expectedRegions.at(i).endPos()));
                            return ReportResult_Finished;
                        }
                        i++;
                    }
                    return ReportResult_Finished;
                }
            }
        }
        if (expectedRegions.size() != 0) {
            stateInfo.setError(GTest::tr("Regions is incorrect. Expected size:%1 Actual size:%2").arg(expectedRegions.size()).arg(0));
            return ReportResult_Finished;
        }
    }

    return ReportResult_Finished;
}


GTest_RemovePartFromSequenceTask::~GTest_RemovePartFromSequenceTask() {
}

//////////////////////////////////////////////////////////////////////////

void GTest_ReplacePartOfSequenceTask::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);
    QString buf;
    buf = el.attribute(DOC_NAME_ATTR);
    if (!buf.isEmpty()){
        docName=buf;
    }
    buf = el.attribute(SEQ_NAME_ATTR);
    if (!buf.isEmpty()){
        seqName=buf;
    }
    buf = el.attribute(START_POS_ATTR);
    if (!buf.isEmpty()){
        startPos=buf.toInt();
    }
    buf = el.attribute(LENGTH_ATTR);
    if (!buf.isEmpty()){
        length=buf.toInt();
    }

    buf = el.attribute(INSERTED_SEQ_ATTR);
    if (!buf.isEmpty()){
        insertedSequence=buf;
    }

    buf = el.attribute(ANNOTATION_NAME_ATTR);
    if (!buf.isEmpty()){
        annotationName=buf;
    }
    buf = el.attribute(EXPECTED_SEQ_ATTR);
    if (!buf.isEmpty()){
        expectedSequence=buf;
    }
    buf = el.attribute(EXPECTED_REGIONS_ATTR);
    if (!buf.isEmpty()){
        QStringList items = buf.split(',', QString::SkipEmptyParts);
        foreach(QString str, items){
            int startPos = str.split("..")[0].toInt() - 1;
            int len = str.split("..")[1].toInt() - startPos;
            expectedRegions.append(U2Region(startPos, len));
        }
    }else{
        expectedRegions.clear();
    }

    buf = el.attribute(EXPECTED_ANNOTATION_STRATEGY_ATTR);
    if(buf.toLower() == "remove"){
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }else{
        strat = U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    }
}

void GTest_ReplacePartOfSequenceTask::prepare(){
    Document* loadedDocument = getContext<Document>(this, docName);
    if (loadedDocument == NULL) {
        stateInfo.setError(GTest::tr("Context not found %1").arg(docName));
        return;
    }
    dnaso=(U2SequenceObject*)loadedDocument->findGObjectByName(seqName);
    if (dnaso==NULL){
        stateInfo.setError(GTest::tr("Sequence %1 not found").arg(seqName));
    }else{
        QList<Document*> docList;
        docList.append(loadedDocument);
        DNASequence dna("Inserted DNA", insertedSequence.toLatin1());
        addSubTask(new ModifySequenceContentTask(loadedDocument->getDocumentFormatId(), dnaso, U2Region(startPos, length), dna, false, strat));
    }
}

Task::ReportResult GTest_ReplacePartOfSequenceTask::report(){
    if (dnaso->getSequenceLength()!=expectedSequence.size())
    {
        stateInfo.setError(GTest::tr("Length of sequence is incorrect. Expected:%2, but Actual:%1")
            .arg(dnaso->getSequenceLength())
            .arg(expectedSequence.length()));
        return ReportResult_Finished;
    }
    if (QString::compare (dnaso->getWholeSequenceData(stateInfo), expectedSequence, Qt::CaseInsensitive)!=0)//may be refactor this place
    {
        CHECK_OP(stateInfo, ReportResult_Finished);
        stateInfo.setError(GTest::tr("Sequence is incorrect. Actual:%1, but expected:%2")
            .arg((QString)(dnaso->getWholeSequenceData(stateInfo)))
            .arg(expectedSequence));
        return ReportResult_Finished;
    }
    CHECK_OP(stateInfo, ReportResult_Finished);
    if (!annotationName.isEmpty()) {
        Document* loadedDocument = getContext<Document>(this, docName);
        QList<GObject*> annotationTablesList = loadedDocument->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
        foreach(GObject *table, annotationTablesList){
            AnnotationTableObject *ato = dynamic_cast<AnnotationTableObject *>(table);
            foreach (Annotation *curentAnnotation, ato->getAnnotations()) {
                if (curentAnnotation->getName() == annotationName) {
                    int i = 0;
                    if (curentAnnotation->getRegions().size() != expectedRegions.size()) {
                        stateInfo.setError(GTest::tr("Regions is incorrect. Expected size:%1 Actual size:%2")
                            .arg(expectedRegions.size()).arg(curentAnnotation->getRegions().size()));
                        break;
                    }
                    foreach (const U2Region& curRegion, curentAnnotation->getRegions()) {
                        if (curRegion != expectedRegions.at(i)) {
                            stateInfo.setError(GTest::tr("Regions is incorrect. Expected:%3,%4, but Actual:%1,%2")
                                .arg(curRegion.startPos).arg(curRegion.endPos())
                                .arg(expectedRegions.at(i).startPos).arg(expectedRegions.at(i).endPos()));
                        }
                        i++;
                    }
                    return ReportResult_Finished;
                }
            }
        }
    }

    return ReportResult_Finished;
}



//////////////////////////////////////////////////////////////////////////

QList< XMLTestFactory* > EditSequenceTests::createTestFactories()
{
    QList< XMLTestFactory* > res;
    res.append(GTest_AddPartToSequenceTask::createFactory());
    res.append(GTest_RemovePartFromSequenceTask::createFactory());
    res.append(GTest_ReplacePartOfSequenceTask::createFactory());
    return res;
}

}
