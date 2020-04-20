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

#include <QFileInfo>
#include <QTemporaryFile>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/GenbankLocationParser.h>
#include <U2Formats/GenbankPlainTextFormat.h>

#include "CloningUtilTasks.h"
#include "EnzymesIO.h"
#include "EnzymesTests.h"
#include "FindEnzymesTask.h"

namespace U2 {

void GTest_FindEnzymes::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    loadTask = NULL;
    contextIsAdded = false;

    seqObjCtx = el.attribute("sequence");
    if (seqObjCtx.isEmpty()) {
        stateInfo.setError( "Sequence object context not specified");
        return;
    }
    aObjName = el.attribute("result-name");
    if (aObjName.isEmpty()) {
        aObjName = "annotations";
    }

    U2OpStatusImpl os;
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os,);
    aObj = new AnnotationTableObject(aObjName, dbiRef);

    SAFE_POINT(AppContext::getIOAdapterRegistry() != NULL, "IOAdapter registry is NULL",);
    IOAdapterFactory *ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QTemporaryFile *t = new QTemporaryFile(this);
    Document *doc = new Document(new GenbankPlainTextFormat(this), ioFactory,
                                 GUrl(QFileInfo(*t).absoluteFilePath()), dbiRef,
                                 QList<GObject*>() << aObj);
    aObj->setParent(doc);

    QString buf = el.attribute("minHits");
    bool ok;
    minHits = buf.toInt(&ok);
    if(!ok){
        minHits = 1;
    }

    buf = el.attribute("maxHits");
    maxHits = buf.toInt(&ok);
    if(!ok){
        maxHits = INT_MAX;
    }

    // read url of a file with enzymes
    enzymesUrl = el.attribute("url");
    if (seqObjCtx.isEmpty()) {
        stateInfo.setError( "Enzymes database URL not specified");
        return;
    }
    enzymesUrl=env->getVar("COMMON_DATA_DIR")+"/" + enzymesUrl;

    // get regions to exclude
    QString regionStr = el.attribute("exclude-regions");
    if (!regionStr.isEmpty()) {
        U2Location location;
        Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
        excludedRegions = location->regions;
    }

    // get enzymes names to find
    QString ensymesStr = el.attribute("enzymes");
    enzymeNames = ensymesStr.split(",", QString::SkipEmptyParts);
    if (enzymeNames.isEmpty()) {
        stateInfo.setError( QString("Invalid enzyme names: '%1'").arg(ensymesStr));
        return;
    }

    // read expected results
    QString resultsStr = el.attribute("result");
    if (resultsStr.isEmpty()) {
        stateInfo.setError( "'result' value not set");
        return;
    }
    QStringList perEnzymeResults = resultsStr.split(";", QString::SkipEmptyParts);
    foreach(const QString& enzymeResult, perEnzymeResults) {
        int nameIdx = enzymeResult.indexOf(':');
        if (nameIdx <=0 || nameIdx+1 == enzymeResult.size()) {
            stateInfo.setError( QString("Error parsing results token %1").arg(enzymeResult));
            return;
        }
        QString enzymeId = enzymeResult.left(nameIdx);
        QString regions = enzymeResult.mid(nameIdx+1);

        if (!enzymeNames.contains(enzymeId)) {
            stateInfo.setError( QString("Result enzyme not in the search list %1").arg(enzymeId));
            return;
        }

        QRegExp rx2("(\\d+)(..)(\\d+)");
        int pos = 0;
        while ((pos = rx2.indexIn(regions, pos)) != -1) {
            int start=rx2.cap(1).toInt();
            int end=rx2.cap(3).toInt();
            resultsPerEnzyme.insert(enzymeId, U2Region(start-1, end - start + 1));
            pos += rx2.matchedLength();
        }
        if (!resultsPerEnzyme.contains(enzymeId)) {
            stateInfo.setError( QString("Can't parse regions in results token: %1").arg(enzymeResult));
            return;
        }
    }
}

void GTest_FindEnzymes::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }

    //get sequence object
    seqObj = getContext<U2SequenceObject>(this, seqObjCtx);
    if (seqObj == NULL) {
        stateInfo.setError( QString("Sequence context not found %1").arg(seqObjCtx));
        return;
    }

    aObj->addObjectRelation(seqObj, ObjectRole_Sequence);

    loadTask = new LoadEnzymeFileTask(enzymesUrl);
    addSubTask(loadTask);
}

