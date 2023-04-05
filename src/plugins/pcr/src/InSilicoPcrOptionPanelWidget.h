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

#include "PcrOptionsPanelSavableTab.h"
#include "ui_InSilicoPcrOptionPanelWidget.h"

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class TmCalculator;
class InSilicoPcrTask;
class PrimerGroupBox;

class InSilicoPcrOptionPanelWidget : public QWidget, public Ui_InSilicoPcrOptionPanelWidget {
    Q_OBJECT
public:
    InSilicoPcrOptionPanelWidget(AnnotatedDNAView* annotatedDnaView);
    ~InSilicoPcrOptionPanelWidget() override;

    AnnotatedDNAView* getDnaView() const;

    void setResultTableShown(bool show);

private slots:
    void sl_onPrimerChanged();
    void sl_findProduct();
    void sl_extractProduct();
    void sl_onFindTaskFinished();
    void sl_onSequenceChanged(ADVSequenceObjectContext* sequenceContext);

    /** Updates state of the widget to match the new active sequence. */
    void sl_activeSequenceChanged();

    void sl_onProductsSelectionChanged();
    void sl_onProductDoubleClicked();
    void sl_showDetails(const QString& link);
    void sl_temperatureSettingsChanged();

private:
    static bool isDnaSequence(ADVSequenceObjectContext* sequenceContext);
    void showResults(InSilicoPcrTask* task);

private:
    QPointer<AnnotatedDNAView> annotatedDnaView;
    InSilicoPcrTask* pcrTask;
    bool resultTableShown;
    PcrOptionsPanelSavableTab savableWidget;
    // Required, because @annotatedDnaView is already dead in the destructor
    QString TmCalculatorId;
    QSharedPointer<TmCalculator> temperatureCalculator;
};

}  // namespace U2
