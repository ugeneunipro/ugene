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

#ifndef _U2_MR_BAYES_DIALOG_WIDGET_
#define _U2_MR_BAYES_DIALOG_WIDGET_

#include <U2Core/AppContext.h>
#include <U2Core/MultipleSequenceAlignment.h>

#include <U2View/CreatePhyTreeWidget.h>

#include "ui_MrBayesDialog.h"

namespace U2 {

class MrBayesWidget : public CreatePhyTreeWidget, private Ui_MrBayesDialog {
   Q_OBJECT
public:
    MrBayesWidget(const MultipleSequenceAlignment &ma, QWidget *parent);

    void fillSettings(CreatePhyTreeSettings& settings);
    void storeSettings();
    void restoreDefault();
    int getRandomSeed();
    bool checkSettings(QString &msg, const CreatePhyTreeSettings &settings);

private:
    void setComboText(QComboBox* combo, const QString& text);
    QString generateMrBayesSettingsScript();
    bool isAminoAcidAlphabet;

private slots:
     void sl_onRateChanged(const QString& modelName);
};

}   // namespace U2

#endif // _U2_MR_BAYES_DIALOG_WIDGET_
