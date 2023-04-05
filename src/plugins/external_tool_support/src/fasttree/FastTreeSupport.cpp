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

#include "FastTreeSupport.h"

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRunTask.h>

#include <U2View/CreatePhyTreeWidget.h>

#include "FastTreeTask.h"
#include "FastTreeWidget.h"

namespace U2 {

const QString FastTreeSupport::FAST_TREE_ID("USUPP_FAST_TREE");
const QString FastTreeSupport::ET_FAST_TREE_ALGORITHM_NAME_AND_KEY("FastTree");

FastTreeSupport::FastTreeSupport()
    : ExternalTool(FastTreeSupport::FAST_TREE_ID, "fasttree", "FastTree") {
    executableFileName = isOsWindows() ? "FastTree.exe" : "FastTree";
    validationArguments << "-expert";
    validMessage = "Detailed usage for FastTree";
    description = tr("<i>FastTree</i> builds phylogenetic trees from alignments of nucleotide or protein sequences.<br>FastTree can handle alignments with up to a million of sequences.");
    versionRegExp = QRegExp(R"(Detailed usage for FastTree (\d+\.\d+\.\d+).*)");
    toolKitName = "FastTree";

    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new FastTreeAdapter(), FastTreeSupport::ET_FAST_TREE_ALGORITHM_NAME_AND_KEY);
    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinTemporaryDirPath;
    }
}

////////////////////////////////////////
// FastTreeAdapter

Task* FastTreeAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings) {
    return new FastTreeTask(msa, settings);
}

CreatePhyTreeWidget* FastTreeAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment& msa, QWidget* parent) {
    return new FastTreeWidget(msa, parent);
}

}  // namespace U2
