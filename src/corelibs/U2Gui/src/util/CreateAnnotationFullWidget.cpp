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
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GObjectComboBoxController.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2LongLongValidator.h>

#include "CreateAnnotationFullWidget.h"

namespace U2 {

CreateAnnotationFullWidget::CreateAnnotationFullWidget(QWidget *parent) :
    CreateAnnotationWidget(parent),
    formatType(Simple)
{
    setupUi(this);
    initLayout();
    init();
    initOsDependingLayoutSettings();
    connectSignals();
}

CreateAnnotationFullWidget::~CreateAnnotationFullWidget() {
    countDescriptionUsage();
}

void CreateAnnotationFullWidget::setGroupNameVisible(bool visible) {
    lblGroupName->setVisible(visible);
    leGroupName->setVisible(visible);
    tbSelectGroupName->setVisible(visible);
}

void CreateAnnotationFullWidget::setLocationVisible(bool visible) {
    gbLocation->setVisible(visible);
}

void CreateAnnotationFullWidget::setAnnotationParametersVisible(bool /*visible*/) {
    // do nothing
}

void CreateAnnotationFullWidget::setAnnotationTypeVisible(bool visible) {
    lblAnnotationType->setVisible(visible);
    lwAnnotationType->setVisible(visible);
}

void CreateAnnotationFullWidget::setAnnotationNameVisible(bool visible) {
    lblAnnotationName->setVisible(visible);
    leAnnotationName->setVisible(visible);
}

void CreateAnnotationFullWidget::setAutoTableOptionVisible(bool visible) {
    rbUseAutoTable->setVisible(visible);
}

void CreateAnnotationFullWidget::setDescriptionVisible(bool visible) {
    lblDescription->setVisible(visible);
    leDescription->setVisible(visible);
}

void CreateAnnotationFullWidget::setUsePatternNamesVisible(bool visible) {
    chbUsePatternNames->setVisible(visible);
}

void CreateAnnotationFullWidget::setAnnotationTableOptionVisible(bool visible) {
    gbSaveAnnotationsInnerWidget->setVisible(visible);
}

void CreateAnnotationFullWidget::setAnnotationNameEnabled(bool enable) {
    leAnnotationName->setEnabled(enable);
}

void CreateAnnotationFullWidget::useAminoAnnotationTypes(bool useAmino) {
    QStringList featureTypes = getFeatureTypes(useAmino);
    qSort(featureTypes.begin(), featureTypes.end(), caseInsensitiveLessThan);

    lwAnnotationType->clear();
    lwAnnotationType->addItems(featureTypes);

    const int index = featureTypes.indexOf(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature));
    lwAnnotationType->setCurrentRow(index);
}

void CreateAnnotationFullWidget::focusGroupName() {
    leGroupName->setFocus();
}

void CreateAnnotationFullWidget::focusAnnotationType() {
    lwAnnotationType->setFocus();
}

void CreateAnnotationFullWidget::focusAnnotationName() {
    leAnnotationName->setFocus();
}

void CreateAnnotationFullWidget::focusLocation() {
    if (rbSimpleFormat->isChecked()) {
        leRegionStart->setFocus();
    } else {
        leLocation->setFocus();
    }
}

void CreateAnnotationFullWidget::setNewTablePath(const QString &path) {
    leNewTablePath->setText(path);
}

void CreateAnnotationFullWidget::setGroupName(const QString &name) {
    leGroupName->setText(name);
}

void CreateAnnotationFullWidget::setAnnotationType(U2FeatureType type) {
    QList<QListWidgetItem *> items = lwAnnotationType->findItems(U2FeatureTypes::getVisualName(type), Qt::MatchExactly);
    if (Q_LIKELY(!items.isEmpty())) {
        lwAnnotationType->setCurrentItem(items.first());
        return;
    }

    Q_ASSERT(false);    // an incorrect type
    items = lwAnnotationType->findItems(U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature), Qt::MatchExactly);
    if (Q_LIKELY(!items.isEmpty())) {
        lwAnnotationType->setCurrentItem(items.first());
        return;
    }

    lwAnnotationType->setCurrentRow(0);
}

void CreateAnnotationFullWidget::setAnnotationName(const QString &name) {
    leAnnotationName->setText(name);
}

void CreateAnnotationFullWidget::setLocation(const U2Location &location) {
    QString startString;
    QString endString;
    if (!location->isEmpty()) {
        startString = QString::number(location->regions.first().startPos + 1);
        endString = QString::number(location->regions.first().endPos());
    }

    leRegionStart->setText(startString);
    leRegionEnd->setText(endString);
    chbComplement->setChecked(location->strand.isCompementary());
    leLocation->setText(getGenbankLocationString(location));

    if (location->isMultiRegion()) {
        rbGenbankFormat->setChecked(true);
    }
}

void CreateAnnotationFullWidget::setDescription(const QString &description) {
    leDescription->setText(description);
}

QString CreateAnnotationFullWidget::getAnnotationTypeString() const {
    QListWidgetItem *currentItem = lwAnnotationType->currentItem();
    SAFE_POINT(NULL != currentItem, "Annotation type is not selected", "");
    return currentItem->text();
}

QString CreateAnnotationFullWidget::getGroupName() const {
    return leGroupName->text();
}

QString CreateAnnotationFullWidget::getAnnotationName() const {
    return leAnnotationName->text();
}

QString CreateAnnotationFullWidget::getDescription() const {
    return leDescription->text();
}

QString CreateAnnotationFullWidget::getLocationString() const {
    return leLocation->text();
}

