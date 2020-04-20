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

#ifndef _U2_IN_SILICO_PCR_OPTION_PANEL_WIDGET_H_
#define _U2_IN_SILICO_PCR_OPTION_PANEL_WIDGET_H_

#include "PcrOptionsPanelSavableTab.h"

#include "ui_InSilicoPcrOptionPanelWidget.h"

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class InSilicoPcrTask;
class PrimerGroupBox;

class InSilicoPcrOptionPanelWidget : public QWidget, public Ui_InSilicoPcrOptionPanelWidget {
    Q_OBJECT
public:
    InSilicoPcrOptionPanelWidget(AnnotatedDNAView *annotatedDnaView);
    ~InSilicoPcrOptionPanelWidget();

    AnnotatedDNAView * getDnaView() const;

    bool isResultTableShown() const;
    void setResultTableShown(bool show);

private slots:
    void sl_onPrimerChanged();
    void sl_findProduct();
    void sl_extractProduct();
    void sl_onFindTaskFinished();
    void sl_onSequenceChanged(ADVSequenceObjectContext *sequenceContext);
    void sl_onFocusChanged();
    void sl_onProductsSelectionChanged();
    void sl_onProductDoubleClicked();
    void sl_showDetails(const QString &link);

private:
    static bool isDnaSequence(ADVSequenceObjectContext *sequenceContext);
    void showResults(InSilicoPcrTask *task);

private:
    AnnotatedDNAView *annotatedDnaView;
    InSilicoPcrTask *pcrTask;
    bool resultTableShown;
    PcrOptionsPanelSavableTab savableWidget;
};

} // U2

#endif // _U2_IN_SILICO_PCR_OPTION_PANEL_WIDGET_H_
