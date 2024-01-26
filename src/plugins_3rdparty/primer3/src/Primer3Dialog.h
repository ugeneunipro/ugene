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

#include <QDialog>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/RegionSelector.h>
#include <U2Gui/U2SavableWidget.h>

#include <U2View/ADVSequenceObjectContext.h>

#include "task/Primer3TaskSettings.h"
#include "ui_Primer3Dialog.h"

namespace U2 {

class Primer3Dialog : public QDialog, private Ui_Primer3Dialog {
    Q_OBJECT
public:
    Primer3Dialog(ADVSequenceObjectContext* context);
    Primer3Dialog(const Primer3Dialog& settings) = delete;
    Primer3Dialog(Primer3Dialog&& settings) = delete;
    Primer3Dialog& operator=(const Primer3Dialog& settings) = delete;
    Primer3Dialog& operator=(Primer3Dialog&& settings) = delete;
    ~Primer3Dialog();

    const QSharedPointer<Primer3TaskSettings>& getSettings();
    const CreateAnnotationModel& getCreateAnnotationModel() const;
    U2Region getRegion(bool* ok = nullptr) const;
    QString getResultFileName() const;
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
    bool updateErrorState(const QMap<QWidget*, bool>& widgetStates, const QStringList& errors);
    QString getWidgetTemplateError(QWidget* wgt, const QString& errorWgtLabel = QString()) const;

private slots:
    void sl_resetClicked();
    void sl_pickClicked();
    void sl_saveSettings();
    void sl_loadSettings();
    void sl_taskChanged(const QString& text);
    void sl_presetChanged(const QString& text);
    void sl_checkComplementStateChanged();
    void sl_ChooseCsvReportPathButtonClicked();

private:
    void updateNoSequenceDialogState();
    void saveSettings(const QString& filePath);
    bool loadSettings(const QString& filePath);

    U2Region selection;
    QList<QPair<QString, QByteArray>> repeatLibraries;

    CreateAnnotationWidgetController* createAnnotationWidgetController = nullptr;
    QSharedPointer<Primer3TaskSettings> settings;
    RegionSelector* regionSelector = nullptr;
    SaveDocumentController* saveDocumentController = nullptr;
    QLineEdit* outputFileLineEdit = nullptr;
    ADVSequenceObjectContext* context = nullptr;

    U2SavableWidget savableWidget;

    const Primer3TaskSettings defaultSettings;
    const QByteArray primer3DataDirectory;
    const QList<QPair<QString, QString>> presetNamesMap;

    static const QStringList LINE_EDIT_PARAMETERS;
    static const QRegularExpression MUST_MATCH_END_REGEX;
    static const QRegularExpression MUST_MATCH_START_CODON_SEQUENCE_REGEX;
    static constexpr int MAXIMUM_ALLOWED_SEQUENCE_LENGTH = 1024 * 256;
};

}  // namespace U2

