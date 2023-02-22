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

#include <QPixmap>

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ImageExportTask.h>

class Ui_MSAExportSettings;

namespace U2 {

class MaEditorWgt;

struct MSAImageExportSettings {
    bool exportAll = true;
    U2Region region;
    QList<int> seqIdx;

    bool includeSeqNames = false;
    bool includeConsensus = false;
    bool includeRuler = true;

    /** When defined and > region.length the export image will contain a multi-line rendered MSA. */
    int basesPerLine = 0;
};

class MSAImageExportTask : public ImageExportTask {
    Q_OBJECT
public:
    // TODO: unsafe code: ui may be destroyed during the task execution (the editor is closed)!!
    MSAImageExportTask(MaEditorWgt* ui,
                       const MSAImageExportSettings& msaSettings,
                       const ImageExportTaskSettings& settings);

protected:
    void paintSequencesNames(QPainter& painter);
    void paintConsensusAndRuler(QPainter& painter, const U2Region& region);
    bool paintSequenceArea(QPainter& painter, const U2Region& region);

    MaEditorWgt* ui = nullptr;
    MSAImageExportSettings msaSettings;
};

class MSAImageExportToBitmapTask : public MSAImageExportTask {
    Q_OBJECT
public:
    MSAImageExportToBitmapTask(MaEditorWgt* ui,
                               const MSAImageExportSettings& msaSettings,
                               const ImageExportTaskSettings& settings);
    void run() override;

private:
    /**
     * Appends a new line built from 'sequencePixmap', 'namesPixmap' and 'consensusPixmap' to 'multilinePixmap'
     * and return a new multiline pixmap.
     */
    QPixmap mergePixmaps(
        const QPixmap& multilinePixmap,
        const QPixmap& sequencesPixmap,
        const QPixmap& namesPixmap,
        const QPixmap& consensusPixmap);
};

class MSAImageExportToSvgTask : public MSAImageExportTask {
    Q_OBJECT
public:
    MSAImageExportToSvgTask(MaEditorWgt* ui,
                            const MSAImageExportSettings& msaSettings,
                            const ImageExportTaskSettings& settings);
    void run() override;
};

class MSAImageExportController : public ImageExportController {
    Q_OBJECT
public:
    MSAImageExportController(MaEditorWgt* ui);
    ~MSAImageExportController() override;

public slots:
    void sl_showSelectRegionDialog();
    void sl_regionTypeChanged(int newRegionIndex);

protected:
    void initSettingsWidget() final override;

    Task* getExportToBitmapTask(const ImageExportTaskSettings& settings) const override;
    Task* getExportToSvgTask(const ImageExportTaskSettings&) const override;

private slots:
    void sl_onFormatChanged(const DocumentFormatId&) override;

private:
    void flushUiStateToSettings() const;
    void checkRegionToExport();
    bool fitsInLimits() const;
    bool canExportToSvg() const;
    void updateSeqIdx() const;

    MaEditorWgt* ui = nullptr;
    Ui_MSAExportSettings* settingsUi;
    mutable MSAImageExportSettings msaSettings;
    DocumentFormatId format;
};

}  // namespace U2
