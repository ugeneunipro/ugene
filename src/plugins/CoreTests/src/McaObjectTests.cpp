/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "McaObjectTests.h"

#include <QDomElement>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/MsaObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/SQLiteDbi.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>

#include <U2View/MaEditorFactory.h>

namespace U2 {

void GTest_OpenMcaDatabase::init(XMLTestFormat*, const QDomElement& element) {
    addTaskResource(TaskResourceUsage(UGENE_RESOURCE_ID_PROJECT, 1, TaskResourceStage::Prepare));
    checkBooleanAttribute(element, "corrupt", true);
    checkBooleanAttribute(element, "saved-state", true);
    corrupt = element.attribute("corrupt") == "true";
    savedState = element.attribute("saved-state") == "true";
}

void GTest_OpenMcaDatabase::prepare() {
    CHECK_EXT(tempDir.isValid(), setError("Cannot create a temporary directory"), );
    url = tempDir.filePath("alignment.ugenedb");
    const QString source = env->getVar("SAMPLE_DATA_DIR") + "/Sanger/alignment.ugenedb";
    CHECK_EXT(QFile::copy(source, url), setError("Cannot copy the Sanger sample database: " + source), );

    if (corrupt) {
        SQLiteDbi dbi;
        dbi.init({{U2DbiOptions::U2_DBI_OPTION_URL, url}}, {}, stateInfo);
        CHECK_OP(stateInfo, );
        const qint64 pageSize = SQLiteReadQuery("PRAGMA page_size", dbi.getDbRef(), stateInfo).selectInt64();
        const qint64 rootPage = SQLiteReadQuery("SELECT rootpage FROM sqlite_master WHERE name = 'UdrSchema_RawData'", dbi.getDbRef(), stateInfo).selectInt64();
        dbi.shutdown(stateInfo);
        CHECK_OP(stateInfo, );
        CHECK_EXT(pageSize > 0 && rootPage > 1, setError("RawData table page not found"), );

        QFile file(url);
        CHECK_EXT(file.open(QIODevice::ReadWrite), setError(file.errorString()), );
        CHECK_EXT(file.seek((rootPage - 1) * pageSize), setError(file.errorString()), );
        // Invalid B-tree page type: object metadata stays readable, chromatogram loading fails.
        CHECK_EXT(file.write(QByteArray(1, '\0')) == 1, setError(file.errorString()), );
    }

    QVariantMap hints;
    hints[ProjectLoaderHint_LoadWithoutView] = true;
    loadTask = AppContext::getProjectLoader()->openWithProjectTask(GUrl(url), hints);
    CHECK_EXT(loadTask != nullptr, setError("Document loading task was not created"), );
    addSubTask(loadTask);
}

QList<Task*> GTest_OpenMcaDatabase::onSubTaskFinished(Task* subTask) {
    CHECK(subTask == loadTask, {});
    CHECK_EXT(!subTask->hasError(), setError(subTask->getError()), {});
    auto project = AppContext::getProject();
    CHECK_EXT(project != nullptr, setError("Project was not created"), {});
    document = project->findDocumentByURL(url);
    CHECK_EXT(document != nullptr && document->isLoaded(), setError("Document metadata was not loaded"), {});
    const QList<GObject*> objects = document->findGObjectByType(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT);
    CHECK_EXT(objects.size() == 1, setError("Expected one chromatogram alignment"), {});
    alignment = qobject_cast<MsaObject*>(objects.first());
    CHECK_EXT(alignment != nullptr, setError("Alignment object not found"), {});
    CHECK_EXT(GObjectViewUtils::findViewsWithObject(alignment).isEmpty(), setError("View opened before the test"), {});

    auto factory = AppContext::getObjectViewFactoryRegistry()->getFactoryById(McaEditorFactory::ID);
    if (savedState) {
        const QVariantMap state = {{"view_id", McaEditorFactory::ID},
                                   {"ma_obj_ref", QVariant::fromValue(GObjectReference(alignment))}};
        openTask = factory->createViewTask("Restored chromatogram alignment", state);
    } else {
        GObjectSelection selection;
        selection.addToSelection(alignment);
        MultiGSelection multiSelection;
        multiSelection.addSelection(&selection);
        openTask = factory->createViewTask(multiSelection, true);
    }
    CHECK_EXT(openTask != nullptr, setError("View opening task was not created"), {});
    return {openTask};
}

Task::ReportResult GTest_OpenMcaDatabase::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    CHECK_EXT(openTask != nullptr && alignment != nullptr, setError("View opening was not tested"), ReportResult_Finished);
    const auto views = GObjectViewUtils::findViewsWithObject(alignment);
    if (corrupt) {
        CHECK_EXT(openTask->getError().contains("database disk image is malformed"),
                  setError("Expected database corruption error, got: " + openTask->getError()),
                  ReportResult_Finished);
        CHECK_EXT(views.isEmpty(), setError("An editor was created for a corrupt database"), ReportResult_Finished);
    } else {
        CHECK_EXT(!openTask->hasError(), setError(openTask->getError()), ReportResult_Finished);
        CHECK_EXT(views.size() == 1, setError("An editor was not created for a valid database"), ReportResult_Finished);
        CHECK_EXT(alignment->getAlphabet() != nullptr && alignment->getRowCount() > 0,
                  setError("Valid alignment data was not loaded"),
                  ReportResult_Finished);
    }
    return ReportResult_Finished;
}

