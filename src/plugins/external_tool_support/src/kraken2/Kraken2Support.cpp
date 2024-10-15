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

#include "Kraken2Support.h"

#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>

namespace U2 {


const QString Kraken2Support::CLASSIFY_TOOL = "kraken 2";
const QString Kraken2Support::CLASSIFY_TOOL_ID = "USUPP_KRAKEN";

Kraken2Support::Kraken2Support()
    : ExternalTool(id, "Kraken 2", name) {
    toolKitName = "Kraken 2";

    validationArguments << "--version";
    versionRegExp = QRegExp("Kraken version (\\d+\\.\\d+(\\.\\d+)?(\\-[a-zA-Z]*)?)");
    dependencies << "USUPP_PERL";
    toolRunnerProgram = "USUPP_PERL";
    executableFileName = "kraken2";
    description = tr("The tool is used to classify a set of sequences. It does this by examining the k-mers within a read and querying a database with those k-mers.");
}

}  // namespace U2
