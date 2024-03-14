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

#pragma once

#include "CreateAnnotationWidget.h"
#include "ui_CreateAnnotationOptionsPanelWidget.h"

namespace U2 {

class ShowHideSubgroupWidget;

class CreateAnnotationOptionsPanelWidget : public CreateAnnotationWidget, private Ui_CreateAnnotationOptionsPanelWidget {
    Q_OBJECT
public:
    CreateAnnotationOptionsPanelWidget(QWidget* parent = nullptr);

    void setGroupNameVisible(bool visible) override;
    void setLocationVisible(bool visible) override;
    void setAnnotationParametersVisible(bool visible) override;
    void setAnnotationTypeVisible(bool visible) override;
    void setAnnotationNameVisible(bool visible) override;
    void setAutoTableOptionVisible(bool visible) override;
    void setDescriptionVisible(bool visible) override;
    void setUsePatternNamesVisible(bool visible) override;
    void setAnnotationTableOptionVisible(bool visible) override;

    void setAnnotationNameEnabled(bool enable) override;

    void useAminoAnnotationTypes(bool useAmino) override;

    void focusGroupName() override;
    void focusAnnotationType() override;
    void focusAnnotationName() override;
    void focusLocation() override;

    void setNewTablePath(const QString& path) override;
    void setGroupName(const QString& name) override;
    void setAnnotationType(U2FeatureType type) override;
    void setAnnotationName(const QString& name) override;
    void setLocation(const U2Location& location) override;
    void setDescription(const QString& description) override;

    QString getAnnotationTypeString() const override;
    QString getGroupName() const override;
    QString getAnnotationName() const override;
    QString getLocationString() const override;
    QString getDescription() const override;
    bool isUsePatternNamesChecked() const override;

    bool isExistingTablesListEmpty() const override;

    void selectNewTableOption() override;
    void selectAutoTableOption() override;

    void setExistingTableOptionEnable(bool enable) override;

    bool isNewTableOptionSelected() const override;
    bool isExistingTableOptionSelected() const override;
    bool isAutoTableOptionSelected() const override;

    void showSelectGroupMenu(QMenu& menu) override;
    GObjectComboBoxController* createGObjectComboBoxController(const GObjectComboBoxControllerConstraints& constraints) override;

    void countDescriptionUsage() const override;
    void fillSaveDocumentControllerConfig(SaveDocumentControllerConfig& config) const override;

private:
    void initLayout();
    void init();
    void connectSignals();

    ShowHideSubgroupWidget* annotationParametersWidget;
};

}  // namespace U2
