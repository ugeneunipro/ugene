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

#include "DotPlotTasks.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/AnnotationTableObject.h>

#include <U2Formats/DocumentFormatUtils.h>


namespace U2 {

void SaveDotPlotTask::run() {

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    saveDotPlot(stream);

    file.close();
}

void LoadDotPlotTask::run() {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    if (!loadDotPlot(stream, file.size())) {

        stateInfo.setError(tr("Wrong dotplot format"));
    }

    file.close();
}

// check if the file opens
DotPlotErrors SaveDotPlotTask::checkFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return ErrorOpen;
    }

    file.close();
    return NoErrors;
}

void SaveDotPlotTask::saveDotPlot(QTextStream &stream){

    SAFE_POINT (sequenceX, "sequenceX is NULL", );
    SAFE_POINT (sequenceY, "sequenceY is NULL", );

    stream << sequenceX->getGObjectName() << endl;
    stream << sequenceY->getGObjectName() << endl;

    stream << minLen << " " << identity << endl;

    SAFE_POINT(directList, "directList is NULL", );

    int listSizes = directList->size() + inverseList->size();
    SAFE_POINT (listSizes, "listSizes is NULL", );

    int i = 0;
    foreach(const DotPlotResults &r, *directList) {
        if (stateInfo.cancelFlag) {
            return;
        }

        stream << r.x << " " << r.y << " " << r.len << endl;
        stateInfo.progress = (100*i)/listSizes;

        i++;
    }

    stream << endl << "0 0 0" << endl;

    SAFE_POINT(inverseList, "inverseList is NULL", );

    foreach(const DotPlotResults &r, *inverseList) {
        if (stateInfo.cancelFlag) {
            return;
        }

        stream << r.x << " " << r.y << " " << r.len << endl;
        stateInfo.progress = (100*i)/listSizes;

        i++;
    }
}

// check if the file opens and sequence names are the same
DotPlotErrors LoadDotPlotTask::checkFile(const QString &filename, const QString &seqXName, const QString &seqYName) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ErrorOpen;
    }

    QTextStream stream(&file);

    QString readedXName;
    QString readedYName;

    readedXName = stream.readLine();
    readedYName = stream.readLine();

    DotPlotErrors err = NoErrors;

    if (seqXName != readedXName || seqYName != readedYName) {
        err = ErrorNames;
    }

    file.close();

    return err;
}

bool LoadDotPlotTask::loadDotPlot(QTextStream &stream, int fileSize) {

    QString readedXName;
    QString readedYName;

    readedXName = stream.readLine();
    readedYName = stream.readLine();

    // not checking names, just loading dotplot

    int newMinLen, newIdentity;
    QList<DotPlotResults> newDotPlotDirectList;
    QList<DotPlotResults> newDotPlotInverseList;

    stream >> newMinLen >> newIdentity;

    if (newMinLen<2 || newIdentity<50) {
        // wrong format

        return false;
    }

    *inverted = *direct = false;
    bool readingDirect = true;
    long count = 0;
    while (!stream.atEnd() && !stateInfo.cancelFlag) {

        DotPlotResults r;
        stream >> r.x >> r.y >> r.len;
        if (readingDirect && (r.x == 0) && (r.y == 0) && (r.len == 0)) {

            if (count>0) {
                *direct = true;
                count = 0;
            }
            readingDirect = false;
            continue;
        }

        if (readingDirect) {
            newDotPlotDirectList.push_back(r);
        }
        else {
            newDotPlotInverseList.push_back(r);
        }

        SAFE_POINT (stream.device(), "stream.device() is NULL", false);
        SAFE_POINT (fileSize, "fileSize is NULL", false);

        int streamPos = stream.device()->pos();
        stateInfo.progress = (100*streamPos)/fileSize;

        count++;
    }

    if (!readingDirect && (count>0)) {
        *inverted = true;
    }

    SAFE_POINT(directList, "directList is NULL", false);
    directList->clear();
    *directList = newDotPlotDirectList;

    SAFE_POINT(inverseList, "inverseList is NULL", false);
    inverseList->clear();
    *inverseList = newDotPlotInverseList;

    SAFE_POINT(minLen, "minLen is NULL", false);
    SAFE_POINT(identity, "identity is NULL", false);

    *minLen = newMinLen;
    *identity = newIdentity;

    return true;
}

DotPlotLoadDocumentsTask::DotPlotLoadDocumentsTask(QString firstF, int firstG, QString secondF, int secondG, bool view)
: Task(tr("DotPlot loading"), TaskFlags(TaskFlag_NoRun | TaskFlag_FailOnSubtaskCancel)), noView(!view)
{
    firstFile = firstF;
    firstGap = firstG;

    secondFile = secondF;
    secondGap= secondG;
}

void DotPlotLoadDocumentsTask::prepare() {

    // load sequences
    Document *doc = loadFile(firstFile, firstGap);
    if (doc) {
        docs << doc;
    }

    if (hasError()) {
        return;
    }
    doc = loadFile(secondFile, secondGap);
    if (doc) {
        docs << doc;
    }
}

