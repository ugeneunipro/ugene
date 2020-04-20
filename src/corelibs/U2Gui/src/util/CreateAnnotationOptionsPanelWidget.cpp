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

#include <QMenu>

#include <U2Core/Counter.h>

#include <U2Gui/GObjectComboBoxController.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/ShowHideSubgroupWidget.h>

#include "CreateAnnotationOptionsPanelWidget.h"

namespace U2 {

CreateAnnotationOptionsPanelWidget::CreateAnnotationOptionsPanelWidget(QWidget *parent) :
    CreateAnnotationWidget(parent)
{
    setupUi(this);
    initLayout();
    init();
    connectSignals();
}

void CreateAnnotationOptionsPanelWidget::setGroupNameVisible(bool visible) {
    lblGroupName->setVisible(visible);
    leGroupName->setVisible(visible);
    tbSelectGroupName->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setLocationVisible(bool visible) {
    lblLocation->setVisible(visible);
    leLocation->setVisible(visible);
    tbDoComplement->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationParametersVisible(bool visible) {
    annotationParametersWidget->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationTypeVisible(bool visible) {
    lblAnnotationType->setVisible(visible);
    cbAnnotationType->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationNameVisible(bool visible) {
    lblAnnotationName->setVisible(visible);
    leAnnotationName->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAutoTableOptionVisible(bool visible) {
    rbUseAutoTable->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setDescriptionVisible(bool visible) {
    lblDescription->setVisible(visible);
    leDescription->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setUsePatternNamesVisible(bool visible) {
    chbUsePatternNames->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationTableOptionVisible(bool visible) {
    annotationParametersWidget->setVisible(visible);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationNameEnabled(bool enable) {
    leAnnotationName->setEnabled(enable);
}

void CreateAnnotationOptionsPanelWidget::useAminoAnnotationTypes(bool useAmino) {
    const QStringList featureTypes = getFeatureTypes(useAmino);
    cbAnnotationType->clear();
    cbAnnotationType->addItems(featureTypes);
    cbAnnotationType->setCurrentIndex(featureTypes.indexOf(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature)));
}

void CreateAnnotationOptionsPanelWidget::focusGroupName() {
    leGroupName->setFocus();
}

void CreateAnnotationOptionsPanelWidget::focusAnnotationType() {
    cbAnnotationType->setFocus();
}

void CreateAnnotationOptionsPanelWidget::focusAnnotationName() {
    leAnnotationName->setFocus();
}

void CreateAnnotationOptionsPanelWidget::focusLocation() {
    leLocation->setFocus();
}

void CreateAnnotationOptionsPanelWidget::setNewTablePath(const QString &path) {
    leNewTablePath->setText(path);
}

void CreateAnnotationOptionsPanelWidget::setGroupName(const QString &name) {
    leGroupName->setText(name);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationType(U2FeatureType type) {
    int index = cbAnnotationType->findText(U2FeatureTypes::getVisualName(type));
    if (Q_UNLIKELY(index == -1)) {
        Q_ASSERT(false);    // an incorrect type
        index = cbAnnotationType->findText(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature));
    }
    cbAnnotationType->setCurrentIndex(index);
}

void CreateAnnotationOptionsPanelWidget::setAnnotationName(const QString &name) {
    leAnnotationName->setText(name);
}

void CreateAnnotationOptionsPanelWidget::setLocation(const U2Location &location) {
    leLocation->setText(getGenbankLocationString(location));
}

void CreateAnnotationOptionsPanelWidget::setDescription(const QString &description) {
    leDescription->setText(description);
}

QString CreateAnnotationOptionsPanelWidget::getAnnotationTypeString() const {
    return cbAnnotationType->currentText();
}

QString CreateAnnotationOptionsPanelWidget::getGroupName() const {
    return leGroupName->text();
}

QString CreateAnnotationOptionsPanelWidget::getAnnotationName() const {
    return leAnnotationName->text();
}

QString CreateAnnotationOptionsPanelWidget::getLocationString() const {
    return leLocation->text();
}

QString CreateAnnotationOptionsPanelWidget::getDescription() const {
    return leDescription->text();
}

bool CreateAnnotationOptionsPanelWidget::isUsePatternNamesChecked() const {
    return chbUsePatternNames->isChecked();
}

bool CreateAnnotationOptionsPanelWidget::isExistingTablesListEmpty() const {
    return cbExistingTable->count() == 0;
}

void CreateAnnotationOptionsPanelWidget::selectNewTableOption() {
    rbCreateNewTable->setChecked(true);
}

void CreateAnnotationOptionsPanelWidget::selectAutoTableOption() {
    rbUseAutoTable->setChecked(true);
}

void CreateAnnotationOptionsPanelWidget::setExistingTableOptionEnable(bool enable) {
    rbExistingTable->setCheckable(enable);
    rbExistingTable->setEnabled(enable);
    cbExistingTable->setEnabled(enable && rbExistingTable->isChecked());
    tbBrowseExistingTable->setEnabled(enable && rbExistingTable->isChecked());
}

bool CreateAnnotationOptionsPanelWidget::isNewTableOptionSelected() const {
    return rbCreateNewTable->isChecked();
}

bool CreateAnnotationOptionsPanelWidget::isExistingTableOptionSelected() const {
    return rbExistingTable->isChecked();
}

bool CreateAnnotationOptionsPanelWidget::isAutoTableOptionSelected() const {
    return rbUseAutoTable->isChecked();
}

void CreateAnnotationOptionsPanelWidget::showSelectGroupMenu(QMenu &menu) {
    const QPoint menuPos = tbSelectGroupName->mapToGlobal(tbSelectGroupName->rect().bottomLeft());
    menu.exec(menuPos);
}

GObjectComboBoxController * CreateAnnotationOptionsPanelWidget::createGObjectComboBoxController(const GObjectComboBoxControllerConstraints &constraints) {
    return new GObjectComboBoxController(this, constraints, cbExistingTable);
}

void CreateAnnotationOptionsPanelWidget::countDescriptionUsage() const {
    if (!leDescription->text().isEmpty()) {
        GCOUNTER(cvar, tvar, "CreateAnnotationOptionsPanelWidget: description is used");
    }
}

void CreateAnnotationOptionsPanelWidget::fillSaveDocumentControllerConfig(SaveDocumentControllerConfig &config) const {
    config.fileNameEdit = leNewTablePath;
    config.fileDialogButton = tbBrowseNewTable;
}

void CreateAnnotationOptionsPanelWidget::initLayout() {
    ShowHideSubgroupWidget *saveShowHideWidget = new ShowHideSubgroupWidget("save_params", tr("Save annotation(s) to"), saveAnnotationsInnerWidget, false);
    mainLayout->insertWidget(0, saveShowHideWidget);

    annotationParametersWidget = new ShowHideSubgroupWidget("annotparams", tr("Annotation parameters"), annotationParametersInnerWidget, false);
    mainLayout->insertWidget(1, annotationParametersWidget);
}

void CreateAnnotationOptionsPanelWidget::init() {
    useAminoAnnotationTypes(false);
}

void CreateAnnotationOptionsPanelWidget::connectSignals() {
    connect(tbBrowseExistingTable, SIGNAL(clicked()), SIGNAL(si_selectExistingTableRequest()));
    connect(tbSelectGroupName, SIGNAL(clicked()), SIGNAL(si_selectGroupNameMenuRequest()));
    connect(tbDoComplement, SIGNAL(clicked()), SLOT(sl_complementLocation()));
    connect(leGroupName, SIGNAL(textEdited(const QString &)), SIGNAL(si_groupNameEdited()));
    connect(leGroupName, SIGNAL(textChanged(const QString &)), SIGNAL(si_groupNameEdited()));
    connect(leAnnotationName, SIGNAL(textEdited(const QString &)), SIGNAL(si_annotationNameEdited()));
    connect(leAnnotationName, SIGNAL(textChanged(const QString &)), SIGNAL(si_annotationNameEdited()));
    connect(chbUsePatternNames, SIGNAL(stateChanged(int)), SIGNAL(si_usePatternNamesStateChanged()));
}

}   // namespace U2
