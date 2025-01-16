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

#include "GUIUtils.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QProcess>
#include <QUrl>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/Theme.h>

namespace U2 {

QAction* GUIUtils::getCheckedAction(QList<QAction*> actions) {
    foreach (QAction* action, actions) {
        if (action->isChecked()) {
            return action;
        }
    }
    return nullptr;
}

QAction* GUIUtils::findActionByData(QList<QAction*> actions, const QString& data) {
    foreach (QAction* action, actions) {
        if (action->data() == data) {
            return action;
        }
    }
    return nullptr;
}

QAction* GUIUtils::findAction(const QList<QAction*>& actions, const QString& name) {
    foreach (QAction* a, actions) {
        const QString& actionName = a->objectName();
        if (actionName == name) {
            return a;
        }
    }
    return nullptr;
}

QAction* GUIUtils::findActionAfter(const QList<QAction*>& actions, const QString& name) {
    bool found = false;
    foreach (QAction* a, actions) {
        if (found) {
            return a;
        }
        const QString& actionName = a->objectName();
        if (actionName == name) {
            found = true;
        }
    }
    if (found) {
        return nullptr;
    }
    return actions.first();
}

QMenu* GUIUtils::findSubMenu(const QMenu* m, const QString& name) {
    QAction* action = findAction(m->actions(), name);
    if (action == nullptr) {
        return nullptr;
    }
    return action->menu();
}

void GUIUtils::updateActionToolTip(QAction* action) {
    const QString actionShortcutString = action->shortcut().toString(QKeySequence::NativeText);
    const QString toolTip = actionShortcutString.isEmpty() ? action->text() : QString("%1 (%2)").arg(action->text()).arg(actionShortcutString);
    action->setToolTip(toolTip);
}

void GUIUtils::updateButtonToolTip(QAbstractButton* button, const QKeySequence& shortcut) {
    const QString actionShortcutString = shortcut.toString(QKeySequence::NativeText);
    const QString toolTip = actionShortcutString.isEmpty() ? button->text() : QString("%1 (%2)").arg(button->text()).arg(actionShortcutString);
    button->setToolTip(toolTip);
}

void GUIUtils::disableEmptySubmenus(const QMenu* m) {
    foreach (QAction* action, m->actions()) {
        QMenu* am = action->menu();
        if (am != nullptr && am->actions().isEmpty()) {
            action->setEnabled(false);
        }
    }
}

QIcon GUIUtils::createSquareIcon(const QColor& c, int size) {
    int w = size;
    int h = size;
    QPixmap pix(w, h);
    QPainter p(&pix);
    p.setPen(QPalette().text().color());
    p.drawRect(0, 0, w - 1, h - 1);
    p.fillRect(1, 1, w - 2, h - 2, c);
    p.end();
    return {pix};
}

QIcon GUIUtils::createRoundIcon(const QColor& c, int size) {
    int w = size;
    int h = size;

    QPixmap pix(w, h);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    QPainterPath ep;
    // ep.addEllipse(1, 1, w-2, h-2);
    ep.addEllipse(0, 0, w - 1, h - 1);
    p.fillPath(ep, c);
    p.setPen(QPalette().text().color());
    p.drawEllipse(0, 0, w - 1, h - 1);
    p.end();
    return {pix};
}

bool GUIUtils::runWebBrowser(const QString& url) {
    bool useDefaultWebBrowser = AppContext::getAppSettings()->getUserAppsSettings()->useDefaultWebBrowser();
    if (url.isEmpty()) {
        QMessageBox::critical(nullptr, tr("Error!"), tr("Document URL is empty!"));
        return false;
    }

    QString program = AppContext::getAppSettings()->getUserAppsSettings()->getWebBrowserURL();
    bool ok = !program.isEmpty() && QFile(program).exists();

    if (useDefaultWebBrowser) {
        bool launched = QDesktopServices::openUrl(QUrl(url));
        if (!launched) {
            QMessageBox::critical(nullptr, tr("Error!"), tr("Unable to launch default web browser."));
            AppContext::getAppSettings()->getUserAppsSettings()->setUseDefaultWebBrowser(false);
            AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_USER_APPS);
            program = AppContext::getAppSettings()->getUserAppsSettings()->getWebBrowserURL();
            ok = !program.isEmpty() && QFile(program).exists();
            if (!ok) {
                return false;
            }
            QString p = program;
            QStringList arguments;
            arguments.append(url);
            return QProcess::startDetached(program, arguments);
        }
        return launched;
    }
    if (!ok) {
        QMessageBox::critical(nullptr, tr("Error!"), tr("Please specify the browser executable"));
        AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_USER_APPS);
        program = AppContext::getAppSettings()->getUserAppsSettings()->getWebBrowserURL();

