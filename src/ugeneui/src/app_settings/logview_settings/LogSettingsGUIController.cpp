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

#include "LogSettingsGUIController.h"

#include <QHeaderView>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/LogCache.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

LogSettingsPageState::LogSettingsPageState() {
}

LogSettingsPageController::LogSettingsPageController(LogSettingsHolder* h, QObject* p)
    : AppSettingsGUIPageController(tr("Logging"), APP_SETTINGS_GUI_LOG, p), target(h) {
}

AppSettingsGUIPageState* LogSettingsPageController::getSavedState() {
    auto state = new LogSettingsPageState();
    state->settings = target->getSettings();
    return state;
}

void LogSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    auto state = qobject_cast<LogSettingsPageState*>(s);
    target->setSettings(state->settings);
}

AppSettingsGUIPageWidget* LogSettingsPageController::createWidget(AppSettingsGUIPageState* data) {
    auto w = new LogSettingsPageWidget();
    w->setState(data);
    return w;
}

const QString LogSettingsPageController::helpPageId = QString("65929356");

//////////////////////////////////////////////////////////////////////////
// widget

LogSettingsPageWidget::LogSettingsPageWidget() {
    setupUi(this);
    tableWidget->verticalHeader()->setVisible(false);

    initSaveController();

    connect(tableWidget, SIGNAL(currentCellChanged(int, int, int, int)), SLOT(sl_currentCellChanged(int, int, int, int)));
    connect(fileOutCB, SIGNAL(stateChanged(int)), SLOT(sl_outFileStateChanged(int)));

#ifdef Q_OS_DARWIN
    // Layout fix for mac: the font size is bigger than in another systems.
    tableWidget->horizontalHeader()->setDefaultSectionSize(110);
#endif
}

void LogSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    auto state = qobject_cast<LogSettingsPageState*>(s);
    LogSettings& settings = state->settings;

    tableWidget->clearContents();

    tableWidget->setRowCount(1 + settings.getLoggerSettings().size());

    // set header
    tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Category")));
    for (int i = 0; i < LogLevel_NumLevels; i++) {
        QString catName = LogCategories::getLocalizedLevelName((LogLevel)i);
        tableWidget->setHorizontalHeaderItem(i + 1, new QTableWidgetItem(catName));
    }

    // create global line
    auto item00 = new QTableWidgetItem(tr("<<all>>"));
    tableWidget->setItem(0, 0, item00);
    for (int i = 0; i < LogLevel_NumLevels; i++) {
        auto levelItem = new QTableWidgetItem();
        tableWidget->setItem(0, i + 1, levelItem);

        auto tw = new LogSettingsTopLineWidget(this, settings.levelColors[i], LogLevel(i));
        auto l = new QHBoxLayout();

        int marginLeft = 6;  // TODO: align with usual setCheckState boxes
        l->setContentsMargins(marginLeft, 1, 10, 1);
        l->setSpacing(0);

        tw->cb = new QCheckBox(tw);
        tw->cb->setTristate(true);
        connect(tw->cb, SIGNAL(stateChanged(int)), SLOT(sl_levelStateChanged(int)));
        l->addWidget(tw->cb);

        auto label = new QLabel(tw);
        updateColorLabel(label, settings.levelColors[i]);
        connect(label, SIGNAL(linkActivated(const QString&)), SLOT(sl_changeColor(const QString&)));
        l->addWidget(label);

        tw->setLayout(l);
        tableWidget->setCellWidget(0, i + 1, tw);
    }

    // create category lines
    int row = 1;
    int nEqual[LogLevel_NumLevels];
    for (int i = 0; i < LogLevel_NumLevels; i++) {
        nEqual[i] = 0;
    }

    QList<QString> categoryNames = settings.getLoggerSettings().keys();
    std::sort(categoryNames.begin(), categoryNames.end());
    foreach (const QString& categoryName, categoryNames) {
        const LoggerSettings& cs = settings.getLoggerSettings(categoryName);
        auto nameItem = new QTableWidgetItem(cs.categoryName);
        tableWidget->setItem(row, 0, nameItem);
        for (int i = 0; i < LogLevel_NumLevels; i++) {
            auto catItem = new QTableWidgetItem();
            catItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            catItem->setCheckState(cs.activeLevelFlag[i] ? Qt::Checked : Qt::Unchecked);
            nEqual[i] += cs.activeLevelFlag[i] ? 1 : 0;
            tableWidget->setItem(row, i + 1, catItem);
        }
        row++;
    }

    for (int i = 0; i < LogLevel_NumLevels; i++) {
        auto tw = qobject_cast<LogSettingsTopLineWidget*>(tableWidget->cellWidget(0, i + 1));
        tw->cb->setCheckState(nEqual[i] == 0 ? Qt::Unchecked : nEqual[i] == settings.getLoggerSettings().size() ? Qt::Checked
                                                                                                                : Qt::PartiallyChecked);
    }

    tableWidget->resizeRowsToContents();
    connect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(sl_catItemStateChanged(QTableWidgetItem*)));

    showCategoryCB->setChecked(settings.showCategory);
    showDateCB->setChecked(settings.showDate);
    showLevelCB->setChecked(settings.showLevel);
    colorCB->setChecked(settings.enableColor);
    dateFormatEdit->setText(settings.logPattern);
    fileOutCB->setChecked(settings.toFile);
    saveController->setPath(settings.outputFile);

    sl_outFileStateChanged(settings.toFile ? Qt::Checked : Qt::Unchecked);
}

