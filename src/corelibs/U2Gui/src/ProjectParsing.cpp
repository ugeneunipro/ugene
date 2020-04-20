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

#include <QDir>
#include <QDomDocument>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ProjectUtils.h>

#include "ProjectParsing.h"

namespace U2 {

static QString map2String(const QVariantMap& map) {
    QByteArray a;
    QVariant v(map);
    QDataStream s(&a, QIODevice::WriteOnly);
    s << v;
    QString res(a.toBase64());
    return res;
}

static QVariantMap string2Map(const QString& string, bool emptyMapIfError) {
    Q_UNUSED(emptyMapIfError);

    QDataStream s(QByteArray::fromBase64(string.toLatin1()));
    QVariant res(QVariant::Map);
    s >> res;
    if (res.type() == QVariant::Map) {
        return res.toMap();
    }
    assert(emptyMapIfError);
    return QVariantMap();
}

static QList<GObjectRelation> toAbsoluteRelations(const QList<GObjectRelation>& relList, const QDir& projDir)  {
    QList<GObjectRelation> absoluteRelations;
    foreach(GObjectRelation rel, relList){
        if (GUrl::getURLType(rel.ref.docUrl) == GUrl_File) { //make URL absolute
            QFileInfo ff(rel.ref.docUrl);
            if(!ff.isAbsolute()){
                QFileInfo tmpFi(projDir.path() + "/" + rel.ref.docUrl);
                rel.ref.docUrl = tmpFi.canonicalFilePath();
            }
        }
        absoluteRelations.append(rel);
    }
    return absoluteRelations;
}

static QList<GObjectRelation> hintToRelations(QVariant qv, const QDir& projDir) {
    if(!qv.isNull()) {
        return toAbsoluteRelations(qv.value<QList<GObjectRelation> >(), projDir);
    } else {
        return QList<GObjectRelation>();
    }
}

static QVariant relationsToHint(const QList<GObjectRelation> &relList) {
    return QVariant::fromValue<QList<GObjectRelation> >(relList);
}

static QList<GObjectRelation> removeDuplicates(const QList<GObjectRelation> &relList) {
    return relList.toSet().toList();
}

static QVariant toRelativeRelations(const QList<GObjectRelation>& absRelations, const QDir& projDir, 
                                    const QMap<QString, QString>& urlRemap = QMap<QString, QString>()) 
{
    QList<GObjectRelation> relativeRelations;
    foreach (const GObjectRelation& absRel, absRelations) {
        GObjectRelation relRel = absRel;
        relRel.ref.docUrl = urlRemap.value(absRel.getDocURL(), absRel.getDocURL());
        if (GUrl::getURLType(relRel.getDocURL()) == GUrl_File) { //make URL absolute

#ifdef _DEBUG //ensure internal URLs are always canonical
            QFileInfo fi(relRel.getDocURL());
            QString canonicalPath = fi.canonicalFilePath();
            assert(canonicalPath.isEmpty() || canonicalPath == relRel.getDocURL());
#endif

            QString relativeURL = projDir.relativeFilePath(relRel.getDocURL());
            relRel.ref.docUrl = relativeURL;
        }
        relativeRelations.append(relRel);
    }
    return QVariant::fromValue<QList<GObjectRelation> >(relativeRelations);
}


void ProjectFileUtils::saveProjectFile(U2OpStatus& ts, Project* project, 
                                          const QString& projectUrl, const QMap<QString, QString>& docUrlRemap) 
{
    QDomDocument xmlDoc("GB2PROJECT");

    QDomElement projectElement = xmlDoc.createElement("gb2project");
    projectElement.setAttribute("name", project->getProjectName());
    const QString ver(CURRENT_PROJECT_VERSION);
    projectElement.setAttribute("version", ver);
    projectElement.setAttribute("oid", project->getObjectIdCounter());

    QFileInfo projectFile(projectUrl);
    QDir projectDir = projectFile.absoluteDir();

    //save documents
    foreach(Document* gbDoc, project->getDocuments()) {
        if (ProjectUtils::isDatabaseDoc(gbDoc)) {
            continue;
        }
        gbDoc->getDocumentFormat()->updateFormatSettings(gbDoc);

        QString docUrl = gbDoc->getURLString();
        if (docUrlRemap.contains(docUrl)) {
            docUrl = docUrlRemap.value(docUrl);
        }

        QString adapterId = gbDoc->getIOAdapterFactory()->getAdapterId();
        QDomElement docElement = xmlDoc.createElement("document");
        docElement.setAttribute("io-adapter", adapterId);

        //for local file save relative path
        if (adapterId == BaseIOAdapters::LOCAL_FILE || adapterId == BaseIOAdapters::GZIPPED_LOCAL_FILE) {
            QString relativeUrl = projectDir.relativeFilePath(docUrl);
            docElement.setAttribute("url", relativeUrl);
        } else {
            docElement.setAttribute("url", docUrl);
        }

        //store lock info for documents
        DocumentFormat* f = gbDoc->getDocumentFormat();
        QString formatId = f->getFormatId();
        docElement.setAttribute("format", formatId);
        docElement.setAttribute("readonly", gbDoc->hasUserModLock() ? 1 : 0);
        StateLock* l = gbDoc->getDocumentModLock(DocumentModLock_FORMAT_AS_INSTANCE);
        if (l!=NULL) {
            docElement.setAttribute("format-lock", 1);
        }

        //store doc-level hints
        if (!gbDoc->getGHintsMap().isEmpty()) {
            QString hintsStr = map2String(gbDoc->getGHintsMap());
            QDomText hintsNode = xmlDoc.createCDATASection(hintsStr);
            docElement.appendChild(hintsNode);
        }

        //now save unloaded objects info for all document objects
        foreach(GObject* obj, gbDoc->getObjects()) {
            QDomElement objElement = xmlDoc.createElement("object");
            UnloadedObjectInfo info(obj);
            objElement.setAttribute("name", info.name);
            objElement.setAttribute("type", info.type);

            if (!info.hints.isEmpty()) {
                //for all object relations make path relative
                info.hints[GObjectHint_RelatedObjects] = toRelativeRelations(obj->getObjectRelations(), projectDir, docUrlRemap);
                QString hintsStr = map2String(info.hints);
                QDomText hintsNode = xmlDoc.createCDATASection(hintsStr);
                objElement.appendChild(hintsNode);
            }
            docElement.appendChild(objElement);
        }

        projectElement.appendChild(docElement);
    }

    //save views states
    foreach(GObjectViewState* view, project->getGObjectViewStates()) {
        //save document info
        QDomElement viewElement = xmlDoc.createElement("view");
        viewElement.setAttribute("factory", view->getViewFactoryId());
        viewElement.setAttribute("viewName", view->getViewName());
        viewElement.setAttribute("stateName", view->getStateName());
        QString dataStr = map2String(view->getStateData());
        QDomText textNode = xmlDoc.createCDATASection(dataStr);
        viewElement.appendChild(textNode);

        projectElement.appendChild(viewElement);
    }

    xmlDoc.appendChild(projectElement);

    QByteArray rawData = xmlDoc.toByteArray();
    //  printf(">>%s", xmlDoc.toString().toStdString().c_str());

    //check that project folder exists
    if (!projectDir.exists()) {
        QDir root;
        root.mkpath(projectDir.absolutePath());
    }

    //save project file text to file
    QFile f(projectUrl);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qint64 s = f.write(rawData);
        f.close();
        if (s!=rawData.size()) {
            ts.setError(L10N::errorWritingFile(projectUrl));
        }
    } else {
        ts.setError(L10N::errorOpeningFileWrite(projectUrl));
    }
}



