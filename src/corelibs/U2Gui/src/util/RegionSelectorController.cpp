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
    const auto& selectedRegions = selection->getSelectedRegions();
    U2Region region = selectedRegions.isEmpty() ? U2Region(0, maxLen) : selectedRegions.first();
    if (selectedRegions.size() == 2) {
        U2Region secondReg = selectedRegions.last();
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
        SAFE_POINT(settings.circular, "Region end position is out of range, but circular mode is not enabled", );

        end = region.endPos() % settings.maxLen;
    }

    gui.startLineEdit->setText(QString::number(region.startPos + 1));
    gui.endLineEdit->setText(QString::number(end));

    emitRegionChanged(region);
}

U2Location RegionSelectorController::getLocation(bool* ok) const {
    U2Location res;
    if (gui.presetsComboBox->currentText() == RegionPreset::getLocationModeDisplayName()) {
        qint64 circularSequenceLength = settings.circular ? settings.maxLen : -1;
        const QByteArray qb = gui.locationLineEdit->text().toLatin1();
        *ok = Genbank::LocationParser::parseLocation(qb.constData(), qb.length(), res, circularSequenceLength) == Genbank::LocationParser::Success;
    } else {
        res.data()->regions << getRegion(ok);
    }
    return res;
}

void RegionSelectorController::setLocation(const U2Location& location) {
    SAFE_POINT_NN(gui.locationLineEdit, );
    SAFE_POINT(gui.presetsComboBox->currentText() == RegionPreset::getLocationModeDisplayName(), "Incorrect preset type", );

    gui.locationLineEdit->setText(U1AnnotationUtils::buildLocationString(location->regions));
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
    for (const RegionPreset& r : qAsConst(settings.presetRegions)) {
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

void RegionSelectorController::sl_onRegionChanged() {
    SAFE_POINT_NN(gui.startLineEdit, );
    SAFE_POINT_NN(gui.endLineEdit, );

    auto startLineEditText = gui.startLineEdit->text();
    auto endLineEditText = gui.endLineEdit->text();
    if (startLineEditText.isEmpty() || endLineEditText.isEmpty()) {
        GUIUtils::setWidgetWarningStyle(gui.startLineEdit, startLineEditText.isEmpty());
        GUIUtils::setWidgetWarningStyle(gui.endLineEdit, endLineEditText.isEmpty());
        return;
    }

    SAFE_POINT_NN(gui.presetsComboBox, );
    gui.presetsComboBox->setCurrentIndex(gui.presetsComboBox->findText(RegionPreset::getCustomRegionModeDisplayName()));

    bool ok = false;
    const U2Region region = getRegion(&ok);
    GUIUtils::setWidgetWarningStyle(gui.startLineEdit, !ok);
    GUIUtils::setWidgetWarningStyle(gui.endLineEdit, !ok);
    CHECK(ok, );

    emitRegionChanged(region);
}

void RegionSelectorController::sl_onPresetChanged(int index) {
    if (index == gui.presetsComboBox->findText(RegionPreset::getLocationModeDisplayName())) {
        // This is a special case, because combobox isn't affected on location lineedit editing
        setLocation(gui.presetsComboBox->itemData(index).value<U2Location>());
        return;
    }

    if (index == gui.presetsComboBox->findText(RegionPreset::getCustomRegionModeDisplayName())) {
        return;
    }

    U2Region newRegion;
    if (index == gui.presetsComboBox->findText(RegionPreset::getSelectedRegionDisplayName())) {
        newRegion = settings.getOneRegionFromSelection();
    } else if (index == gui.presetsComboBox->findText(RegionPreset::getWholeSequenceModeDisplayName())) {
        const U2Location location = gui.presetsComboBox->itemData(index).value<U2Location>();
        newRegion = location.data()->regions.first();
    } else {
        FAIL("Unexpected selection preset", );
    }

    setRegion(newRegion);
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

void RegionSelectorController::init() {
    SAFE_POINT_NN(gui.startLineEdit, );
    SAFE_POINT_NN(gui.endLineEdit, );

    const int BASE_WIDTH = 10;
    const int MIN_FIELD_WIDTH = 50;
    int w = qMax(((int)log10((double)settings.maxLen)) * BASE_WIDTH, MIN_FIELD_WIDTH);

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
    auto defaultPresetRegions = gui.presetsComboBox->itemData(gui.presetsComboBox->findText(settings.defaultPreset)).value<U2Location>().data()->regions;
    const U2Region region = defaultPresetRegions.isEmpty() ?
                            U2Region() :
                            defaultPresetRegions.first();
    setRegion(region);
}

void RegionSelectorController::connectSlots() {
    SAFE_POINT_NN(gui.startLineEdit, );
    SAFE_POINT_NN(gui.endLineEdit, );
    SAFE_POINT_NN(gui.presetsComboBox, );

    connect(gui.startLineEdit, &QLineEdit::textEdited, this, &RegionSelectorController::sl_onRegionChanged);
    connect(gui.endLineEdit, &QLineEdit::textEdited, this, &RegionSelectorController::sl_onRegionChanged);
    connect(gui.presetsComboBox, &QComboBox::activated, this, &RegionSelectorController::sl_onPresetChanged);

    if (settings.selection != nullptr) {
        connect(settings.selection, SIGNAL(si_onSelectionChanged(GSelection*)), SLOT(sl_onSelectionChanged(GSelection*)));
    }
}

void RegionSelectorController::emitRegionChanged(const U2Region& newRegion) {
    emit si_regionChanged(newRegion);
}

}  // namespace U2
