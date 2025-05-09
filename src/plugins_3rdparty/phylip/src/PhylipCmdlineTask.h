/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Algorithm/PhyTreeGeneratorTask.h>

namespace U2 {

class CmdlineInOutTaskRunner;
class MsaObject;
class PhyTreeObject;
class TmpDbiHandle;

class PhylipCmdlineTask : public PhyTreeGeneratorTask {
    Q_OBJECT
public:
    PhylipCmdlineTask(const Msa& msa, const CreatePhyTreeSettings& settings);
    ~PhylipCmdlineTask();

    void prepare() override;
    ReportResult report() override;

    static const QString PHYLIP_CMDLINE;
    static const QString MATRIX_ARG;
    static const QString GAMMA_ARG;
    static const QString ALPHA_ARG;
    static const QString TT_RATIO_ARG;
    static const QString BOOTSTRAP_ARG;
    static const QString REPLICATES_ARG;
    static const QString SEED_ARG;
    static const QString FRACTION_ARG;
    static const QString CONSENSUS_ARG;

private:
    void createCmdlineTask();
    void prepareTempDbi();

private:
    CmdlineInOutTaskRunner* cmdlineTask;
    MsaObject* msaObject;
    PhyTreeObject* treeObject;
    QString dbiPath;
    U2DbiRef dbiRef;
};

}  // namespace U2

