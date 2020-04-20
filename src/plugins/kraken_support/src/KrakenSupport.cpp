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

#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>

#include "KrakenSupport.h"

namespace U2 {

const QString KrakenSupport::BUILD_TOOL = "kraken-build";
const QString KrakenSupport::BUILD_TOOL_ID = "USUPP_KRAKEN_BUILD";
const QString KrakenSupport::CLASSIFY_TOOL = "kraken";
const QString KrakenSupport::CLASSIFY_TOOL_ID = "USUPP_KRAKEN";
const QString KrakenSupport::GROUP_NAME = "Kraken";

KrakenSupport::KrakenSupport(const QString& id, const QString &name)
    : ExternalTool(id, name, "")
{
    toolKitName = GROUP_NAME;

    validationArguments << "--version";
    validMessage = "Kraken version ";
    versionRegExp = QRegExp("Kraken version (\\d+\\.\\d+(\\.\\d+)?(\\-[a-zA-Z]*)?)");
    dependencies << "USUPP_PERL";
    toolRunnerProgram = "USUPP_PERL";

    if (BUILD_TOOL == name) {
        initBuild();
    }

    if (CLASSIFY_TOOL == name) {
        initClassify();
    }
}

QStringList KrakenSupport::getAdditionalPaths() const {
    if (BUILD_TOOL == name) {
        return QStringList() << QFileInfo(path).dir().absolutePath() + "/jellyfish"
                             << QFileInfo(path).dir().absolutePath() + "/util";
    }
    return QStringList();
}

void KrakenSupport::initBuild() {
    executableFileName = "kraken-build";
    description = tr("The tool is used to build a Kraken database.");
}

void KrakenSupport::initClassify() {
    executableFileName = "kraken";
    description = tr("The tool is used to classify a set of sequences. It does this by examining the k-mers within a read and querying a database with those k-mers.");
}

}   // namespace U2
