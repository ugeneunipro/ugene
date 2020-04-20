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

#ifndef _U2_SMITH_WATERMAN_ALG_IMPL_H_
#define _U2_SMITH_WATERMAN_ALG_IMPL_H_

#include <U2Algorithm/SmithWatermanTaskFactory.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>
#include <U2Algorithm/SmithWatermanSettings.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/Task.h>

#include "SWAlgorithmTask.h"

#include <QByteArray>


namespace U2 {

class PairwiseAlignmentTask;

class SWTaskFactory: public SmithWatermanTaskFactory {          //for ADV search only
public:
    SWTaskFactory(SW_AlgType _algType);
    virtual ~SWTaskFactory();
    virtual Task* getTaskInstance(const SmithWatermanSettings& config, const QString& taskName) const;

private:
    bool isValidParameters(const SmithWatermanSettings& sWatermanConfig,  SequenceWalkerSubtask* t) const;      //not realized
    SW_AlgType algType;

};

class PairwiseAlignmentSmithWatermanTaskFactory : public AbstractAlignmentTaskFactory {           //for pairwise alignment only
public:
    PairwiseAlignmentSmithWatermanTaskFactory(SW_AlgType _algType);
    virtual ~PairwiseAlignmentSmithWatermanTaskFactory();

    virtual AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings* settings) const;

private:
    SW_AlgType algType;
};

} // namespace

#endif // _U2_SMITH_WATERMAN_ALG_IMPL_H_
