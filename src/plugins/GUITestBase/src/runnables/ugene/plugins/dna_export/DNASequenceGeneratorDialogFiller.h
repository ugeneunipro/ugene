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

#ifndef _U2_GT_RUNNABLES_DNA_SEQUENCE_GENERATOR_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_DNA_SEQUENCE_GENERATOR_DIALOG_FILLER_H_

#include <U2Core/U2IdTypes.h>

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class DNASequenceGeneratorDialogFillerModel {
public:
    DNASequenceGeneratorDialogFillerModel(const QString& url);

    QString url;

    /** When referenceUrl is set it overrides percentage values. */
    QString referenceUrl;

    int length = 1000;
    int window = 1000;

    /** Count of sequences in the result file. */
    int numberOfSequences = 1;

    int percentA = 25;
    int percentC = 25;
    int percentG = 25;
    int percentT = 25;

    int seed = -1;

    /** Document format to use. If empty - the default one is used. */
    DocumentFormatId formatId;
};

class DNASequenceGeneratorDialogFiller : public Filler {
public:
    /** Fills generator dialog with the given set of values and runs it. */
    DNASequenceGeneratorDialogFiller(GUITestOpStatus& os, const DNASequenceGeneratorDialogFillerModel& model);
    void commonScenario() override;

private:
    DNASequenceGeneratorDialogFillerModel model;
};

}  // namespace U2

#endif
