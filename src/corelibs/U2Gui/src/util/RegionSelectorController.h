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

#ifndef _U2_REGION_SELECTOR_CONTROLLER_H_
#define _U2_REGION_SELECTOR_CONTROLLER_H_

#include <QComboBox>
#include <QLineEdit>

#include <U2Core/U2Region.h>
#include <U2Core/global.h>

namespace U2 {

class DNASequenceSelection;
class GSelection;

struct RegionSelectorGui {
    RegionSelectorGui()
        : startLineEdit(nullptr),
          endLineEdit(nullptr),
          presetsComboBox(nullptr) {
    }

    RegionSelectorGui(QLineEdit* start, QLineEdit* end, QComboBox* presets = nullptr)
        : startLineEdit(start),
          endLineEdit(end),
          presetsComboBox(presets) {
    }

    QLineEdit* startLineEdit;
    QLineEdit* endLineEdit;
    QComboBox* presetsComboBox;
};

struct U2GUI_EXPORT RegionPreset {
    RegionPreset() {
    }
    RegionPreset(const QString& text, const U2Region& region)
        : text(text),
          region(region) {
    }

    QString text;
    U2Region region;

    bool operator==(const RegionPreset& other) const {
        return (text == other.text);
    }

    /** Whole sequence localized preset name. */
    static QString WHOLE_SEQUENCE();

    /** Selected sequence localized preset name. */
    static QString SELECTED_REGION();

    /** Custom sequence localized preset name. */
    static QString CUSTOM_REGION();
};

struct RegionSelectorSettings {
    RegionSelectorSettings(qint64 maxLen,
                           bool circular = false,
                           DNASequenceSelection* selection = nullptr,
                           QList<RegionPreset> presetRegions = QList<RegionPreset>(),
                           QString defaultPreset = RegionPreset::SELECTED_REGION());

    qint64 maxLen;
    DNASequenceSelection* selection;
    bool circular;

    QList<RegionPreset> presetRegions;
    QString defaultPreset;

    // Returns circular region or the first selected. If none is selected, returns full sequence range.
    U2Region getOneRegionFromSelection() const;
};

class RegionSelectorController : public QObject {
    Q_OBJECT
public:
    RegionSelectorController(RegionSelectorGui gui,
                             RegionSelectorSettings settings,
                             QObject* parent);

    U2Region getRegion(bool* ok = nullptr) const;
    void setRegion(const U2Region& region);

    QString getPresetName() const;
    void setPreset(const QString& preset);
    void removePreset(const QString& preset);

    void reset();

    bool hasError() const;
    QString getErrorMessage() const;

signals:
    void si_regionChanged(const U2Region& newRegion);

private slots:
    //! rename
    void sl_regionChanged();
    void sl_onPresetChanged(int index);
    void sl_onRegionChanged();
    void sl_onSelectionChanged(GSelection* selection);
    void sl_onValueEdited();

private:
    void init();
    void setupPresets();
    void connectSlots();

private:
    RegionSelectorGui gui;
    RegionSelectorSettings settings;
};

}  // namespace U2

#endif  // _U2_REGION_SELECTOR_CONTROLLER_H_
