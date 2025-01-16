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

#pragma once

#include <QHash>
#include <QSet>

#include <U2Core/Msa.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

#include "ui_DistanceMatrixMSAProfileDialog.h"

class QFile;

namespace U2 {

class MsaEditor;
class MsaDistanceAlgorithm;
class SaveDocumentController;

class DistanceMatrixMSAProfileDialog : public QDialog, public Ui_DistanceMatrixMSAProfileDialog {
    Q_OBJECT

public:
    DistanceMatrixMSAProfileDialog(QWidget* p, MsaEditor* ctx);

    void accept() override;

private slots:
    void sl_formatSelected();
    void sl_formatChanged(const QString& newFormatId);

private:
    void initSaveController();

    MsaEditor* ctx;
    SaveDocumentController* saveController;

    static const QString HTML;
    static const QString CSV;
};

enum DistanceMatrixMSAProfileOutputFormat {
    DistanceMatrixMSAProfileOutputFormat_Show,
    DistanceMatrixMSAProfileOutputFormat_CSV,
    DistanceMatrixMSAProfileOutputFormat_HTML
};

class DistanceMatrixMSAProfileTaskSettings {
public:
    DistanceMatrixMSAProfileTaskSettings();

    QString algoId;  // selected algorithm id
    QString profileName;  // usually object name
    QString profileURL;  // document url
    Msa ma;
    bool usePercents;  // report percents but not counts
    bool excludeGaps;  // exclude gaps when calculate distance
    bool showGroupStatistic;
    DistanceMatrixMSAProfileOutputFormat outFormat;
    QString outURL;
    MsaEditor* ctx;
};

class DistanceMatrixMSAProfileTask : public Task {
    Q_OBJECT
public:
    DistanceMatrixMSAProfileTask(const DistanceMatrixMSAProfileTaskSettings& s);

    void prepare() override;
    QString generateReport() const override;
    bool isReportingEnabled() const override;

    void createDistanceTable(MsaDistanceAlgorithm* algo, const QList<MsaRow>& rows, QFile* f);

    QList<Task*> createStatisticsDocument(Task* subTask);

    QList<Task*> onSubTaskFinished(Task* subTask) override;
    // void run();
    ReportResult report() override;

private:
    DistanceMatrixMSAProfileTaskSettings s;
    QString resultText;
};

}  // namespace U2
