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

#ifndef _U2_WEIGHT_MATRIX_BUILD_DIALOG_CONTROLLER_H_
#define _U2_WEIGHT_MATRIX_BUILD_DIALOG_CONTROLLER_H_

#include <QDialog>

#include <U2Algorithm/PWMConversionAlgorithm.h>

#include <U2Core/DNASequence.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>
#include <U2Core/Task.h>

#include <U2View/AlignmentLogo.h>

#include "WeightMatrixAlgorithm.h"
#include "WeightMatrixPlugin.h"
#include "ui_PWMBuildDialog.h"

namespace U2 {

class LoadDocumentTask;
class PWMBuildSettings;
class PWMModel;
class SaveDocumentController;

class PWMBuildDialogController : public QDialog, public Ui_PWMBuildDialog {
    Q_OBJECT

public:
    PWMBuildDialogController(QWidget* w = NULL);
    QString lastURL;

public slots:
    virtual void reject();

private slots:
    void sl_inFileButtonClicked();
    void sl_formatChanged(const QString &newFormatId);
    void sl_okButtonClicked();
    void sl_matrixTypeChanged(bool);

    void sl_onStateChanged();
    void sl_onProgressChanged();

private:
    void initFrequencySaveController();
    void initWeightSaveController();
    void reportError(const QString &message);

    Task* task;
    SaveDocumentController *saveController;
    AlignmentLogoRenderArea* logoArea;
    QPushButton* okButton;
    QPushButton* cancelButton;
    void replaceLogo(const MultipleSequenceAlignment& ma);
};


class PFMatrixBuildTask : public Task {
    Q_OBJECT
public:
    PFMatrixBuildTask(const PMBuildSettings& s, const MultipleSequenceAlignment& ma);
    void run();
    PFMatrix getResult() const {return m;}

private:
    PMBuildSettings        settings;
    MultipleSequenceAlignment              ma;
    PFMatrix                m;
};

class PFMatrixBuildToFileTask : public Task {
    Q_OBJECT
public:
    PFMatrixBuildToFileTask(const QString& inFile, const QString& outFile, const PMBuildSettings& s);
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    LoadDocumentTask*       loadTask;
    PFMatrixBuildTask*           buildTask;
    QString                 outFile;
    PMBuildSettings         settings;
};

class PWMatrixBuildTask : public Task {
    Q_OBJECT
public:
    PWMatrixBuildTask(const PMBuildSettings& s, const MultipleSequenceAlignment& ma);
    PWMatrixBuildTask(const PMBuildSettings& s, const PFMatrix& m);
    void run();
    PWMatrix getResult() const {return m;}

private:
    PMBuildSettings         settings;
    MultipleSequenceAlignment              ma;
    PFMatrix                tempMatrix;
    PWMatrix                m;
};

class PWMatrixBuildToFileTask : public Task {
    Q_OBJECT
public:
    PWMatrixBuildToFileTask(const QString& inFile, const QString& outFile, const PMBuildSettings& s);
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    LoadDocumentTask*       loadTask;
    PWMatrixBuildTask*       buildTask;
    QString                 outFile;
    PMBuildSettings    settings;
};

} //namespace

#endif

