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

#ifndef _U2_GT_RUNNABLES_EDIT_SEQUENCE_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_EDIT_SEQUENCE_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class InsertSequenceFiller : public Filler {
public:
    enum documentFormat {FASTA, Genbank};
    enum RegionResolvingMode {Resize, Remove, SplitJoin, SplitSeparate};

    InsertSequenceFiller(HI::GUITestOpStatus &_os, const QString &_pasteDataHere, RegionResolvingMode _regionResolvingMode = Resize, int _insertPosition = 1,
        const QString &_documentLocation = "",  documentFormat _format = FASTA, bool _saveToNewFile = false, bool _mergeAnnotations = false,
        GTGlobals::UseMethod method = GTGlobals::UseMouse, bool _wrongInput = false, bool recalculateQuals = false);
    void commonScenario();

private:
    QString pasteDataHere;
    RegionResolvingMode regionResolvingMode;
    int insertPosition;
    QString documentLocation;
    documentFormat format;
    bool saveToNewFile;
    bool mergeAnnotations;
    GTGlobals::UseMethod useMethod;
    bool wrongInput;
    bool recalculateQuals;

    QMap<documentFormat, QString> comboBoxItems;
};

}

#endif
