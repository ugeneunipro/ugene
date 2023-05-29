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

#include "ComboBoxWithCheckBoxes.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QStylePainter>
#include <QWidget>

namespace U2 {

ComboBoxWithCheckBoxes::ComboBoxWithCheckBoxes(QWidget *parent)
    : QComboBox(parent),
      displayRectDelta(4, 1, -25, 0) {
    connect(model(), &QAbstractItemModel::rowsInserted, this, &ComboBoxWithCheckBoxes::sl_modelRowsInserted);
    connect(model(), &QAbstractItemModel::rowsRemoved, this, &ComboBoxWithCheckBoxes::sl_modelRowsRemoved);

    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel *>(model());

    connect(standartModel, &QStandardItemModel::itemChanged, this, &ComboBoxWithCheckBoxes::sl_modelItemChanged);

}

const QStringList& ComboBoxWithCheckBoxes::getCheckedItems() const {
    return checkedItems;
}

void ComboBoxWithCheckBoxes::setCheckedItems(const QStringList &items) {
    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel *>(model());
    SAFE_POINT(standartModel != nullptr, L10N::nullPointerError("QStandardItemModel"), );

    disconnect(standartModel, &QStandardItemModel::itemChanged, this, &ComboBoxWithCheckBoxes::sl_modelItemChanged);

    QList<int> checkedIndexes;
    for (int i = 0; i < items.count(); ++i) {
        checkedIndexes << findText(items.at(i));
    }

    for (int i = 0; i < count(); ++i) {
        QStandardItem* currentItem = standartModel->item(i);
        SAFE_POINT(currentItem != nullptr, L10N::nullPointerError("QStandardItem"), );

        Qt::CheckState newState = checkedIndexes.contains(i) ? Qt::Checked : Qt::Unchecked;

        auto item = standartModel->item(i);
        auto checkStateRole = static_cast<Qt::CheckState>(item->data(Qt::CheckStateRole).toInt());
        if (checkStateRole != newState) {
            item->setData(newState, Qt::CheckStateRole);
        }
    }

    connect(standartModel, &QStandardItemModel::itemChanged, this, &ComboBoxWithCheckBoxes::sl_modelItemChanged);

    updateOnCheckedItemsChange();
}

void ComboBoxWithCheckBoxes::paintEvent(QPaintEvent *) {
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox option;
    initStyleOption(&option);
    painter.drawComplexControl(QStyle::CC_ComboBox, option);

    QRect textRect = rect().adjusted(displayRectDelta.left(), displayRectDelta.top(), displayRectDelta.right(), displayRectDelta.bottom());
    painter.drawText(textRect, Qt::AlignVCenter, displayText);
}

void ComboBoxWithCheckBoxes::resizeEvent(QResizeEvent *e) {
    updateDisplayText();
    QComboBox::resizeEvent(e);
}

void ComboBoxWithCheckBoxes::sl_modelRowsInserted(const QModelIndex &, int start, int end) {
    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel *>(model());
    SAFE_POINT(standartModel != nullptr, L10N::nullPointerError("QStandardItemModel"), );

    disconnect(standartModel, &QStandardItemModel::itemChanged, this, &ComboBoxWithCheckBoxes::sl_modelItemChanged);

    for (int i = start; i <= end; ++i) {
        standartModel->item(i)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        standartModel->item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }

    connect(standartModel, &QStandardItemModel::itemChanged, this, &ComboBoxWithCheckBoxes::sl_modelItemChanged);
}

void ComboBoxWithCheckBoxes::sl_modelRowsRemoved(const QModelIndex &, int , int ) {
    updateOnCheckedItemsChange();
}

void ComboBoxWithCheckBoxes::sl_modelItemChanged(QStandardItem *) {
    updateOnCheckedItemsChange();
}

void ComboBoxWithCheckBoxes::updateOnCheckedItemsChange() {
    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel *>(model());
    SAFE_POINT(standartModel != nullptr, L10N::nullPointerError("QStandardItemModel"), );

    checkedItems.clear();
    for (int i = 0; i < count(); ++i) {
        QStandardItem *currentItem = standartModel->item(i);
        SAFE_POINT(currentItem != nullptr, L10N::nullPointerError("QStandardItem"), );

        Qt::CheckState checkState = static_cast<Qt::CheckState>(currentItem->data(Qt::CheckStateRole).toInt());
        if (checkState == Qt::Checked) {
            checkedItems.push_back(currentItem->text());
        }
    }

    updateDisplayText();
    update();

    emit si_checkedChanged(checkedItems);
}

void ComboBoxWithCheckBoxes::updateDisplayText() {
    QRect textRect = rect().adjusted(displayRectDelta.left(), displayRectDelta.top(), displayRectDelta.right(), displayRectDelta.bottom());
    QFontMetrics fontMetrics(font());

    displayText = checkedItems.join(", ");

    if (fontMetrics.size(Qt::TextSingleLine, displayText).width() > textRect.width()) {
        while (displayText != "" && fontMetrics.size(Qt::TextSingleLine, displayText + "...").width() > textRect.width()) {
            displayText.remove(displayText.length() - 1, 1);
        }

        displayText += "...";
    }
}

}
