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

#ifndef _U2_FIND_HIGH_FLEX_REGIONS_H_
#define _U2_FIND_HIGH_FLEX_REGIONS_H_

#include "HighFlexSettings.h"
#include "HighFlexResult.h"

#include <U2Core/Task.h>
#include <U2Core/GObjectReference.h>

#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>


namespace U2 {

/**
* A listener used to provide data from the high flex algorithm
* task (FindHighFlexRegionsAlgorithm) to the task that launches
* the algorithm task (FindHighFlexRegions).
*/
class FindHighFlexRegionsListener
{
public:
    virtual ~FindHighFlexRegionsListener(){}
    virtual void onResult(const HighFlexResult& result) = 0;
};


/**
* The task processes the data selected / input by a user and
* launches the algorithm task (FindHighFlexRegionsAlgorithm).
*/
class FindHighFlexRegions : public Task, public FindHighFlexRegionsListener
{
    Q_OBJECT

public:
    FindHighFlexRegions(const DNASequence& sequence,
                        const HighFlexSettings& settings);

    void run();
    QList<HighFlexResult> getResults() const;

    virtual void onResult(const HighFlexResult& result);

private:
    const DNASequence       sequence;
    const HighFlexSettings  settings;
    QList<HighFlexResult>   results;
};

} // namespace


#endif

