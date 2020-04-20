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

#ifndef _U2_DISTANCE_MATRIX_MSA_PROFILE_DIALOG_H_
#define _U2_DISTANCE_MATRIX_MSA_PROFILE_DIALOG_H_

#include <QHash>
#include <QSet>

#include <U2Core/global.h>
#include <U2Core/Task.h>
#include <U2Core/MultipleSequenceAlignment.h>

#include <QHash>
#include <QSet>
#include "ui_DistanceMatrixMSAProfileDialog.h"

class QFile;

namespace U2 {

class MSAEditor;
class MSADistanceAlgorithm;
class SaveDocumentController;

class DistanceMatrixMSAProfileDialog : public QDialog, public Ui_DistanceMatrixMSAProfileDialog {
    Q_OBJECT

public:
    DistanceMatrixMSAProfileDialog(QWidget* p, MSAEditor* ctx);

    void accept();

private slots:
    void sl_formatSelected();
    void sl_formatChanged(const QString &newFormatId);

private:
    void initSaveController();

    MSAEditor* ctx;
    SaveDocumentController *saveController;

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

    QString                         algoId;    // selected algorithm id
    QString                         profileName; // usually object name
    QString                         profileURL;  // document url
    MultipleSequenceAlignment                      ma;
    bool                            usePercents; //report percents but not counts
    bool                            excludeGaps; //exclude gaps when calculate distance
    bool                            showGroupStatistic;
    DistanceMatrixMSAProfileOutputFormat   outFormat;
    QString                         outURL;
    MSAEditor*                      ctx;
};

class DistanceMatrixMSAProfileTask : public Task {
    Q_OBJECT
public:
    DistanceMatrixMSAProfileTask(const DistanceMatrixMSAProfileTaskSettings& s);

    virtual void prepare();
    QString generateReport() const;
    virtual bool isReportingEnabled() const;

    void createDistanceTable(MSADistanceAlgorithm* algo, const QList<MultipleSequenceAlignmentRow> &rows, QFile *f);

    QList<Task*> createStatisticsDocument(Task* subTask);

    QList<Task*> onSubTaskFinished(Task* subTask);
    //void run();
    ReportResult report();

private:
    DistanceMatrixMSAProfileTaskSettings   s;
    QString                         resultText;
};

}//namespace

#endif
