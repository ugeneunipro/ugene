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

#include <QAbstractButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include "U2SavableWidget.h"

Q_DECLARE_METATYPE(QVector<QVector<QString> >)

namespace U2 {

U2SavableWidget::U2SavableWidget(QWidget *wrappedWidget, MWMDIWindow *contextWindow)
    : wrappedWidget(wrappedWidget), contextWindow(contextWindow), widgetStateSaved(false)
{
    SAFE_POINT(NULL != wrappedWidget, L10N::nullPointerError("wrapped widget"), );
}

U2SavableWidget::~U2SavableWidget() {
    if (!widgetStateSaved) {
        U2WidgetStateStorage::saveWidgetState(*this);
    }
}

QString U2SavableWidget::getWidgetId() const {
    return wrappedWidget->metaObject()->className();
}

QSet<QString> U2SavableWidget::getChildIds() const {
    const QSet<QWidget *> compounChildren = getCompoundChildren();
    const QSet<QWidget *> allChildren = wrappedWidget->findChildren<QWidget *>().toSet();
    QSet<QWidget *> childrenToConsider = allChildren;

    foreach (QWidget *compoundChild, compounChildren) {
        childrenToConsider -= compoundChild->findChildren<QWidget *>().toSet();
    }

    QSet<QString> result;
    foreach(QWidget *child, childrenToConsider) {
        if (childCanBeSaved(child)) {
            result.insert(getChildId(child));
        }
    }
    return result;
}

bool U2SavableWidget::childValueIsAcceptable(const QString &childId, const QVariant &value) const {
    Q_UNUSED(value);
    return childExists(childId);
}

namespace {

QVector<QVector<QString> > getTableWidgetContent(QTableWidget *tableWidget) {
    const int columnCount = tableWidget->columnCount();
    const int rowCount = tableWidget->rowCount();

    QVector<QVector<QString> > result;
    result.reserve(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        QVector<QString> rowContent;
        rowContent.reserve(columnCount);
        for (int j = 0; j < columnCount; ++j) {
            rowContent.append(tableWidget->item(i, j)->text());
        }
        result.prepend(rowContent);
    }
    return result;
}

void setTableWidgetContent(QTableWidget *tableWidget, const QVector<QVector<QString> > &content) {
    CHECK(!content.isEmpty() && !content[0].isEmpty(), );

    const int rowCount = content.size();
    const int columnCount = content[0].size();

    tableWidget->setRowCount(rowCount);
    tableWidget->setColumnCount(columnCount);

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < columnCount; ++j) {
            QTableWidgetItem *newItem = new QTableWidgetItem(content[i][j]);
            tableWidget->setItem(i, j, newItem);
        }
    }
}

}

QVariant U2SavableWidget::getChildValue(const QString &childId) const {
    QVariant result;
    SAFE_POINT(childExists(childId), "Child widget expected", result);

    QWidget *child = getChildWidgetById(childId);
    if (NULL != qobject_cast<QLineEdit *>(child)) {
        result = qobject_cast<QLineEdit *>(child)->text();
    } else if (NULL != qobject_cast<QTextEdit *>(child)) {
        result = qobject_cast<QTextEdit *>(child)->toPlainText();
    } else if (NULL != qobject_cast<QComboBox *>(child)) {
        result = qobject_cast<QComboBox *>(child)->currentIndex();
    } else if (NULL != qobject_cast<QAbstractButton *>(child)) {
        result = qobject_cast<QAbstractButton *>(child)->isChecked();
    } else if (NULL != qobject_cast<QGroupBox *>(child)) {
        result = qobject_cast<QGroupBox *>(child)->isChecked();
    } else if (NULL != qobject_cast<QSpinBox *>(child)) {
        result = qobject_cast<QSpinBox *>(child)->value();
    } else if (NULL != qobject_cast<QDoubleSpinBox *>(child)) {
        result = qobject_cast<QDoubleSpinBox *>(child)->value();
    } else if (NULL != qobject_cast<QSlider *>(child)) {
        result = qobject_cast<QSlider *>(child)->value();
    } else if (NULL != qobject_cast<QTableWidget *>(child)) {
        return QVariant::fromValue<QVector<QVector<QString> > >(getTableWidgetContent(qobject_cast<QTableWidget *>(child)));
    } else if (NULL != qobject_cast<ShowHideSubgroupWidget *>(child)) {
        result = qobject_cast<ShowHideSubgroupWidget *>(child)->isSubgroupOpened();
    } else {
        FAIL("Unexpected child widget type", result);
    }
    return result;
}

