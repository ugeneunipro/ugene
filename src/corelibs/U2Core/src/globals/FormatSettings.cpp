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

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include "FormatSettings.h"

namespace U2 {

#define SETTINGS_ROOT   QString("/format_settings/")
#define CASE_ANNS_MODE   QString("case_anns_mode")
#define LOWER_CASE_MODE QString("lower")
#define UPPER_CASE_MODE QString("upper")
#define NO_CASE_MODE QString("no")

FormatAppsSettings::FormatAppsSettings() {

}

FormatAppsSettings::~FormatAppsSettings() {

}

CaseAnnotationsMode FormatAppsSettings::getCaseAnnotationsMode() {
    QString modeStr = AppContext::getSettings()->getValue(SETTINGS_ROOT + CASE_ANNS_MODE, NO_CASE_MODE).toString();
    if (LOWER_CASE_MODE == modeStr) {
        return LOWER_CASE;
    } else if (UPPER_CASE_MODE == modeStr) {
        return UPPER_CASE;
    } else {
        return NO_CASE_ANNS;
    }
}

void FormatAppsSettings::setCaseAnnotationsMode(CaseAnnotationsMode mode) {
    QString modeStr;
    switch (mode) {
        case LOWER_CASE:
            modeStr = LOWER_CASE_MODE;
            break;
        case UPPER_CASE:
            modeStr = UPPER_CASE_MODE;
            break;
        case NO_CASE_ANNS:
            modeStr = NO_CASE_MODE;
    }
    AppContext::getSettings()->setValue(SETTINGS_ROOT + CASE_ANNS_MODE, modeStr);
}

} // U2
