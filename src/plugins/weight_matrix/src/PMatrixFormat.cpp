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

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/PFMatrixObject.h>
#include <U2Core/PWMatrixObject.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/Task.h>
#include <U2Core/PWMatrix.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/IOAdapterUtils.h>

#include "ViewMatrixDialogController.h"
#include "WeightMatrixIO.h"
#include "PMatrixFormat.h"

namespace U2{

PFMatrixFormat::PFMatrixFormat(QObject* p): DocumentFormat(p, DocumentFormatId("PFMatrix"), DocumentFormatFlag_SingleObjectFormat, QStringList("pfm")) {
    formatName = tr("Position frequency matrix");
    supportedObjectTypes += PFMatrixObject::TYPE;
    formatDescription = tr("Position frequency matrix file.");
}

FormatCheckResult PFMatrixFormat::checkRawData( const QByteArray& rawData, const GUrl& /*url*/ ) const{
    const char* data = rawData.constData();
    int size = rawData.size();
    if(TextUtils::contains(TextUtils::BINARY, data, size)){
        return FormatDetection_NotMatched;
    }

    QString dataStr(rawData);
    QStringList qsl = dataStr.split("\n");
    qsl.removeAll(QString());
    if(qsl.size() > 5 || qsl.size() < 4){ //actually can be 4 or 5
        return FormatDetection_NotMatched;
    }
    foreach(QString str, qsl){
        QStringList line = str.split(QRegExp("\\s+"));
        foreach(QString word, line){
            if (!word.isEmpty()){
                bool isInt;
                word.toInt(&isInt);
                if (!isInt){
                    return FormatDetection_NotMatched;
                }
            }
        }
    }

    return FormatDetection_Matched;
}

Document* PFMatrixFormat::loadDocument( IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os ){
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    QList<GObject*> objs;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(io->getAdapterId());
    TaskStateInfo siPFM;
    PFMatrix m = WeightMatrixIO::readPFMatrix(iof, io->getURL().getURLString(), siPFM);
    if (siPFM.hasError()) {
        os.setError(tr("The file format is not PFM"));
    } else {
       if (m.getLength() == 0) {
            os.setError(tr("Zero length or corrupted model\nMaybe model data are not enough for selected algorithm"));
        }
    }
    CHECK_OP(os, NULL);

    PFMatrixObject *mObj = PFMatrixObject::createInstance(m, QFileInfo(io->getURL().getURLString()).baseName(), dbiRef, os, fs);
    CHECK_OP(os, NULL);
    objs.append(mObj);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);
}

//Factory
//////////////////////////////////////////////////////////////////////////
const PFMatrixViewFactoryId PFMatrixViewFactory::ID("pfm-view-factory");


