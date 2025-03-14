/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "FindEnzymesDialogBase.h"

namespace U2 {

class ADVSequenceObjectContext;
class ResultsCountFilter;
class RegionSelectorWithExclude;

/**
 * @brief This class describes a dialog, which openes in the Sequence View
 * and allows one to chose restriction enzymes to be found.
 */
class FindEnzymesDialogSequenceView : public FindEnzymesDialogBase {
    Q_OBJECT
public:

    /**
     * @brief Constructor.
     * @param parent parent widget.
     * @param advSequenceContext Context of opened sequence.
     */
    FindEnzymesDialogSequenceView(QWidget* parent, const QPointer<ADVSequenceObjectContext>& advSequenceContext);

protected:
    bool acceptProtected() override;

    void initResultsCountFilter();
    void initRegionSelectorWithExclude();

    void saveSettings() override;

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    ResultsCountFilter* countFilter = nullptr;
    RegionSelectorWithExclude* regionSelector = nullptr;
};


}  // namespace U2
