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

#include <QCheckBox>
#include <QPointer>
#include <QWidget>

#include <U2Core/U2Location.h>

namespace U2 {

class ADVSequenceObjectContext;
class RegionSelector;

/**
 * @brief This class describes a pair of region slectors - outer and inner -
 * which allows one to set one uncut region or two different regions to process the task in.
 */
class RegionSelectorWithExclude : public QWidget {
public:
    /**
     * @brief Constructor.
     * @param parent parent widget.
     * @param advSequenceContext Context of opened sequence.
     */
    RegionSelectorWithExclude(QWidget* parent, const QPointer<ADVSequenceObjectContext>& advSequenceContext);

     /**
     * @param ok if true - then region is valid.
     * @return selected location.
     */
    U2Location getRegionSelectorLocation(bool* ok) const;
    /**
     * @param ok if true - then region is valid.
     * @return selected exclude location.
     */
    U2Location getExcludeRegionSelectorLocation(bool* ok) const;
    /**
     * @return true if "exclude" checkbox is checked.
     */
    bool isExcludeCheckboxChecked() const;

    /**
     * @brief Load file with enzymes settings from .ini file.
     */
    void saveSettings();

private:
    void fixPreviousLocation(U2Location& prevLocation) const;

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    RegionSelector* regionSelector = nullptr;
    RegionSelector* excludeRegionSelector = nullptr;
    QCheckBox* excludeCheckbox = nullptr;

};

}  // namespace U2
