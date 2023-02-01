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

#pragma once

#include <U2Algorithm/PhyTreeGeneratorTask.h>

namespace U2 {

class ExternalToolRunTask;
class LoadDocumentTask;

/** Set of data required by FastTreeTask. */
struct FastTreeTaskContext {
    /** Creates a new context for FastTreeTask. */
    FastTreeTaskContext(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings);

    /** Alignment to process. Provided during object instantiation. */
    MultipleSequenceAlignment msa;

    /** Extra setting for the tool. Provided during object instantiation. */
    const CreatePhyTreeSettings settings;

    /** Tmp dir to run FastTree tool. Set by PrepareFastTreeWorkDirTask. */
    QString tmpDirUrl;

    /** Path to MSA object in 'tmpDirUrl'. Set by PrepareFastTreeWorkDirTask*/
    QString alignmentFilePath;

    /** Result tree produced by FastTree tool. */
    PhyTree resultTree;
};

class FastTreeTask : public PhyTreeGeneratorTask {
    Q_OBJECT
public:
    FastTreeTask(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings);

    Task::ReportResult report() override;

    FastTreeTaskContext context;
};

/**
 * Prepares a temporary work directory for FastTree tool.
 * Copies MSA into a file in the work dir.
 *
 * Updates FastTreeTaskContext with a new file location.
 */
class PrepareFastTreeWorkDirTask : public Task {
    Q_OBJECT
public:
    PrepareFastTreeWorkDirTask(FastTreeTaskContext* context);

    void run() override;

    FastTreeTaskContext* const context;
};

/**
 * Runs FastTree external tool and loads a result tree.
 * Saves the loaded tree data structure into FastTreeTaskContext.
 */
class RunFastTreeExternalToolTask : public Task {
    Q_OBJECT
public:
    RunFastTreeExternalToolTask(FastTreeTaskContext* context);

    void prepare() override;

    Task::ReportResult report() override;

private:
    FastTreeTaskContext* const context;

    ExternalToolRunTask* externalToolTask = nullptr;
    LoadDocumentTask* loadResultTask = nullptr;
};

}  // namespace U2
