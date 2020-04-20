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

#include <QColorDialog>
#include <QFileDialog>
#include <QMouseEvent>

#include <U2Core/AppContext.h>
#include <U2Core/Version.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Lang/WorkflowSettings.h>

#include "WorkflowSettingsController.h"
#include "WorkflowViewController.h"
#include "WorkflowViewItems.h"

namespace U2 {

WorkflowSettingsPageController::WorkflowSettingsPageController(QObject* p)
: AppSettingsGUIPageController(tr("Workflow Designer"), WorkflowSettingsPageId, p) {}


AppSettingsGUIPageState* WorkflowSettingsPageController::getSavedState() {
    WorkflowSettingsPageState* state = new WorkflowSettingsPageState();
    state->showGrid = WorkflowSettings::showGrid();
    state->snap2grid = WorkflowSettings::snap2Grid();
    state->lockRun = WorkflowSettings::monitorRun();
    state->enableDebugger = WorkflowSettings::isDebuggerEnabled();
    state->style = WorkflowSettings::defaultStyle();
    state->font = WorkflowSettings::defaultFont();
    state->path = WorkflowSettings::getUserDirectory();
    state->color = WorkflowSettings::getBGColor();
    state->externalToolCfgDir = WorkflowSettings::getExternalToolDirectory();
    state->includedElementsDir = WorkflowSettings::getIncludedElementsDirectory();
    state->workflowOutputDir = WorkflowSettings::getWorkflowOutputDirectory();
    return state;
}

void WorkflowSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    WorkflowSettingsPageState* state = qobject_cast<WorkflowSettingsPageState*>(s);
    WorkflowSettings::setShowGrid(state->showGrid);
    WorkflowSettings::setSnap2Grid(state->snap2grid);
    WorkflowSettings::setMonitorRun(state->lockRun);
    WorkflowSettings::setDebuggerEnabled(state->enableDebugger);
    WorkflowSettings::setDefaultStyle(state->style);
    WorkflowSettings::setDefaultFont(state->font);
    WorkflowSettings::setUserDirectory(state->path);
    WorkflowSettings::setBGColor(state->color);
    WorkflowSettings::setExternalToolDirectory(state->externalToolCfgDir);
    WorkflowSettings::setIncludedElementsDirectory(state->includedElementsDir);

    const QString previousWWorkflowOutputDirectory = WorkflowSettings::getWorkflowOutputDirectory();
    WorkflowSettings::setWorkflowOutputDirectory(state->workflowOutputDir);

    if (previousWWorkflowOutputDirectory != state->workflowOutputDir) {
        AppContext::getDashboardInfoRegistry()->scanDashboardsDir();
    }
}

AppSettingsGUIPageWidget* WorkflowSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    WorkflowSettingsPageWidget* r = new WorkflowSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString WorkflowSettingsPageController::helpPageId = QString("24748736");

WorkflowSettingsPageWidget::WorkflowSettingsPageWidget(WorkflowSettingsPageController* ) {
    setupUi(this);
    styleCombo->addItem(U2::WorkflowView::tr("Minimal"), ItemStyles::SIMPLE);
    styleCombo->addItem(U2::WorkflowView::tr("Extended"), ItemStyles::EXTENDED);
    connect(dirButton, SIGNAL(clicked()), SLOT(sl_getDirectory()));
    connect(extToolDirButton, SIGNAL(clicked()), SLOT(sl_getExternalToolCfgDir()));
    connect(includedDirButton, SIGNAL(clicked()), SLOT(sl_getIncludedElementsDir()));
    connect(workflowOutputButton, SIGNAL(clicked()), SLOT(sl_getWorkflowOutputDir()));
    colorWidget->setMinimumHeight(label->height());
    colorWidget->installEventFilter(this);
}

void WorkflowSettingsPageWidget::sl_getDirectory() {
    QString url = WorkflowSettings::getUserDirectory();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDirectory(url);
    if(dialog.exec() == QDialog::Accepted) {
        QString dir = dialog.selectedFiles().first();
        dirEdit->setText(dir + "/");
    }
}

void WorkflowSettingsPageWidget::sl_getColor() {
    QColor newColor = QColorDialog::getColor(colorWidget->palette().color(colorWidget->backgroundRole()), this);
    if(newColor.isValid()) {
        QPalette pal = colorWidget->palette();
        pal.setColor(colorWidget->backgroundRole() , newColor);
        colorWidget->setPalette(pal);
    }
}

bool WorkflowSettingsPageWidget::eventFilter(QObject *, QEvent * event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e= static_cast<QMouseEvent*>(event);
        if(e->button() == Qt::LeftButton) {
            sl_getColor();
        }
    }
    return false;
}

void WorkflowSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    WorkflowSettingsPageState* state = qobject_cast<WorkflowSettingsPageState*>(s);
    gridBox->setChecked(state->showGrid);
    snapBox->setChecked(state->snap2grid);
    lockBox->setChecked(state->lockRun);
    debuggerBox->setChecked(state->enableDebugger);
    int idx = styleCombo->findData(state->style);
    if (idx < 0) idx = 1;
    styleCombo->setCurrentIndex(idx);
    fontCombo->setCurrentFont(state->font);
    dirEdit->setText(state->path);
    colorWidget->setAutoFillBackground(true);
    QPalette pal = colorWidget->palette();
    pal.setColor(colorWidget->backgroundRole(), state->color);
    colorWidget->setPalette(pal);
    extToolDirEdit->setText(state->externalToolCfgDir);
    includedlDirEdit->setText(state->includedElementsDir);
    workflowOutputEdit->setText(state->workflowOutputDir);
}

AppSettingsGUIPageState* WorkflowSettingsPageWidget::getState(QString& ) const {
    WorkflowSettingsPageState* state = new WorkflowSettingsPageState();
    state->showGrid = gridBox->isChecked();
    state->snap2grid = snapBox->isChecked();
    state->lockRun = lockBox->isChecked();
    state->enableDebugger = debuggerBox->isChecked();
    state->style = styleCombo->itemData(styleCombo->currentIndex()).toString();
    state->font = fontCombo->currentFont();
    state->path = dirEdit->text();
    state->color = colorWidget->palette().color(colorWidget->backgroundRole());
    state->externalToolCfgDir = extToolDirEdit->text();
    state->includedElementsDir = includedlDirEdit->text();
    state->workflowOutputDir = workflowOutputEdit->text();
    return state;
}

static void chooseDir(const QString &current, QLineEdit *edit,  QWidget *parent) {
    QFileDialog dialog(parent);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDirectory(current);
    bool chosen = (dialog.exec() == QDialog::Accepted);
    if(chosen) {
        QString dir = dialog.selectedFiles().first();
        edit->setText(dir + "/");
    }
}

void WorkflowSettingsPageWidget::sl_getExternalToolCfgDir() {
    chooseDir(WorkflowSettings::getExternalToolDirectory(), extToolDirEdit, this);
}

void WorkflowSettingsPageWidget::sl_getIncludedElementsDir() {
    chooseDir(WorkflowSettings::getIncludedElementsDirectory(), includedlDirEdit, this);
}

void WorkflowSettingsPageWidget::sl_getWorkflowOutputDir() {
    chooseDir(WorkflowSettings::getWorkflowOutputDirectory(), workflowOutputEdit, this);
}

} //namespace
