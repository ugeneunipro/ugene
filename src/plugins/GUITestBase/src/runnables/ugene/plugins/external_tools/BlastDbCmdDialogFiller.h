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

#ifndef _U2_BLAST_DB_CMD_DIALOG_FILLER_
#define _U2_BLAST_DB_CMD_DIALOG_FILLER_

#include "utils/GTUtilsDialog.h"

namespace U2 {
// "Fetch Sequences from BLAST Database" dialog. Can be found by right-clicking on the annotation named "blast result"->
// "Fetch sequences from local BLAST database".
class BlastDbCmdDialogFiller : public HI::Filler {
    QString dbPath;
    QString outputPath;

public:
    BlastDbCmdDialogFiller(HI::GUITestOpStatus& os, const QString& dbPath, const QString& outputPath);
    BlastDbCmdDialogFiller(HI::GUITestOpStatus& os, HI::CustomScenario* customScenario);

    void commonScenario() override;
};
}  // namespace U2

#endif  // _U2_BLAST_DB_CMD_DIALOG_FILLER_
