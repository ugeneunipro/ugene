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
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>

#include "CMDLineCoreOptions.h"

namespace U2 {

const QString CMDLineCoreOptions::INI_FILE                      = "ini-file";
const QString CMDLineCoreOptions::SUITE_URLS                    = "test-suite";
const QString CMDLineCoreOptions::API_TEST_URLS                 = "api-test-suite";
const QString CMDLineCoreOptions::TEST_REPORT                   = "test-report";
const QString CMDLineCoreOptions::HELP                          = "help";
const QString CMDLineCoreOptions::HELP_SHORT                    = "h";
const QString CMDLineCoreOptions::TRANSLATION                   = "lang";
const QString CMDLineCoreOptions::TEST_THREADS                  = "test-threads";
const QString CMDLineCoreOptions::TEAMCITY_OUTPUT               = "log-teamcity-out";
const QString CMDLineCoreOptions::LOG_FORMAT                    = "log-format";
const QString CMDLineCoreOptions::LOG_LEVEL                     = "log-level";
const QString CMDLineCoreOptions::CREATE_GUI_TEST               = "create-gui-test";
const QString CMDLineCoreOptions::LAUNCH_GUI_TEST               = "gui-test";
const QString CMDLineCoreOptions::LAUNCH_GUI_TEST_NO_IGNORED    = "gui-test-no-ignored";
const QString CMDLineCoreOptions::LAUNCH_GUI_TEST_BATCH         = "gui-test-batch";
const QString CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE         = "gui-test-suite";
const QString CMDLineCoreOptions::LAUNCH_GUI_TEST_CRAZY_USER    = "gui-test-crazy-user";
const QString CMDLineCoreOptions::USAGE                         = "usage";
const QString CMDLineCoreOptions::DOWNLOAD_DIR                  = "download-dir";
const QString CMDLineCoreOptions::CUSTOM_TOOLS_CONFIG_DIR       = "custom-external-tool-config-dir";
const QString CMDLineCoreOptions::TMP_DIR                       = "tmp-dir";
const QString CMDLineCoreOptions::DEFAULT_DATA_DIR              = "default-data-dir";
const QString CMDLineCoreOptions::FILE_STORAGE_DIR              = "file-storage-dir";
const QString CMDLineCoreOptions::SESSION_DB                    = "session-db";
const QString CMDLineCoreOptions::USE_SAME_INI_FOR_TESTS        = "use-same-ini-for-tests";


void CMDLineCoreOptions::initHelp() {
    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );

    CMDLineHelpProvider * helpSection = new CMDLineHelpProvider(
        HELP,
        tr( "Shows help information." ),
        "",         // No full description
        "",         // No arguments
        HELP_SHORT);

    CMDLineHelpProvider * loadSettingsFileSection = new CMDLineHelpProvider(
        INI_FILE,
        tr( "Loads UGENE configuration." ),
        tr( "Loads configuration from the specified .ini file. By default the UGENE.ini file is used." ),
        tr( "<path_to_file>" ));

    CMDLineHelpProvider * translSection = new CMDLineHelpProvider(
        TRANSLATION,
        tr( "Specifies the language to use." ),
        tr( "Specifies the language to use. The following values are available: CS, EN, RU, ZH." ),
        tr( "<language_code>" ));

    CMDLineHelpProvider * tmpDirSection = new CMDLineHelpProvider(
        TMP_DIR,
        "Path to temporary folder",
        "",
        tr( "<path_to_file>" ));

    CMDLineHelpProvider * sessionDatabaseSection = new CMDLineHelpProvider(
        SESSION_DB,
        tr("Path to the session database file"),
        tr("Session database is stored in the temporary file that is created for every UGENE run.\n"
        "But it can be supplied with the command line argument.\n"
        "If the supplied file does not exist it will be created.\n"
        "The session database file is removed after closing of UGENE."),
        tr( "<path_to_file>" ));

    cmdLineRegistry->registerCMDLineHelpProvider( helpSection );
    cmdLineRegistry->registerCMDLineHelpProvider( loadSettingsFileSection );
    cmdLineRegistry->registerCMDLineHelpProvider( translSection );
    cmdLineRegistry->registerCMDLineHelpProvider( tmpDirSection );
    cmdLineRegistry->registerCMDLineHelpProvider( sessionDatabaseSection);
}

} // U2