        ok = !program.isEmpty() && QFile(program).exists();
    }
    if (!ok) {
        return false;
    }
    QString p = program;
    QStringList arguments;
    arguments.append(url);
    return QProcess::startDetached(program, arguments);
}

bool GUIUtils::isMutedLnF(const QTreeWidgetItem* item) {
    static QBrush disabledBrush;
    if (disabledBrush.style() == Qt::NoBrush) {
        disabledBrush = QApplication::palette().brush(QPalette::Disabled, QPalette::WindowText);
    }
    return item->foreground(0) == disabledBrush;
}

void GUIUtils::setMutedLnF(QTreeWidgetItem* item, bool enableMute, bool recursive) {
    QPalette::ColorGroup colorGroup = enableMute ? QPalette::Disabled : QPalette::Active;
    QBrush brush = QApplication::palette().brush(colorGroup, QPalette::WindowText);
    int count = item->columnCount();
    for (int column = 0; column < count; column++) {
        item->setForeground(column, brush);
    }
    if (recursive) {
        int childCount = item->childCount();
        for (int i = 0; i < childCount; ++i) {
            QTreeWidgetItem* childItem = item->child(i);
            setMutedLnF(childItem, enableMute, recursive);
        }
    }
}

void GUIUtils::setWidgetWarningStyle(QWidget* widget, bool value) {
    QString color = value ? Theme::errorColorTextFieldStr() : "palette(base)";
    widget->setStyleSheet("background-color: " + color + ";");
}

void GUIUtils::showMessage(QWidget* widgetToPaintOn, QPainter& painter, const QString& message) {
    painter.fillRect(widgetToPaintOn->rect(), Qt::gray);

    QFontMetrics metrics(painter.font(), widgetToPaintOn);
    painter.drawText(widgetToPaintOn->rect(), Qt::AlignCenter, metrics.elidedText(message, Qt::ElideRight, widgetToPaintOn->rect().width()));
}

namespace {

QPixmap getPixmapResource(const QString& cathegory, const QString& iconName, bool hasColorCathegory) {
    QString resourceName = GUIUtils::getResourceName(cathegory, iconName, hasColorCathegory);
    QPixmap pixmap = QPixmap(resourceName);
    SAFE_POINT(!pixmap.isNull(), QString("Can't find icon from %1 named %2").arg(cathegory).arg(iconName), QPixmap());

    return pixmap;
}

}

QString GUIUtils::getResourceName(const QString& cathegory, const QString& iconName, bool hasColorCathegory, const QString& innerDirName) {
    bool isDark = AppContext::getMainWindow()->isDarkMode();
    QString colorCathegory;
    if (hasColorCathegory) {
        if (isDark) {
            colorCathegory = "dark/";
        } else {
            colorCathegory = "light/";
        }
    }
    QString inner;
    if (!innerDirName.isEmpty()) {
        inner = innerDirName + "/";
    }
    return QString(":%1/images/%2%3%4").arg(cathegory).arg(inner).arg(colorCathegory).arg(iconName);
}


