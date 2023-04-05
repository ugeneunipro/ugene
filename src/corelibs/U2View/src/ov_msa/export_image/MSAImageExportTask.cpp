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

#include "MSAImageExportTask.h"

#include <QCheckBox>
#include <QSvgGenerator>
#include <QThread>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorConsensusArea.h>
#include <U2View/MaEditorNameList.h>

#include "ov_msa/BaseWidthController.h"
#include "ov_msa/MSASelectSubalignmentDialog.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/RowHeightController.h"
#include "ui_MSAExportSettings.h"

namespace U2 {

MSAImageExportTask::MSAImageExportTask(MaEditorWgt* _ui,
                                       const MSAImageExportSettings& _msaSettings,
                                       const ImageExportTaskSettings& settings)
    : ImageExportTask(settings),
      ui(_ui),
      msaSettings(_msaSettings) {
    SAFE_POINT_EXT(ui != nullptr, setError(tr("MSA Editor UI is NULL")), );
}

void MSAImageExportTask::paintSequencesNames(QPainter& painter) {
    CHECK(msaSettings.includeSeqNames, );
    MaEditorNameList* namesArea = ui->getEditorNameList();
    SAFE_POINT_EXT(ui->getEditor() != nullptr, setError(tr("MSA Editor is NULL")), );
    namesArea->drawNames(painter, msaSettings.seqIdx);
}

void MSAImageExportTask::paintConsensusAndRuler(QPainter& painter, const U2Region& region) {
    CHECK(msaSettings.includeConsensus || msaSettings.includeRuler, );
    MaEditorConsensusArea* consensusArea = ui->getConsensusArea();
    SAFE_POINT_EXT(consensusArea != nullptr, setError(tr("MSA Consensus area is NULL")), );

    MaEditorConsensusAreaSettings consensusSettings = consensusArea->getDrawSettings();
    consensusSettings.visibleElements = MaEditorConsElements();
    if (msaSettings.includeConsensus) {
        consensusSettings.visibleElements |= MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_HISTOGRAM;
    }
    if (msaSettings.includeRuler) {
        consensusSettings.visibleElements |= MSAEditorConsElement_RULER;
    }

    consensusArea->drawContent(painter, msaSettings.seqIdx, region, consensusSettings);
}

bool MSAImageExportTask::paintSequenceArea(QPainter& painter, const U2Region& region) {
    MaEditorSequenceArea* seqArea = ui->getSequenceArea();
    return seqArea->drawContent(painter, region, msaSettings.seqIdx, 0, 0);
}

MSAImageExportToBitmapTask::MSAImageExportToBitmapTask(MaEditorWgt* ui,
                                                       const MSAImageExportSettings& msaSettings,
                                                       const ImageExportTaskSettings& settings)
    : MSAImageExportTask(ui,
                         msaSettings,
                         settings) {
}

void MSAImageExportToBitmapTask::run() {
    SAFE_POINT_EXT(settings.isBitmapFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("MSAImageExportToBitmapTask")), );
    SAFE_POINT_EXT(ui->getEditor() != nullptr, setError(L10N::nullPointerError("MSAEditor")), );
    SAFE_POINT_EXT(QCoreApplication::instance()->thread() == QThread::currentThread(), setError("MSAImageExportToBitmapTask: not a main thread!"), );

    MultipleAlignmentObject* mObj = ui->getEditor()->getMaObject();
    SAFE_POINT_EXT(mObj != nullptr, setError(L10N::nullPointerError("MultipleAlignmentObject")), );

    bool exportAll = msaSettings.exportAll;
    bool ok = (exportAll && mObj->getLength() > 0 && mObj->getRowCount() > 0) || (!msaSettings.region.isEmpty() && !msaSettings.seqIdx.isEmpty());
    CHECK_EXT(ok, setError(tr("Nothing to export")), );

