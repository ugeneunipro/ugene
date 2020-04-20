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

#include "RegionSelectorController.h"

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <QApplication>
#include <math.h>


namespace U2 {

const QString RegionSelectorSettings::WHOLE_SEQUENCE = QApplication::translate("RegionSelectorController", "Whole sequence");
const QString RegionSelectorSettings::SELECTED_REGION = QApplication::translate("RegionSelectorController", "Selected region");
const QString RegionSelectorSettings::CUSTOM_REGION = QApplication::translate("RegionSelectorController", "Custom region");


RegionSelectorSettings::RegionSelectorSettings(qint64 maxLen,
                                               bool isCircularSelectionAvailable,
                                               DNASequenceSelection* selection,
                                               QList<RegionPreset> _presetRegions,
                                               QString defaultPreset)
    : maxLen(maxLen),
      selection(selection),
      circular(isCircularSelectionAvailable),
      presetRegions(_presetRegions),
      defaultPreset(defaultPreset) {

    if (selection != NULL && !selection->isEmpty()) {
        U2Region region = getOneRegionFromSelection();
        presetRegions.prepend(RegionPreset(SELECTED_REGION, region));
    }
    presetRegions.prepend(RegionPreset(WHOLE_SEQUENCE, U2Region(0, maxLen)));
    presetRegions.prepend(RegionPreset(CUSTOM_REGION, U2Region()));
}

U2Region RegionSelectorSettings::getOneRegionFromSelection() const {
    U2Region region = selection->getSelectedRegions().isEmpty()
            ? U2Region(0, maxLen)
            : selection->getSelectedRegions().first();
    if (selection->getSelectedRegions().size() == 2) {
        U2Region secondReg = selection->getSelectedRegions().last();
        bool circularSelection = (region.startPos == 0 && secondReg.endPos() == maxLen)
                || (region.endPos() == maxLen && secondReg.startPos == 0);
        if (circularSelection) {
            if (secondReg.startPos == 0) {
                region.length += secondReg.length;
            } else {
                region.startPos = secondReg.startPos;
                region.length += secondReg.length;
            }
        }
    }

    return region;
}

RegionSelectorController::RegionSelectorController(RegionSelectorGui gui, RegionSelectorSettings settings, QObject* parent)
    : QObject(parent),
      gui(gui),
      settings(settings) {

    init();
    setupPresets();
    connectSlots();
}

U2Region RegionSelectorController::getRegion(bool *_ok) const {
    SAFE_POINT_EXT(gui.startLineEdit != NULL && gui.endLineEdit != NULL, *_ok = false, U2Region());

    bool ok = false;
    qint64 v1 = gui.startLineEdit->text().toLongLong(&ok) - 1;

    if (!ok || v1 < 0 || v1 > settings.maxLen) {
        if (_ok != NULL) {
            *_ok = false;
        }
        return U2Region();
    }

    int v2 = gui.endLineEdit->text().toLongLong(&ok);

    if (!ok || v2 <= 0 || v2 > settings.maxLen) {
        if (_ok != NULL) {
            *_ok = false;
        }
        return U2Region();
    }

    if (v1 > v2 && !settings.circular) { // start > end
        if (_ok != NULL) {
            *_ok = false;
        }
        return U2Region();
    }

    if (_ok != NULL) {
        *_ok = true;
    }

    if (v1 < v2) {
        return U2Region(v1, v2 - v1);
    } else {
        return U2Region(v1, v2 + settings.maxLen - v1);
    }
}

void RegionSelectorController::setRegion(const U2Region &region) {
    CHECK(region != getRegion(), );
    SAFE_POINT(region.startPos >=0 && region.startPos < settings.maxLen && region.length <= settings.maxLen, tr("Region is not in sequence range"), );

    qint64 end = region.endPos();
    if (end > settings.maxLen) {
        if (settings.circular) {
            end = region.endPos() % settings.maxLen;
        } else {
            end = settings.maxLen;
        }
    }

    gui.startLineEdit->setText(QString::number(region.startPos + 1));
    gui.endLineEdit->setText(QString::number(end));

    emit si_regionChanged(region);
}

QString RegionSelectorController::getPresetName() const {
    SAFE_POINT(gui.presetsComboBox != NULL, tr("Cannot get preset name, ComboBox is NULL"), QString());
    return gui.presetsComboBox->currentText();
}

void RegionSelectorController::setPreset(const QString& preset) {
    SAFE_POINT(gui.presetsComboBox != NULL, tr("Cannot set preset, ComboBox is NULL"), );
    gui.presetsComboBox->setCurrentText(preset);
}

void RegionSelectorController::removePreset(const QString& preset) {
    gui.presetsComboBox->removeItem(gui.presetsComboBox->findText(preset));
    RegionPreset settingsPreset;
    foreach (const RegionPreset &r, settings.presetRegions) {
        if (r.text == preset) {
            settingsPreset = r;
            break;
        }
    }
    settings.presetRegions.removeOne(settingsPreset);
}

void RegionSelectorController::reset() {
    SAFE_POINT(gui.presetsComboBox != NULL, tr("Cannot set preset, ComboBox is NULL"), );
    gui.presetsComboBox->setCurrentText(settings.defaultPreset);
}

bool RegionSelectorController::hasError() const {
    return !getErrorMessage().isEmpty();
}

namespace {
const QString START_IS_INVALID = QApplication::translate("RegionSelectorController", "Invalid Start position of region");
const QString END_IS_INVALID = QApplication::translate("RegionSelectorController", "Invalid End position of region");
const QString REGION_IS_INVALID = QApplication::translate("RegionSelectorController", "Start position is greater than End position");
}

QString RegionSelectorController::getErrorMessage() const {
    bool ok = false;
    qint64 v1 = gui.startLineEdit->text().toLongLong(&ok) - 1;
    if (!ok || v1 < 0 || v1 > settings.maxLen) {
        return START_IS_INVALID;
    }

    int v2 = gui.endLineEdit->text().toLongLong(&ok);
    if (!ok || v2 <= 0 || v2 > settings.maxLen) {
        return END_IS_INVALID;
    }

    if (v1 > v2 && !settings.circular) { // start > end
        return REGION_IS_INVALID;
    }

    return QString();
}

void RegionSelectorController::sl_onPresetChanged(int index) {
    blockSignals(true);

    // set the region
    if (index == gui.presetsComboBox->findText(RegionSelectorSettings::CUSTOM_REGION)) {
        connect(this, SIGNAL(si_regionChanged(U2Region)), this, SLOT(sl_regionChanged()));
        return;
    }

    if (index == gui.presetsComboBox->findText(RegionSelectorSettings::SELECTED_REGION)) {
        setRegion(settings.getOneRegionFromSelection());
    } else {
        const U2Region region = gui.presetsComboBox->itemData(index).value<U2Region>();
        setRegion(region);
    }
    blockSignals(false);
}

void RegionSelectorController::sl_regionChanged() {
    gui.presetsComboBox->blockSignals(true);
    gui.presetsComboBox->setCurrentIndex(gui.presetsComboBox->findText(RegionSelectorSettings::CUSTOM_REGION));
    gui.presetsComboBox->blockSignals(false);
}

void RegionSelectorController::sl_onRegionChanged() {
    SAFE_POINT(gui.startLineEdit != NULL && gui.endLineEdit != NULL, tr("Region lineEdit is NULL"), );

    bool ok = false;

    int v1 = gui.startLineEdit->text().toInt(&ok);
    if (!ok || v1 < 1 || v1 > settings.maxLen) {
        return;
    }

    int v2 = gui.endLineEdit->text().toInt(&ok);
    if (!ok || v2 < 1 || v2 > settings.maxLen) {
        return;
    }
    if (!settings.circular && v2 < v1) {
        return;
    }

    U2Region r;
    if (v1 <= v2) {
        r = U2Region(v1 - 1, v2 - (v1 - 1));
    } else {
        r = U2Region(v1 - 1, v2 + settings.maxLen - (v1 - 1));
    }

    emit si_regionChanged(r);
}

void RegionSelectorController::sl_onSelectionChanged(GSelection *selection) {
    CHECK(gui.presetsComboBox != NULL, ); // no combobox - no selection dependency

    SAFE_POINT(settings.selection == selection, "Invalid sequence selection", );
    int selectedRegionIndex = gui.presetsComboBox->findText(RegionSelectorSettings::SELECTED_REGION);
    if (-1 == selectedRegionIndex) {
        selectedRegionIndex = gui.presetsComboBox->findText(RegionSelectorSettings::WHOLE_SEQUENCE) + 1;
        gui.presetsComboBox->insertItem(selectedRegionIndex, RegionSelectorSettings::SELECTED_REGION);
    }

    U2Region region = settings.getOneRegionFromSelection();
    if (region != gui.presetsComboBox->itemData(selectedRegionIndex).value<U2Region>()) {
        gui.presetsComboBox->setItemData(selectedRegionIndex, qVariantFromValue(region));
        if (selectedRegionIndex == gui.presetsComboBox->currentIndex()) {
            sl_onPresetChanged(selectedRegionIndex);
        }
    }
}

void RegionSelectorController::sl_onValueEdited() {
    SAFE_POINT(gui.startLineEdit != NULL && gui.endLineEdit != NULL, tr("Region lineEdit is NULL"), );

    if (gui.startLineEdit->text().isEmpty() || gui.endLineEdit->text().isEmpty()) {
        GUIUtils::setWidgetWarning(gui.startLineEdit, gui.startLineEdit->text().isEmpty());
        GUIUtils::setWidgetWarning(gui.endLineEdit, gui.endLineEdit->text().isEmpty());
        return;
    }

    const U2Region region = getRegion();
    GUIUtils::setWidgetWarning(gui.startLineEdit, region.isEmpty());
    GUIUtils::setWidgetWarning(gui.endLineEdit, region.isEmpty());
}

void RegionSelectorController::init() {
    SAFE_POINT(gui.startLineEdit != NULL && gui.endLineEdit != NULL, tr("Region lineEdit is NULL"), );

    int w = qMax(((int)log10((double)settings.maxLen))*10, 50);

    gui.startLineEdit->setValidator(new QIntValidator(1, settings.maxLen, gui.startLineEdit));
    gui.startLineEdit->setMinimumWidth(w);
    gui.startLineEdit->setAlignment(Qt::AlignRight);

    gui.endLineEdit->setValidator(new QIntValidator(1, settings.maxLen, gui.endLineEdit));
    gui.endLineEdit->setMinimumWidth(w);
    gui.endLineEdit->setAlignment(Qt::AlignRight);

    setRegion(U2Region(0, settings.maxLen));
}

void RegionSelectorController::setupPresets() {
    CHECK(gui.presetsComboBox != NULL, );

    bool foundDefaultPreset = false;
    foreach(const RegionPreset &presetRegion, settings.presetRegions) {
        gui.presetsComboBox->addItem(presetRegion.text, QVariant::fromValue(presetRegion.region));
        if (presetRegion.text == settings.defaultPreset) {
            foundDefaultPreset = true;
        }
    }
    if (!foundDefaultPreset) {
        settings.defaultPreset = RegionSelectorSettings::WHOLE_SEQUENCE;
    }

    gui.presetsComboBox->setCurrentText(settings.defaultPreset);
    const U2Region region = gui.presetsComboBox->itemData(gui.presetsComboBox->findText(settings.defaultPreset)).value<U2Region>();
    setRegion(region);
}

void RegionSelectorController::connectSlots() {
    SAFE_POINT(gui.startLineEdit != NULL && gui.endLineEdit != NULL, tr("Region lineEdit is NULL"), );

    connect(gui.startLineEdit, SIGNAL(editingFinished()),                  SLOT(sl_onRegionChanged()));
    connect(gui.startLineEdit, SIGNAL(textEdited(const QString &)),        SLOT(sl_onValueEdited()));
    connect(gui.startLineEdit, SIGNAL(textChanged(QString)),               SLOT(sl_onRegionChanged()));

    connect(gui.endLineEdit,   SIGNAL(editingFinished()),                  SLOT(sl_onRegionChanged()));
    connect(gui.endLineEdit,   SIGNAL(textEdited(const QString &)),        SLOT(sl_onValueEdited()));
    connect(gui.endLineEdit,   SIGNAL(textChanged(QString)),               SLOT(sl_onRegionChanged()));

    if (gui.presetsComboBox != NULL) {
        connect(gui.presetsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onPresetChanged(int)));
        connect(this, SIGNAL(si_regionChanged(U2Region)), this, SLOT(sl_regionChanged()));
    }

    if (settings.selection != NULL) {
        connect(settings.selection, SIGNAL(si_onSelectionChanged(GSelection*)), SLOT(sl_onSelectionChanged(GSelection*)));
    }
}


} // namespace
