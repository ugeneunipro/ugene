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

#include <math.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Core/QObjectScopedPointer.h>

#include "RegionSelector.h"

namespace U2 {
////////////////////////////////////////
// RangeSelectorWidget
const QString RegionSelector::WHOLE_SEQUENCE = QApplication::translate("RegionSelector", "Whole sequence");
const QString RegionSelector::SELECTED_REGION = QApplication::translate("RegionSelector", "Selected region");
const QString RegionSelector::CUSTOM_REGION = QApplication::translate("RegionSelector", "Custom region");

RegionSelector::RegionSelector(QWidget* p, qint64 len, bool isVertical,
                               DNASequenceSelection* selection,
                               bool isCircularSelectionAvailable,
                               QList<RegionPreset> presetRegions) :
    QWidget(p),
    maxLen(len),
    startEdit(NULL),
    endEdit(NULL),
    isVertical(isVertical)
{
    initLayout();

    RegionSelectorGui gui(startEdit, endEdit, comboBox);
    RegionSelectorSettings settings(len, isCircularSelectionAvailable, selection, presetRegions);
    controller = new RegionSelectorController(gui, settings, this);
    connect(controller, SIGNAL(si_regionChanged(U2Region)), this, SIGNAL(si_regionChanged(U2Region)));
}

U2Region RegionSelector::getRegion(bool *_ok) const {
    return controller->getRegion(_ok);
}

bool RegionSelector::isWholeSequenceSelected() const {
    return controller->getPresetName() == RegionSelectorSettings::WHOLE_SEQUENCE;
}

void RegionSelector::setCustomRegion(const U2Region& value) {
    controller->setRegion(value);
}

void RegionSelector::setWholeRegionSelected() {
    controller->setPreset(RegionSelectorSettings::WHOLE_SEQUENCE);
}

void RegionSelector::setCurrentPreset(const QString &presetName) {
    controller->setPreset(presetName);
}

void RegionSelector::reset() {
    controller->reset();
}

void RegionSelector::removePreset(const QString &itemName) {
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

void RegionSelector::initLayout() {
    int w = qMax(((int)log10((double)maxLen))*10, 50);

    comboBox = new QComboBox(this);

    startEdit = new RegionLineEdit(this, tr("Set minimum"), 1);
    startEdit->setValidator(new QIntValidator(1, maxLen, startEdit));
    startEdit->setMinimumWidth(w);
    startEdit->setAlignment(Qt::AlignRight);

    endEdit = new RegionLineEdit(this, tr("Set maximum"), maxLen);
    endEdit->setValidator(new QIntValidator(1, maxLen, endEdit));
    endEdit->setMinimumWidth(w);
    endEdit->setAlignment(Qt::AlignRight);

    if (isVertical) {
        QGroupBox* gb = new QGroupBox(this);
        gb->setTitle(tr("Region"));

        QGridLayout* l = new QGridLayout(gb);
        l->setSizeConstraint(QLayout::SetMinAndMaxSize);
        gb->setLayout(l);

        l->addWidget(comboBox, 0, 0, 1, 3);
        l->addWidget(startEdit, 1, 0);
        l->addWidget(new QLabel(tr("-"), gb), 1, 1);
        l->addWidget(endEdit, 1, 2);
        l->addWidget(new QLabel(" ", gb), 2, 0);

        QVBoxLayout* rootLayout = new QVBoxLayout(this);
        rootLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        rootLayout->setMargin(0);
        setLayout(rootLayout);
        rootLayout->addWidget(gb);
    } else {
        QHBoxLayout* l = new QHBoxLayout(this);
        l->setMargin(0);
        setLayout(l);

        QLabel* rangeLabel = new QLabel(tr("Region"), this);
        rangeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        l->addWidget(rangeLabel);
        l->addWidget(comboBox);
        l->addWidget(startEdit);
        l->addWidget(new QLabel(tr("-"), this));
        l->addWidget(endEdit);
    }

    startEdit->setObjectName("start_edit_line");
    endEdit->setObjectName("end_edit_line");
    comboBox->setObjectName("region_type_combo");
    setObjectName("range_selector");
}

///////////////////////////////////////
//! RegionLineEdit
//! only for empty field highlight
void RegionLineEdit::focusOutEvent ( QFocusEvent * event) {
    bool ok = false;
    text().toInt(&ok);
    if (!ok) {
        QPalette p = palette();
        p.setColor(QPalette::Base, QColor(255,200,200));//pink color
        setPalette(p);
    }
    QLineEdit::focusOutEvent(event);
}
void RegionLineEdit::contextMenuEvent(QContextMenuEvent *event){
        QMenu *menu = createStandardContextMenu();
        QAction* setDefaultValue=new QAction(actionName,this);
        connect(setDefaultValue,SIGNAL(triggered()),this,SLOT(sl_onSetMinMaxValue()));
        menu->insertSeparator(menu->actions().first());
        menu->insertAction(menu->actions().first(),setDefaultValue);
        menu->exec(event->globalPos());
        delete menu;
}
void RegionLineEdit::sl_onSetMinMaxValue(){
    setText(QString::number(defaultValue));
    emit textEdited(QString::number(defaultValue));
}

} //namespace
