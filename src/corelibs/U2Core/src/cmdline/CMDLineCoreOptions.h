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

#ifndef _U2_CMDLINE_OPTIONS_H_
#define _U2_CMDLINE_OPTIONS_H_

#include <QObject>
#include <QString>
#include <U2Core/global.h>

namespace U2 {

// QObject needed for translations
class U2CORE_EXPORT CMDLineCoreOptions : public QObject {
    Q_OBJECT
public:
    static const QString INI_FILE;
    static const QString SUITE_URLS;
    static const QString API_TEST_URLS;
    static const QString TEST_REPORT;
    static const QString HELP;
    static const QString HELP_SHORT;
    static const QString TRANSLATION;
    static const QString TEST_THREADS;
    static const QString TEAMCITY_OUTPUT;
    static const QString LOG_FORMAT;
    static const QString LOG_LEVEL;
    static const QString CREATE_GUI_TEST;
    static const QString LAUNCH_GUI_TEST;
    static const QString LAUNCH_GUI_TEST_NO_IGNORED;
    static const QString LAUNCH_GUI_TEST_BATCH;
    static const QString LAUNCH_GUI_TEST_SUITE;
    static const QString LAUNCH_GUI_TEST_CRAZY_USER;
    static const QString USAGE;
    static const QString DOWNLOAD_DIR;
    static const QString CUSTOM_TOOLS_CONFIG_DIR;
    static const QString TMP_DIR;
    static const QString DEFAULT_DATA_DIR;
    static const QString FILE_STORAGE_DIR;
    static const QString SESSION_DB;
    static const QString USE_SAME_INI_FOR_TESTS;

public:
    // initialize help for core cmdline options
    static void initHelp();

}; // CMDLineCoreOptions

} // U2

#endif // _U2_CMDLINE_OPTIONS_H_
