/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GT_UTILS_H_
#define _U2_GT_UTILS_H_

#include <GTGlobals.h>

#include <QString>

namespace U2 {

using namespace HI;

class GTUtils {
public:
    /** Generates unique string with the given prefix. Tries to preserve suffix uniqueness to be used safely in parallel test runs. */
    static QString genUniqueString(const QString& prefix = "");

    /** Waits until service is enabled. Fails if the service in not active within the default timeout. */
    static void checkServiceIsEnabled(HI::GUITestOpStatus& os, const QString& serviceName);

    static void checkExportServiceIsEnabled(HI::GUITestOpStatus& os) {
        checkServiceIsEnabled(os, "DNA export service");
    }

    /**
     * Match text from test with an actual text from UI according to the flag. Returns true if texts are matched.
     * For all 'contains'-line matchers 'textInUi' is checked to contain 'textInTest'.
     * If some match flag from the flags is not supported an error is set to 'os'.
     */
    static bool matchText(HI::GUITestOpStatus& os, const QString& textInTest, const QString& textInUi, const Qt::MatchFlags& matchFlags);
};

}  // namespace U2
#endif