bool PFMatrixViewFactory::canCreateView( const MultiGSelection& multiSelection ){
    foreach(GObject* go, SelectionUtils::findObjects(PFMatrixObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        QString cname = go->metaObject()->className();
        if (cname == "U2::PFMatrixObject") {
            return true;
        }
    }
    return false;
}

Task* PFMatrixViewFactory::createViewTask( const MultiGSelection& multiSelection, bool single /* = false*/ ){
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(PFMatrixObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return NULL;
    }
    Task* result = (single || documents.size() == 1) ? NULL : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach(Document* d, documents) {
        Task* t = new OpenPFMatrixViewTask(d);
        if (result == NULL) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}


OpenPFMatrixViewTask::OpenPFMatrixViewTask( Document* doc ): ObjectViewTask(PFMatrixViewFactory::ID), document(doc){
    if(!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach(GObject* go, doc->findGObjectByType(PFMatrixObject::TYPE)) {
            selectedObjects.append(go) ;
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenPFMatrixViewTask::open(){
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach(GObject* go, documentsToLoad.first()->findGObjectByType(PFMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
    }
    foreach(QPointer<GObject> po, selectedObjects) {
        PFMatrixObject* o = qobject_cast<PFMatrixObject*>(po);
        MatrixViewController *view = new MatrixViewController(o->getMatrix());
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
    }
}

///PWM

PWMatrixFormat::PWMatrixFormat( QObject* p ): DocumentFormat(p, DocumentFormatId("PWMatrix"), DocumentFormatFlag_SingleObjectFormat, QStringList("pwm")) {
    formatName = tr("Position weight matrix");
    supportedObjectTypes += PFMatrixObject::TYPE;
    formatDescription = tr("Position weight matrix file.");
}

FormatCheckResult PWMatrixFormat::checkRawData( const QByteArray& rawData, const GUrl& ) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    if(TextUtils::contains(TextUtils::BINARY, data, size)){
        return FormatDetection_NotMatched;
    }

    QString dataStr(rawData);
    QStringList qsl = dataStr.split("\n");
    qsl.removeAll(QString());
    if(qsl.size() > 5 || qsl.size() < 4){ //actually can be 5 or 6
        return FormatDetection_NotMatched;
    }
    qsl.pop_front(); //skip first line
    foreach(QString str, qsl){
        QStringList words = str.split(QRegExp("\\s+"));
        CHECK(!words.isEmpty(), FormatDetection_NotMatched);

        QString firstWord = words.takeFirst();
        CHECK(2 == firstWord.size(), FormatDetection_NotMatched);
        CHECK(':' == firstWord[1], FormatDetection_NotMatched);

        foreach (QString word, words) {
            if (!word.isEmpty()) {
                bool isFloat;
                word.toFloat(&isFloat);
                if (!isFloat) {
                    return FormatDetection_NotMatched;
                }
            }
        }
    }

    return FormatDetection_Matched;
}

Document* PWMatrixFormat::loadDocument( IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os ){
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    QList<GObject*> objs;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(io->getAdapterId());
    TaskStateInfo siPWM;
    PWMatrix m = WeightMatrixIO::readPWMatrix(iof, io->getURL().getURLString(), siPWM);
    if (siPWM.hasError()) {
        os.setError(tr("The file format is not PWM"));
    } else {
        if (m.getLength() == 0) {
            os.setError(tr("Zero length or corrupted model.\nMaybe model data are not enough for selected algorithm"));
        }
    }
    CHECK_OP(os, NULL);

    PWMatrixObject *mObj = PWMatrixObject::createInstance(m, QFileInfo(io->getURL().getURLString()).baseName(), dbiRef, os, fs);
    CHECK_OP(os, NULL);
    objs.append(mObj);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);
}

//Factory
//////////////////////////////////////////////////////////////////////////
const PWMatrixViewFactoryId PWMatrixViewFactory::ID("pwm-view-factory");

bool PWMatrixViewFactory::canCreateView( const MultiGSelection& multiSelection ){
    foreach(GObject* go, SelectionUtils::findObjects(PWMatrixObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        QString cname = go->metaObject()->className();
        if (cname == "U2::PWMatrixObject") {
            return true;
        }
    }
    return false;
}

Task* PWMatrixViewFactory::createViewTask( const MultiGSelection& multiSelection, bool single /* = false*/ ){
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(PWMatrixObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return NULL;
    }
    Task* result = (single || documents.size() == 1) ? NULL : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach(Document* d, documents) {
        Task* t = new OpenPWMatrixViewTask(d);
        if (result == NULL) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}


OpenPWMatrixViewTask::OpenPWMatrixViewTask( Document* doc ): ObjectViewTask(PWMatrixViewFactory::ID), document(doc){
    if(!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach(GObject* go, doc->findGObjectByType(PWMatrixObject::TYPE)) {
            selectedObjects.append(go) ;
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenPWMatrixViewTask::open(){
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach(GObject* go, documentsToLoad.first()->findGObjectByType(PWMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
    }
    foreach(QPointer<GObject> po, selectedObjects) {
        PWMatrixObject* o = qobject_cast<PWMatrixObject*>(po);
        MatrixViewController *view = new MatrixViewController(o->getMatrix());
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
    }
}

}