AppSettingsGUIPageState* LogSettingsPageWidget::getState(QString& err) const {
    Q_UNUSED(err);
    auto state = new LogSettingsPageState();
    LogSettings& settings = state->settings;

    // process global settings
    for (int i = 0; i < LogLevel_NumLevels; i++) {
        QWidget* w = tableWidget->cellWidget(0, i + 1);
        auto tw = qobject_cast<LogSettingsTopLineWidget*>(w);
        settings.levelColors[i] = tw->color;
        settings.activeLevelGlobalFlag[i] = tw->cb->checkState() != Qt::Unchecked;
    }

    // process per-category settings
    for (int row = 1; row < tableWidget->rowCount(); row++) {
        LoggerSettings logCat;
        logCat.categoryName = tableWidget->item(row, 0)->text();
        for (int i = 0; i < LogLevel_NumLevels; i++) {
            logCat.activeLevelFlag[i] = tableWidget->item(row, i + 1)->checkState() == Qt::Checked;
        }
        settings.addCategory(logCat);
    }

    if (fileOutCB->isChecked() != settings.toFile || settings.outputFile != saveController->getSaveFileName()) {
        auto lce = qobject_cast<LogCacheExt*>(LogCache::getAppGlobalInstance());
        if (fileOutCB->isChecked()) {
            lce->setFileOutputEnabled(saveController->getSaveFileName());
        } else {
            lce->setFileOutputDisabled();
        }
    }

    if (fileOutCB->isChecked()) {
        QString logFile(saveController->getSaveFileName());
        QFileInfo lf(logFile);
        QFile file(logFile);
        bool writeble = file.open(QIODevice::WriteOnly);
        file.close();
        if (!writeble || lf.fileName().isEmpty()) {
            QMessageBox::warning(nullptr, tr("Warning"), tr("Unable to open log file for writing, log writing to file disabled"), QMessageBox::Ok);
            fileOutCB->setChecked(false);
        }
    }

    settings.showCategory = showCategoryCB->isChecked();
    settings.showDate = showDateCB->isChecked();
    settings.showLevel = showLevelCB->isChecked();
    settings.enableColor = colorCB->isChecked();
    settings.logPattern = dateFormatEdit->text();
    settings.toFile = fileOutCB->isChecked();
    settings.outputFile = saveController->getSaveFileName();

    return state;
}

void LogSettingsPageWidget::sl_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn) {
    Q_UNUSED(currentRow);
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    tableWidget->setCurrentCell(0, 0, QItemSelectionModel::NoUpdate);
}

void LogSettingsPageWidget::sl_levelStateChanged(int state) {
    if (state == Qt::PartiallyChecked) {
        return;
    }
    disconnect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(sl_catItemStateChanged(QTableWidgetItem*)));

    auto cb = qobject_cast<QCheckBox*>(sender());
    int column = 1 + qobject_cast<LogSettingsTopLineWidget*>(cb->parent())->level;
    assert(column >= 1 && column <= LogLevel_NumLevels);

    for (int row = 1; row < tableWidget->rowCount(); row++) {
        QTableWidgetItem* catItem = tableWidget->item(row, column);
        catItem->setCheckState(cb->checkState());
    }

    connect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(sl_catItemStateChanged(QTableWidgetItem*)));
}

void LogSettingsPageWidget::sl_catItemStateChanged(QTableWidgetItem* item) {
    if (tableWidget->row(item) == 0) {
        return;
    }

    int column = tableWidget->column(item);
    assert(column >= 1 && column <= LogLevel_NumLevels);

    auto mainItem = qobject_cast<LogSettingsTopLineWidget*>(tableWidget->cellWidget(0, column));

    int nEqual = 0;
    for (int row = 1; row < tableWidget->rowCount(); row++) {
        QTableWidgetItem* catItem = tableWidget->item(row, column);
        assert(catItem != nullptr);
        nEqual += catItem->checkState() == item->checkState() ? 1 : 0;
    }
    if (nEqual == tableWidget->rowCount() - 1) {
        mainItem->cb->setCheckState(item->checkState());
    } else {
        mainItem->cb->setCheckState(Qt::PartiallyChecked);
    }
}

void LogSettingsPageWidget::sl_changeColor(const QString&) {
    auto label = qobject_cast<QLabel*>(sender());
    auto tw = qobject_cast<LogSettingsTopLineWidget*>(label->parent());
    QColor color = U2ColorDialog::getColor(tw->color, this);
    if (color.isValid()) {
        tw->color = color.name();
        updateColorLabel(label, tw->color);
    }
}

void LogSettingsPageWidget::sl_outFileStateChanged(int state) {
    if (state == Qt::Unchecked) {
        outFileEdit->setEnabled(false);
        browseFileButton->setEnabled(false);
    }
    if (state == Qt::Checked) {
        outFileEdit->setEnabled(true);
        browseFileButton->setEnabled(true);
    }
}

void LogSettingsPageWidget::updateColorLabel(QLabel* l, const QString& color) {
    QString style = "{color: " + color + "; text-decoration: none;};";
    l->setText("<style> a " + style + " a:visited " + style + " a:hover " + style + "</style>" + "<a href=\".\">" + tr("Sample text") + "</a>");
}

void LogSettingsPageWidget::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = "log";
    config.fileDialogButton = browseFileButton;
    config.fileNameEdit = outFileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save to...");
    config.rollFileName = false;

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat("log", "log plain text", QStringList() << "log"
                                                             << "txt");

    saveController = new SaveDocumentController(config, formats, this);
}

}  // namespace U2
