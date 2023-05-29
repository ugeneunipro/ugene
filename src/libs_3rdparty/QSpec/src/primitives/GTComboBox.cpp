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

#include "primitives/GTComboBox.h"
#include <utils/GTThread.h>

#include <QListView>
#include <QStandardItemModel>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTComboBox"

#define GT_METHOD_NAME "selectItemByIndex"
void GTComboBox::selectItemByIndex(GUITestOpStatus& os, QComboBox* comboBox, int index, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");
    if (comboBox->currentIndex() == index) {
        return;
    }

    // Access to the internal comboBox->view() must be done from the main thread (the view is lazily instantiated).
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QComboBox* comboBox, int index, GTGlobals::UseMethod method)
            : CustomScenario(), comboBox(comboBox), index(index), method(method) {
        }
        void run(HI::GUITestOpStatus& os) override {
            int itemsCount = comboBox->count();
            GT_CHECK(index >= 0 && index < itemsCount, "invalid index: " + QString::number(index) + ", count: " + QString::number(itemsCount));

            if (comboBox->isEditable()) {
                GTWidget::click(os, comboBox, Qt::LeftButton, QPoint(comboBox->rect().width() - 10, 10));
                GTThread::waitForMainThread();
            } else if (!comboBox->view()->isVisible()) {  // activate dropdown if it is not visible.
                GTWidget::click(os, comboBox);
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
                    QListView* listView = comboBox->findChild<QListView*>();
                    GT_CHECK(listView != nullptr, "list view not found");
                    QModelIndex modelIndex = listView->model()->index(index, 0);
                    GTWidget::scrollToIndex(os, listView, modelIndex);
#ifdef Q_OS_DARWIN
                    // Due to animation a single scroll on MacOS is not enough for long lists. See GUITest_common_scenarios_create_annotation_widget_test_0002.
                    GTWidget::scrollToIndex(os, listView, modelIndex);
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
    GTThread::runInMainThread(os, new MainThreadAction(comboBox, index, method));
    // Activate the final action from a separate thread. Reason: it may trigger other popups or dialogs (they can't be processed in the main thread).
    if (method == GTGlobals::UseMouse) {
        GTMouseDriver::click();
    } else {
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemByText"
void GTComboBox::selectItemByText(GUITestOpStatus& os, QComboBox* comboBox, const QString& text, GTGlobals::UseMethod method) {
    GT_CHECK(comboBox != nullptr, "QComboBox* == NULL");
    int index = comboBox->findText(text, Qt::MatchExactly);
    GT_CHECK(index != -1, "Text " + text + " was not found");
    selectItemByIndex(os, comboBox, index, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemByText"
void GTComboBox::selectItemByText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent, const QString& text, GTGlobals::UseMethod method) {
    selectItemByText(os, GTWidget::findComboBox(os, comboBoxName, parent), text, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCurrentText"
QString GTComboBox::getCurrentText(GUITestOpStatus& os, QComboBox* const comboBox) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(comboBox != NULL, "comboBox is NULL", "");
    return comboBox->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCurrentText"
QString GTComboBox::getCurrentText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent) {
    return getCurrentText(os, GTWidget::findComboBox(os, comboBoxName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getValues"
QStringList GTComboBox::getValues(GUITestOpStatus& os, QComboBox* comboBox) {
    QStringList result;
    GT_CHECK_RESULT(NULL != comboBox, "Combobox is NULL", result);
    for (int i = 0; i < comboBox->count(); i++) {
        result << comboBox->itemText(i);
    }
    return result;
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkValues"
void GTComboBox::checkValues(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& values) {
    GT_CHECK(NULL != comboBox, "comboBox is NULL");

    GTWidget::setFocus(os, comboBox);
    GTGlobals::sleep();

    QListView* view = comboBox->findChild<QListView*>();
    GT_CHECK(NULL != view, "list view is not found");
    auto model = dynamic_cast<QStandardItemModel*>(view->model());
    GT_CHECK(NULL != model, "model is not found");
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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkValuesPresence"
void GTComboBox::checkValuesPresence(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& values) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");

    for (const QString& s : qAsConst(values)) {
        int index = comboBox->findText(s);
        GT_CHECK(index != -1, "ComboBox item with text not found: " + s);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkCurrentValue"
void GTComboBox::checkCurrentValue(GUITestOpStatus& os, QComboBox* comboBox, const QString& expectedText) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");
    QString currentText = comboBox->currentText();
    GT_CHECK(currentText == expectedText, QString("Unexpected value: expected '%1', got '%2'").arg(expectedText).arg(currentText));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkCurrentUserDataValue"
void GTComboBox::checkCurrentUserDataValue(GUITestOpStatus& os, QComboBox* comboBox, const QString& expectedValue) {
    GT_CHECK(comboBox != nullptr, "ComboBox is NULL");
    QString dataValue = comboBox->currentData(Qt::UserRole).toString();
    GT_CHECK(dataValue == expectedValue, QString("Unexpected user data value: expected '%1', got '%2'").arg(expectedValue).arg(dataValue));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
