/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PMatrixFormat.h"

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/PFMatrixObject.h>
#include <U2Core/PWMatrix.h>
#include <U2Core/PWMatrixObject.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ViewMatrixDialogController.h"
#include "WeightMatrixIO.h"

namespace U2 {

PFMatrixFormat::PFMatrixFormat(QObject* p)
    : TextDocumentFormat(p, DocumentFormatId("PFMatrix"), DocumentFormatFlag_SingleObjectFormat, QStringList("pfm")) {
    formatName = tr("Position frequency matrix");
    supportedObjectTypes += PFMatrixObject::TYPE;
    formatDescription = tr("Position frequency matrix file.");
}

FormatCheckResult PFMatrixFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString dataPrefixCopy = dataPrefix;
    QStringList qsl = dataPrefixCopy.replace("\r\n", "\n").split("\n");
    qsl.removeAll("");

    if (qsl.size() > 5 || qsl.size() < 4) {  // actually can be 4 or 5
        return FormatDetection_NotMatched;
    }
    foreach (QString str, qsl) {
        QStringList line = str.split(QRegExp("\\s+"));
        for (const QString& word : qAsConst(line)) {
            if (!word.isEmpty()) {
                bool isInt;
                word.toInt(&isInt);
                if (!isInt) {
                    return FormatDetection_NotMatched;
                }
            }
        }
    }

    return FormatDetection_Matched;
}

Document* PFMatrixFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, nullptr);

    QList<GObject*> objs;
    TaskStateInfo siPFM;
    PFMatrix m = WeightMatrixIO::readPFMatrix(reader, siPFM);
    CHECK_OP_EXT(siPFM, os.setError(tr("The file format is not PFM")), nullptr);
    CHECK_EXT(m.getLength() > 0, tr("Zero length or corrupted model\nMaybe model data are not enough for selected algorithm"), nullptr);

    PFMatrixObject* mObj = PFMatrixObject::createInstance(m, QFileInfo(reader.getURL().getURLString()).baseName(), dbiRef, os, hints);
    CHECK_OP(os, nullptr);
    objs.append(mObj);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objs, hints);
}

// Factory
//////////////////////////////////////////////////////////////////////////
const PFMatrixViewFactoryId PFMatrixViewFactory::ID("pfm-view-factory");

bool PFMatrixViewFactory::canCreateView(const MultiGSelection& multiSelection) {
    foreach (GObject* go, SelectionUtils::findObjects(PFMatrixObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        QString cname = go->metaObject()->className();
        if (cname == "U2::PFMatrixObject") {
            return true;
        }
    }
    return false;
}

Task* PFMatrixViewFactory::createViewTask(const MultiGSelection& multiSelection, bool single /* = false*/) {
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(PFMatrixObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return nullptr;
    }
    Task* result = (single || documents.size() == 1) ? nullptr : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach (Document* d, documents) {
        Task* t = new OpenPFMatrixViewTask(d);
        if (result == nullptr) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}

OpenPFMatrixViewTask::OpenPFMatrixViewTask(Document* doc)
    : ObjectViewTask(PFMatrixViewFactory::ID), document(doc) {
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach (GObject* go, doc->findGObjectByType(PFMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenPFMatrixViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach (GObject* go, documentsToLoad.first()->findGObjectByType(PFMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
    }
    foreach (QPointer<GObject> po, selectedObjects) {
        PFMatrixObject* o = qobject_cast<PFMatrixObject*>(po);
        MatrixViewController* view = new MatrixViewController(o->getMatrix());
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
    }
}

/// PWM

PWMatrixFormat::PWMatrixFormat(QObject* p)
    : TextDocumentFormat(p, DocumentFormatId("PWMatrix"), DocumentFormatFlag_SingleObjectFormat, QStringList("pwm")) {
    formatName = tr("Position weight matrix");
    supportedObjectTypes += PFMatrixObject::TYPE;
    formatDescription = tr("Position weight matrix file.");
}

FormatCheckResult PWMatrixFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString dataPrefixCopy = dataPrefix;
    QStringList qsl = dataPrefixCopy.replace("\r\n", "\n").split("\n");
    qsl.removeAll("");

    if (qsl.size() > 5 || qsl.size() < 4) {  // actually can be 5 or 6
        return FormatDetection_NotMatched;
    }
    qsl.pop_front();  // skip first line
    foreach (QString str, qsl) {
        QStringList words = str.split(QRegExp("\\s+"));
        CHECK(!words.isEmpty(), FormatDetection_NotMatched);

        QString firstWord = words.takeFirst();
        CHECK(firstWord.size() == 2, FormatDetection_NotMatched);
        CHECK(firstWord[1] == ':', FormatDetection_NotMatched);

        for (const QString& word : qAsConst(words)) {
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

Document* PWMatrixFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, nullptr);

    QList<GObject*> objs;
    TaskStateInfo siPWM;
    PWMatrix m = WeightMatrixIO::readPWMatrix(reader, siPWM);
    CHECK_OP_EXT(siPWM, os.setError(tr("The file format is not PWM")), nullptr);
    CHECK_EXT(m.getLength() > 0, tr("Zero length or corrupted model\nMaybe model data are not enough for selected algorithm"), nullptr);

    PWMatrixObject* mObj = PWMatrixObject::createInstance(m, QFileInfo(reader.getURL().getURLString()).baseName(), dbiRef, os, hints);
    CHECK_OP(os, nullptr);
    objs.append(mObj);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objs, hints);
}

// Factory
//////////////////////////////////////////////////////////////////////////
const PWMatrixViewFactoryId PWMatrixViewFactory::ID("pwm-view-factory");

bool PWMatrixViewFactory::canCreateView(const MultiGSelection& multiSelection) {
    foreach (GObject* go, SelectionUtils::findObjects(PWMatrixObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        QString cname = go->metaObject()->className();
        if (cname == "U2::PWMatrixObject") {
            return true;
        }
    }
    return false;
}

Task* PWMatrixViewFactory::createViewTask(const MultiGSelection& multiSelection, bool single /* = false*/) {
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(PWMatrixObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return nullptr;
    }
    Task* result = (single || documents.size() == 1) ? nullptr : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach (Document* d, documents) {
        Task* t = new OpenPWMatrixViewTask(d);
        if (result == nullptr) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}

OpenPWMatrixViewTask::OpenPWMatrixViewTask(Document* doc)
    : ObjectViewTask(PWMatrixViewFactory::ID), document(doc) {
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach (GObject* go, doc->findGObjectByType(PWMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenPWMatrixViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach (GObject* go, documentsToLoad.first()->findGObjectByType(PWMatrixObject::TYPE)) {
            selectedObjects.append(go);
        }
    }
    foreach (QPointer<GObject> po, selectedObjects) {
        PWMatrixObject* o = qobject_cast<PWMatrixObject*>(po);
        MatrixViewController* view = new MatrixViewController(o->getMatrix());
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
    }
}

}  // namespace U2
