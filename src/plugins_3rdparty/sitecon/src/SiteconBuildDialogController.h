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

#ifndef _U2_SITECON_BUILD_DIALOG_CONTROLLER_H_
#define _U2_SITECON_BUILD_DIALOG_CONTROLLER_H_

#include <QDialog>

#include <ui_SiteconBuildDialog.h>
#include "SiteconAlgorithm.h"
#include <U2Core/Task.h>
#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class LoadDocumentTask;
class SiteconPlugin;
class SaveDocumentController;

class SiteconBuildDialogController : public QDialog, public Ui_SiteconBuildDialog {
    Q_OBJECT

public:
    SiteconBuildDialogController(SiteconPlugin* p, QWidget* w = NULL);

public slots:
    virtual void reject();

private slots:
    void sl_inFileButtonClicked();
    void sl_okButtonClicked();

    void sl_onStateChanged();
    void sl_onProgressChanged();

private:
    void initSaveController();

    Task* task;
    SiteconPlugin* plug;
    QPushButton* okButton;
    QPushButton* cancelButton;
    SaveDocumentController *saveController;
};

class SiteconBuildTask : public Task {
    Q_OBJECT
public:
    SiteconBuildTask(const SiteconBuildSettings& s, const MultipleSequenceAlignment& ma, const QString& origin = QString());
    void run();
    SiteconModel getResult() const {return m;}

private:
    SiteconBuildSettings    settings;
    MultipleSequenceAlignment              ma;
    SiteconModel            m;
};

class SiteconBuildToFileTask : public Task {
    Q_OBJECT
public:
    SiteconBuildToFileTask(const QString& inFile, const QString& outFile, const SiteconBuildSettings& s);
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    LoadDocumentTask*       loadTask;
    SiteconBuildTask*       buildTask;
    QString                 outFile;
    SiteconBuildSettings    settings;
};

} //namespace

#endif

