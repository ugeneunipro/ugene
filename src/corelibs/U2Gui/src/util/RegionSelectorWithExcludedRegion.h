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

#ifndef _U2_REGION_SELECTOR_WITH_EXCLUDED_REGION_H_
#define _U2_REGION_SELECTOR_WITH_EXCLUDED_REGION_H_

#include <U2Core/global.h>

#include "RegionSelector.h"

class Ui_RegionSelectorWithExcludedRegion;


namespace U2 {

class U2GUI_EXPORT RegionSelectorWithExludedRegion : public QWidget {
    Q_OBJECT
public:
    RegionSelectorWithExludedRegion(QWidget* parent,
                                    qint64 maxLen,
                                    DNASequenceSelection* selection = NULL,
                                    bool isCircularAvailable = false);
    ~RegionSelectorWithExludedRegion();

    U2Region getIncludeRegion(bool *ok = NULL) const;
    U2Region getExcludeRegion(bool *ok = NULL) const;

    void setIncludeRegion(const U2Region& r);
    void setExcludeRegion(const U2Region& r);
    void setExcludedCheckboxChecked(bool checked);

    bool hasError() const;
    QString getErrorMessage() const;

private:
    void connectSlots();

private:
    Ui_RegionSelectorWithExcludedRegion* ui;

    RegionSelectorController* includeController;
    RegionSelectorController* excludeController;
};

} // namespace

#endif // _U2_REGION_SELECTOR_WITH_EXCLUDED_REGION_H_
