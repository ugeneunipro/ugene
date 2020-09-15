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

#include "DashboardWidget.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QUrl>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

DashboardWidget::DashboardWidget(const QString &title, QWidget *contentWidget) {
    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    setStyleSheet("QWidget#tabWidgetStyleRoot {"
                  " border-radius: 6px;"
                  " border: 1px solid #ddd;"
                  "}");

    auto styleRootWidget = new QWidget();
    styleRootWidget->setObjectName("tabWidgetStyleRoot");
    layout->addWidget(styleRootWidget);

    auto styleRootWidgetLayout = new QVBoxLayout();
    styleRootWidgetLayout->setMargin(0);
    styleRootWidgetLayout->setSpacing(0);
    styleRootWidget->setLayout(styleRootWidgetLayout);

    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("background: rgb(239, 239, 239);"
                              "color: #222;"
                              "padding: 5px;"
                              "border-top-left-radius: 6px;"
                              "border-top-right-radius: 6px;");
    styleRootWidgetLayout->addWidget(titleLabel);

    auto contentStyleWidget = new QWidget();
    contentStyleWidget->setObjectName("tabWidgetContentStyleRoot");
    contentStyleWidget->setStyleSheet("QWidget#tabWidgetContentStyleRoot {"
                                      " background: white;"
                                      " border-bottom-left-radius: 6px;"
                                      " border-bottom-right-radius: 6px;"
                                      "}");
    styleRootWidgetLayout->addWidget(contentStyleWidget);

    auto contentStyleWidgetLayout = new QVBoxLayout();
    contentStyleWidgetLayout->setMargin(0);
    contentStyleWidgetLayout->setSpacing(0);
    contentStyleWidget->setLayout(contentStyleWidgetLayout);

    contentStyleWidgetLayout->addWidget(contentWidget);
}

void DashboardWidgetUtils::addTableHeadersRow(QGridLayout *gridLayout, const QStringList &headerNameList) {
    QString commonHeaderStyle = "border: 1px solid #999; background-color: rgb(101, 101, 101);";
    for (int i = 0; i < headerNameList.size(); i++) {
        auto headerNameWidget = new QWidget();
        headerNameWidget->setObjectName("tableHeaderCell");
        if (i == 0) {
            headerNameWidget->setStyleSheet("#tableHeaderCell { " + commonHeaderStyle + "border-top-left-radius: 4px; border-right: 0px;}");
        } else if (i == headerNameList.size() - 1) {
            headerNameWidget->setStyleSheet("#tableHeaderCell { " + commonHeaderStyle + "border-left: 1px solid white; border-top-right-radius: 4px;}");
        } else {
            headerNameWidget->setStyleSheet("#tableHeaderCell { " + commonHeaderStyle + "border-left: 1px solid white; border-right: 0px;}");
        }
        auto headerNameWidgetLayout = new QVBoxLayout();
        headerNameWidgetLayout->setContentsMargins(0, 0, 0, 0);
        headerNameWidget->setLayout(headerNameWidgetLayout);
        auto headerNameWidgetLabel = new QLabel(headerNameList.at(i));
        headerNameWidgetLabel->setStyleSheet("color: white; padding: 5px 10px;");
        headerNameWidgetLayout->addWidget(headerNameWidgetLabel);
        gridLayout->addWidget(headerNameWidget, 0, i);
    }
}

#define ID_KEY "DashboardWidget-Row-Id"

static QString cellStyle = "border: 1px solid #ddd; border-top: 0px; border-right: 0px;";
static QString rightCellStyle = "border-right: 1px solid #ddd;";
static QString lastRowLeftCellStyle = "border-bottom-left-radius: 4px;";
static QString lastRowRightCellStyle = "border-bottom-right-radius: 4px;";

void DashboardWidgetUtils::addTableCell(QGridLayout *gridLayout, const QString &rowId, QWidget *widget, int row, int column, bool isLastRow, bool isLastColumn) {
    auto cellWidget = new QWidget();
    cellWidget->setObjectName("tableCell");
    QString extraCellStyle = "";
    if (isLastColumn) {
        extraCellStyle += rightCellStyle;
    }
    if (isLastRow) {
        extraCellStyle += column == 0 ? lastRowLeftCellStyle : "";
        extraCellStyle += isLastColumn ? lastRowRightCellStyle : "";
    }
    cellWidget->setStyleSheet("#tableCell {" + cellStyle + extraCellStyle + "}");
    auto cellWidgetLayout = new QVBoxLayout();
    cellWidgetLayout->setContentsMargins(10, 7, 10, 7);
    cellWidget->setLayout(cellWidgetLayout);
    cellWidgetLayout->addWidget(widget);
    cellWidgetLayout->addStretch();

    auto layoutItem = gridLayout->itemAtPosition(row, column);
    if (layoutItem != nullptr) {
        QWidget *oldWidget = layoutItem->widget();
        gridLayout->replaceWidget(oldWidget, cellWidget, Qt::FindDirectChildrenOnly);
        delete oldWidget;
    } else {
        gridLayout->addWidget(cellWidget, row, column);
    }
    cellWidget->setProperty(ID_KEY, rowId);
}

