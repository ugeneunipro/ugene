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

#ifndef PRIMER3DIALOG_H
#define PRIMER3DIALOG_H

#include <U2View/ADVSequenceObjectContext.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/RegionSelector.h>

#include "ui_Primer3Dialog.h"
#include "Primer3Task.h"

#include <QDialog>

namespace U2 {

class Primer3Dialog : public QDialog
{
    Q_OBJECT
public:
    Primer3Dialog(const Primer3TaskSettings &defaultSettings, ADVSequenceObjectContext *context);

    Primer3TaskSettings getSettings()const;
    const CreateAnnotationModel &getCreateAnnotationModel()const;
    U2Region getRegion(bool *ok = NULL)const;
    QString checkModel();
    bool prepareAnnotationObject();
public:
    enum IntervalDefinition {
        Start_Length,
        Start_End
    };

    static bool parseIntervalList(const QString& inputString, const QString& delimiter, QList< U2Region > *outputList,
                                  IntervalDefinition way = Start_Length);
    static QString intervalListToString(const QList< U2Region>& intervalList, const QString& delimiter,
                                        IntervalDefinition way = Start_Length);
private:
    void reset();
    bool doDataExchange();

    void showInvalidInputMessage(QWidget *field, QString fieldLabel);
private:
    Ui_Primer3Dialog ui;

    CreateAnnotationWidgetController *createAnnotationWidgetController;
    U2Region selection;
    int sequenceLength;

    QList<QPair<QString, QByteArray> > repeatLibraries;

    Primer3TaskSettings defaultSettings;
    Primer3TaskSettings settings;
    RegionSelector* rs;
    ADVSequenceObjectContext *context;
private slots:
    void sl_pbReset_clicked();
    void sl_pbPick_clicked();
    void sl_saveSettings();
    void sl_loadSettings();
};

} // namespace U2

#endif // PRIMER3DIALOG_H