void ProjectFileUtils::loadXMLProjectModel(const QString& url, U2OpStatus& si, QDomDocument& doc, QString& version) {
    assert(doc.isNull());

    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) {
        si.setError(L10N::errorOpeningFileRead(url));
        return;
    }
    QByteArray  xmlData = f.readAll();
    f.close();

    bool res = doc.setContent(xmlData);
    if (!res) {
        si.setError(L10N::notValidFileFormat("XML", url));
        return;
    }

    const QString curVerStr(CURRENT_PROJECT_VERSION);

    QDomElement projectElement = doc.documentElement();
    version = projectElement.attribute("version");

    if(!version.isEmpty()){
        bool ok;
        double projVer = version.toDouble(&ok), curVer = curVerStr.toDouble();

        if (!ok) {
            si.setError(tr("Project version is not a double value"));
            doc.clear();
        }

        if (projVer > curVer){
            si.setError(tr("Project file was created by a newer version of UGENE. Please update UGENE."));
            doc.clear();
        }
    }

    if (!res || doc.doctype().name()!="GB2PROJECT") {
        si.setError(  tr("Not a valid UGENE project file %1").arg(url) );
        doc.clear();
    }
}

//////////////////////////////////////////////////////////////////////////
void ProjectParserRegistry::init(){
    parsers.qlist.append(new ProjectParser10());
}


ProjectParser* ProjectParserRegistry::getProjectParserByVersion( const QString& id ){
    foreach(ProjectParser *p,  parsers.qlist){
        if(p->getVersion() == id){
            return p;
        }
    }
    return NULL;
}

ProjectParserRegistry * ProjectParserRegistry::instance(){
    static ProjectParserRegistry ppr;
    return &ppr;
}



//////////////////////////////////////////////////////////////////////////
// Parser for v1.0 format

namespace {
    GUrl getUrl(const QString &docUrl, const DocumentFormatId &format) {
        if (BaseDocumentFormats::DATABASE_CONNECTION == format) {
            return GUrl(docUrl, GUrl_Network);
        } else {
            return docUrl;
        }
    }
}