    if (exportAll) {
        msaSettings.region = U2Region(0, mObj->getLength());
        QList<int> seqIdx;
        for (int i = 0; i < mObj->getRowCount(); i++) {
            seqIdx << i;
        }
        msaSettings.seqIdx = seqIdx;
    }

    MaEditorConsElements visibleConsensusElements;
    if (msaSettings.includeConsensus) {
        visibleConsensusElements |= MSAEditorConsElement_HISTOGRAM | MSAEditorConsElement_CONSENSUS_TEXT;
    }
    if (msaSettings.includeRuler) {
        visibleConsensusElements |= MSAEditorConsElement_RULER;
    }

    int basesPerLine = msaSettings.basesPerLine > 0 ? msaSettings.basesPerLine : (int)msaSettings.region.length;
    QList<U2Region> multilineRegions = U2Region::split(msaSettings.region, basesPerLine);
    QPixmap multilinePixmap;
    for (const auto& region : qAsConst(multilineRegions)) {
        QPixmap sequencesPixmap(ui->getSequenceArea()->getCanvasSize(msaSettings.seqIdx, region));
        QPixmap namesPixmap = msaSettings.includeSeqNames ? QPixmap(ui->getEditorNameList()->getCanvasSize(msaSettings.seqIdx)) : QPixmap();
        QPixmap consensusPixmap = visibleConsensusElements ? QPixmap(ui->getConsensusArea()->getCanvasSize(region, visibleConsensusElements)) : QPixmap();

        sequencesPixmap.fill(Qt::white);
        namesPixmap.fill(Qt::white);
        consensusPixmap.fill(Qt::white);

        QPainter sequencesPainter(&sequencesPixmap);
        QPainter namesPainter;
        if (msaSettings.includeSeqNames) {
            namesPainter.begin(&namesPixmap);
        }
        QPainter consensusPainter;
        if (visibleConsensusElements) {
            consensusPainter.begin(&consensusPixmap);
        }

        ok = paintSequenceArea(sequencesPainter, region);
        CHECK_EXT(ok, setError(tr("Alignment is too big. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );

        paintSequencesNames(namesPainter);
        paintConsensusAndRuler(consensusPainter, region);

        multilinePixmap = mergePixmaps(multilinePixmap, sequencesPixmap, namesPixmap, consensusPixmap);
    }
    CHECK_EXT(!multilinePixmap.isNull(),
              setError(tr("Alignment is too big. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
    CHECK_EXT(multilinePixmap.save(settings.fileName, qPrintable(settings.format), settings.imageQuality),
              setError(tr("Cannot save the file. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

/** Gap between pages in multiline mode in pixels. */
static const int multilineVerticalSpacing = 30;

QPixmap MSAImageExportToBitmapTask::mergePixmaps(const QPixmap& multilinePixmap,
                                                 const QPixmap& sequencesPixmap,
                                                 const QPixmap& namesPixmap,
                                                 const QPixmap& consensusPixmap) {
    int newPixmapWidth = qMax(namesPixmap.width() + sequencesPixmap.width(), multilinePixmap.width());
    CHECK(newPixmapWidth < IMAGE_SIZE_LIMIT, multilinePixmap);  // Too wide.

    int verticalSpacing = multilinePixmap.height() == 0 ? 0 : multilineVerticalSpacing;
    int newPixmapHeight = multilinePixmap.height() + verticalSpacing + consensusPixmap.height() + sequencesPixmap.height();  // Too high.
    CHECK(newPixmapHeight < IMAGE_SIZE_LIMIT, multilinePixmap);

    QPixmap newPixmap = QPixmap(newPixmapWidth, newPixmapHeight);

    newPixmap.fill(Qt::white);
    QPainter p(&newPixmap);
    p.drawPixmap(multilinePixmap.rect(), multilinePixmap);
    p.translate(0, multilinePixmap.height() + verticalSpacing);

    p.translate(namesPixmap.width(), 0);
    p.drawPixmap(consensusPixmap.rect(), consensusPixmap);
    p.translate(-namesPixmap.width(), consensusPixmap.height());
    p.drawPixmap(namesPixmap.rect(), namesPixmap);
    p.translate(namesPixmap.width(), 0);
    p.drawPixmap(sequencesPixmap.rect(), sequencesPixmap);
    p.end();

    return newPixmap;
}

MSAImageExportToSvgTask::MSAImageExportToSvgTask(MaEditorWgt* ui,
                                                 const MSAImageExportSettings& msaSettings,
                                                 const ImageExportTaskSettings& settings)
    : MSAImageExportTask(ui, msaSettings, settings) {
}

void MSAImageExportToSvgTask::run() {
    SAFE_POINT_EXT(settings.isSVGFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("MSAImageExportToSvgTask")), );

    SAFE_POINT_EXT(QCoreApplication::instance()->thread() == QThread::currentThread(), setError("MSAImageExportToBitmapTask: not a main thread!"), );
    MaEditor* editor = ui->getEditor();
    SAFE_POINT_EXT(editor != nullptr, setError(L10N::nullPointerError("MSAEditor")), );
    MultipleAlignmentObject* mObj = editor->getMaObject();
    SAFE_POINT_EXT(mObj != nullptr, setError(L10N::nullPointerError("MultipleAlignmentObject")), );

    int ok = msaSettings.exportAll || (!msaSettings.region.isEmpty() && !msaSettings.seqIdx.isEmpty());
    SAFE_POINT_EXT(ok, setError(tr("Nothing to export")), );

    // Repeating logic of ExportToBitmap task. TODO: use common code for both tasks.
    if (msaSettings.exportAll) {
        msaSettings.region = U2Region(0, mObj->getLength());
    }

    QSvgGenerator generator;
    generator.setFileName(settings.fileName);

    MaEditorNameList* nameListArea = ui->getEditorNameList();
    SAFE_POINT_EXT(nameListArea != nullptr, setError(L10N::nullPointerError("MSAEditorNameList")), );
    MaEditorConsensusArea* consArea = ui->getConsensusArea();
    SAFE_POINT_EXT(consArea != nullptr, setError(L10N::nullPointerError("MSAEditorConsensusArea")), );

    MaEditorConsElements visibleConsensusElements;
    if (msaSettings.includeConsensus) {
        visibleConsensusElements |= MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_HISTOGRAM;
    }
    if (msaSettings.includeRuler) {
        visibleConsensusElements |= MSAEditorConsElement_RULER;
    }

    int namesWidth = nameListArea->width();
    int consensusHeight = consArea->getCanvasSize(msaSettings.region, visibleConsensusElements).height();

    int basesPerLine = msaSettings.basesPerLine > 0 ? msaSettings.basesPerLine : (int)msaSettings.region.length;
    QList<U2Region> multilineRegions = U2Region::split(msaSettings.region, basesPerLine);

    qint64 width = (msaSettings.includeSeqNames ? namesWidth : 0) + editor->getColumnWidth() * basesPerLine;
    int lineHeight = (msaSettings.includeConsensus ? consensusHeight : 0) +
                     (msaSettings.exportAll ? ui->getRowHeightController()->getTotalAlignmentHeight() : ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(msaSettings.seqIdx));
    qint64 height = lineHeight * multilineRegions.length() + multilineVerticalSpacing * (multilineRegions.length() - 1);
    SAFE_POINT_EXT(qMax(width, height) < IMAGE_SIZE_LIMIT, setError(tr("The image size is too big.") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );

    generator.setSize(QSize((int)width, (int)height));
    generator.setViewBox(QRect(0, 0, (int)width, (int)height));
    generator.setTitle(tr("SVG %1").arg(mObj->getGObjectName()));
    generator.setDescription(tr("SVG image of multiple alignment created by Unipro UGENE"));

    QPainter p;
    p.begin(&generator);
    p.fillRect(QRect(0, 0, (int)width, (int)height), Qt::white);
    for (const auto& region : qAsConst(multilineRegions)) {
        // Draw consensus & ruler.
        if (msaSettings.includeConsensus || msaSettings.includeRuler) {
            int consensusOffsetX = msaSettings.includeSeqNames ? namesWidth : 0;
            p.translate(consensusOffsetX, 0);
            paintConsensusAndRuler(p, region);
            p.translate(-consensusOffsetX, consensusHeight);
        }
        // Draw names.
        if (msaSettings.includeSeqNames) {
            paintSequencesNames(p);
        }
        // Draw sequences.
        if (msaSettings.includeSeqNames) {
            p.translate(namesWidth, 0);
        }
        paintSequenceArea(p, region);
        if (msaSettings.includeSeqNames) {
            p.translate(-namesWidth, 0);
        }
        // Shift to the next page vertically.
        p.translate(0, int(lineHeight + multilineVerticalSpacing));
    }
    p.end();
}

MSAImageExportController::MSAImageExportController(MaEditorWgt* ui)
    : ImageExportController(ExportImageFormatPolicy(EnableRasterFormats | SupportSvg)),
      ui(ui) {
    SAFE_POINT(ui != nullptr, L10N::nullPointerError("MSAEditorUI"), );
    shortDescription = tr("Alignment");
    initSettingsWidget();
    checkRegionToExport();
}

MSAImageExportController::~MSAImageExportController() {
    delete settingsUi;
}

void MSAImageExportController::sl_showSelectRegionDialog() {
    QObjectScopedPointer<SelectSubalignmentDialog> dialog = new SelectSubalignmentDialog(ui->getEditor(), msaSettings.region, msaSettings.seqIdx);
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (dialog->result() == QDialog::Accepted) {
        msaSettings.region = dialog->getRegion();
        msaSettings.seqIdx = dialog->getSelectedSeqIndexes();
        if (settingsUi->comboBox->currentIndex() != 1 /*customIndex*/) {
            settingsUi->comboBox->setCurrentIndex(1 /*customIndex*/);
            msaSettings.exportAll = false;
        }
    } else {
        if (msaSettings.region.isEmpty()) {
            settingsUi->comboBox->setCurrentIndex(0 /*wholeAlIndex*/);
            msaSettings.exportAll = true;
        }
    }
    checkRegionToExport();
}

void MSAImageExportController::sl_regionTypeChanged(int regionType) {
    bool isCustomRegion = regionType == 1;
    msaSettings.exportAll = !isCustomRegion;
    if (isCustomRegion && msaSettings.region.isEmpty()) {
        sl_showSelectRegionDialog();
    } else {
        checkRegionToExport();
    }
}

void MSAImageExportController::initSettingsWidget() {
    settingsUi = new Ui_MSAExportSettings();
    settingsWidget = new QWidget();
    settingsUi->setupUi(settingsWidget);

    connect(settingsUi->selectRegionButton, &QPushButton::clicked, this, &MSAImageExportController::sl_showSelectRegionDialog);
    connect(settingsUi->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_regionTypeChanged(int)));  // Can't use function pointer until QT_DISABLE_DEPRECATED_BEFORE < 5.15.

    settingsUi->multilineModeCheckbox->setChecked(ui->getEditor()->isMultilineMode());
    settingsUi->multilineWidthSpinbox->setValue(ui->getSequenceArea()->getNumVisibleBases());

    const QList<QRect>& selectedRects = ui->getEditor()->getSelection().getRectList();
    if (!selectedRects.isEmpty()) {
        msaSettings.region = U2Region(selectedRects[0].x(), selectedRects[0].width());
        MaCollapseModel* model = ui->getEditor()->getCollapseModel();
        for (const QRect& selectedRect : qAsConst(selectedRects)) {
            for (int viewRowIndex = selectedRect.y(); viewRowIndex <= selectedRect.bottom(); viewRowIndex++) {
                int maRowIndex = model->getMaRowIndexByViewRowIndex(viewRowIndex);
                msaSettings.seqIdx.append(maRowIndex);
            }
        }
    }
}

void MSAImageExportController::flushUiStateToSettings() const {
    msaSettings.includeConsensus = settingsUi->exportConsensus->isChecked();
    msaSettings.includeRuler = settingsUi->exportRuler->isChecked();
    msaSettings.includeSeqNames = settingsUi->exportSeqNames->isChecked();
    msaSettings.basesPerLine = settingsUi->multilineModeCheckbox->isChecked() ? settingsUi->multilineWidthSpinbox->value() : 0;
    updateSeqIdx();
}

Task* MSAImageExportController::getExportToBitmapTask(const ImageExportTaskSettings& settings) const {
    flushUiStateToSettings();
    return new MSAImageExportToBitmapTask(ui, msaSettings, settings);
}

Task* MSAImageExportController::getExportToSvgTask(const ImageExportTaskSettings& settings) const {
    flushUiStateToSettings();
    updateSeqIdx();

    return new MSAImageExportToSvgTask(ui, msaSettings, settings);
}

void MSAImageExportController::sl_onFormatChanged(const DocumentFormatId& newFormat) {
    format = newFormat;
    checkRegionToExport();
}

void MSAImageExportController::checkRegionToExport() {
    bool exportToSvg = format.contains("svg", Qt::CaseInsensitive);
    bool isRegionOk = fitsInLimits();
    disableMessage = isRegionOk ? "" : tr("Warning: selected region is too big to be exported. You can try to zoom out the alignment or select another region.");
    if (isRegionOk && exportToSvg) {
        isRegionOk = canExportToSvg();
        disableMessage = isRegionOk ? "" : tr("Warning: selected region is too big to be exported. You can try to select another region.");
    }

    emit si_disableExport(!isRegionOk);
    emit si_showMessage(disableMessage);
}

// 400000 characters convert to 200 mb file in SVG format.
static constexpr qint64 MaxSvgCharacters = 400'000;
// SVG renderer can crash on regions larger than 40mb.
static constexpr qint64 MaxSvgImageSize = 40'000'000;

bool MSAImageExportController::fitsInLimits() const {
    MaEditor* editor = ui->getEditor();
    SAFE_POINT(editor != nullptr, L10N::nullPointerError("MSAEditor"), false);
    qint64 imageWidth = (msaSettings.exportAll ? editor->getAlignmentLen() : msaSettings.region.length) * editor->getColumnWidth();
    qint64 imageHeight = msaSettings.exportAll ? ui->getRowHeightController()->getTotalAlignmentHeight() : ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(msaSettings.seqIdx);
    if (imageWidth > IMAGE_SIZE_LIMIT || imageHeight > IMAGE_SIZE_LIMIT) {
        return false;
    }
    if (format.contains("svg", Qt::CaseInsensitive) && imageWidth * imageHeight > MaxSvgImageSize) {
        return false;
    }
    return true;
}

bool MSAImageExportController::canExportToSvg() const {
    MaEditor* editor = ui->getEditor();
    SAFE_POINT(editor != nullptr, L10N::nullPointerError("MSAEditor"), false);
    qint64 charactersNumber = msaSettings.exportAll ? (editor->getNumSequences() * editor->getAlignmentLen()) : (msaSettings.region.length * msaSettings.seqIdx.size());
    return charactersNumber < MaxSvgCharacters;
}

void MSAImageExportController::updateSeqIdx() const {
    CHECK(msaSettings.exportAll, );
    MaCollapseModel* model = ui->getEditor()->getCollapseModel();
    msaSettings.seqIdx.clear();
    for (int i = 0; i < ui->getEditor()->getNumSequences(); i++) {
        if (model->getViewRowIndexByMaRowIndex(i, true) != -1) {
            msaSettings.seqIdx.append(i);
        }
    }
}

}  // namespace U2
