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
#include <U2Core/DNASequence.h>
#include <U2Core/GUrl.h>
#include <U2Core/StrPackUtils.h>
#include <U2Core/Task.h>

#include "MfoldSettings.h"

namespace U2 {
class OutputCollector;
class U2SequenceObject;

// For a given sequence fragment, task runs external script mfold, which calculates foldings and saves imgs
// of found hairpins. Output files are saved in temporary dir by default, but the most important files are saved in
// a user-specified dir. As a report, it displays HTML table with folding data and hairpin imgs.
class MfoldTask final : public Task {
    Q_OBJECT

    class ReportHelper;

    GUrl cwd;  // tmp subfolder, all output is stored here
    DNASequence tmpSeq;  // part of sequence for analysis
    QString tmpSeqPath;  // tmpSeq will be saved here. Lies in cwd. Used as ET input.

    MfoldSettings settings;
    MfoldSequenceInfo seqInfo;  // for dumping info to HTML report
    int windowWidth = 0;

    QString outHtmlPath;
    OutputCollector* etStdoutStderrListener = nullptr;
    QString log;
    QString report;  // HTML report to be displayed in UGENE and saved to a file if necessary

    // For ET task input sequence saved in separate file in cwd. Returns this path. Expects valid cwd.
    QString constructTmpSeqFilePath() const;
    // Prepare seqInfo.seqName for dumping to tmp file.
    QString constructSeqName() const;
    // Like /dir_with_sequence/mfold/2006.01.02_15-04-05/
    QString constructOutPath() const;

    // Expects valid tmpSeqPath.
    QStringList constructEtArgs() const;
    // Expects valid outPath.
    StrStrMap constructEtEnv() const;

    // Expects valid tmpSeq and tmpSeqPath.
    void saveTmpSeq();

public:
    // tmpSeq -- sequence for analysis
    // settings -- settings changed by the user inside the dialog are used as tool args
    // seqInfo -- internal sequence parameters used as tool args
    // windowWidth -- used for pretty display of HTML report
    MfoldTask(const DNASequence& tmpSeq,
              const MfoldSettings& settings,
              const MfoldSequenceInfo& seqInfo,
              int windowWidth);

    void prepare() override;
    void run() override;
    QString generateReport() const override;
};

// Creates new MfoldTask based on sequence context. windowWidth is used for image size.
MfoldTask* createMfoldTask(U2SequenceObject* ctx, const MfoldSettings& settings, int windowWidth, U2OpStatus& os);
}  // namespace U2