void U2SavableWidget::setChildValue(const QString &childId, const QVariant &value) {
    SAFE_POINT(childExists(childId), "Child widget expected", );

    QWidget *child = getChildWidgetById(childId);
    if (NULL != qobject_cast<QLineEdit *>(child)) {
        qobject_cast<QLineEdit *>(child)->setText(value.toString());
    } else if (NULL != qobject_cast<QTextEdit *>(child)) {
        qobject_cast<QTextEdit *>(child)->setText(value.toString());
    } else if (NULL != qobject_cast<QComboBox *>(child)) {
        QComboBox *cb = qobject_cast<QComboBox *>(child);
        if (cb->count() == 0) {
            return;
        } else if (cb->count() <= value.toInt()) {
            cb->setCurrentIndex(0);
        } else {
            cb->setCurrentIndex(value.toInt());
        }
    } else if (NULL != qobject_cast<QAbstractButton *>(child)) {
        qobject_cast<QAbstractButton *>(child)->setChecked(value.toBool());
    } else if (NULL != qobject_cast<QGroupBox *>(child)) {
        qobject_cast<QGroupBox *>(child)->setChecked(value.toBool());
    } else if (NULL != qobject_cast<QSpinBox *>(child)) {
        qobject_cast<QSpinBox *>(child)->setValue(value.toInt());
    } else if (NULL != qobject_cast<QDoubleSpinBox *>(child)) {
        qobject_cast<QDoubleSpinBox *>(child)->setValue(value.toDouble());
    } else if (NULL != qobject_cast<QSlider *>(child)) {
        qobject_cast<QSlider *>(child)->setValue(value.toInt());
    } else if (NULL != qobject_cast<QTableWidget *>(child)) {
        setTableWidgetContent(qobject_cast<QTableWidget *>(child), value.value<QVector<QVector<QString> > >());
    } else if (NULL != qobject_cast<ShowHideSubgroupWidget *>(child)) {
        qobject_cast<ShowHideSubgroupWidget *>(child)->setSubgroupOpened(value.toBool());
    } else {
        FAIL("Unexpected child widget type", );
    }
}

MWMDIWindow * U2SavableWidget::getContextWindow() const {
    return contextWindow;
}

QSet<QWidget *> U2SavableWidget::getCompoundChildren() const {
    return QSet<QWidget *>();
}

bool U2SavableWidget::childCanBeSaved(QWidget *child) const {
    const QString widgetName = child->objectName();
    return ((NULL != qobject_cast<QLineEdit *>(child)
        && NULL == qobject_cast<QFontComboBox *>(child->parent()) && widgetName != "qt_spinbox_lineedit") // skip fake line edit inside a spin box
        || NULL != qobject_cast<QTextEdit *>(child)
        || NULL != qobject_cast<QComboBox *>(child)
        || (NULL != qobject_cast<QAbstractButton *>(child) && qobject_cast<QAbstractButton *>(child)->isCheckable())
        || (NULL != qobject_cast<QGroupBox *>(child) && qobject_cast<QGroupBox *>(child)->isCheckable())
        || NULL != qobject_cast<QSpinBox *>(child)
        || NULL != qobject_cast<QDoubleSpinBox *>(child)
        || NULL != qobject_cast<QSlider *>(child)
        || NULL != qobject_cast<QTableWidget *>(child)
        || NULL != qobject_cast<ShowHideSubgroupWidget *>(child))
        && !widgetName.isEmpty();
}

QString U2SavableWidget::getChildId(QWidget *child) const {
    return child->objectName();
}

QWidget * U2SavableWidget::getChildWidgetById(const QString &childId) const {
    SAFE_POINT(childExists(childId), "Unexpected child widget ID", NULL);
    return wrappedWidget->findChildren<QWidget *>(childId).first();
}

bool U2SavableWidget::childExists(const QString &childId) const {
    QList<QWidget *> allChildWidgets = wrappedWidget->findChildren<QWidget *>(childId);
    SAFE_POINT(allChildWidgets.size() < 2, "Widget ID duplicated", false);
    return 1 == allChildWidgets.size();
}

} // namespace U2
