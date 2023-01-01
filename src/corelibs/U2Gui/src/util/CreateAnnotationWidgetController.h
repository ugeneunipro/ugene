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

#ifndef _U2_CREATE_ANNOTATION_WIDGET_CONTROLLER_H_
#define _U2_CREATE_ANNOTATION_WIDGET_CONTROLLER_H_

#include <U2Core/AnnotationCreationPattern.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/GObjectReference.h>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QRadioButton;
class QToolButton;

namespace U2 {

class AnnotationTableObject;
class CreateAnnotationWidget;
class GObjectComboBoxController;
class SaveDocumentController;
class ShowHideSubgroupWidget;

class U2GUI_EXPORT CreateAnnotationModel {
public:
    CreateAnnotationModel();

    CreateAnnotationModel(CreateAnnotationModel const&) = default;

    CreateAnnotationModel& operator=(const CreateAnnotationModel& other) = default;

    /** A sequence object the new annotation will be created for. */
    GObjectReference sequenceObjectRef;

    /** If 'true' "New Document" option is selected. */
    bool defaultIsNewDoc = false;

    /** Disables "Annotation group field". */
    bool hideGroupName = false;

    /** Hides location field and does not run location validation at all. */
    bool hideLocation = false;

    /** Hides "Annotation Type" list. */
    bool hideAnnotationType = false;

    /** Hides annotation name input. */
    bool hideAnnotationName = false;

    /** Hides annotation description input. */
    bool hideDescription = false;

    /** Hides pattern names checkbox. */
    bool hideUsePatternNames = true;

    /** Enables unloaded documents in the "Existing document" selector. */
    bool useUnloadedObjects = false;

    /** Populates annotation type list with amino sequence specific feature types. */
    bool useAminoAnnotationTypes = false;

    /** Annotation group name. If empty the group name is automatically selected from the current annotation type (name). */
    QString groupName;

    /** Annotation description. Saved as a qualifier. */
    QString description;

    /** An existing annotation table object to save the annotation. */
    GObjectReference annotationObjectRef;

    /** A file path of the annotation table document to save the annotation. */
    QString newDocUrl;

    /** Length of the sequence object. Used for validation. */
    qint64 sequenceLen;

    /**  Hides the target annotation table document selector widget: a new file and existing docs options. */
    bool hideAnnotationTableOption = false;

    /** Hides "use auto-annotation table as a target" option. */
    bool hideAutoAnnotationsOption = true;

    /** Hides all annotation parameters except location selector. Effective only in Normal & Option Panel widgets. */
    bool hideAnnotationParameters = false;

    /** Current annotation data model. */
    SharedAnnotationData data;

    AnnotationTableObject* getAnnotationObject() const;
};

class U2GUI_EXPORT CreateAnnotationWidgetController : public QObject {
    Q_OBJECT
public:
    enum AnnotationWidgetMode {
        Full,
        Normal,
        OptionsPanel
    };

    // useCompact defines the layout of the widget (normal or compact for the Options Panel)
    CreateAnnotationWidgetController(const CreateAnnotationModel& m, QObject* p, AnnotationWidgetMode layoutMode = Normal);
    ~CreateAnnotationWidgetController();

    // returns error message or empty string if no error found;
    // does not create any new objects
    QString validate();

    // Ensures that annotationObeject is valid
    // for a validated model creates new document (newDocUrl), adds annotation table object
    // to the document created and stores the reference to the object to annotationObject
    // Does nothing if annotationObject is not NULL
    // returns true if annotation object is prepared
    // return false otherwise
    bool prepareAnnotationObject();

    // add model property instead ??
    bool isNewObject() const;

    // property of GUI
    bool useAutoAnnotationModel() const;

    void setFocusToAnnotationType();
    void setFocusToNameEdit();

    void setEnabledNameEdit(bool enbaled);

    QWidget* getWidget() const;

    const CreateAnnotationModel& getModel() const {
        return model;
    }
    AnnotationCreationPattern getAnnotationPattern() const;

    void updateWidgetForAnnotationModel(const CreateAnnotationModel& model);

    /** It is called from the constructor and updateWidgetForAnnotationModel(...) */
    void commonWidgetUpdate();

    QPair<QWidget*, QWidget*> getTaborderEntryAndExitPoints() const;

    void countDescriptionUsage() const;

signals:
    void si_annotationNamesEdited();
    void si_usePatternNamesStateChanged();

private slots:
    void sl_onLoadObjectsClicked();
    void sl_groupName();
    void sl_setPredefinedGroupName();

    void sl_documentsComboUpdated();

    // edit slots
    void sl_annotationNameEdited();
    void sl_groupNameEdited();
    void sl_usePatternNamesStateChanged();

private:
    void updateModel(bool forValidation);
    void createWidget(AnnotationWidgetMode layoutMode);
    static QString defaultDir();
    void initSaveController();
    bool isAnnotationsTableVirtual();

    CreateAnnotationModel model;
    GObjectComboBoxController* occ;
    CreateAnnotationWidget* w;
    SaveDocumentController* saveController;

    QString GROUP_NAME_AUTO;
    static const QString DESCRIPTION_QUALIFIER_KEY;
    static const QString SETTINGS_LASTDIR;

    CreateAnnotationWidgetController(CreateAnnotationWidgetController&&) = delete;
    CreateAnnotationWidgetController& operator=(CreateAnnotationWidgetController&&) = delete;
    Q_DISABLE_COPY(CreateAnnotationWidgetController)
};

}  // namespace U2

#endif