void GTest_OpenMcaDatabase::cleanup() {
    if (alignment != nullptr) {
        for (auto view : GObjectViewUtils::findViewsWithObject(alignment)) {
            AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(view);
        }
    }
    if (document != nullptr) {
        AppContext::getProject()->removeDocument(document);
    }
    XmlTest::cleanup();
}

const QString GTest_CompareTwoMca::DOC1_ATTR = "doc1";
const QString GTest_CompareTwoMca::DOC2_ATTR = "doc2";

void GTest_CompareTwoMca::init(XMLTestFormat*, const QDomElement& element) {
    docContextName = element.attribute(DOC1_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC1_ATTR);
        return;
    }

    secondDocContextName = element.attribute(DOC2_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(DOC2_ATTR);
        return;
    }
}

Task::ReportResult GTest_CompareTwoMca::report() {
    Document* doc1 = getContext<Document>(this, docContextName);
    CHECK_EXT(doc1 != nullptr, setError(QString("document not found: %1").arg(docContextName)), ReportResult_Finished);
    const QList<GObject*> objs1 = doc1->getObjects();
    CHECK_EXT(1 == objs1.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(docContextName)), ReportResult_Finished);
    auto mca1 = qobject_cast<MsaObject*>(objs1.first());
    CHECK_EXT(mca1 != nullptr, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'").arg(docContextName).arg(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT).arg(objs1.first()->getGObjectType())), ReportResult_Finished);

    Document* doc2 = getContext<Document>(this, secondDocContextName);
    CHECK_EXT(doc2 != nullptr, setError(QString("document not found: %1").arg(secondDocContextName)), ReportResult_Finished);
    const QList<GObject*> objs2 = doc2->getObjects();
    CHECK_EXT(1 == objs2.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(secondDocContextName)), ReportResult_Finished);
    auto mca2 = qobject_cast<MsaObject*>(objs2.first());
    CHECK_EXT(mca2 != nullptr, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'").arg(secondDocContextName).arg(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT).arg(objs2.first()->getGObjectType())), ReportResult_Finished);

    const qint64 rowsNumber1 = mca1->getRowCount();
    const qint64 rowsNumber2 = mca2->getRowCount();
    CHECK_EXT(rowsNumber1 == rowsNumber2,
              setError(QString("The rows numbers differ: the object '%1' from the document '%2' contains %3 rows, the object '%4' from the document '%5' contains %6 rows")
                           .arg(mca1->getGObjectName())
                           .arg(docContextName)
                           .arg(rowsNumber1)
                           .arg(mca2->getGObjectName())
                           .arg(secondDocContextName)
                           .arg(rowsNumber2)),
              ReportResult_Finished);

    for (int i = 0; i < rowsNumber1; i++) {
        const MsaRow& row1 = mca1->getRow(i);
        const MsaRow& row2 = mca2->getRow(i);
        const bool areEqual = row1->isEqualCore(*row2);
        CHECK_EXT(areEqual, setError(QString("The rows with number %1 differ from each other").arg(i)), ReportResult_Finished);
    }

    U2SequenceObject* reference1 = mca1->getReferenceObj();
    U2SequenceObject* reference2 = mca2->getReferenceObj();
    CHECK_EXT((reference1 == nullptr) == (reference2 == nullptr),
              setError("One object has a reference, but another one doesn't"),
              ReportResult_Finished);

    CHECK(reference1 != nullptr && reference2 != nullptr, ReportResult_Finished);

    QByteArray reference1Sequence = reference1->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    QByteArray reference2Sequence = reference2->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    CHECK_EXT(reference1Sequence == reference2Sequence, setError("References are not equal"), ReportResult_Finished);

    return ReportResult_Finished;
}

QList<XMLTestFactory*> McaObjectTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_OpenMcaDatabase::createFactory());
    res.append(GTest_CompareTwoMca::createFactory());
    return res;
}

}  // namespace U2
