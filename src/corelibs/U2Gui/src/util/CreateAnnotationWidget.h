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

#ifndef _U2_CREATE_ANNOTATION_WIDGET_H_
#define _U2_CREATE_ANNOTATION_WIDGET_H_

#include <QWidget>

#include <U2Gui/ShowHideSubgroupWidget.h>

#include "CreateAnnotationWidgetController.h"
#include "ui_CreateAnnotationFullWidget.h"
#include "ui_CreateAnnotationNormalWidget.h"
#include "ui_CreateAnnotationOptionsPanelWidget.h"

namespace U2 {

class GObjectComboBoxControllerConstraints;
class SaveDocumentControllerConfig;
class ShowHideSubgroupWidget;

class CreateAnnotationWidget : public QWidget {
    Q_OBJECT
    friend class CreateAnnotationWidgetController;
public:
    CreateAnnotationWidget(QWidget *parent = NULL);

    virtual void setGroupNameVisible(bool visible) = 0;
    virtual void setLocationVisible(bool visible) = 0;
    virtual void setAnnotationParametersVisible(bool visible) = 0;
    virtual void setAnnotationTypeVisible(bool visible) = 0;
    virtual void setAnnotationNameVisible(bool visible) = 0;
    virtual void setAutoTableOptionVisible(bool visible) = 0;
    virtual void setDescriptionVisible(bool visible) = 0;
    virtual void setUsePatternNamesVisible(bool visible) = 0;
    virtual void setAnnotationTableOptionVisible(bool visible) = 0;

    virtual void setAnnotationNameEnabled(bool enable) = 0;

    virtual void useAminoAnnotationTypes(bool useAmino) = 0;

    virtual void focusGroupName() = 0;
    virtual void focusAnnotationType() = 0;
    virtual void focusAnnotationName() = 0;
    virtual void focusLocation() = 0;

    virtual void setNewTablePath(const QString &path) = 0;
    virtual void setGroupName(const QString &name) = 0;
    virtual void setAnnotationType(U2FeatureType featureType) = 0;
    virtual void setAnnotationName(const QString &name) = 0;
    virtual void setLocation(const U2Location &location) = 0;
    virtual void setDescription(const QString &description) = 0;

    virtual QString getAnnotationTypeString() const = 0;
    virtual QString getGroupName() const = 0;
    virtual QString getAnnotationName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QString getLocationString() const = 0;
    virtual bool isUsePatternNamesChecked() const = 0;

    virtual bool isExistingTablesListEmpty() const = 0;

    virtual void selectNewTableOption() = 0;
    virtual void selectAutoTableOption() = 0;

    virtual void setExistingTableOptionEnable(bool enable) = 0;

    virtual bool isNewTableOptionSelected() const = 0;
    virtual bool isExistingTableOptionSelected() const = 0;
    virtual bool isAutoTableOptionSelected() const = 0;

    virtual void showSelectGroupMenu(QMenu &menu) = 0;
    QPair<QWidget*, QWidget*> getTabOrderEntryAndExitPoints() const;
    virtual GObjectComboBoxController * createGObjectComboBoxController(const GObjectComboBoxControllerConstraints &constraints) = 0;

    virtual void countDescriptionUsage() const = 0;
    virtual void fillSaveDocumentControllerConfig(SaveDocumentControllerConfig &config) const = 0;

signals:
    void si_selectExistingTableRequest();
    void si_selectGroupNameMenuRequest();
    void si_groupNameEdited();
    void si_annotationNameEdited();
    void si_usePatternNamesStateChanged();

protected slots:
    void sl_selectExistingTableRequest();
    void sl_selectGroupNameMenuRequest();
    void sl_groupNameEdited();
    void sl_annotationNameEdited();
    void sl_usePatternNamesStateChanged();

private slots:
    void sl_complementLocation();

protected:
    static QString getGenbankLocationString(const U2Location &location);
    static U2Location parseGenbankLocationString(const QString &locationString);
    static bool isComplementLocation(const QString &locationString);
    static QStringList getFeatureTypes(bool useAminoAnnotationTypes);
    static bool caseInsensitiveLessThan(const QString &first, const QString &second);
};

}   // namespace U2

#endif // _U2_CREATE_ANNOTATION_WIDGET_H_
