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

#include "CreateAnnotationWidgetController.h"

#include <QLabel>
#include <QMainWindow>
#include <QMenu>

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/SaveDocumentController.h>

#include "CreateAnnotationFullWidget.h"
#include "CreateAnnotationNormalWidget.h"
#include "CreateAnnotationOptionsPanelWidget.h"
#include "GObjectComboBoxController.h"

namespace U2 {

CreateAnnotationModel::CreateAnnotationModel()
    : data(new AnnotationData()) {
}

AnnotationTableObject* CreateAnnotationModel::getAnnotationObject() const {
    GObject* res = GObjectUtils::selectObjectByReference(annotationObjectRef, UOF_LoadedOnly);
    auto aobj = qobject_cast<AnnotationTableObject*>(res);
    SAFE_POINT(aobj != nullptr, "Invalid annotation table detected!", nullptr);
    return aobj;
}

const QString CreateAnnotationWidgetController::SETTINGS_LASTDIR = "create_annotation/last_dir";

CreateAnnotationWidgetController::CreateAnnotationWidgetController(const CreateAnnotationModel& m,
                                                                   QObject* p,
                                                                   AnnotationWidgetMode layoutMode)
    : QObject(p),
      model(m),
      saveController(nullptr) {
    GROUP_NAME_AUTO = tr("<auto>");

    this->setObjectName("CreateAnnotationWidgetController");
    assert(AppContext::getProject() != nullptr);
    assert(model.sequenceObjectRef.isValid());

    createWidget(layoutMode);

    GObjectComboBoxControllerConstraints occc;
    occc.relationFilter.ref = model.sequenceObjectRef;
    occc.relationFilter.role = ObjectRole_Sequence;
    occc.typeFilter = GObjectTypes::ANNOTATION_TABLE;
    occc.onlyWritable = true;
    occc.uof = model.useUnloadedObjects ? UOF_LoadedAndUnloaded : UOF_LoadedOnly;
    occ = w->createGObjectComboBoxController(occc);

    commonWidgetUpdate();

    connect(w, SIGNAL(si_selectExistingTableRequest()), SLOT(sl_onLoadObjectsClicked()));
    connect(w, SIGNAL(si_selectGroupNameMenuRequest()), SLOT(sl_groupName()));
    connect(w, SIGNAL(si_groupNameEdited()), SLOT(sl_groupNameEdited()));
    connect(w, SIGNAL(si_annotationNameEdited()), SLOT(sl_annotationNameEdited()));
    connect(w, SIGNAL(si_usePatternNamesStateChanged()), SLOT(sl_usePatternNamesStateChanged()));
    connect(occ, SIGNAL(si_comboBoxChanged()), SLOT(sl_documentsComboUpdated()));
}

CreateAnnotationWidgetController::~CreateAnnotationWidgetController() {
    // Do not remove the empty destructor because https://doc.qt.io/qt-5/qscopedpointer.html#forward-declared-pointers
}

void CreateAnnotationWidgetController::updateWidgetForAnnotationModel(const CreateAnnotationModel& newModel) {
    SAFE_POINT(newModel.sequenceObjectRef.isValid(),
               "Internal error: incorrect sequence object reference was supplied"
               "to the annotation widget controller.", );

    model = newModel;

    GObjectComboBoxControllerConstraints occc;
    occc.relationFilter.ref = newModel.sequenceObjectRef;
    occc.relationFilter.role = ObjectRole_Sequence;
    occc.typeFilter = GObjectTypes::ANNOTATION_TABLE;
    occc.onlyWritable = true;
    occc.uof = newModel.useUnloadedObjects ? UOF_LoadedAndUnloaded : UOF_LoadedOnly;

    occ->updateConstrains(occc);

    commonWidgetUpdate();
}

void CreateAnnotationWidgetController::commonWidgetUpdate() {
    w->setLocationVisible(!model.hideLocation);
    w->setAnnotationNameVisible(!model.hideAnnotationName);

    initSaveController();

    if (model.annotationObjectRef.isValid()) {
        occ->setSelectedObject(model.annotationObjectRef);
    }

    // default field values

    w->setAnnotationName(model.data->name);
    w->setGroupName(model.groupName.isEmpty() ? GROUP_NAME_AUTO : model.groupName);
    w->setDescription(model.description);

    if (!model.data->location->isEmpty()) {
        w->setLocation(model.data->location);
    }

    if (w->isExistingTablesListEmpty()) {
        w->setExistingTableOptionEnable(false);
        w->selectNewTableOption();
    } else {
        w->setExistingTableOptionEnable(true);
        if (model.defaultIsNewDoc) {
            w->selectNewTableOption();
        }
    }

    w->setAnnotationTableOptionVisible(!model.hideAnnotationTableOption);
    w->setAutoTableOptionVisible(!model.hideAutoAnnotationsOption);
    if (!model.hideAutoAnnotationsOption) {
        w->selectAutoTableOption();
    }

    w->setGroupNameVisible(!model.hideGroupName);
    w->setDescriptionVisible(!model.hideDescription);
    w->setAnnotationTypeVisible(!model.hideAnnotationType);
    w->setAnnotationParametersVisible(!model.hideAnnotationParameters);
    w->setUsePatternNamesVisible(!model.hideUsePatternNames);

    w->useAminoAnnotationTypes(model.useAminoAnnotationTypes);
    if (model.data->type != U2FeatureTypes::Invalid) {
        w->setAnnotationType(model.data->type);
    }
}

class PTCAnnotationObjectFilter : public PTCObjectRelationFilter {
public:
    PTCAnnotationObjectFilter(const GObjectRelation& _rel, bool _allowUnloaded, QObject* p = nullptr)
        : PTCObjectRelationFilter(_rel, p), allowUnloaded(_allowUnloaded) {
    }

