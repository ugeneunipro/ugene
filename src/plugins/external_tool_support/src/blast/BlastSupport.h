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

#include <U2Core/ExternalToolRegistry.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class BlastSupport : public ExternalTool {
    Q_OBJECT
public:
    BlastSupport(const QString& id);

    static const QString ET_BLASTN_ID;
    static const QString ET_BLASTP_ID;
    static const QString ET_BLASTX_ID;
    static const QString ET_TBLASTN_ID;
    static const QString ET_TBLASTX_ID;
    static const QString ET_RPSBLAST_ID;
    static const QString ET_BLASTDBCMD_ID;
    static const QString ET_MAKEBLASTDB_ID;

    static const QString BLAST_TMP_DIR;

    /**
     * Checks if the tool is configured correctly and is ready to run.
     * Asks user to set-up the BLAST tool correctly (shows a dialog) if needed.
     */
    static bool checkBlastTool(const QString& toolId);

    /** Returns BLAST tool id (ex: 'USUPP_BLASTX') by the visual program name (ex: 'blastx'). */
    static QString getToolIdByProgramName(const QString& programName);

    /** Returns BLAST program name (ex: 'blastp') by  the tool id (ex: 'USUPP_BLASTP'). */
    static QString getProgramNameByToolId(const QString& toolId);

public slots:
    void sl_runAlignToReference();

    void sl_runBlastSearch();

    void sl_runBlastDbCmd();

    void sl_runMakeBlastDb();
};

class BlastSupportContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    BlastSupportContext(QObject* p);

protected slots:
    void sl_showDialog();
    void sl_fetchSequenceById();

protected:
    void initViewContext(GObjectViewController* view) override;
    void buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) override;

private:
    QStringList searchToolIds;
    QString commaSeparatedSelectedSequenceIds;
    QAction* fetchSequenceByIdAction = nullptr;
};

}  // namespace U2
