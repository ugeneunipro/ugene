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

#ifndef _U2_IQTREE_TASK_H_
#define _U2_IQTREE_TASK_H_

#include <U2Algorithm/PhyTreeGeneratorTask.h>

namespace U2 {

/** Set of data required by IQTreeTask. */
struct IQTreeTaskContext {
    /** Creates a new context for IQTreeTask. */
    IQTreeTaskContext(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings);

    /** Alignment to process. Provided during object instantiation. */
    MultipleSequenceAlignment msa;

    /** Extra setting for the tool. Provided during object instantiation. */
    const CreatePhyTreeSettings settings;

    /** Tmp dir to run iqtree tool. Set by PrepareIQTreeWorkDirTask. */
    QString tmpDirUrl;

    /** Path to MSA object in 'tmpDirUrl'. Set by PrepareIQTreeWorkDirTask*/
    QString alignmentFilePath;

    /** Result tree produced by IQTree tool. */
    PhyTree resultTree;
};

class IQTreeTask : public PhyTreeGeneratorTask {
    Q_OBJECT
public:
    IQTreeTask(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings);

    Task::ReportResult report() override;

    IQTreeTaskContext context;
};

/**
 * Prepares a temporary work directory for IQTree tool.
 * Copies MSA into a file in the work dir.
 *
 * Updates IQTreeTaskContext with a new file location.
 */
class PrepareIQTreeWorkDirTask : public Task {
    Q_OBJECT
public:
    PrepareIQTreeWorkDirTask(IQTreeTaskContext* context);

    void run() override;

    IQTreeTaskContext* const context;
};

/**
 * Runs IQTree external tool and loads a result tree.
 * Saves the loaded tree data structure into IQTreeTaskContext.
 */
class RunIQTreeExternalToolTask : public Task {
    Q_OBJECT
public:
    RunIQTreeExternalToolTask(IQTreeTaskContext* context);

    void prepare() override;

    Task::ReportResult report() override;

    IQTreeTaskContext* const context;
};

}  // namespace U2

#endif  // _U2_IQTREE_TASK_H_
