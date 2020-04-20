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
#include <U2Core/AppSettings.h>
#include <U2Core/Settings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/U2FileDialog.h>

#include "GenomeAlignerSettingsController.h"

namespace U2 {

#define SETTINGS_ROOT   QString("/genome_aligner_settings/")
#define INDEX_DIR       QString("index_dir")

QString GenomeAlignerSettingsUtils::getIndexDir() {
    QString defaultDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("aligner");
    QString res = AppContext::getSettings()->getValue(SETTINGS_ROOT + INDEX_DIR, defaultDir, true).toString();

    return res;
}

void GenomeAlignerSettingsUtils::setIndexDir(const QString &indexDir) {
    QString defaultDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("aligner");
    if (defaultDir != indexDir) {
        AppContext::getSettings()->setValue(SETTINGS_ROOT + INDEX_DIR, indexDir, true);
    }
}


} //namespace