    bool filter(GObject* obj) const {
        if (PTCObjectRelationFilter::filter(obj)) {
            return true;
        }
        if (obj->isUnloaded()) {
            return !allowUnloaded;
        }
        SAFE_POINT(qobject_cast<AnnotationTableObject*>(obj) != nullptr, "Invalid annotation table object!", false);
        return obj->isStateLocked();
    }
    bool allowUnloaded;
};

void CreateAnnotationWidgetController::sl_onLoadObjectsClicked() {
    ProjectTreeControllerModeSettings s;
    s.allowMultipleSelection = false;
    s.objectTypesToShow.insert(GObjectTypes::ANNOTATION_TABLE);
    s.groupMode = ProjectTreeGroupMode_ByDocument;
    GObjectRelation rel(model.sequenceObjectRef, ObjectRole_Sequence);
    QScopedPointer<PTCAnnotationObjectFilter> filter(new PTCAnnotationObjectFilter(rel, model.useUnloadedObjects));
    s.objectFilter = filter.data();
    QList<GObject*> objs = ProjectTreeItemSelectorDialog::selectObjects(s, AppContext::getMainWindow()->getQMainWindow());
    if (objs.isEmpty()) {
        return;
    }
    assert(objs.size() == 1);
    GObject* obj = objs.first();
    occ->setSelectedObject(obj);
}

QString CreateAnnotationWidgetController::validate() {
    updateModel(true);
    if (!model.annotationObjectRef.isValid()) {
        if (model.newDocUrl.isEmpty()) {
            return tr("Select annotation saving parameters");
        }
        if (AppContext::getProject()->findDocumentByURL(model.newDocUrl) != nullptr) {
            return tr("Document is already added to the project: '%1'").arg(model.newDocUrl);
        }
        QString dirUrl = QFileInfo(saveController->getSaveFileName()).absoluteDir().absolutePath();
        QDir dir(dirUrl);
        if (!dir.exists()) {
            return tr("Illegal folder: %1").arg(dirUrl);
        }
    }

    if (!w->isUsePatternNamesChecked() && !model.hideAnnotationName && !Annotation::isValidAnnotationName(model.data->name)) {
        return tr("Illegal annotation name! ");
    }

    if (model.groupName.isEmpty()) {
        w->focusGroupName();
        return tr("Illegal group name");
    }

    if (!model.hideLocation && model.data->location->isEmpty()) {
        w->focusLocation();
        return tr("Invalid location! Location must be in GenBank format.\nSimple examples:\n1..10\njoin(1..10,15..45)\ncomplement(5..15)");
    }
    if (!model.hideLocation) {
        foreach (const U2Region& reg, model.data->getRegions()) {
            if (reg.endPos() > model.sequenceLen || reg.startPos < 0 || reg.endPos() < reg.startPos) {
                return tr("Invalid location! Location must be in GenBank format.\nSimple examples:\n1..10\njoin(1..10,15..45)\ncomplement(5..15)");
            }
        }
    }

    return "";
}

void CreateAnnotationWidgetController::updateModel(bool forValidation) {
    model.data->type = U2FeatureTypes::getTypeByName(w->getAnnotationTypeString());

    model.data->name = w->getAnnotationName();
    if (model.data->name.isEmpty()) {
        model.data->name = U2FeatureTypes::getVisualName(model.data->type);
    }

    model.groupName = w->getGroupName();
    if (model.groupName == GROUP_NAME_AUTO || model.groupName.isEmpty()) {
        model.groupName = model.data->name;
    }

    model.data->location->reset();

    if (!model.hideLocation) {
        QByteArray locEditText = w->getLocationString().toLatin1();
        Genbank::LocationParser::parseLocation(locEditText.constData(),
                                               locEditText.length(),
                                               model.data->location,
                                               model.sequenceLen);
    }

    model.description = w->getDescription();
    if (forValidation) {
        U1AnnotationUtils::addDescriptionQualifier(model.data, model.description);
    }

    if (w->isExistingTableOptionSelected()) {
        model.annotationObjectRef = occ->getSelectedObjectReference();
        model.newDocUrl = "";
    } else {
        if (!forValidation) {
            model.annotationObjectRef = GObjectReference();
        }
        model.newDocUrl = saveController->getSaveFileName();
    }
}

void CreateAnnotationWidgetController::createWidget(CreateAnnotationWidgetController::AnnotationWidgetMode layoutMode) {
    switch (layoutMode) {
        case Full:
            w = new CreateAnnotationFullWidget(model.sequenceLen);
            break;
        case Normal:
            w = new CreateAnnotationNormalWidget();
            break;
        case OptionsPanel:
            w = new CreateAnnotationOptionsPanelWidget();
            break;
        default:
            w = nullptr;
            FAIL("Unexpected widget type", );
    }
}

QString CreateAnnotationWidgetController::defaultDir() {
    QString dir = AppContext::getSettings()->getValue(SETTINGS_LASTDIR, QString(""), true).toString();
    if (dir.isEmpty() || !QDir(dir).exists()) {
        dir = GUrlUtils::getDefaultDataPath();
        Project* prj = AppContext::getProject();
        if (prj != nullptr) {
            const QString& prjUrl = prj->getProjectURL();
            if (!prjUrl.isEmpty()) {
                QFileInfo fi(prjUrl);
                const QDir& prjDir = fi.absoluteDir();
                dir = prjDir.absolutePath();
            }
        }
    }
    return dir;
}

void CreateAnnotationWidgetController::initSaveController() {
    SaveDocumentControllerConfig conf;
    conf.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    conf.defaultDomain = SETTINGS_LASTDIR;
    conf.defaultFileName = defaultDir() + "/MyDocument.gb";
    conf.parentWidget = w;
    conf.saveTitle = tr("Save File");
    conf.rollOutProjectUrls = true;
    w->fillSaveDocumentControllerConfig(conf);

    QList<DocumentFormatId> formats;
    formats << BaseDocumentFormats::PLAIN_GENBANK;

    delete saveController;
    saveController = new SaveDocumentController(conf, formats, this);
}

bool CreateAnnotationWidgetController::isAnnotationsTableVirtual() {
    return qHash(occ->getSelectedObjectReference()) == qHash(model.sequenceObjectRef);
}

bool CreateAnnotationWidgetController::prepareAnnotationObject() {
    updateModel(false);
    QString v = validate();
    if (w->isExistingTableOptionSelected() && isAnnotationsTableVirtual()) {
        Document* d = AppContext::getProject()->findDocumentByURL(model.sequenceObjectRef.docUrl);
        SAFE_POINT(d != nullptr, "cannot create a annotation table in same document", false);
        U2OpStatusImpl os;
        const U2DbiRef localDbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        SAFE_POINT_OP(os, false);
        AnnotationTableObject* ann = new AnnotationTableObject(model.sequenceObjectRef.objName + FEATURES_TAG, localDbiRef);
        ann->addObjectRelation(GObjectRelation(model.sequenceObjectRef, ObjectRole_Sequence));
        d->addObject(ann);
        occ->setSelectedObject(ann);
        model.annotationObjectRef = ann->getReference();
    }
    SAFE_POINT(v.isEmpty(), "Annotation model is not valid", false);
    if (!model.annotationObjectRef.isValid() && w->isNewTableOptionSelected()) {
        SAFE_POINT(!model.newDocUrl.isEmpty(), "newDocUrl is empty", false);
        SAFE_POINT(AppContext::getProject()->findDocumentByURL(model.newDocUrl) == nullptr, "cannot create a document that is already in the project", false);
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
        U2OpStatus2Log os;
        Document* d = df->createNewLoadedDocument(iof, model.newDocUrl, os);
        CHECK_OP(os, false);
        const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        SAFE_POINT_OP(os, false);
        AnnotationTableObject* aobj = new AnnotationTableObject("Annotations", dbiRef);
        aobj->addObjectRelation(GObjectRelation(model.sequenceObjectRef, ObjectRole_Sequence));
        d->addObject(aobj);
        AppContext::getProject()->addDocument(d);
        model.annotationObjectRef = aobj->getReference();
    }

    return true;
}

void CreateAnnotationWidgetController::sl_groupName() {
    GObject* obj = occ->getSelectedObject();
    QStringList groupNames = {GROUP_NAME_AUTO};
    if (obj != nullptr && !obj->isUnloaded() && !isAnnotationsTableVirtual()) {
        auto ao = qobject_cast<AnnotationTableObject*>(obj);
        ao->getRootGroup()->getSubgroupPaths(groupNames);
    }
    SAFE_POINT(!groupNames.isEmpty(), "Unable to find annotation groups!", );
    if (groupNames.size() == 1) {
        w->setGroupName(groupNames.first());
        return;
    }
    std::sort(groupNames.begin(), groupNames.end());

    QMenu menu(w);
    for (const QString& str : qAsConst(groupNames)) {
        QAction* a = new QAction(str, &menu);
        connect(a, SIGNAL(triggered()), SLOT(sl_setPredefinedGroupName()));
        menu.addAction(a);
    }
    w->showSelectGroupMenu(menu);
}

void CreateAnnotationWidgetController::sl_setPredefinedGroupName() {
    auto a = qobject_cast<QAction*>(sender());
    QString name = a->text();
    w->setGroupName(name);
}

bool CreateAnnotationWidgetController::isNewObject() const {
    return w->isNewTableOptionSelected();
}

void CreateAnnotationWidgetController::setFocusToNameEdit() {
    w->focusAnnotationName();
}

void CreateAnnotationWidgetController::setEnabledNameEdit(bool enbaled) {
    w->setAnnotationNameEnabled(enbaled);
}

bool CreateAnnotationWidgetController::useAutoAnnotationModel() const {
    return w->isAutoTableOptionSelected();
}

void CreateAnnotationWidgetController::setFocusToAnnotationType() {
    w->focusAnnotationType();
}

void CreateAnnotationWidgetController::sl_documentsComboUpdated() {
    commonWidgetUpdate();
}

void CreateAnnotationWidgetController::sl_annotationNameEdited() {
    emit si_annotationNamesEdited();
}

void CreateAnnotationWidgetController::sl_groupNameEdited() {
    emit si_annotationNamesEdited();
}

void CreateAnnotationWidgetController::sl_usePatternNamesStateChanged() {
    emit si_usePatternNamesStateChanged();
}

QWidget* CreateAnnotationWidgetController::getWidget() const {
    return w;
}

AnnotationCreationPattern CreateAnnotationWidgetController::getAnnotationPattern() const {
    AnnotationCreationPattern pattern;
    pattern.annotationName = model.data->name;
    pattern.type = model.data->type;
    pattern.groupName = model.groupName;
    pattern.description = model.description;
    return pattern;
}

QPair<QWidget*, QWidget*> CreateAnnotationWidgetController::getTaborderEntryAndExitPoints() const {
    return w->getTabOrderEntryAndExitPoints();
}

void CreateAnnotationWidgetController::countDescriptionUsage() const {
    w->countDescriptionUsage();
}

}  // namespace U2