QList<Task*> GTest_FindEnzymes::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask!=loadTask || loadTask->enzymes.isEmpty()) {
        return res;
    }

    QList<SEnzymeData> enzymesToSearch;
    foreach(const QString& enzymeId, enzymeNames) {
        SEnzymeData enzyme = EnzymesIO::findEnzymeById(enzymeId, loadTask->enzymes);
        if (enzyme.constData() == NULL) {
            stateInfo.setError( QString("Enzyme not found: %1").arg(enzymeId));
            return res;
        }
        enzymesToSearch.append(enzyme);
    }

    FindEnzymesTaskConfig cfg;
    cfg.maxResults = INT_MAX;
    cfg.minHitCount = minHits;
    cfg.maxHitCount = maxHits;
    cfg.excludedRegions = excludedRegions;

    FindEnzymesToAnnotationsTask* t = new FindEnzymesToAnnotationsTask(aObj, seqObj->getSequenceRef(), enzymesToSearch, cfg);
    res.append(t);
    return res;
}

Task::ReportResult GTest_FindEnzymes::report() {
    if (hasError() || isCanceled()) {
        return Task::ReportResult_Finished;
    }
    //for each enzyme from resultsPerEnzyme check that all annotations are present
    foreach(const QString& enzymeId, resultsPerEnzyme.keys()) {
        QList<U2Region> regions = resultsPerEnzyme.values(enzymeId);
        AnnotationGroup *ag = aObj->getRootGroup()->getSubgroup(enzymeId, false);
        if (NULL == ag) {
            stateInfo.setError( QString("Group not found %1").arg(enzymeId));
            break;
        }
        const QList<Annotation *> anns = ag->getAnnotations();
        if (anns.size() != regions.size()) {
            stateInfo.setError(QString("Number of results not matched for :%1, results: %2, expected %3")
                .arg(enzymeId).arg(anns.size()).arg(regions.size()));
            break;
        }
        foreach (Annotation *a, anns) {
            const U2Region r = a->getRegions().first();
            if (!regions.contains(r)) {
                stateInfo.setError(QString("Illegal region! Enzyme :%1, region %2..%3").arg(enzymeId).arg(r.startPos + 1).arg(r.endPos()));
                break;
            }
        }
    }

    addContext(aObjName, aObj);
    contextIsAdded = true;

    return Task::ReportResult_Finished;
}

void GTest_FindEnzymes::cleanup() {
    if (aObj != NULL) {
        if (contextIsAdded) {
            removeContext(aObjName);
        }
        delete aObj;
    }

    XmlTest::cleanup();
}

//////////////////////////////////////////////////////////////////////////

void GTest_DigestIntoFragments::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    loadTask = NULL;

    seqObjCtx = el.attribute("sequence");
    if (seqObjCtx.isEmpty()) {
        stateInfo.setError( "Sequence object context not specified");
        return;
    }

    QString isCircularBuf = el.attribute("circular");
    isCircular = isCircularBuf == "true" ? true : false;

    aObjCtx = el.attribute("annotation-table");
    if (aObjCtx.isEmpty()) {
        stateInfo.setError( "Annotation object context not specified");
        return;
    }

    // read url of a file with enzymes
    enzymesUrl = el.attribute("url");
    if (enzymesUrl.isEmpty()) {
        stateInfo.setError( "Enzymes database URL not specified");
        return;
    }
    enzymesUrl=env->getVar("COMMON_DATA_DIR")+"/" + enzymesUrl;


    QString buf = el.attribute("search-for-enzymes");
    searchForEnzymes = buf == "true" ? true : false;

    // get enzymes names to find
    QString ensymesStr = el.attribute("enzymes");
    enzymeNames = ensymesStr.split(",", QString::SkipEmptyParts);
    if (enzymeNames.isEmpty()) {
        stateInfo.setError( QString("Invalid enzyme names: '%1'").arg(ensymesStr));
        return;
    }
}

void GTest_DigestIntoFragments::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }

    //get sequence object
    seqObj = getContext<U2SequenceObject>(this, seqObjCtx);
    if (seqObj == NULL) {
        stateInfo.setError( QString("Sequence context not found %1").arg(seqObjCtx));
        return;
    }

    aObj = getContext<AnnotationTableObject>(this, aObjCtx);
    if (aObj == NULL) {
        stateInfo.setError( QString("Annotation context not found %1").arg(aObjCtx));
        return;
    }

    loadTask = new LoadEnzymeFileTask(enzymesUrl);
    addSubTask(loadTask);
}

