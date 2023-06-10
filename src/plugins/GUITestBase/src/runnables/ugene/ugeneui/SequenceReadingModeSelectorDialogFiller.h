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
class SequenceReadingModeSelectorDialogFiller : public Filler {
public:
    enum ReadingMode {
        /** Opens as separate sequences. */
        Separate,
        /** Merges sequences into a single sequence. */
        Merge,
        /** Joins sequences into an alignment. */
        Join,
        /** Aligns sequences to a reference. */
        Align
    };

    SequenceReadingModeSelectorDialogFiller(ReadingMode _mode = Separate, int _bases = 10, bool cancel = false)
        : Filler("SequenceReadingModeSelectorDialog"), readingMode(_mode), bases(_bases), cancel(cancel) {
    }
    SequenceReadingModeSelectorDialogFiller(CustomScenario* c);

    void commonScenario() override;

private:
    ReadingMode readingMode;
    int bases;
    bool cancel;
};
}  // namespace U2
