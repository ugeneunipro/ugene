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

#include <math.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/U2LongLongValidator.h>

#include "RegionSelector.h"

namespace U2 {
////////////////////////////////////////
// RangeSelectorWidget
RegionSelector::RegionSelector(QWidget* p, qint64 len, bool isVertical, DNASequenceSelection* selection, bool isCircularSelectionAvailable, QList<RegionPreset> presetRegions)
    : QWidget(p),
      maxLen(len),
      startEdit(nullptr),
      endEdit(nullptr),
      isVertical(isVertical) {
    initLayout();

    RegionSelectorGui gui(startEdit, endEdit, locationLineEdit, comboBox);
    RegionSelectorSettings settings(len, isCircularSelectionAvailable, selection, presetRegions);
    controller = new RegionSelectorController(gui, settings, this);
    connect(controller, SIGNAL(si_regionChanged(U2Region)), this, SIGNAL(si_regionChanged(U2Region)));
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RegionSelector::sl_presetChanged);
    sl_presetChanged(comboBox->currentIndex());
}

U2Region RegionSelector::getRegion(bool* _ok) const {
    return controller->getRegion(_ok);
}

U2Location RegionSelector::getLocation(bool* ok) const {
    return controller->getLocation(ok);
}

bool RegionSelector::isWholeSequenceSelected() const {
    return controller->getPresetName() == RegionPreset::getWholeSequenceModeDisplayName();
}

void RegionSelector::setCustomRegion(const U2Region& value) {
    controller->setRegion(value);
}

void RegionSelector::setWholeRegionSelected() {
    controller->setPreset(RegionPreset::getWholeSequenceModeDisplayName());
}

void RegionSelector::setCurrentPreset(const QString& presetName) {
    controller->setPreset(presetName);
}

void RegionSelector::reset() {
    controller->reset();
}

void RegionSelector::removePreset(const QString& itemName) {
    controller->removePreset(itemName);
}

void RegionSelector::showErrorMessage() {
    if (controller->hasError()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::NoIcon, L10N::errorTitle(), tr("Invalid sequence region!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(controller->getErrorMessage());
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
    }
}

const QLineEdit* RegionSelector::getStartLineEdit() const {
    return startEdit;
}
const QLineEdit* RegionSelector::getEndLineEdit() const {
    return endEdit;
}

static void hideLayoutMembers(QLayout* layout, bool hide) {
    CHECK(layout != nullptr, )
    for (int i = 0; i < layout->count(); ++i) {
        QWidget* layoutMember = layout->itemAt(i)->widget();
        layoutMember->setHidden(hide);
    }
}

void RegionSelector::sl_presetChanged(int index) {
    bool hideLocation = index != comboBox->findText(RegionPreset::getLocationModeDisplayName());
    hideLayoutMembers(locationLayout, hideLocation);
    hideLayoutMembers(regionLayout, !hideLocation);
}

void RegionSelector::initLayout() {
    int w = qMax(((int)log10((double)maxLen)) * 10, 50);

    comboBox = new QComboBox(this);

    startEdit = new RegionLineEdit(this, tr("Set minimum"), 1);
    startEdit->setValidator(new U2LongLongValidator(1, maxLen, startEdit));
    startEdit->setMinimumWidth(w);
    startEdit->setAlignment(Qt::AlignRight);

    endEdit = new RegionLineEdit(this, tr("Set maximum"), maxLen);
    endEdit->setValidator(new U2LongLongValidator(1, maxLen, endEdit));
    endEdit->setMinimumWidth(w);
    endEdit->setAlignment(Qt::AlignRight);

    if (isVertical) {
        auto gb = new QGroupBox(this);
        gb->setTitle(tr("Region"));

        auto l = new QGridLayout(gb);
        l->setSizeConstraint(QLayout::SetMinAndMaxSize);
        gb->setLayout(l);

        l->addWidget(comboBox, 0, 0, 1, 3);
        l->addWidget(startEdit, 1, 0);
        l->addWidget(new QLabel(tr("-"), gb), 1, 1);
        l->addWidget(endEdit, 1, 2);
        l->addWidget(new QLabel(" ", gb), 2, 0);

        auto rootLayout = new QVBoxLayout(this);
        rootLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        rootLayout->setMargin(0);
        setLayout(rootLayout);
        rootLayout->addWidget(gb);
    } else {
        locationLineEdit = new QLineEdit(this);
        locationLineEdit->setAlignment(Qt::AlignRight);
        locationLineEdit->setObjectName("location_line_edit");

        auto l = new QHBoxLayout(this);
        l->setMargin(0);

        auto rangeLabel = new QLabel(tr("Region"), this);
        rangeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        l->addWidget(rangeLabel);
        l->addWidget(comboBox);
        
        regionLayout = new QHBoxLayout(this);
        regionLayout->addWidget(startEdit);
        regionLayout->addWidget(new QLabel(tr("-"), this));
        regionLayout->addWidget(endEdit);

        locationLayout = new QHBoxLayout(this);
        locationLayout->setMargin(0);
        locationLayout->addWidget(locationLineEdit);
        
        auto selectorsLayout = new QVBoxLayout(this);
        selectorsLayout->setMargin(0);
        selectorsLayout->addLayout(regionLayout);
        selectorsLayout->addLayout(locationLayout);

        l->addLayout(selectorsLayout);        
        setLayout(l);
    }

    startEdit->setObjectName("start_edit_line");
    endEdit->setObjectName("end_edit_line");
    comboBox->setObjectName("region_type_combo");
    setObjectName("range_selector");
}

///////////////////////////////////////
//! RegionLineEdit
//! only for empty field highlight
void RegionLineEdit::focusOutEvent(QFocusEvent* event) {
    bool ok = false;
    text().toInt(&ok);
    if (!ok) {
        QPalette p = palette();
        p.setColor(QPalette::Base, QColor(255, 200, 200));  // pink color
        setPalette(p);
    }
    QLineEdit::focusOutEvent(event);
}
void RegionLineEdit::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = createStandardContextMenu();
    auto setDefaultValue = new QAction(actionName, this);
    connect(setDefaultValue, SIGNAL(triggered()), this, SLOT(sl_onSetMinMaxValue()));
    menu->insertSeparator(menu->actions().first());
    menu->insertAction(menu->actions().first(), setDefaultValue);
    menu->exec(event->globalPos());
    delete menu;
}
void RegionLineEdit::sl_onSetMinMaxValue() {
    setText(QString::number(defaultValue));
    emit textEdited(QString::number(defaultValue));
}

}  // namespace U2
