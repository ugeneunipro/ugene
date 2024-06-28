/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
 
 #include <U2Core/U1AnnotationUtils.h>
 #include <U2Core/U2SafePoints.h>

 #include <U2Formats/GenbankLocationParser.h>

 #include <U2Gui/GUIUtils.h>

 #include "GenbankStyleRegionSelectorController.h"

 #include "Ui_GenbankStyleRegionSelector.h"

namespace U2 {

const QString GenbankStyleRegionSelectorController::WHOLE_SEQUENCE = QObject::tr("Whole sequence");

const QString GenbankStyleRegionSelectorController::SELECTED_REGIONS = QObject::tr("Region(s)");

GenbankStyleRegionSelectorController::GenbankStyleRegionSelectorController(QWidget* parent, qint64 maxLen, DNASequenceSelection* selection,
                                                                           bool isCircularAvailable) 
    : QWidget(parent), 
    ui(new Ui_GenbankStyleRegionSelector),
    length(maxLen),
    circular(isCircularAvailable) {
    ui->setupUi(this);
    ui->presetsComboBox->addItem(WHOLE_SEQUENCE);
    ui->presetsComboBox->addItem(SELECTED_REGIONS);

    connect(ui->presetsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GenbankStyleRegionSelectorController::sl_presetChanged);
    connect(ui->excludeEnzymesCheckBox, &QCheckBox::stateChanged, this, &GenbankStyleRegionSelectorController::sl_excludeEnzymesChecked);
    connect(ui->searchAreaLineEdit, &QLineEdit::textChanged, this, &GenbankStyleRegionSelectorController::sl_genbankLocationChanged);
    connect(ui->excludeAreaLineEdit, &QLineEdit::textChanged, this, &GenbankStyleRegionSelectorController::sl_genbankLocationChanged);    

    sl_presetChanged();
    sl_excludeEnzymesChecked(ui->excludeEnzymesCheckBox->checkState());
}

GenbankStyleRegionSelectorController::~GenbankStyleRegionSelectorController() {
    delete ui;
}

bool GenbankStyleRegionSelectorController::locationsIsOk() const {
    return ui->excludeAreaLineEdit->isEnabled() ? excludeLocationIsOk && searchLocationIsOk : searchLocationIsOk;
}

void GenbankStyleRegionSelectorController::setSearchLocation(const U2Location& location) {
    if (location->regions.isEmpty()) {
        ui->searchAreaLineEdit->setText(U1AnnotationUtils::buildLocationString({U2Region(0, length)}));
    } else {
        ui->searchAreaLineEdit->setText(U1AnnotationUtils::buildLocationString(location->regions));
    }
}

U2Location GenbankStyleRegionSelectorController::getSearchLocation() const {
    return searchLocationIsOk ? searchLocation : U2Location();
}

void GenbankStyleRegionSelectorController::setExcludedLocation(const U2Location& location) {
    if (location->regions.isEmpty()) {
        ui->excludeAreaLineEdit->setText(U1AnnotationUtils::buildLocationString({U2Region()}));
    } else {
        ui->excludeAreaLineEdit->setText(U1AnnotationUtils::buildLocationString(location->regions));
    }
}

U2Location GenbankStyleRegionSelectorController::getExcludedLocation() const {
    return excludeLocationIsOk ? excludeLocation : U2Location();
}

void GenbankStyleRegionSelectorController::setExcludeChecked(bool enable) {
    ui->excludeEnzymesCheckBox->setChecked(enable);
}

bool GenbankStyleRegionSelectorController::getExcludeChecked() const {
    return ui->excludeEnzymesCheckBox->checkState() == Qt::Checked;
}

void GenbankStyleRegionSelectorController::sl_presetChanged() {
    if (ui->presetsComboBox->currentText() == SELECTED_REGIONS) {
        ui->searchAreaLineEdit->setEnabled(true);
    } else {
        ui->searchAreaLineEdit->setText(U1AnnotationUtils::buildLocationString({U2Region(0, length)}));
        ui->searchAreaLineEdit->setEnabled(false);
    }
}

void GenbankStyleRegionSelectorController::sl_excludeEnzymesChecked(int newState) {
    bool enable = newState == Qt::Checked;
    ui->excludeAreaLineEdit->setEnabled(enable);
    if (enable) {
        ui->excludeAreaLineEdit->emit QLineEdit::textChanged(ui->excludeAreaLineEdit->text());
    } else {
        GUIUtils::setWidgetWarningStyle(ui->excludeAreaLineEdit, false);
    }
}

void GenbankStyleRegionSelectorController::sl_genbankLocationChanged(const QString& text) {    
    QLineEdit* senderLE = qobject_cast<QLineEdit*>(sender());
    CHECK(senderLE != nullptr, )
    CHECK_EXT(!text.isEmpty(), setValidLocation(senderLE, U2Location()), );

    qint64 circLen = circular ? length : -1;
    U2Location location;
    const QByteArray qb = senderLE->text().toLatin1();
    if (Genbank::LocationParser::parseLocation(qb.constData(), qb.length(), location, circLen) == Genbank::LocationParser::Success) {
        GUIUtils::setWidgetWarningStyle(senderLE, false);
        setValidLocation(senderLE, location);
    } else {
        GUIUtils::setWidgetWarningStyle(senderLE, true);
        senderLE == ui->searchAreaLineEdit ? searchLocationIsOk = false : excludeLocationIsOk = false;
    }
}

void GenbankStyleRegionSelectorController::setValidLocation(QLineEdit* lineEdit, const U2Location& location) {
    if (lineEdit == ui->searchAreaLineEdit) {
        searchLocation = location;
        searchLocationIsOk = true;
    } else {
        excludeLocation = location;
        excludeLocationIsOk = true;
    }
}

}  // namespace U2
