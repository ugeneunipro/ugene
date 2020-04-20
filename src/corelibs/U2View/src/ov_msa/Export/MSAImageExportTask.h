/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_IMAGE_EXPORT_TASK_H_
#define _U2_MSA_IMAGE_EXPORT_TASK_H_

#include <QPixmap>

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ImageExportTask.h>

#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MSAEditorSequenceArea.h>

class Ui_MSAExportSettings;

namespace U2 {

class MaEditorWgt;

class MSAImageExportSettings {
public:

    MSAImageExportSettings(bool exportAll = true,
                           bool includeSeqNames = false,
                           bool includeConsensus = false,
                           bool includeRuler = true)
        : exportAll(exportAll),
          includeSeqNames(includeSeqNames),
          includeConsensus(includeConsensus),
          includeRuler(includeRuler) {}

    MSAImageExportSettings(const U2Region &region,
                           const QList<int> &seqIdx,
                           bool includeSeqNames = false,
                           bool includeConsensus = false,
                           bool includeRuler = true)
        : exportAll(false),
          region(region),
          seqIdx(seqIdx),
          includeSeqNames(includeSeqNames),
          includeConsensus(includeConsensus),
          includeRuler(includeRuler) {}

    bool exportAll;
    U2Region region;
    QList<int> seqIdx;

    bool includeSeqNames;
    bool includeConsensus;
    bool includeRuler;
};

class MSAImageExportTask : public ImageExportTask {
    Q_OBJECT
public:
    MSAImageExportTask(MaEditorWgt *ui,
                       const MSAImageExportSettings &msaSettings,
                       const ImageExportTaskSettings &settings);

protected:
    void paintSequencesNames(QPainter &painter);
    void paintConsensus(QPainter &painter);
    void paintRuler(QPainter &painter);
    bool paintContent(QPainter &painter);

    MaEditorWgt* ui;
    MSAImageExportSettings  msaSettings;
};

class MSAImageExportToBitmapTask : public MSAImageExportTask {
    Q_OBJECT
public:
    MSAImageExportToBitmapTask(MaEditorWgt *ui,
                               const MSAImageExportSettings& msaSettings,
                               const ImageExportTaskSettings &settings);
    void run();

private:
    QPixmap mergePixmaps(const QPixmap &sequencesPixmap,
                         const QPixmap &namesPixmap,
                         const QPixmap &consensusPixmap);
};

class MSAImageExportToSvgTask : public MSAImageExportTask {
    Q_OBJECT
public:
    MSAImageExportToSvgTask(MaEditorWgt *ui,
                            const MSAImageExportSettings& msaSettings,
                            const ImageExportTaskSettings &settings);
    void run();
};

class MSAImageExportController : public ImageExportController {
    Q_OBJECT
public:
    MSAImageExportController(MaEditorWgt *ui);
    ~MSAImageExportController();

public slots:
    void sl_showSelectRegionDialog();
    void sl_regionChanged();

protected:
    void initSettingsWidget();

    Task* getExportToBitmapTask(const ImageExportTaskSettings &settings) const;
    Task* getExportToSvgTask(const ImageExportTaskSettings &) const;

private slots:
    void sl_onFormatChanged(const QString&);
private:
    void checkRegionToExport();
    bool fitsInLimits() const;
    bool canExportToSvg() const;
    void updateSeqIdx() const;

    MaEditorWgt* ui;
    Ui_MSAExportSettings    *settingsUi;
    mutable MSAImageExportSettings      msaSettings;
    QString format;
};

} // namespace

#endif // _U2_MSA_IMAGE_EXPORT_TASK_H_
