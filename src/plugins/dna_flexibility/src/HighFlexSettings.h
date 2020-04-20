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

#ifndef _U2_HIGH_FLEX_SETTINGS_H_
#define _U2_HIGH_FLEX_SETTINGS_H_

#include <QString>


namespace U2 {


class HighFlexSettings
{
public:
    HighFlexSettings();

    int             windowSize;
    int             windowStep;
    double          threshold;

    void restoreDefaults();
    void rememberSettings();

private:
    static const int DEFAULT_WINDOW_SIZE = 100;
    static const int DEFAULT_WINDOW_STEP = 1;
    static const double DEFAULT_THRESHOLD;
    static const QString SETTINGS_PATH;
    static const QString SETTING_WINDOW_SIZE;
    static const QString SETTING_WINDOW_STEP;
    static const QString SETTING_THRESHOLD;

    bool restoreRememberedSettings();
};


} // namespace

#endif