bool CreateAnnotationFullWidget::isUsePatternNamesChecked() const {
    return chbUsePatternNames->isChecked();
}

bool CreateAnnotationFullWidget::isExistingTablesListEmpty() const {
    return cbExistingTable->count() == 0;
}

void CreateAnnotationFullWidget::selectNewTableOption() {
    rbCreateNewTable->setChecked(true);
}

void CreateAnnotationFullWidget::selectAutoTableOption() {
    rbUseAutoTable->setChecked(true);
}

void CreateAnnotationFullWidget::setExistingTableOptionEnable(bool enable) {
    rbExistingTable->setCheckable(enable);
    rbExistingTable->setEnabled(enable);
    cbExistingTable->setEnabled(enable && rbExistingTable->isChecked());
    tbBrowseExistingTable->setEnabled(enable && rbExistingTable->isChecked());
}

bool CreateAnnotationFullWidget::isNewTableOptionSelected() const {
    return rbCreateNewTable->isChecked();
}

bool CreateAnnotationFullWidget::isExistingTableOptionSelected() const {
    return rbExistingTable->isChecked();
}

bool CreateAnnotationFullWidget::isAutoTableOptionSelected() const {
    return rbUseAutoTable->isChecked();
}

void CreateAnnotationFullWidget::showSelectGroupMenu(QMenu &menu) {
    const QPoint menuPos = tbSelectGroupName->mapToGlobal(tbSelectGroupName->rect().bottomLeft());
    menu.exec(menuPos);
}

void CreateAnnotationFullWidget::sl_regionChanged() {
    bool ok = false;
    qint64 startPos = leRegionStart->text().toLongLong(&ok);
    CHECK(ok, );
    qint64 endPos = leRegionEnd->text().toLongLong(&ok);
    CHECK(ok, );

    U2Location location;
    location->regions << U2Region(startPos - 1, endPos - startPos + 1);
    location->strand = U2Strand(U2Strand(chbComplement->isChecked() ? U2Strand::Complementary : U2Strand::Direct));

    SharedAnnotationData annotationData(new AnnotationData);
    annotationData->location = location;

    leLocation->setText(U1AnnotationUtils::buildLocationString(annotationData));
}

void CreateAnnotationFullWidget::sl_locationChanged() {
    setLocation(parseGenbankLocationString(leLocation->text()));
}

void CreateAnnotationFullWidget::initLayout() {
    leRegionStart->setValidator(new U2LongLongValidator(1, LLONG_MAX, leRegionStart));
    leRegionEnd->setValidator(new U2LongLongValidator(1, LLONG_MAX, leRegionEnd));
}

void CreateAnnotationFullWidget::init() {
    useAminoAnnotationTypes(false);
}

void CreateAnnotationFullWidget::initOsDependingLayoutSettings() {
//macOS as default
#ifdef Q_OS_WIN
    verticalLayout_6->setSpacing(6);
    horizontalLayout_10->setSpacing(2);
    gbSaveAnnotationsInnerWidget->layout()->setContentsMargins(9, 0, 9, 6);
#elif !defined(Q_OS_OSX)
    verticalLayout_6->setSpacing(6);
    this->layout()->setSpacing(10);
    gbSaveAnnotationsInnerWidget->layout()->setContentsMargins(9, 6, 9, 6);
    horizontalLayout_8->setSpacing(5);
    horizontalLayout_9->setSpacing(8);
    horizontalLayout_10->setSpacing(8);
#endif
}

void CreateAnnotationFullWidget::connectSignals() {
    connect(tbBrowseExistingTable, SIGNAL(clicked()), SLOT(sl_selectExistingTableRequest()));
    connect(leGroupName, SIGNAL(textEdited(const QString &)), SLOT(sl_groupNameEdited()));
    connect(leGroupName, SIGNAL(textChanged(const QString &)), SLOT(sl_groupNameEdited()));
    connect(leAnnotationName, SIGNAL(textEdited(const QString &)), SLOT(sl_annotationNameEdited()));
    connect(leAnnotationName, SIGNAL(textChanged(const QString &)), SLOT(sl_annotationNameEdited()));
    connect(leRegionStart, SIGNAL(textEdited(const QString &)), SLOT(sl_regionChanged()));
    connect(leRegionEnd, SIGNAL(textEdited(const QString &)), SLOT(sl_regionChanged()));
    connect(leLocation, SIGNAL(editingFinished()), SLOT(sl_locationChanged()));
    connect(tbDoComplement, SIGNAL(clicked()), SLOT(sl_complementLocation()));
    connect(chbComplement, SIGNAL(clicked()), SLOT(sl_complementLocation()));
    connect(tbSelectGroupName, SIGNAL(clicked()), SLOT(sl_selectGroupNameMenuRequest()));
}

GObjectComboBoxController * CreateAnnotationFullWidget::createGObjectComboBoxController(const GObjectComboBoxControllerConstraints &constraints) {
    return new GObjectComboBoxController(this, constraints, cbExistingTable);
}

void CreateAnnotationFullWidget::countDescriptionUsage() const {
    if (!leDescription->text().isEmpty()) {
        GCOUNTER(cvar, tvar, "CreateAnnotationFullWidget: description is used");
    }
}

void CreateAnnotationFullWidget::fillSaveDocumentControllerConfig(SaveDocumentControllerConfig &config) const {
    config.fileNameEdit = leNewTablePath;
    config.fileDialogButton = tbBrowseNewTable;
}

}   // namespace U2