Document *DotPlotLoadDocumentsTask::loadFile(QString inFile, int gapSize) {

    if(inFile == ""){
        return NULL;
    }
    GUrl url(inFile);

    Project *project = AppContext::getProject();

    SAFE_POINT(project, "project is NULL", NULL);
    Document *doc = project->findDocumentByURL(url);

    // document already present in the project
    if (doc) {
        return doc;
    }

    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(inFile);
    if (formats.isEmpty()) {
        stateInfo.setError(tr("Detecting format error for file %1").arg(inFile));
        return NULL;
    }

    DocumentFormat* format = formats.first().format;
    SAFE_POINT(format, "format is NULL", NULL);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));

    QVariantMap hints;
    if (gapSize >= 0) {
        QString mergeToken = DocumentReadingMode_SequenceMergeGapSize;
        hints[mergeToken] = gapSize;
    }

    doc = format->createNewUnloadedDocument(iof, url, stateInfo, hints);
    CHECK_OP(stateInfo, NULL);
    doc->setUserModLock(false);

    addSubTask(new AddDocumentTask(doc)); // add document to the project
    addSubTask(new LoadUnloadedDocumentTask(doc)); // load document

    return doc;
}

DotPlotLoadDocumentsTask::~DotPlotLoadDocumentsTask() {
    // error while loading documents
    if (hasError()) {
        Project *project = AppContext::getProject();

        // skip added to the project documents
        if (project) {
            QList<Document*> projectDocs = project->getDocuments();

            foreach (Document *doc, projectDocs) {
                docs.removeAll(doc);
            }
        }

        foreach (Document *doc, docs) {
            delete doc;
        }
   }
}

DotPlotFilterTask::DotPlotFilterTask(ADVSequenceObjectContext* _sequenceX,
                                     ADVSequenceObjectContext* _sequenceY,
                                     const QMultiMap<FilterIntersectionParameter,
                                     QString>& _annotationNames,
                                     QSharedPointer< QList<DotPlotResults> > _initialResults,
                                     QSharedPointer< QList<DotPlotResults> > _filteredResults
                                     ,FilterType _type)
:Task(tr("Applying filter to dotplot"), TaskFlag_None)
,sequenceX(_sequenceX)
,sequenceY(_sequenceY)
,annotationNames(_annotationNames)
,initialResults(_initialResults)
,filteredResults(_filteredResults)
,fType(_type)
,progressStep(0.0)
,progressFloatValue(0.0)
{
    tpm = Progress_Manual;
}

void DotPlotFilterTask::run(){
    stateInfo.progress = 0;
    int size = initialResults->size();
    copyInitialResults();
    progressStep = 100/(float)size;
    switch(fType){
        case All:
            break;
        case Features:
            progressStep /= 2;

            createSuperRegionsList(sequenceX, SequenceX);
            filterForCurrentSuperRegions(SequenceX);

            createSuperRegionsList(sequenceY, SequenceY);
            filterForCurrentSuperRegions(SequenceY);

            break;
    }
}

Task::ReportResult DotPlotFilterTask::report(){
    SAFE_POINT(filteredResults, "There are no filtered results", ReportResult_Finished);
    SAFE_POINT(initialResults, "There are no initial results", ReportResult_Finished);

    switch(fType){
        case All:
            {
               copyInitialResults();
            }
            break;
        case Features:
            {
                if (isCanceled()){
                    copyInitialResults();
                }
            }
            break;
    }

    return ReportResult_Finished;
}

void DotPlotFilterTask::createSuperRegionsList(ADVSequenceObjectContext* seq, FilterIntersectionParameter currentIntersParam) {
    superRegions.clear();
    if (isCanceled()) {
        return;
    }

    QSet<AnnotationTableObject*> aTableSet = seq->getAnnotationObjects(true);
    QList<Annotation *> selectedAnnotations;
    QStringList cursequenceAnnotationNames = annotationNames.values(currentIntersParam);
    if (cursequenceAnnotationNames.isEmpty()) {
        return;
    }

    foreach (const QString &aName, cursequenceAnnotationNames) {
        foreach (AnnotationTableObject* at, aTableSet) {
            selectedAnnotations << at->getAnnotationsByName(aName);
        }
    }

    foreach (Annotation *a, selectedAnnotations) {
        superRegions += a->getRegions();
    }

    superRegions = U2Region::join(superRegions);
}

void DotPlotFilterTask::filterForCurrentSuperRegions(FilterIntersectionParameter currentIntersParam){
    int vectorI = 0;
    int vectSize = superRegions.size();
    if(vectSize == 0){
        return;
    }

    QList<DotPlotResults>::iterator it;

    for (it = filteredResults->begin(); it != filteredResults->end() && !isCanceled(); ){
        progressFloatValue += progressStep;
        stateInfo.progress = (int)progressFloatValue;
        bool noIntersect = true;
        for(vectorI = 0; vectorI < vectSize; vectorI++){
            if(it->intersectRegion(superRegions[vectorI], currentIntersParam)){
                noIntersect = false;
                break;
            }
        }
        if(noIntersect){
            it = filteredResults->erase(it);
        }else{
            ++it;
        }
    }
}

void DotPlotFilterTask::copyInitialResults(){

    filteredResults->clear();
    foreach(DotPlotResults r, *initialResults){
        filteredResults->append(r);
    }
}

} // namespace
