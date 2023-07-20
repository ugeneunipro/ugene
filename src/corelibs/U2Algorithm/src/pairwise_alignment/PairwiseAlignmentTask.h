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

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include "../msa_alignment/AbstractAlignmentTask.h"

namespace U2 {

class PairwiseAlignmentTask;

class U2ALGORITHM_EXPORT PairwiseAlignmentTaskSettings : public AbstractAlignmentTaskSettings {
public:
    PairwiseAlignmentTaskSettings();
    PairwiseAlignmentTaskSettings(const QVariantMap& someSettings);
    PairwiseAlignmentTaskSettings(const PairwiseAlignmentTaskSettings& s);

    bool isValid() const override;

    U2EntityRef firstSequenceRef;  // couldn't be in customSettings. Set manually.
    U2EntityRef secondSequenceRef;  // couldn't be in customSettings. Set manually.

    static const QString DEFAULT_RESULT_FILE_NAME;
};

class U2ALGORITHM_EXPORT PairwiseAlignmentTask : public AbstractAlignmentTask {
    Q_OBJECT

public:
    PairwiseAlignmentTask(TaskFlags flags = TaskFlags_FOSCOE);

protected:
    QByteArray first;
    QByteArray second;
};
}  // namespace U2
