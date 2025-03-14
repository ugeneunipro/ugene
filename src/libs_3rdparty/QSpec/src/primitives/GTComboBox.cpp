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

#include "primitives/GTComboBox.h"
#include <utils/GTThread.h>

#include <QListView>
#include <QStandardItemModel>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTComboBox"

void GTComboBox::selectItemByIndex(QComboBox* comboBox, int index, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");
    if (comboBox->currentIndex() == index) {
        return;
    }

    // Access to the internal comboBox->view() must be done from the main thread (the view is lazily instantiated).
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QComboBox* comboBox, int index, GTGlobals::UseMethod method)
            : comboBox(comboBox), index(index), method(method) {
        }
        void run() override {
            int itemsCount = comboBox->count();
            GT_CHECK(index >= 0 && index < itemsCount, "invalid index: " + QString::number(index) + ", count: " + QString::number(itemsCount));

            if (comboBox->isEditable()) {
                GTWidget::click(comboBox, Qt::LeftButton, QPoint(comboBox->rect().width() - 10, 10));
                GTThread::waitForMainThread();
            } else if (!comboBox->view()->isVisible()) {  // activate dropdown if it is not visible.
                GTWidget::click(comboBox);
                GTThread::waitForMainThread();
            }

            switch (method) {
                case GTGlobals::UseKey:
                case GTGlobals::UseKeyBoard: {
                    int currentIndex = comboBox->currentIndex() == -1 ? 0 : comboBox->currentIndex();
                    Qt::Key key = index > currentIndex ? Qt::Key_Down : Qt::Key_Up;
                    int pressCount = qAbs(index - currentIndex);
                    for (int i = 0; i < pressCount; i++) {
                        GTKeyboardDriver::keyClick(key);
                        GTThread::waitForMainThread();
                    }
                    break;
                }
                case GTGlobals::UseMouse: {
                    auto listView = comboBox->findChild<QListView*>();
                    GT_CHECK(listView != nullptr, "list view not found");
                    QModelIndex modelIndex = listView->model()->index(index, 0);
                    GTWidget::scrollToIndex(listView, modelIndex);
#ifdef Q_OS_DARWIN
                    // Due to animation a single scroll on MacOS is not enough for long lists. See GUITest_common_scenarios_create_annotation_widget_test_0002.
                    GTWidget::scrollToIndex(listView, modelIndex);
#endif
                    QRect rect = listView->visualRect(modelIndex);
                    QPoint itemPointLocal = rect.topLeft() + QPoint(25, rect.height() / 2);  // Why +25px: Qt 5.12 may report too big rect with the center() out of the item.
                    QPoint itemPointGlobal = listView->viewport()->mapToGlobal(itemPointLocal);
                    qDebug("GT_DEBUG_MESSAGE moving to the list item: %d %d -> %d %d", QCursor::pos().x(), QCursor::pos().y(), itemPointGlobal.x(), itemPointGlobal.y());
                    GTMouseDriver::moveTo(itemPointGlobal);
                    break;
                }
                default:
                    GT_FAIL("Unexpected method", );
            }
        }
        QComboBox* comboBox;
        int index;
        GTGlobals::UseMethod method;
    };
    GTThread::runInMainThread(new MainThreadAction(comboBox, index, method));
    // Activate the final action from a separate thread. Reason: it may trigger other popups or dialogs (they can't be processed in the main thread).
    if (method == GTGlobals::UseMouse) {
        GTMouseDriver::click();
    } else {
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
    }
}

void GTComboBox::selectItemByText(QComboBox* comboBox, const QString& text, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");
    int index = comboBox->findText(text, Qt::MatchExactly);
    GT_CHECK(index != -1, "Text " + text + " was not found");
    selectItemByIndex(comboBox, index, method);
}

void GTComboBox::selectItemByText(const QString& comboBoxName, QWidget* parent, const QString& text, GTGlobals::UseMethod method) {
    selectItemByText(GTWidget::findComboBox(comboBoxName, parent), text, method);
}

QString GTComboBox::getCurrentText(QComboBox* const comboBox) {

    GT_CHECK_RESULT(comboBox != nullptr, "comboBox is NULL", "");
    return comboBox->currentText();
}

QString GTComboBox::getCurrentText(const QString& comboBoxName, QWidget* parent) {
    return getCurrentText(GTWidget::findComboBox(comboBoxName, parent));
}

QStringList GTComboBox::getValues(QComboBox* comboBox) {
    QStringList result;
    GT_CHECK_RESULT(comboBox != nullptr, "Combobox is NULL", result);
    for (int i = 0; i < comboBox->count(); i++) {
        result << comboBox->itemText(i);
    }
    return result;
}


void GTComboBox::checkValues(QComboBox* comboBox, const QStringList& values) {
    GT_CHECK(comboBox != nullptr, "comboBox is NULL");

    GTWidget::setFocus(comboBox);
    GTGlobals::sleep();

    auto view = comboBox->findChild<QListView*>();
    GT_CHECK(view != nullptr, "list view is not found");
    auto model = dynamic_cast<QStandardItemModel*>(view->model());
    GT_CHECK(model != nullptr, "model is not found");
    QList<QStandardItem*> items = model->findItems("", Qt::MatchContains);

    for (QStandardItem* item : items) {
        if (values.contains(item->data().toString())) {
            if (item->checkState() != Qt::Checked) {
                QModelIndex modelIndex = item->index();
                view->scrollTo(modelIndex);
                GTGlobals::sleep(500);
                QRect itemRect = view->visualRect(modelIndex);
                QPoint checkPoint(itemRect.left() + 10, itemRect.center().y());
                GTMouseDriver::moveTo(view->viewport()->mapToGlobal(checkPoint));
                GTMouseDriver::click();
                GTGlobals::sleep(500);
                GT_CHECK(item->checkState() == Qt::Checked, "Item is not checked: " + item->data().toString());
            }
        } else {
            if (item->checkState() == Qt::Checked) {
                QModelIndex modelIndex = item->index();
                view->scrollTo(modelIndex);
                GTGlobals::sleep(500);
                QRect itemRect = view->visualRect(modelIndex);
                QPoint checkPoint(itemRect.left() + 10, itemRect.center().y());
                GTMouseDriver::moveTo(view->viewport()->mapToGlobal(checkPoint));
                GTMouseDriver::click();
                GTGlobals::sleep(500);
                GT_CHECK(item->checkState() != Qt::Checked, "Item is checked: " + item->data().toString());
            }
        }
    }
}

void GTComboBox::checkValuesPresence(QComboBox* comboBox, const QStringList& values) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");

    for (const QString& s : qAsConst(values)) {
        int index = comboBox->findText(s);
        GT_CHECK(index != -1, "ComboBox item with text not found: " + s);
    }
}

void GTComboBox::checkCurrentValue(QComboBox* comboBox, const QString& expectedText) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");
    QString currentText = comboBox->currentText();
    GT_CHECK(currentText == expectedText, QString("Unexpected value: expected '%1', got '%2'").arg(expectedText).arg(currentText));
}

void GTComboBox::checkCurrentUserDataValue(QComboBox* comboBox, const QString& expectedValue) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");
    QString dataValue = comboBox->currentData(Qt::UserRole).toString();
    GT_CHECK(dataValue == expectedValue, QString("Unexpected user data value: expected '%1', got '%2'").arg(expectedValue).arg(dataValue));
}

#undef GT_CLASS_NAME

}  // namespace HI
