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

#ifndef _U2_BLAST_ALL_SUPPORT_DIALOG_FILLER_H_
#define _U2_BLAST_ALL_SUPPORT_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

/**
 * @brief The BlastAllSupportDialogFiller class
 * It is very simple filler. It can only click the "Cancel" button.
 */
class BlastAllSupportDialogFiller : public Filler {
public:
    class Parameters {
    public:
        Parameters():
          runBlast(false),
          programNameText("blastn"),
          withInputFile(false),
          test_3211(false) {}


          bool runBlast;
          QString programNameText;
          QString dbPath;
          bool withInputFile;
          QString inputPath;
          bool test_3211;
    };

    BlastAllSupportDialogFiller(const Parameters &parameters, HI::GUITestOpStatus &os);
    BlastAllSupportDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

private:
    void test_3211();

private:
    Parameters parameters;
    QWidget *dialog;
};

}

#endif // _U2_BLAST_ALL_SUPPORT_DIALOG_FILLER_H_
