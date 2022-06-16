/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <QDialog>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>

#include "Primer3Task.h"
#include "ui_Primer3Dialog.h"

namespace U2 {

class Primer3Dialog : public QDialog, private Ui_Primer3Dialog {
    Q_OBJECT
public:
    Primer3Dialog(const Primer3TaskSettings& defaultSettings, ADVSequenceObjectContext* context);

    Primer3TaskSettings getSettings() const;
    const CreateAnnotationModel& getCreateAnnotationModel() const;
    U2Region getRegion(bool* ok = nullptr) const;
    QString checkModel();
    bool prepareAnnotationObject();

public:
    enum class IntervalDefinition {
        Start_Length,
        Start_End
    };

    static bool parseIntervalList(const QString& inputString, const QString& delimiter, QList<U2Region>* outputList, IntervalDefinition way = IntervalDefinition::Start_Length);
    static bool parseIntList(const QString& inputString, QList<int>* outputList);
    static bool parseOkRegions(const QString& inputString, QList<QList<int>>* outputList);
    static QString intervalListToString(const QList<U2Region>& intervalList, const QString& delimiter, IntervalDefinition way = IntervalDefinition::Start_Length);
    static QString intListToString(const QList<int>& intList, const QString& delimiter);
    static QString okRegions2String(const QList<QList<int>>& regionLins);

private:
    void reset();
    bool doDataExchange();

    void showInvalidInputMessage(QWidget* field, QString fieldLabel);

private slots:
    void sl_resetClicked();
    void sl_pickClicked();
    void sl_saveSettings();
    void sl_loadSettings();


private:
    CreateAnnotationWidgetController* createAnnotationWidgetController;
    U2Region selection;
    int sequenceLength;

    QList<QPair<QString, QByteArray>> repeatLibraries;

    Primer3TaskSettings defaultSettings;
    Primer3TaskSettings settings;
    RegionSelector* rs;
    ADVSequenceObjectContext* context;

    static const QStringList LINE_EDIT_PARAMETERS;
    static const QRegularExpression MUST_MATCH_END_REGEX;
};

}  // namespace U2

#endif  // PRIMER3DIALOG_H
