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
 
#include <QWidget>

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/global.h>
#include <U2Core/U2Location.h>
 
class Ui_GenbankStyleRegionSelector;
class QLineEdit;

namespace U2 {
    
class U2GUI_EXPORT GenbankStyleRegionSelectorController : public QWidget {
    Q_OBJECT
public:
    GenbankStyleRegionSelectorController(QWidget* parent,
                                         qint64 maxLen,
                                         DNASequenceSelection* selection = nullptr,
                                         bool isCircularAvailable = false);

    ~GenbankStyleRegionSelectorController();

    bool locationsIsOk() const;
    void setSearchLocation(const U2Location& location);
    U2Location getSearchLocation() const;
    void setExcludedLocation(const U2Location& location);
    U2Location getExcludedLocation() const;
    void setExcludeChecked(bool enable);
    bool getExcludeChecked() const;

    static const QString WHOLE_SEQUENCE;
    static const QString SELECTED_REGIONS;

private slots:
    void sl_presetChanged();
    void sl_excludeEnzymesChecked(int newState);
    void sl_genbankLocationChanged(const QString& text);

private:
    void setValidLocation(QLineEdit* lineEdit, const U2Location& location);

    Ui_GenbankStyleRegionSelector* ui;
    qint64 length;
    bool circular;
    U2Location searchLocation;
    U2Location excludeLocation;
    bool searchLocationIsOk = false;
    bool excludeLocationIsOk = false;
};

} //ns
