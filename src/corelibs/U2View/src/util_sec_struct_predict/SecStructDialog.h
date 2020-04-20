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

#ifndef _U2_EXPORT_SEC_STRUCT_DIALOG_H_
#define _U2_EXPORT_SEC_STRUCT_DIALOG_H_

#include <U2Core/AnnotationData.h>
#include <ui_SecStructDialog.h>

namespace U2 {

class ADVSequenceObjectContext;
class SecStructPredictTask;
class SecStructPredictAlgRegistry;
class Task;
class RegionSelector;

class U2VIEW_EXPORT SecStructDialog : public QDialog, private Ui_SecStructDialog {
    Q_OBJECT
public:
    SecStructDialog(ADVSequenceObjectContext* ctx, QWidget *p = NULL);

private slots:
    void sl_onStartPredictionClicked();
    void sl_onTaskFinished(Task*);
    void sl_onSaveAnnotations();

private:
    QList<SharedAnnotationData> results;
    ADVSequenceObjectContext *ctx;
    SecStructPredictTask *task;
    SecStructPredictAlgRegistry *sspr;
    QPushButton *saveAnnotationButton;
    QPushButton *startButton;
    QPushButton *cancelButton;
    RegionSelector* regionSelector;

    U2Region region;

    void connectGUI();
    void updateState();
    void showResults();
};



} // namespace

#endif //  _U2_EXPORT_SEC_STRUCT_DIALOG_H_

