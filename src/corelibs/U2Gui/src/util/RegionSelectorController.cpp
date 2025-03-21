/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include <math.h>

#include <QApplication>

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/U2LongLongValidator.h>

namespace U2 {

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
    if (selection != nullptr && !selection->isEmpty()) {
        presetRegions.prepend(RegionPreset(RegionPreset::getSelectedRegionDisplayName(), U2Location({getOneRegionFromSelection()})));
    }
    presetRegions.prepend(RegionPreset(RegionPreset::getWholeSequenceModeDisplayName(), U2Location({U2Region(0, maxLen)})));
    presetRegions.prepend(RegionPreset(RegionPreset::getCustomRegionModeDisplayName(), U2Location({U2Region()})));
}

U2Region RegionSelectorSettings::getOneRegionFromSelection() const {
    U2Region region = selection->getSelectedRegions().isEmpty() ? U2Region(0, maxLen) : selection->getSelectedRegions().first();
    if (selection->getSelectedRegions().size() == 2) {
        U2Region secondReg = selection->getSelectedRegions().last();
        bool circularSelection = (region.startPos == 0 && secondReg.endPos() == maxLen) || (region.endPos() == maxLen && secondReg.startPos == 0);
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

QString RegionPreset::getWholeSequenceModeDisplayName() {
    return RegionSelectorController::tr("Whole sequence");
}

QString RegionPreset::getSelectedRegionDisplayName() {
    return RegionSelectorController::tr("Selected region");
}

QString RegionPreset::getCustomRegionModeDisplayName() {
    return RegionSelectorController::tr("Custom region");
}
QString RegionPreset::getLocationModeDisplayName() {
    return RegionSelectorController::tr("Location");
}

RegionSelectorController::RegionSelectorController(RegionSelectorGui gui, RegionSelectorSettings settings, QObject* parent)
    : QObject(parent),
      gui(gui),
      settings(settings) {
    init();
    setupPresets();
    connectSlots();
}

U2Region RegionSelectorController::getRegion(bool* _ok) const {
    SAFE_POINT_EXT(gui.startLineEdit != nullptr && gui.endLineEdit != nullptr, *_ok = false, U2Region());

    bool ok = false;
    qint64 startPosInclusive = gui.startLineEdit->text().toLongLong(&ok) - 1;

    if (!ok || startPosInclusive < 0 || startPosInclusive > settings.maxLen) {
        if (_ok != nullptr) {
            *_ok = false;
        }
        return {};
    }

    qint64 endPosExlusive = gui.endLineEdit->text().toLongLong(&ok);

    if (!ok || endPosExlusive <= 0 || endPosExlusive > settings.maxLen) {
        if (_ok != nullptr) {
            *_ok = false;
        }
        return {};
    }

    if (startPosInclusive >= endPosExlusive && !settings.circular) {  // start > end
        if (_ok != nullptr) {
            *_ok = false;
        }
        return {};
    }

    if (_ok != nullptr) {
        *_ok = true;
    }

    if (startPosInclusive < endPosExlusive) {
        return {startPosInclusive, endPosExlusive - startPosInclusive};
    }
    return {startPosInclusive, endPosExlusive + settings.maxLen - startPosInclusive};
}

void RegionSelectorController::setRegion(const U2Region& region) {
    CHECK(region != getRegion(), );
    SAFE_POINT(region.startPos >= 0 && region.startPos < settings.maxLen && region.length <= settings.maxLen, "Region is not in sequence range", );

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

U2Location RegionSelectorController::getLocation(bool* ok) const {
    U2Location res;
    if (gui.presetsComboBox->currentText() == RegionPreset::getLocationModeDisplayName ()) {
        qint64 circularSequenceLength = settings.circular ? settings.maxLen : -1;
        const QByteArray qb = gui.locationLineEdit->text().toLatin1();
        *ok = Genbank::LocationParser::parseLocation(qb.constData(), qb.length(), res, circularSequenceLength) == Genbank::LocationParser::Success;
    } else {
        res.data()->regions << getRegion(ok);
    }
    return res;
}

void RegionSelectorController::setLocation(const U2Location& location) {
    if (gui.presetsComboBox->currentText() == RegionPreset::getLocationModeDisplayName ()) {
        gui.locationLineEdit->setText(U1AnnotationUtils::buildLocationString(location->regions));
    } else {
        setRegion(location.data()->regions.isEmpty() ? U2Region() : location.data()->regions.first());
    }
}

QString RegionSelectorController::getPresetName() const {
    SAFE_POINT(gui.presetsComboBox != nullptr, "Cannot get preset name, ComboBox is NULL", QString());
    return gui.presetsComboBox->currentText();
}

void RegionSelectorController::setPreset(const QString& preset) {
    SAFE_POINT(gui.presetsComboBox != nullptr, "Cannot set preset, ComboBox is NULL", );
    gui.presetsComboBox->setCurrentText(preset);
}

void RegionSelectorController::removePreset(const QString& preset) {
    gui.presetsComboBox->removeItem(gui.presetsComboBox->findText(preset));
    RegionPreset settingsPreset;
    foreach (const RegionPreset& r, settings.presetRegions) {
        if (r.text == preset) {
            settingsPreset = r;
            break;
        }
    }
    settings.presetRegions.removeOne(settingsPreset);
}

void RegionSelectorController::reset() {
    SAFE_POINT(gui.presetsComboBox != nullptr, "Cannot set preset, ComboBox is NULL", );
    gui.presetsComboBox->setCurrentText(settings.defaultPreset);
}

bool RegionSelectorController::hasError() const {
    return !getErrorMessage().isEmpty();
}

namespace {
const QString START_IS_INVALID = QApplication::translate("RegionSelectorController", "Invalid Start position of region");
const QString END_IS_INVALID = QApplication::translate("RegionSelectorController", "Invalid End position of region");
const QString REGION_IS_INVALID = QApplication::translate("RegionSelectorController", "Start position is greater than End position");
}  // namespace

QString RegionSelectorController::getErrorMessage() const {
    bool ok = false;
    qint64 startInclusive = gui.startLineEdit->text().toLongLong(&ok) - 1;
    if (!ok || startInclusive < 0 || startInclusive > settings.maxLen) {
        return START_IS_INVALID;
    }
    qint64 endExclusive = gui.endLineEdit->text().toLongLong(&ok);
    if (!ok || endExclusive <= 0 || endExclusive > settings.maxLen) {
        return END_IS_INVALID;
    }
    if (startInclusive >= endExclusive && !settings.circular) {  // start >= end
        return REGION_IS_INVALID;
    }
    return {};
}

void RegionSelectorController::sl_onPresetChanged(int index) {
    //emit si_presetChanged(index);
    blockSignals(true);

    // set the region
    if (index == gui.presetsComboBox->findText(RegionPreset::getCustomRegionModeDisplayName())) {
        connect(this, SIGNAL(si_regionChanged(U2Region)), this, SLOT(sl_regionChanged()));
        return;
    }

    if (index == gui.presetsComboBox->findText(RegionPreset::getSelectedRegionDisplayName())) {
        setRegion(settings.getOneRegionFromSelection());
    } else if(index == gui.presetsComboBox->findText(RegionPreset::getLocationModeDisplayName ())) {
        setLocation(gui.presetsComboBox->itemData(index).value<U2Location>());
    } else{
        const U2Location location = gui.presetsComboBox->itemData(index).value<U2Location>();
        setRegion(location.data()->regions.first());
    }
    blockSignals(false);
}

void RegionSelectorController::sl_regionChanged() {
    gui.presetsComboBox->blockSignals(true);
    gui.presetsComboBox->setCurrentIndex(gui.presetsComboBox->findText(RegionPreset::getCustomRegionModeDisplayName()));
    gui.presetsComboBox->blockSignals(false);
}

void RegionSelectorController::sl_onRegionChanged() {
    SAFE_POINT(gui.startLineEdit != nullptr && gui.endLineEdit != nullptr, "Region lineEdit is NULL", );

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

void RegionSelectorController::sl_onSelectionChanged(GSelection* selection) {
    CHECK(gui.presetsComboBox != nullptr, );  // no combobox - no selection dependency

    SAFE_POINT(settings.selection == selection, "Invalid sequence selection", );
    int selectedRegionIndex = gui.presetsComboBox->findText(RegionPreset::getSelectedRegionDisplayName());
    if (-1 == selectedRegionIndex) {
        selectedRegionIndex = gui.presetsComboBox->findText(RegionPreset::getWholeSequenceModeDisplayName()) + 1;
        gui.presetsComboBox->insertItem(selectedRegionIndex, RegionPreset::getSelectedRegionDisplayName());
    }

    const U2Region regionFromSelection = settings.getOneRegionFromSelection();
    const U2Region regionFromComboBox = gui.presetsComboBox->itemData(selectedRegionIndex).value<U2Location>().data()->regions.first();
    if (regionFromSelection != regionFromComboBox) {
        gui.presetsComboBox->setItemData(selectedRegionIndex, QVariant::fromValue(U2Location({regionFromSelection})));
        if (selectedRegionIndex == gui.presetsComboBox->currentIndex()) {
            sl_onPresetChanged(selectedRegionIndex);
        }
    }
}

void RegionSelectorController::sl_onValueEdited() {
    SAFE_POINT(gui.startLineEdit != nullptr && gui.endLineEdit != nullptr, "Region lineEdit is NULL", );

    if (gui.startLineEdit->text().isEmpty() || gui.endLineEdit->text().isEmpty()) {
        GUIUtils::setWidgetWarningStyle(gui.startLineEdit, gui.startLineEdit->text().isEmpty());
        GUIUtils::setWidgetWarningStyle(gui.endLineEdit, gui.endLineEdit->text().isEmpty());
        return;
    }

    const U2Region region = getRegion();
    GUIUtils::setWidgetWarningStyle(gui.startLineEdit, region.isEmpty());
    GUIUtils::setWidgetWarningStyle(gui.endLineEdit, region.isEmpty());
}

void RegionSelectorController::init() {
    SAFE_POINT(gui.startLineEdit != nullptr && gui.endLineEdit != nullptr, "Region lineEdit is NULL", );

    int w = qMax(((int)log10((double)settings.maxLen)) * 10, 50);

    gui.startLineEdit->setValidator(new U2LongLongValidator(1, settings.maxLen, gui.startLineEdit));
    gui.startLineEdit->setMinimumWidth(w);
    gui.startLineEdit->setAlignment(Qt::AlignRight);

    gui.endLineEdit->setValidator(new U2LongLongValidator(1, settings.maxLen, gui.endLineEdit));
    gui.endLineEdit->setMinimumWidth(w);
    gui.endLineEdit->setAlignment(Qt::AlignRight);

    setRegion(U2Region(0, settings.maxLen));
}

void RegionSelectorController::setupPresets() {
    CHECK(gui.presetsComboBox != nullptr, );

    bool foundDefaultPreset = false;
    foreach (const RegionPreset& presetRegion, settings.presetRegions) {
        gui.presetsComboBox->addItem(presetRegion.text, QVariant::fromValue(presetRegion.location));
        if (presetRegion.text == settings.defaultPreset) {
            foundDefaultPreset = true;
        }
    }
    if (!foundDefaultPreset) {
        settings.defaultPreset = RegionPreset::getWholeSequenceModeDisplayName();
    }

    gui.presetsComboBox->setCurrentText(settings.defaultPreset);
    const U2Region region = gui.presetsComboBox->itemData(gui.presetsComboBox->findText(settings.defaultPreset)).value<U2Location>().data()->regions.isEmpty() ? 
                            U2Region() : 
                            gui.presetsComboBox->itemData(gui.presetsComboBox->findText(settings.defaultPreset)).value<U2Location>().data()->regions.first();
    setRegion(region);
}

void RegionSelectorController::connectSlots() {
    SAFE_POINT(gui.startLineEdit != nullptr && gui.endLineEdit != nullptr, "Region lineEdit is NULL", );

    connect(gui.startLineEdit, SIGNAL(editingFinished()), SLOT(sl_onRegionChanged()));
    connect(gui.startLineEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_onValueEdited()));
    connect(gui.startLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_onRegionChanged()));

    connect(gui.endLineEdit, SIGNAL(editingFinished()), SLOT(sl_onRegionChanged()));
    connect(gui.endLineEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_onValueEdited()));
    connect(gui.endLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_onRegionChanged()));

    if (gui.presetsComboBox != nullptr) {
        connect(gui.presetsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onPresetChanged(int)));
        connect(this, SIGNAL(si_regionChanged(U2Region)), this, SLOT(sl_regionChanged()));
    }

    if (settings.selection != nullptr) {
        connect(settings.selection, SIGNAL(si_onSelectionChanged(GSelection*)), SLOT(sl_onSelectionChanged(GSelection*)));
    }
}

}  // namespace U2
