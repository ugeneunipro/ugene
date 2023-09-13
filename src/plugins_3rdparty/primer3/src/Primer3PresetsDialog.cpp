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

#include "Primer3PresetsDialog.h"

#include <U2Gui/HelpButton.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QFileInfo>

namespace U2 {

const QString Primer3PresetsDialog::PRESETS_DIRECTORY = QFileInfo(QString(PATH_PREFIX_DATA) + "primer3/presets/").absoluteFilePath().toLatin1();
const QMap<Primer3PresetsDialog::Preset, QString> Primer3PresetsDialog::PRESET_PATH = {
    {Primer3PresetsDialog::Preset::Default, PRESETS_DIRECTORY + "Default.txt"},
    {Primer3PresetsDialog::Preset::RPA, PRESETS_DIRECTORY + "RPA.txt"} };

Primer3PresetsDialog::Primer3PresetsDialog(const QString& dnaViewName) : QDialog(),
        savableWidget(this, GObjectViewUtils::findViewByName(dnaViewName)) {
    setupUi(this);

    new HelpButton(this, helpButton, "TODO");

    sl_presetChanged(swPresets->currentIndex());
    connect(swPresets, &QStackedWidget::currentChanged, this, &Primer3PresetsDialog::sl_presetChanged);
    connect(helpButton, &QDialogButtonBox::accepted, this, [this]() {
        currentPreset = static_cast<Preset>(cbPreset->currentIndex());
        QDialog::accept();
    });
    connect(helpButton, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(helpButton, &QDialogButtonBox::rejected, this, &QDialog::reject);

    U2WidgetStateStorage::restoreWidgetState(savableWidget);
}

QString Primer3PresetsDialog::getCurrentPresetPath() const {
    SAFE_POINT(PRESET_PATH.contains(currentPreset), "Unexpected preset", QString());

    return PRESET_PATH.value(currentPreset);
}

void Primer3PresetsDialog::sl_presetChanged(int index) {
    for (int i = 0; i < swPresets->count(); i++) {
        CHECK_CONTINUE(i != index);

        swPresets->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    auto currentWidget = swPresets->widget(index);
    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), );

    currentWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    swPresets->adjustSize();
    adjustSize();
}

}