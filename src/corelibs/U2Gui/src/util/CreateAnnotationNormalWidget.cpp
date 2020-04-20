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

#include "CreateAnnotationNormalWidget.h"

namespace U2 {

CreateAnnotationNormalWidget::CreateAnnotationNormalWidget(QWidget *parent) :
    CreateAnnotationWidget(parent)
{
    setupUi(this);
    initLayout();
    init();
    connectSignals();
}

CreateAnnotationNormalWidget::~CreateAnnotationNormalWidget() {
    countDescriptionUsage();
}

void CreateAnnotationNormalWidget::setGroupNameVisible(bool visible) {
    lblGroupName->setVisible(visible);
    leGroupName->setVisible(visible);
    tbSelectGroupName->setVisible(visible);
}

void CreateAnnotationNormalWidget::setLocationVisible(bool visible) {
    lblLocation->setVisible(visible);
    leLocation->setVisible(visible);
    tbDoComplement->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAnnotationParametersVisible(bool visible) {
    annotationParametersWidget->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAnnotationTypeVisible(bool visible) {
    lblAnnotationType->setVisible(visible);
    cbAnnotationType->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAnnotationNameVisible(bool visible) {
    lblAnnotationName->setVisible(visible);
    leAnnotationName->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAutoTableOptionVisible(bool visible) {
    rbUseAutoTable->setVisible(visible);
}

void CreateAnnotationNormalWidget::setDescriptionVisible(bool visible) {
    lblDescription->setVisible(visible);
    leDescription->setVisible(visible);
}

void CreateAnnotationNormalWidget::setUsePatternNamesVisible(bool visible) {
    chbUsePatternNames->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAnnotationTableOptionVisible(bool visible) {
    saveAnnotationsInnerWidget->setVisible(visible);
}

void CreateAnnotationNormalWidget::setAnnotationNameEnabled(bool enable) {
    leAnnotationName->setEnabled(enable);
}

void CreateAnnotationNormalWidget::useAminoAnnotationTypes(bool useAmino) {
    const QStringList featureTypes = getFeatureTypes(useAmino);
    cbAnnotationType->clear();
    cbAnnotationType->addItems(featureTypes);
    cbAnnotationType->setCurrentIndex(featureTypes.indexOf(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature)));
}

void CreateAnnotationNormalWidget::focusGroupName() {
    leGroupName->setFocus();
}

void CreateAnnotationNormalWidget::focusAnnotationType() {
    cbAnnotationType->setFocus();
}

void CreateAnnotationNormalWidget::focusAnnotationName() {
    leAnnotationName->setFocus();
}

void CreateAnnotationNormalWidget::focusLocation() {
    leLocation->setFocus();
}

void CreateAnnotationNormalWidget::setNewTablePath(const QString &path) {
    leNewTablePath->setText(path);
}

void CreateAnnotationNormalWidget::setGroupName(const QString &name) {
    leGroupName->setText(name);
}

void CreateAnnotationNormalWidget::setAnnotationType(U2FeatureType type) {
    int index = cbAnnotationType->findText(U2FeatureTypes::getVisualName(type));
    if (Q_UNLIKELY(index == -1)) {
        Q_ASSERT(false);    // an incorrect type
        index = cbAnnotationType->findText(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature));
    }
    cbAnnotationType->setCurrentIndex(index);
}

void CreateAnnotationNormalWidget::setAnnotationName(const QString &name) {
    leAnnotationName->setText(name);
}

void CreateAnnotationNormalWidget::setLocation(const U2Location &location) {
    leLocation->setText(getGenbankLocationString(location));
}

void CreateAnnotationNormalWidget::setDescription(const QString &description) {
    leDescription->setText(description);
}

QString CreateAnnotationNormalWidget::getAnnotationTypeString() const {
    return cbAnnotationType->currentText();
}

QString CreateAnnotationNormalWidget::getGroupName() const {
    return leGroupName->text();
}

QString CreateAnnotationNormalWidget::getAnnotationName() const {
    return leAnnotationName->text();
}

QString CreateAnnotationNormalWidget::getLocationString() const {
    return leLocation->text();
}

QString CreateAnnotationNormalWidget::getDescription() const {
    return leDescription->text();
}

bool CreateAnnotationNormalWidget::isUsePatternNamesChecked() const {
    return chbUsePatternNames->isChecked();
}

bool CreateAnnotationNormalWidget::isExistingTablesListEmpty() const {
    return cbExistingTable->count() == 0;
}

void CreateAnnotationNormalWidget::selectNewTableOption() {
    rbCreateNewTable->setChecked(true);
}

void CreateAnnotationNormalWidget::selectAutoTableOption() {
    rbUseAutoTable->setChecked(true);
}

void CreateAnnotationNormalWidget::setExistingTableOptionEnable(bool enable) {
    rbExistingTable->setCheckable(enable);
    rbExistingTable->setEnabled(enable);
    cbExistingTable->setEnabled(enable && rbExistingTable->isChecked());
    tbBrowseExistingTable->setEnabled(enable && rbExistingTable->isChecked());
}

bool CreateAnnotationNormalWidget::isNewTableOptionSelected() const {
    return rbCreateNewTable->isChecked();
}

bool CreateAnnotationNormalWidget::isExistingTableOptionSelected() const {
    return rbExistingTable->isChecked();
}

bool CreateAnnotationNormalWidget::isAutoTableOptionSelected() const {
    return rbUseAutoTable->isChecked();
}

void CreateAnnotationNormalWidget::showSelectGroupMenu(QMenu &menu) {
    const QPoint menuPos = tbSelectGroupName->mapToGlobal(tbSelectGroupName->rect().bottomLeft());
    menu.exec(menuPos);
}

GObjectComboBoxController *CreateAnnotationNormalWidget::createGObjectComboBoxController(const GObjectComboBoxControllerConstraints &constraints) {
    return new GObjectComboBoxController(this, constraints, cbExistingTable);
}

void CreateAnnotationNormalWidget::countDescriptionUsage() const {
    if (!leDescription->text().isEmpty()) {
        GCOUNTER(cvar, tvar, "CreateAnnotationNormalWidget: description is used");
    }
}

void CreateAnnotationNormalWidget::fillSaveDocumentControllerConfig(SaveDocumentControllerConfig &config) const {
    config.fileNameEdit = leNewTablePath;
    config.fileDialogButton = tbBrowseNewTable;
}

void CreateAnnotationNormalWidget::initLayout() {
    ShowHideSubgroupWidget *saveShowHideWidget = new ShowHideSubgroupWidget("save_params", tr("Save annotation(s) to"), saveAnnotationsInnerWidget, true);
    saveShowHideWidget->setPermanentlyOpen(true);
    mainLayout->insertWidget(0, saveShowHideWidget);

    annotationParametersWidget = new ShowHideSubgroupWidget("annotparams", tr("Annotation parameters"), annotationParametersInnerWidget, true);
    annotationParametersWidget->setPermanentlyOpen(true);
    mainLayout->insertWidget(1, annotationParametersWidget);
}

void CreateAnnotationNormalWidget::init() {
    useAminoAnnotationTypes(false);
}

void CreateAnnotationNormalWidget::connectSignals() {
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
