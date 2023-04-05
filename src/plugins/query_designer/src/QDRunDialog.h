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

#pragma once

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include <U2Gui/RegionSelector.h>

#include <ui_QDDialog.h>
#include <ui_RunQueryDialog.h>

namespace U2 {

class AnnotationTableObject;
class Document;
class DocumentProviderTask;
class QDScheduler;
class QDScheme;
class SaveDocumentController;
class SaveDocumentTask;

class QDRunDialogTask : public Task {
    Q_OBJECT
public:
    QDRunDialogTask(QDScheme* scheme, const QString& inUri, const QString& outUri, bool addToProject);

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private slots:
    void sl_updateProgress();

private:
    QList<Task*> init();
    void setupQuery();

private:
    QDScheme* scheme = nullptr;
    QString inUri;
    QString output;
    bool addToProject = false;
    Task* openProjTask = nullptr;
    DocumentProviderTask* loadTask = nullptr;
    QDScheduler* scheduler = nullptr;
    Document* docWithSequence = nullptr;
    AnnotationTableObject* annObj = nullptr;
};

class QueryViewController;

class QDRunDialog : public QDialog, public Ui_RunQueryDlg {
    Q_OBJECT
public:
    QDRunDialog(QDScheme* scheme, QWidget* parent, const QString& defaultIn = "", const QString& defaultOut = "");

    static const QString OUTPUT_FILE_DIR_DOMAIN;

private slots:
    void sl_run();
    void sl_selectInputFile();
    void sl_outputFileChanged();

private:
    void initSaveController(const QString& defaultOut);

    QDScheme* scheme = nullptr;
    SaveDocumentController* saveController = nullptr;
};

class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;

class QDDialog : public QDialog, public Ui_QDDialog {
    Q_OBJECT
public:
    QDDialog(ADVSequenceObjectContext* ctx);

private:
    void addAnnotationsWidget();
    void connectGUI();

private slots:
    void sl_selectScheme();
    void sl_okBtnClicked();

private:
    void updateSchemaOnUrlUpdate();

    ADVSequenceObjectContext* advSequenceContext = nullptr;
    CreateAnnotationWidgetController* annotationWidgetController = nullptr;
    QDScheme* scheme = nullptr;
    RegionSelector* regionSelector = nullptr;
    QString renderedSchemaUrl;
    QString hintHtml;
    QPushButton* searchButton = nullptr;
};

}  // namespace U2
