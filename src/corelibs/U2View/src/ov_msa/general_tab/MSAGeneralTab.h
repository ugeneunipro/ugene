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

#ifndef _U2_MSA_GENERAL_TAB_H_
#define _U2_MSA_GENERAL_TAB_H_

#include <U2Core/MultipleSequenceAlignment.h>

#include "../MsaOpSavableTab.h"
#include "ui_GeneralTabOptionsPanelWidget.h"

namespace U2 {

class MaModificationInfo;
class MSAEditor;

class MSAGeneralTab : public QWidget, public Ui_GeneralTabOptionsPanelWidget {
    Q_OBJECT
public:
    MSAGeneralTab(MSAEditor* msaEditor);

public slots:
    void sl_alignmentChanged();
    void sl_copyFormatSelectionChanged(int value);
    void sl_convertNucleicAlphabetButtonClicked();

private:
    void connectSignals();
    void initializeParameters();
    void reInitializeParameters();
    void updateState();
    void updateConvertAlphabetButtonState();

    MSAEditor* msaEditor;
    MsaOpSavableTab savableTab;
};

}  // namespace U2

#endif  // _U2_MSA_GENERAL_TAB_H_
