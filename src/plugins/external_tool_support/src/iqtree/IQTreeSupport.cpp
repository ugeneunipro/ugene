/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "IQTreeSupport.h"

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRunTask.h>

#include <U2View/CreatePhyTreeWidget.h>

#include "IQTreeTask.h"
#include "IQTreeWidget.h"

namespace U2 {

const QString IQTreeSupport::IQTREE_ID("USUPP_IQTREE");
const QString IQTreeSupport::ET_IQTREE_ALGORITHM_NAME_AND_KEY("IQ-TREE");

IQTreeSupport::IQTreeSupport()
    : ExternalTool(IQTreeSupport::IQTREE_ID, "iqtree", "IQ-TREE") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = isOsWindows() ? "iqtree.exe" : "iqtree";
    validationArguments << "--version";
    validMessage = "IQ-TREE";
    description = tr("<i>IQ-TREE</i>  Efficient software for phylogenomic inference");
    versionRegExp = QRegExp("IQ-TREE .* version (\\d+\\.\\d+\\.\\d+).*");
    toolKitName = "IQ-TREE";

    // register the method
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new IQTreeAdapter(), IQTreeSupport::ET_IQTREE_ALGORITHM_NAME_AND_KEY);
    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinTemporaryDirPath;
    }
}

////////////////////////////////////////
// IQTreeAdapter

Task* IQTreeAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings) {
    return new IQTreeTask(msa, settings);
}

CreatePhyTreeWidget* IQTreeAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment& msa, QWidget* parent) {
    return new IQTreeWidget(msa, parent);
}

}  // namespace U2
