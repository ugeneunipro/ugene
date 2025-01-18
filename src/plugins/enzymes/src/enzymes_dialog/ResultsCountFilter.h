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

#include <ui_ResultsCountFilter.h>

namespace U2 {

/**
 * @brief This widget describes a group box with minimum and maximum numbers of restriction enzymes, which could be found.
 */
class ResultsCountFilter : public QWidget, public Ui_ResultsCountFilter {
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param parent parent widget.
     */
    ResultsCountFilter(QWidget* parent);

    /**
    * @brief Load file with enzymes settings from .ini file.
    */
    void saveSettings();

private:
    void initSettings();

    static constexpr int MIN_HIT_DEFAULT_VALUE = 1;
    static constexpr int MAX_HIT_DEFAULT_VALUE = 2;

};

}  // namespace U2