Project* ProjectParser10::createProjectFromXMLModel( const QString& pURL, const QDomDocument& xmlDoc, U2OpStatus& os) {
    GCOUNTER(cvar, tvar, "ProjectParser10: createProjectFromXMLModel");

    QDomElement projectElement = xmlDoc.documentElement();
    QString name = projectElement.attribute("name");
    quint64 oid = qMax(quint64(0), projectElement.attribute("oid").toULongLong());

    QList<Document*> documents;
    QList<GObjectViewState*> states;

    //read all documents
    QSet<QString> docUrls;
    QDomNodeList documentList = projectElement.elementsByTagName("document");

    bool projectContainsInvalidFormats = false;

    for(int i=0; i < documentList.size(); i++) {
        QDomNode dn = documentList.item(i);
        if (!dn.isElement()) {
            continue;
        }
        QDomElement docElement = dn.toElement();
        QString ioAdapterId = docElement.attribute("io-adapter");
        QFileInfo proj(pURL);
        //QString docURL = proj.absoluteDir().path() + "/" + docElement.attribute("url");
        QString fURL(docElement.attribute("url"));
        QFileInfo fi(docElement.attribute("url"));
        QString xmlDocUrl = docElement.attribute("url");
        QString docUrl = xmlDocUrl;
        if ( ioAdapterId == BaseIOAdapters::GZIPPED_LOCAL_FILE || ioAdapterId == BaseIOAdapters::LOCAL_FILE ){
            if(!fi.isAbsolute()) {
                QString tmp = proj.absoluteDir().path() + "/" + docElement.attribute("url");
                QFileInfo ff(proj.absoluteDir().path() + "/" + docElement.attribute("url"));
                docUrl = ff.canonicalFilePath();
                if (docUrl.isEmpty()) {
                    docUrl = ff.absoluteFilePath();
                }
            } else {
                docUrl = fi.canonicalFilePath();
                if (docUrl.isEmpty()) {
                    docUrl = fi.absoluteFilePath();
                }
            }
        }
        if (docUrl.isEmpty()) {
            ioLog.info(tr("Can't find project file: %1, ignoring").arg(fi.absoluteFilePath()));
            continue;
        }
        if (docUrls.contains(docUrl)) {
            ioLog.info(tr("Duplicate document found: %1, ignoring").arg(docUrl));
            continue;
        }

        DocumentFormatId format = docElement.attribute("format");
        projectContainsInvalidFormats |= BaseDocumentFormats::isInvalidId(format);
        if (BaseDocumentFormats::equal(BaseDocumentFormats::DATABASE_CONNECTION, format)) {
            ioLog.info(tr("Database document: %1, ignoring").arg(docUrl));
            continue;
        }

        docUrls.insert(docUrl);

        bool readonly = docElement.attribute("readonly").toInt() != 0;
        bool instanceLock = docElement.attribute("format-lock").toInt() != 0;
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioAdapterId);
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(format);
        if (df == NULL) { // this can happen when close ugene on startup
            continue;
        }
        QVariantMap fs = string2Map(docElement.text(), true);

        QList<UnloadedObjectInfo> unloadedObjects;
        QDomNodeList objectList = docElement.elementsByTagName("object");
        QSet<QString> objNames;
        for(int j=0; j < objectList.size(); j++) {
            QDomNode on = objectList.item(j);
            if (!on.isElement()) {
                continue;
            }
            QDomElement objElement = on.toElement();
            UnloadedObjectInfo info;
            info.name = objElement.attribute("name");
            info.type = objElement.attribute("type");
            info.hints = string2Map(objElement.text(), true);

            if (!info.hints.isEmpty()) {
                // For backward compatibility: join relations from old and new hint key, avoid duplication
                QList<GObjectRelation> relations;
                relations << hintToRelations(info.hints[GObjectHint_RelatedObjects], proj.absoluteDir());
                relations << hintToRelations(info.hints[GObjectHint_RelatedObjectsLegacy], proj.absoluteDir());
                relations = removeDuplicates(relations);

                if(! relations.empty()) {
                    info.hints[GObjectHint_RelatedObjects] = relationsToHint(relations);
                }
            }

            if (info.isValid() && !objNames.contains(info.name)) {
                unloadedObjects.append(info);
                objNames.insert(info.name);
            }
        
        }
        QString lockReason = instanceLock ? tr("The last loaded state was locked by format") : QString();
        Document* d = df->createNewUnloadedDocument(iof, getUrl(docUrl, format), os, fs, unloadedObjects, lockReason);
        CHECK_OP_EXT(os, qDeleteAll(documents), NULL);
        d->setUserModLock(readonly);
        documents.append(d);
    }

    if (projectContainsInvalidFormats) {
        GCOUNTER(cvar1, tvar1, "Invalid format IDs: a project was created with 1.26.0");
    }

    // read all saved views
    QDomNodeList viewElements = projectElement.elementsByTagName("view");
    for(int i=0;i<viewElements.size(); i++) {
        QDomNode n = viewElements.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement viewElement = n.toElement();
        GObjectViewFactoryId id = viewElement.attribute("factory");
        QString viewName = viewElement.attribute("viewName");
        QString stateName = viewElement.attribute("stateName");
        QVariantMap map  = string2Map(viewElement.text(), false);
        GObjectViewState* state = new GObjectViewState(id, viewName, stateName, map);
        states.append(state);
    }
    Project* project = AppContext::getProjectLoader()->createProject(name, pURL, documents, states);
    project->setObjectIdCounter(oid);
    return project;
}

} //namespace
