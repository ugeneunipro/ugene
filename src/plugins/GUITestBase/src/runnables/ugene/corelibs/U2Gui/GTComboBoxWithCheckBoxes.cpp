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

#include "GTComboBoxWithCheckBoxes.h"

#include <utils/GTThread.h>

#include <QListView>
#include <QStandardItemModel>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {


#define GT_CLASS_NAME "GTComboBoxWithCheckBoxes"

#define GT_METHOD_NAME "selectItemByIndex"
void GTComboBoxWithCheckBoxes::selectItemByIndex(GUITestOpStatus& os, QComboBox* comboBox, const QList<int>& indexes, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");

    // Access to the internal comboBox->view() must be done from the main thread (the view is lazily instantiated).
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QComboBox* comboBox, const QList<int>& indexes, GTGlobals::UseMethod method)
            : comboBox(comboBox), indexes(indexes), method(method) {
        }

        void run(HI::GUITestOpStatus& os) override {
            GTWidget::click(os, comboBox);
            QStandardItemModel* standartModel = qobject_cast<QStandardItemModel*>(comboBox->model());
            GT_CHECK(standartModel != nullptr, "QStandardItemModel* == nullptr");

            int itemsCount = comboBox->count();
            for (int index = 0; index < itemsCount; index++) {
                Qt::CheckState expectedState = Qt::Checked;
                if (!indexes.contains(index)) {
                    expectedState = Qt::Unchecked;
                }

                QStandardItem* currentItem = standartModel->item(index);
                GT_CHECK(currentItem != nullptr, "QStandardItem* == nullptr");

                auto state = static_cast<Qt::CheckState>(currentItem->data(Qt::CheckStateRole).toInt());
                switch (method) {
                case GTGlobals::UseKey:
                case GTGlobals::UseKeyBoard:
                {
                    if (state != expectedState) {
                        GTKeyboardDriver::keyClick(Qt::Key_Space);
                    }

                    GTKeyboardDriver::keyClick(Qt::Key_Down);
                    break;
                }
                case GTGlobals::UseMouse:
                {
                    CHECK_CONTINUE(state != expectedState);

                    QListView* listView = comboBox->findChild<QListView*>();
                    GT_CHECK(listView != nullptr, "list view not found");
                    QModelIndex modelIndex = listView->model()->index(index, 0);
                    GTWidget::scrollToIndex(os, listView, modelIndex);
                    QRect rect = listView->visualRect(modelIndex);
                    QPoint itemPointLocal = rect.topLeft() + QPoint(12, rect.height() / 2); // 12 - checkbox position
                    QPoint itemPointGlobal = listView->viewport()->mapToGlobal(itemPointLocal);
                    qDebug("GT_DEBUG_MESSAGE moving to the list item: %d %d -> %d %d", QCursor::pos().x(), QCursor::pos().y(), itemPointGlobal.x(), itemPointGlobal.y());
                    GTMouseDriver::moveTo(itemPointGlobal);
                    GTMouseDriver::click();
                    break;
                }
                default:
                    GT_FAIL("Unexpected method", );
                }

            }
        }
        QComboBox* comboBox;
        QList<int> indexes;
        GTGlobals::UseMethod method;
    };
    GTThread::runInMainThread(os, new MainThreadAction(comboBox, indexes, method));
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemByText"
void GTComboBoxWithCheckBoxes::selectItemByText(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& texts, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");

    QList<int> indexes;
    for (const auto& text : qAsConst(texts)) {
        int index = comboBox->findText(text, Qt::MatchExactly);
        GT_CHECK(index != -1, "Text " + text + " was not found");

        indexes << index;
    }

    selectItemByIndex(os, comboBox, indexes, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemByText"
void GTComboBoxWithCheckBoxes::selectItemByText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent, const QStringList& texts, GTGlobals::UseMethod method) {
    selectItemByText(os, GTWidget::findComboBox(os, comboBoxName, parent), texts, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCheckedItemsTexts"
QStringList GTComboBoxWithCheckBoxes::getCheckedItemsTexts(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent) {
    auto cb = GTWidget::findComboBox(os, comboBoxName, parent);
    GT_CHECK_RESULT(cb != nullptr, "QComboBox* == nullptr", QStringList());

    QStandardItemModel* standartModel = qobject_cast<QStandardItemModel*>(cb->model());
    GT_CHECK_RESULT(standartModel != nullptr, "QStandardItemModel* == nullptr", QStringList());

    QStringList result;
    int itemsCount = cb->count();
    for (int index = 0; index < itemsCount; index++) {
        QStandardItem* currentItem = standartModel->item(index);
        GT_CHECK_RESULT(currentItem != nullptr, "QStandardItem* == nullptr", QStringList());

        auto state = static_cast<Qt::CheckState>(currentItem->data(Qt::CheckStateRole).toInt());
        CHECK_CONTINUE(state == Qt::Checked);

        auto text = currentItem->text();
        result << text;
    }

    return result;
}
#undef GT_METHOD_NAME



#undef GT_CLASS_NAME

}
