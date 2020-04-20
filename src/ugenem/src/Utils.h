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

#ifndef _UGENEM_UTILS_H_
#define _UGENEM_UTILS_H_

#include <QString>

/**
 * The methods can be used only if QCoreApplication has instance
 */
class Utils {
public:
    static bool hasReportUrl();
    static QString getReportUrl();
    static bool hasDatabaseUrl();
    static QString getDatabaseUrl();
    static QString getDumpUrl();
    static bool hasSilentModeFlag();
    static QString getFailedTestName();
    static QString loadReportFromUrl(const QString &url);

    static const QString SESSION_DB_UGENE_ARG;

private:
    static bool hasArgument(const QString &key);
    static QString getArgumentValue(const QString &key);
};

#endif // _UGENEM_UTILS_H_