QList<Task*> GTest_DigestIntoFragments::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask!=loadTask || loadTask->enzymes.isEmpty()) {
        return res;
    }

    QList<SEnzymeData> enzymesToSearch;
    foreach(const QString& enzymeId, enzymeNames) {
        SEnzymeData enzyme = EnzymesIO::findEnzymeById(enzymeId, loadTask->enzymes);
        if (enzyme.constData() == NULL) {
            stateInfo.setError( QString("Enzyme not found: %1").arg(enzymeId));
            return res;
        }
        enzymesToSearch.append(enzyme);
    }

    DigestSequenceTaskConfig cfg;
    cfg.searchForRestrictionSites = searchForEnzymes;
    cfg.enzymeData = enzymesToSearch;
    cfg.forceCircular = isCircular;

    DigestSequenceTask* t = new DigestSequenceTask(seqObj, aObj, aObj, cfg);
    res.append(t);
    return res;
}

//////////////////////////////////////////////////////////////////////////

void GTest_LigateFragments::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    ligateTask = NULL;
    contextAdded = false;

    resultDocName = el.attribute("index");
    if (resultDocName.isEmpty()) {
        stateInfo.setError( "Result document name is not specified");
        return;
    }

    QString objCtx = el.attribute("seq-context");
    if (objCtx.isEmpty()) {
        stateInfo.setError( "Sequence object context not specified");
        return;
    }
    seqObjNames = objCtx.split(";");

    objCtx = el.attribute("annotation-context");
    if (objCtx.isEmpty()) {
        stateInfo.setError( "Annotation object context not specified");
        return;
    }
    annObjNames = objCtx.split(";");


    QString fragmentsData = el.attribute("fragments");
    if (fragmentsData.isEmpty()) {
        stateInfo.setError( "Fragments names are not specified");
        return;
    }
    fragmentNames = fragmentsData.split(";");

    QString buf = el.attribute("check-overhangs");
    checkOverhangs = buf == "true" ? true : false;

    buf = el.attribute("circular");
    makeCircular = buf == "true" ? true : false;

}

void GTest_LigateFragments::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }

    foreach (const QString& seqObjCtx, seqObjNames) {
        GObject* seqObj = getContext<U2SequenceObject>(this, seqObjCtx);
        if (seqObj == NULL) {
            stateInfo.setError( QString("Sequence object context not found %1").arg(seqObjCtx));
            return;
        }
        sObjs.append(seqObj);
    }

    foreach (const QString& aObjCtx, annObjNames) {
        GObject* aObj = getContext<AnnotationTableObject>(this, aObjCtx);
        if (aObj == NULL) {
            stateInfo.setError( QString("Annotation context not found %1").arg(aObjCtx));
            return;
        }
        aObjs.append(aObj);
    }

    prepareFragmentsList();

    if (targetFragments.isEmpty() ) {
        stateInfo.setError( QString("Target fragment list is empty!"));
        return;
    }

    LigateFragmentsTaskConfig cfg;
    cfg.checkOverhangs = checkOverhangs;
    cfg.makeCircular = makeCircular;
    cfg.docUrl = resultDocName + ".gb";
    cfg.openView = false;
    cfg.saveDoc = false;
    cfg.addDocToProject = false;
    cfg.annotateFragments = true;

    ligateTask = new LigateFragmentsTask(targetFragments, cfg);
    addSubTask(ligateTask);
}

void GTest_LigateFragments::prepareFragmentsList()
{
    QList<DNAFragment> fragments = DNAFragment::findAvailableFragments(aObjs, sObjs);

    foreach (const QString& searchedName, fragmentNames) {
        QStringList nameData = searchedName.split(":");
        QString fName = nameData.at(1);
        QString fDoc = nameData.at(0);
        bool inverted = nameData.count() > 2 && nameData.at(2) == "inverted";
        bool found = false;
        foreach (DNAFragment fragment, fragments) {
            QString name = fragment.getName();
            QString docName = fragment.getSequenceDocName();
            if (name == fName && docName == fDoc) {
                fragment.setInverted(inverted);
                targetFragments.append(fragment);
                found = true;
                break;
            }
        }
        if (!found) {
            stateInfo.setError(QString("%1 %2 is not found. ").arg(fName).arg(fDoc));
        }
    }
}

Task::ReportResult GTest_LigateFragments::report()
{
    if (hasError()) {
        return ReportResult_Finished;
    }

    if (ligateTask != NULL && ligateTask->hasError()) {
        stateInfo.setError(ligateTask->getError());
    } else if (!resultDocName.isEmpty()) {
        addContext(resultDocName, ligateTask->getResultDocument());
        contextAdded = true;
    }

    return ReportResult_Finished;
}

void GTest_LigateFragments::cleanup()
{
    if (contextAdded) {
        removeContext(resultDocName);
    }

    XmlTest::cleanup();
}


//////////////////////////////////////////////////////////////////////////
QList<XMLTestFactory*> EnzymeTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_FindEnzymes::createFactory());
    res.append(GTest_DigestIntoFragments::createFactory());
    res.append(GTest_LigateFragments::createFactory());
    return res;
}

}//namespace
