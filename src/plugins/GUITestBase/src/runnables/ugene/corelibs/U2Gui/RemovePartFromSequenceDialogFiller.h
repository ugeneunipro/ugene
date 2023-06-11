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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class RemovePartFromSequenceDialogFiller : public Filler {
public:
    enum RemoveType { Remove,
                      Resize };
    enum FormatToUse { FASTA,
                       Genbank };

    RemovePartFromSequenceDialogFiller(QString _range, bool recalculateQuals = false);
    RemovePartFromSequenceDialogFiller(RemoveType _removeType, bool _saveNew, const QString& _saveToFile, FormatToUse _format);
    RemovePartFromSequenceDialogFiller(RemoveType _removeType);

    void commonScenario();

private:
    QString range;
    RemoveType removeType;
    FormatToUse format;
    bool saveNew;
    QString saveToFile;
    QMap<FormatToUse, QString> comboBoxItems;
    bool recalculateQuals;
};

}  // namespace U2
