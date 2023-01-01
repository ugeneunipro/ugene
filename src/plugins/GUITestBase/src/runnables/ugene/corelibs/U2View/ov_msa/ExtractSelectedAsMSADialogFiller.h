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

#ifndef _U2_GT_RUNNABLES_EXTRACT_SELECTED_AS_MSA_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_EXTRACT_SELECTED_AS_MSA_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ExtractSelectedAsMSADialogFiller : public Filler {
public:
    /**
     * Initialize the default filler scenario for the dialog.
     * If 'useDefaultSequenceSelection' is 'true' ignores 'selectedSequenceNameList' values and re-uses currently checked values by MSA editor.
     */
    ExtractSelectedAsMSADialogFiller(HI::GUITestOpStatus& os,
                                     const QString& filepath,
                                     const QStringList& selectedSequenceNameList = QStringList(),
                                     int from = 0,
                                     int to = 0,
                                     bool addToProj = true,
                                     bool invertButtonPress = false,
                                     bool allButtonPress = false,
                                     bool noneButtonPress = false,
                                     bool dontCheckFilepath = false,
                                     const QString& format = "",
                                     bool useDefaultSequenceSelectionFlag = false);

    ExtractSelectedAsMSADialogFiller(HI::GUITestOpStatus& os, CustomScenario* c);

    void commonScenario() override;

    static QStringList getSequences(HI::GUITestOpStatus& os, bool selected = true);

    void setUseDefaultSequenceSelection(bool flag);

private:
    QString filepath;
    QString format;
    QStringList sequenceNameList;
    int from;
    int to;
    bool addToProj;
    bool invertButtonPress;
    bool allButtonPress;
    bool noneButtonPress;
    bool dontCheckFilepath;
    bool useDefaultSequenceSelection;
};

}  // namespace U2

#endif