QIcon GUIUtils::getIconResource(const QString& cathegory, const QString& iconName, bool hasColorCathegory) {
    CHECK((!cathegory.isEmpty() && !iconName.isEmpty()), QIcon());

    QIcon icon;
    QPixmap pixmap = getPixmapResource(cathegory, iconName, hasColorCathegory);
    icon.addPixmap(pixmap);
    if (AppContext::getMainWindow()->isDarkMode()) {
        // automatic disabled icon is no good for dark mode
        // paint transparent black to get disabled look
        QPainter p(&pixmap);
        p.fillRect(pixmap.rect(), QColor(48, 47, 47, 128));
        icon.addPixmap(pixmap, QIcon::Disabled);
    }

    return icon;
}

QIcon GUIUtils::getIconResource(const IconParameters& parameters) {
    return getIconResource(parameters.iconCathegory, parameters.iconName, parameters.hasColorCathegory);
}

QString GUIUtils::getResourceName(const IconParameters& parameters) {
    return getResourceName(parameters.iconCathegory, parameters.iconName, parameters.hasColorCathegory);
}

void GUIUtils::insertActionAfter(QMenu* menu, QAction* insertionPointMarkerAction, QAction* actionToInsert) {
    SAFE_POINT(menu != nullptr, "menu is null", );
    QList<QAction*> actions = menu->actions();
    int markerIndex = actions.indexOf(insertionPointMarkerAction);
    if (markerIndex == -1 || markerIndex == actions.size() - 1) {
        menu->addAction(actionToInsert);
        return;
    }
    QAction* actionBefore = actions[markerIndex + 1];
    menu->insertAction(actionBefore, actionToInsert);
}

QString GUIUtils::getTextWithDialog(const QString& title, const QString& label, const QString& defaultText, bool& ok, QWidget* parent) {
    QInputDialog inputDialog(parent);
    inputDialog.setWindowTitle(title);
    inputDialog.setLabelText(label);
    inputDialog.setTextValue(defaultText);
    inputDialog.setInputMode(QInputDialog::TextInput);

    // Inline logic to enable Enter key for OK button.
    auto lineEdit = inputDialog.findChild<QLineEdit*>();
    if (lineEdit) {
        connect(lineEdit, &QLineEdit::returnPressed, &inputDialog, &QInputDialog::accept);
    }

    if (inputDialog.exec() == QDialog::Accepted) {
        ok = true;
        return inputDialog.textValue();
    }
    ok = false;
    return "";
}

QMap<QString, QString> GUIUtils::fillFormWithDialog(const QString& title, const QVector<FormFieldDescriptor>& fields, QWidget* parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(400);

    QVBoxLayout layout(&dialog);
    QMap<QString, QLineEdit*> lineEdits;
    for (const auto& field : qAsConst(fields)) {
        auto label = new QLabel(field.label, &dialog);
        auto lineEdit = new QLineEdit(&dialog);
        lineEdit->setText(field.defaultValue);
        lineEdit->setObjectName("formDialogField-" + field.name);
        layout.addWidget(label);
        layout.addWidget(lineEdit);
        if (!field.tooltip.isEmpty()) {
            lineEdit->setToolTip(field.tooltip);
        }
        lineEdits[field.name] = lineEdit;
    }
    layout.addSpacing(20);

    // Add OK and Cancel buttons.
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Show the dialog and collect values if OK is clicked.
    QMap<QString, QString> result;
    if (dialog.exec() == QDialog::Accepted) {
        for (auto it = lineEdits.cbegin(); it != lineEdits.cend(); ++it) {
            result[it.key()] = it.value()->text();
        }
    }
    return result;
}

ResetSliderOnDoubleClickBehavior::ResetSliderOnDoubleClickBehavior(QAbstractSlider* slider, QLabel* relatedLabel)
    : QObject(slider), defaultValue(slider->value()) {
    slider->installEventFilter(this);
    if (relatedLabel != nullptr) {
        relatedLabel->installEventFilter(this);
    }
}

bool ResetSliderOnDoubleClickBehavior::eventFilter(QObject*, QEvent* event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
        auto slider = qobject_cast<QAbstractSlider*>(parent());
        SAFE_POINT(slider != nullptr, "Parent object is not a slider", false);
        slider->setValue(defaultValue);
        return true;
    }
    return false;
}

}  // namespace U2