void DashboardWidgetUtils::addTableCell(QGridLayout *gridLayout, const QString &rowId, const QString &text, int row, int column, bool isLastRow, bool isLastColumn) {
    auto cellLabel = new QLabel(text);
    cellLabel->setWordWrap(true);
    cellLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    cellLabel->setStyleSheet("color: #333");
    addTableCell(gridLayout, rowId, cellLabel, row, column, isLastRow, isLastColumn);
}

void DashboardWidgetUtils::addTableRow(QGridLayout *gridLayout, const QString &rowId, const QStringList &valueList) {
    // Update last border style for the old last.
    int lastRowIndex = gridLayout->rowCount() - 1;
    if (lastRowIndex > 0) {    // row = 0 is a header.
        auto leftCellLayoutItem = gridLayout->itemAtPosition(lastRowIndex, 0);
        auto leftCellWidget = leftCellLayoutItem->widget();
        leftCellWidget->setStyleSheet("#tableCell {" + cellStyle + "}");

        auto rightCellLayoutItem = gridLayout->itemAtPosition(lastRowIndex, gridLayout->columnCount() - 1);
        auto rightCellWidget = rightCellLayoutItem->widget();
        rightCellWidget->setStyleSheet("#tableCell {" + cellStyle + rightCellStyle + "}");
    }

    for (int columnIndex = 0; columnIndex < valueList.size(); columnIndex++) {
        QString text = valueList.at(columnIndex);
        bool isLastColumn = columnIndex == valueList.size() - 1;
        addTableCell(gridLayout, rowId, text, lastRowIndex + 1, columnIndex, true, isLastColumn);
    }
}

bool DashboardWidgetUtils::addOrUpdateTableRow(QGridLayout *gridLayout, const QString &rowId, const QStringList &valueList) {
    bool isUpdated = false;
    for (int rowIndex = 0; rowIndex < gridLayout->rowCount(); rowIndex++) {
        auto cellLayoutItem = gridLayout->itemAtPosition(rowIndex, 0);
        auto cellWidget = cellLayoutItem == nullptr ? nullptr : cellLayoutItem->widget();
        if (cellWidget != nullptr && cellWidget->property(ID_KEY).toString() == rowId) {
            for (int columnIndex = 0; columnIndex < valueList.size(); columnIndex++) {
                cellLayoutItem = gridLayout->itemAtPosition(rowIndex, columnIndex);
                auto cellLabel = qobject_cast<QLabel *>(cellLayoutItem == nullptr ? nullptr : cellLayoutItem->widget()->findChild<QLabel *>());
                if (cellLabel != nullptr) {
                    cellLabel->setText(valueList.at(columnIndex));
                }
            }
            isUpdated = true;
            break;
        }
    }
    if (!isUpdated) {
        addTableRow(gridLayout, rowId, valueList);
    }
    return !isUpdated;
}

QString DashboardWidgetUtils::parseOpenUrlValueFromOnClick(const QString &onclickValue) {
    int prefixLen = QString("agent.openUrl('").length();
    int suffixLen = QString("')").length();
    return onclickValue.length() > prefixLen + suffixLen ?
               onclickValue.mid(prefixLen, onclickValue.length() - prefixLen - suffixLen) :
               QString();
}

DashboardPopupMenu::DashboardPopupMenu(QAbstractButton *button, QWidget *parent)
    : QMenu(parent), button(button) {
}

void DashboardPopupMenu::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    QPoint position = this->pos();
    QRect rect = button->geometry();
    this->move(position.x() + rect.width() - this->geometry().width(), position.y());
}

#define FILE_URL_KEY "file-url"

