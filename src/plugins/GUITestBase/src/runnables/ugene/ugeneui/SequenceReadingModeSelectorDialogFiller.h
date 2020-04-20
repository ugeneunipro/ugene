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

#ifndef _U2_GT_RUNNABLES_SEQUENCE_READING_MODE_SELECTOR_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_SEQUENCE_READING_MODE_SELECTOR_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;
    class SequenceReadingModeSelectorDialogFiller : public Filler {
    public:
        enum ReadingMode {Separate, Merge, Join, Align};

        SequenceReadingModeSelectorDialogFiller(HI::GUITestOpStatus &_os, ReadingMode _mode = Separate, int _bases=10, bool cancel = false):
        Filler(_os, "SequenceReadingModeSelectorDialog"), readingMode(_mode), bases(_bases), cancel(cancel) {}
        SequenceReadingModeSelectorDialogFiller(HI::GUITestOpStatus &_os, CustomScenario* c);

        virtual void commonScenario();
    private:
        ReadingMode readingMode;
        int bases;
        bool cancel;
    };
}

#endif
