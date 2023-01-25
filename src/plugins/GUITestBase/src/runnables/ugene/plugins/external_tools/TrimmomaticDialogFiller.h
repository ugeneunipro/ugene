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

#ifndef TRIMMOMATIC_DIALOG_FILLER_H
#define TRIMMOMATIC_DIALOG_FILLER_H

#include <base_dialogs/GTFileDialog.h>

namespace U2 {

using namespace HI;

class WorkflowProcessItem;

class TrimmomaticDialogFiller : public Filler {
public:
    enum class TrimmomaticSteps {
        AVGQUAL,
        CROP,
        HEADCROP,
        ILLUMINACLIP,
        LEADING,
        MAXINFO,
        MINLEN,
        SLIDINGWINDOW,
        TOPHRED33,
        TOPHRED64,
        TRAILING
    };

    enum class TrimmomaticValues {
        // AVGQUAL, LEADING, SLIDINGWINDOW and TRAIILING
        QualityThreshold,
        // CROP, HEADCROP, MINLEN
        Length,
        // ILLUMINACLIP
        AdapterSequence,
        SeedMismatches,
        PalindromeClipThreshold,
        SimpleClipThreshold,
        ProvideOptionalSettings,
        MinAdapterLength,
        KeepBothReads,
        // MAXINFO
        TargetLength,
        Strictness,
        // SLIDINGWINDOW
        WindowSize
    };

    enum class TrimmomaticDirection {
        Up,
        Down
    };

    static void openDialog(HI::GUITestOpStatus& os, WorkflowProcessItem* trimmomaticElement);

    // Add steps
    TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<TrimmomaticSteps, QMap<TrimmomaticValues, QVariant>>>& addValues);
    // Move steps
    TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<QPair<TrimmomaticSteps, int>, TrimmomaticDirection>>& moveValues);
    // Remove steps
    TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<TrimmomaticSteps, int>>& removeValues);
    TrimmomaticDialogFiller(HI::GUITestOpStatus& os, CustomScenario* customScenario);

    void commonScenario() override;

private:
    enum class Action {
        AddSteps,
        MoveSteps,
        RemoveSteps
    };
    void addSteps();
    QList<QPair<TrimmomaticSteps, QMap<TrimmomaticValues, QVariant>>> addValues;

    void moveSteps();
    QList<QPair<QPair<TrimmomaticSteps, int>, TrimmomaticDirection>> moveValues;

    void removeSteps();
    QList<QPair<TrimmomaticSteps, int>> removeValues;

    Action a;
    static const QMap<TrimmomaticSteps, QString> STEPS;
    static const QMap<TrimmomaticValues, QString> VALUES;
};

}  // namespace U2

#endif  // TRIMMOMATIC_DIALOG_FILLER_H