DashboardFileButton::DashboardFileButton(const QStringList &urlList, const QString &dashboardDir, bool isFolderMode)
    : urlList(urlList), dashboardDirInfo(dashboardDir), isFolderMode(isFolderMode) {
    setObjectName("DashboardFileButton");
    QString buttonText = urlList.size() != 1 ? tr("%1 file(s)").arg(urlList.size()) : QFileInfo(urlList[0]).fileName();
    if (buttonText.length() > 27) {
        buttonText = buttonText.left(27) + "…";
    }
    setText(buttonText);
    setToolTip(buttonText);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet("QToolButton {"
                  "  height: 1.33em; border-radius: 4px;"
                  "  border: 1px solid #aaa; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
                  "}"
                  "QToolButton:pressed {"
                  "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);"
                  "}"
                  "QToolButton::menu-button {"
                  "  border: 1px solid #aaa;"
                  "  border-top-right-radius: 4px; border-bottom-right-radius: 4px;"
                  "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
                  "  width: 1.5em;"
                  "}");

    connect(this, SIGNAL(clicked()), SLOT(sl_openFileClicked()));
    if (urlList.size() == 1) {
        QString url = urlList[0];
        if (isFolderMode) {
            setProperty(FILE_URL_KEY, "file\n" + url);
        } else {
            setProperty(FILE_URL_KEY, "ugene\n" + url);
            auto menu = new DashboardPopupMenu(this, this);
            addUrlActionsToMenu(menu, url);
            setMenu(menu);
            setPopupMode(QToolButton::MenuButtonPopup);
        }
    } else {
        auto menu = new DashboardPopupMenu(this);
        for (int i = 0, n = qMin(urlList.size(), 50); i < n; i++) {
            QString url = urlList[i];
            auto perUrlMenu = new QMenu(QFileInfo(url).fileName());
            addUrlActionsToMenu(perUrlMenu, url, !isFolderMode);
            menu->addMenu(perUrlMenu);
        }
        setMenu(menu);
        setPopupMode(QToolButton::InstantPopup);
    }
}

void DashboardFileButton::addUrlActionsToMenu(QMenu *menu, const QString &url, bool addOpenByUgeneAction) {
    if (addOpenByUgeneAction) {
        auto openFolderAction = new QAction(tr("Open file with UGENE"), this);
        openFolderAction->setProperty(FILE_URL_KEY, "ugene\n" + url);
        connect(openFolderAction, SIGNAL(triggered()), SLOT(sl_openFileClicked()));
        menu->addAction(openFolderAction);
    }

    auto openFolderAction = new QAction(tr("Open folder with the file"), this);
    openFolderAction->setProperty(FILE_URL_KEY, "folder\n" + url);
    connect(openFolderAction, SIGNAL(triggered()), SLOT(sl_openFileClicked()));
    menu->addAction(openFolderAction);

    auto openFileAction = new QAction(tr("Open file by OS"), this);
    openFileAction->setProperty(FILE_URL_KEY, "file\n" + url);
    connect(openFileAction, SIGNAL(triggered()), SLOT(sl_openFileClicked()));
    menu->addAction(openFileAction);
}

/**
 * Finds a file in the given dashboard dir by path suffix of the 'fileInfo'.
 * Returns new file info or the old one if the file detection algorithm is failed.
 * This method is designed to find dashboard output files in moved dashboard.
 */
static QFileInfo findFileOpenCandidateInTheDashboardOutputDir(const QFileInfo &dashboardDirInfo, const QFileInfo &fileInfo) {
    // Split 'fileInfo' into path tokens: list of dirs + file name.
    QStringList fileInfoPathTokens;
    QFileInfo currentPathInfo(QDir::cleanPath(fileInfo.absoluteFilePath()));
    while (!currentPathInfo.isRoot()) {
        fileInfoPathTokens.prepend(currentPathInfo.fileName());
        currentPathInfo = QFileInfo(currentPathInfo.path());
    }
    // Try to find the file by the path suffix inside dashboard dir. Check the longest possible variant first.
    while (!fileInfoPathTokens.isEmpty()) {
        QFileInfo resultFileInfo(dashboardDirInfo.absoluteFilePath() + fileInfoPathTokens.join("/"));
        if (resultFileInfo.exists()) {
            return resultFileInfo;
        }
        fileInfoPathTokens.removeFirst();
    }
    return fileInfo;
}

void DashboardFileButton::sl_openFileClicked() {
    QString typeAndUrl = sender()->property(FILE_URL_KEY).toString();
    QStringList tokens = typeAndUrl.split("\n");
    CHECK(tokens.size() == 2, );
    QString type = tokens[0];
    QString url = tokens[1];
    QFileInfo fileInfo(url);
    bool isOpenParentDir = type == "folder";    // A parent dir of the url must be opened.
    if (isOpenParentDir) {
        fileInfo = QFileInfo(fileInfo.absolutePath());
    }
    if (!fileInfo.exists()) {
        fileInfo = findFileOpenCandidateInTheDashboardOutputDir(dashboardDirInfo, fileInfo);
        if (!fileInfo.exists()) {
            if (isOpenParentDir || isFolderMode) {
                // We can't locate the original dashboard sub-folder. Opening the dashboard folder instead of error message.
                fileInfo = dashboardDirInfo;
            } else {
                QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), DashboardWidget::tr("File is not found: %1").arg(fileInfo.absoluteFilePath()));
                return;
            }
        }
    }
    if (type == "ugene") {
        QVariantMap hints;
        hints[ProjectLoaderHint_OpenBySystemIfFormatDetectionFailed] = true;
        Task *task = AppContext::getProjectLoader()->openWithProjectTask(fileInfo.absoluteFilePath(), hints);
        CHECK(task != nullptr, );
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
}

}    // namespace U2
