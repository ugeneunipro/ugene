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

#ifndef _U2_DNA_FLEX_DIALOG_H_
#define _U2_DNA_FLEX_DIALOG_H_

#include "ui_DNAFlexDialog.h"

#include "HighFlexSettings.h"


namespace U2 {


class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;


/**
 * Main and the only dialog of the DNA Flexibility plugin.
 */
class DNAFlexDialog : public QDialog, public Ui_DNAFlexDialog
{
    Q_OBJECT

public:
    DNAFlexDialog(ADVSequenceObjectContext* ctx);
    virtual void accept();

private slots:
    void sl_spinWindowSizeChanged(int);
    void sl_spinWindowStepChanged(int);
    void sl_spinThresholdChanged(double);
    void sl_rememberSettings();
    void sl_defaultSettings();
    void sl_updateSizes(int);

private:
    ADVSequenceObjectContext* ctx;
    CreateAnnotationWidgetController* annotController;
    HighFlexSettings settings;

    void updateHighFlexValues();
};


} // namespace

#endif
