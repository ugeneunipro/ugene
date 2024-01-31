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
#pragma once
#include <QSize>

#include <U2Core/Task.h>

namespace U2 {
struct MfoldSettings;
class SequenceObjectContext;

class MfoldTask final : public Task {
    Q_OBJECT
    QByteArray seq;
    QStringList mfoldArgs;
    QString outPath;  // path for HTML report and imgs. Empty if saved temporarily.
    QString cwd;  // tmp subfolder, all output is stored here. Then, if outPath is set, only imgs are copied to outPath.
    QString report;  // HTML report to be displayed in UGENE and saved to a file if necessary.
    QSize imgSize;

public:
    // seq -- sequence for analysis
    // settings -- settings changed by the user inside the dialog are used as tool args
    // isCircular, isDNA -- internal sequence parameters used as tool args
    // imgSize -- img size for pretty display of HTML report
    MfoldTask(const QByteArray& seq, const MfoldSettings& settings, bool isCircular, bool isDNA, const QSize& imgSize);

    void prepare() override;
    void run() override;
    QString generateReport() const override;
};

// Creates new MfoldTask based on sequence context. windowWidth is used for image size.
MfoldTask* createMfoldTask(SequenceObjectContext* ctx, const MfoldSettings& settings, int windowWidth, U2OpStatus& os);
}  // namespace U2
