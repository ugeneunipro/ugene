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

#include <QCheckBox>
#include <QSvgGenerator>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>

#include "MSAImageExportTask.h"
#include "ui_MSAExportSettings.h"
#include "ov_msa/MSASelectSubalignmentDialog.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/RowHeightController.h"

namespace U2 {

MSAImageExportTask::MSAImageExportTask(MaEditorWgt *ui,
                                       const MSAImageExportSettings &msaSettings,
                                       const ImageExportTaskSettings &settings)
    : ImageExportTask(settings),
      ui(ui),
      msaSettings(msaSettings)
{
    SAFE_POINT_EXT(ui != NULL, setError(tr("MSA Editor UI is NULL")), );
}

void MSAImageExportTask::paintSequencesNames(QPainter &painter) {
    CHECK(msaSettings.includeSeqNames, );
    MaEditorNameList *namesArea = ui->getEditorNameList();
    SAFE_POINT_EXT(ui->getEditor() != NULL, setError(tr("MSA Editor is NULL")), );
    namesArea->drawNames(painter, msaSettings.seqIdx);
}

void MSAImageExportTask::paintConsensus(QPainter &painter) {
    CHECK(msaSettings.includeConsensus || msaSettings.includeRuler, );
    MaEditorConsensusArea *consensusArea = ui->getConsensusArea();
    SAFE_POINT_EXT(consensusArea != NULL, setError(tr("MSA Consensus area is NULL")), );

    MaEditorConsensusAreaSettings consensusSettings = consensusArea->getDrawSettings();
    consensusSettings.visibleElements = MaEditorConsElements();
    if (msaSettings.includeConsensus) {
        consensusSettings.visibleElements |= MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_HISTOGRAM;
    }
    if (msaSettings.includeRuler) {
        consensusSettings.visibleElements |= MSAEditorConsElement_RULER;
    }

    consensusArea->drawContent(painter, msaSettings.seqIdx, msaSettings.region, consensusSettings);
}

void MSAImageExportTask::paintRuler(QPainter &painter) {
    CHECK(msaSettings.includeRuler, );
    MaEditorConsensusArea* consensusArea = ui->getConsensusArea();
    SAFE_POINT_EXT(consensusArea != NULL, setError(tr("MSA Consensus area is NULL")), );

    MaEditorConsensusAreaSettings consensusSettings = consensusArea->getDrawSettings();
    consensusSettings.visibleElements = MSAEditorConsElement_RULER;

    consensusArea->drawContent(painter, msaSettings.seqIdx, msaSettings.region, consensusSettings);
}

bool MSAImageExportTask::paintContent(QPainter &painter) {
    MaEditorSequenceArea* seqArea = ui->getSequenceArea();
    return seqArea->drawContent(painter, msaSettings.region, msaSettings.seqIdx, 0, 0);
}

MSAImageExportToBitmapTask::MSAImageExportToBitmapTask(MaEditorWgt *ui,
                                                       const MSAImageExportSettings& msaSettings,
                                                       const ImageExportTaskSettings &settings)
    : MSAImageExportTask(ui,
                         msaSettings,
                         settings) {
}

void MSAImageExportToBitmapTask::run() {
    SAFE_POINT_EXT(settings.isBitmapFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("MSAImageExportToBitmapTask")), );

    SAFE_POINT_EXT( ui->getEditor() != NULL, setError(L10N::nullPointerError("MSAEditor")), );
    MultipleAlignmentObject *mObj =  ui->getEditor()->getMaObject();
    SAFE_POINT_EXT( mObj != NULL, setError(L10N::nullPointerError("MultipleAlignmentObject")), );
    StateLock *lock = new StateLock();
    mObj->lockState(lock);

    bool exportAll = msaSettings.exportAll;

    bool ok = (exportAll && mObj->getLength() > 0 && mObj->getNumRows() > 0) || (!msaSettings.region.isEmpty() && !msaSettings.seqIdx.isEmpty());
    CHECK_OPERATION( ok, mObj->unlockState(lock));
    CHECK_EXT( ok, setError(tr("Nothing to export")), );

    if (exportAll) {
        msaSettings.region = U2Region(0, mObj->getLength());
        QList<int> seqIdx;
        for (int i = 0; i < mObj->getNumRows(); i++) {
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

    QPixmap sequencesPixmap(ui->getSequenceArea()->getCanvasSize(msaSettings.seqIdx, msaSettings.region));
    QPixmap namesPixmap = msaSettings.includeSeqNames ? QPixmap(ui->getEditorNameList()->getCanvasSize(msaSettings.seqIdx)) : QPixmap();
    QPixmap consensusPixmap = visibleConsensusElements ? QPixmap(ui->getConsensusArea()->getCanvasSize(msaSettings.region, visibleConsensusElements)) : QPixmap();

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

    ok = paintContent(sequencesPainter);
    CHECK_OPERATION(ok, mObj->unlockState(lock));
    CHECK_EXT(ok, setError(tr("Alignment is too big. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );

    paintSequencesNames(namesPainter);
    paintConsensus(consensusPainter);
    mObj->unlockState(lock);

    QPixmap pixmap = mergePixmaps(sequencesPixmap, namesPixmap, consensusPixmap);
    CHECK_EXT( !pixmap.isNull(),
               setError(tr("Alignment is too big. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
    CHECK_EXT( pixmap.save(settings.fileName, qPrintable(settings.format), settings.imageQuality),
               setError(tr("Cannot save the file. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

QPixmap MSAImageExportToBitmapTask::mergePixmaps(const QPixmap &sequencesPixmap,
                                                 const QPixmap &namesPixmap,
                                                 const QPixmap &consensusPixmap)
{
    CHECK( namesPixmap.width() + sequencesPixmap.width() < IMAGE_SIZE_LIMIT &&
           consensusPixmap.height() + + sequencesPixmap.height() < IMAGE_SIZE_LIMIT, QPixmap());
    QPixmap pixmap = QPixmap(namesPixmap.width() + sequencesPixmap.width(),
                             consensusPixmap.height() + sequencesPixmap.height());

    pixmap.fill(Qt::white);
    QPainter p(&pixmap);

    p.translate(namesPixmap.width(), 0);
    p.drawPixmap(consensusPixmap.rect(), consensusPixmap);
    p.translate(-namesPixmap.width(), consensusPixmap.height());
    p.drawPixmap(namesPixmap.rect(), namesPixmap);
    p.translate(namesPixmap.width(), 0);
    p.drawPixmap(sequencesPixmap.rect(), sequencesPixmap);
    p.end();

    return pixmap;
}

MSAImageExportToSvgTask::MSAImageExportToSvgTask(MaEditorWgt *ui,
                                                 const MSAImageExportSettings& msaSettings,
                                                 const ImageExportTaskSettings &settings)
    : MSAImageExportTask(ui,
                         msaSettings,
                         settings) {
}

void MSAImageExportToSvgTask::run() {
    SAFE_POINT_EXT(settings.isSVGFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("MSAImageExportToSvgTask")), );

    MaEditor* editor = ui->getEditor();
    SAFE_POINT_EXT( editor != NULL, setError(L10N::nullPointerError("MSAEditor")), );
    MultipleAlignmentObject *mObj =  editor->getMaObject();
    SAFE_POINT_EXT( mObj != NULL, setError(L10N::nullPointerError("MultipleAlignmentObject")), );

    StateLocker stateLocker(mObj);
    Q_UNUSED(stateLocker);

    int ok = msaSettings.exportAll || (!msaSettings.region.isEmpty() && !msaSettings.seqIdx.isEmpty());
    SAFE_POINT_EXT( ok, setError(tr("Nothing to export")), );

    QSvgGenerator generator;
    generator.setFileName(settings.fileName);

    MaEditorNameList* nameListArea = ui->getEditorNameList();
    SAFE_POINT_EXT(nameListArea != NULL, setError(L10N::nullPointerError("MSAEditorNameList")), );
    MaEditorConsensusArea* consArea = ui->getConsensusArea();
    SAFE_POINT_EXT(consArea != NULL, setError(L10N::nullPointerError("MSAEditorConsensusArea")), );

    MaEditorConsElements visibleConsensusElements;
    if (msaSettings.includeConsensus) {
        visibleConsensusElements |= MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_HISTOGRAM;
    }
    if (msaSettings.includeRuler) {
        visibleConsensusElements |= MSAEditorConsElement_RULER;
    }

    const int namesWidth = nameListArea->width();
    const int consensusHeight = consArea->getCanvasSize(msaSettings.region, visibleConsensusElements).height();

    const int width = msaSettings.includeSeqNames * namesWidth +
            editor->getColumnWidth() * (msaSettings.exportAll ? editor->getAlignmentLen() : msaSettings.region.length);
    const int height = msaSettings.includeConsensus * consensusHeight +
            (msaSettings.exportAll ? ui->getRowHeightController()->getTotalAlignmentHeight() : ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(
                    msaSettings.seqIdx));
    SAFE_POINT_EXT(qMax(width, height) < IMAGE_SIZE_LIMIT, setError(tr("The image size is too big.") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );

    generator.setSize(QSize(width, height));
    generator.setViewBox(QRect(0, 0, width, height));
    generator.setTitle(tr("SVG %1").arg(mObj->getGObjectName()));
    generator.setDescription(tr("SVG image of multiple alignment created by Unipro UGENE"));

    QPainter p;
    p.begin(&generator);

    if ((msaSettings.includeConsensus || msaSettings.includeRuler) && (msaSettings.includeSeqNames)) {
        // fill an empty space in top left corner with white color
        p.fillRect(QRect(0, 0, namesWidth, msaSettings.includeConsensus * consensusHeight), Qt::white);
    }
    p.translate(msaSettings.includeSeqNames * namesWidth, 0);
    paintConsensus(p);
    p.translate(-1 * msaSettings.includeSeqNames * namesWidth, msaSettings.includeConsensus * consensusHeight);
    paintSequencesNames(p);
    p.translate(msaSettings.includeSeqNames * namesWidth, 0);
    paintContent(p);
    p.end();
}

MSAImageExportController::MSAImageExportController(MaEditorWgt *ui)
    : ImageExportController( ExportImageFormatPolicy( EnableRasterFormats | SupportSvg) ),
      ui(ui)
{
    SAFE_POINT(ui != NULL, L10N::nullPointerError("MSAEditorUI"), );
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
        if (settingsUi->comboBox->currentIndex() != 1/*customIndex*/) {
            settingsUi->comboBox->setCurrentIndex( 1/*customIndex*/);
            msaSettings.exportAll = false;
        }
    } else {
        if (msaSettings.region.isEmpty()) {
            settingsUi->comboBox->setCurrentIndex( 0/*wholeAlIndex*/);
            msaSettings.exportAll = true;
        }
    }
    checkRegionToExport();
}

void MSAImageExportController::sl_regionChanged() {
    bool customRegionIsSelected = (settingsUi->comboBox->currentIndex() == 1);
    msaSettings.exportAll = !customRegionIsSelected;
    if (customRegionIsSelected && msaSettings.region.isEmpty()) {
        sl_showSelectRegionDialog();
    } else {
        checkRegionToExport();
    }
}

void MSAImageExportController::initSettingsWidget() {
    settingsUi = new Ui_MSAExportSettings;
    settingsWidget = new QWidget();
    settingsUi->setupUi(settingsWidget);

    connect(settingsUi->selectRegionButton, SIGNAL(clicked()), SLOT(sl_showSelectRegionDialog()));
    connect(settingsUi->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_regionChanged()));

    SAFE_POINT( ui->getSequenceArea() != NULL, tr("MSA sequence area is NULL"), );
    MaEditorSelection selection = ui->getSequenceArea()->getSelection();
    CHECK( !selection.isEmpty(), );
    msaSettings.region = U2Region( selection.x(), selection.width());
    msaSettings.seqIdx.clear();
    if (!ui->isCollapsibleMode()) {
        for (qint64 i = selection.y(); i < selection.height() + selection.y(); i++) {
            msaSettings.seqIdx.append( i );
        }
    } else {
        MaCollapseModel* model = ui->getCollapseModel();
        SAFE_POINT(model != NULL, tr("MSA Collapsible Model is NULL"), );
        for (qint64 i = selection.y(); i < selection.height() + selection.y(); i++) {
                msaSettings.seqIdx.append(model->getMaRowIndexByViewRowIndex(i));
        }
    }
}

Task* MSAImageExportController::getExportToBitmapTask(const ImageExportTaskSettings &settings) const {
    msaSettings.includeConsensus = settingsUi->exportConsensus->isChecked();
    msaSettings.includeRuler = settingsUi->exportRuler->isChecked();
    msaSettings.includeSeqNames = settingsUi->exportSeqNames->isChecked();
    updateSeqIdx();

    return new MSAImageExportToBitmapTask(ui, msaSettings, settings);
}

Task* MSAImageExportController::getExportToSvgTask(const ImageExportTaskSettings &settings) const {
    msaSettings.includeConsensus = settingsUi->exportConsensus->isChecked();
    msaSettings.includeRuler = settingsUi->exportRuler->isChecked();
    msaSettings.includeSeqNames = settingsUi->exportSeqNames->isChecked();
    updateSeqIdx();

    return new MSAImageExportToSvgTask(ui, msaSettings, settings);
}

void MSAImageExportController::sl_onFormatChanged(const QString& newFormat) {
    format = newFormat;
    checkRegionToExport();
}

void MSAImageExportController::checkRegionToExport() {
    bool exportToSvg = format.contains("svg", Qt::CaseInsensitive);
    bool isRegionOk = fitsInLimits();
    disableMessage = isRegionOk ? "" : tr("Warning: selected region is too big to be exported. You can try to zoom out the alignment or select another region.");
    if(isRegionOk && exportToSvg) {
        isRegionOk = canExportToSvg();
        disableMessage = isRegionOk ? "" : tr("Warning: selected region is too big to be exported. You can try to select another region.");
    }

    emit si_disableExport( !isRegionOk );
    emit si_showMessage( disableMessage );
}

namespace {
//400000 characters convert to 200 mb file in SVG format
const qint64 MaxSvgCharacters = 400000;
//SVG renderer can crash on regions large than 40000000
const qint64 MaxSvgImageSize = 40000000;
}

bool MSAImageExportController::fitsInLimits() const {
    MaEditor* editor = ui->getEditor();
    SAFE_POINT(editor != NULL, L10N::nullPointerError("MSAEditor"), false);
    qint64 imageWidth = (msaSettings.exportAll ? editor->getAlignmentLen() : msaSettings.region.length) * editor->getColumnWidth();
    qint64 imageHeight = msaSettings.exportAll ? ui->getRowHeightController()->getTotalAlignmentHeight() : ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(
            msaSettings.seqIdx);
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
    SAFE_POINT(editor != NULL, L10N::nullPointerError("MSAEditor"), false);
    int charactersNumber = msaSettings.exportAll ? (editor->getNumSequences() * editor->getAlignmentLen()) : (msaSettings.region.length * msaSettings.seqIdx.size());
    return charactersNumber < MaxSvgCharacters;
}

void MSAImageExportController::updateSeqIdx() const {
    CHECK(msaSettings.exportAll, );
    if (!ui->isCollapsibleMode()) {
        msaSettings.seqIdx.clear();
        for (qint64 i = 0; i < ui->getEditor()->getNumSequences(); i++) {
            msaSettings.seqIdx.append(i);
        }
        msaSettings.region = U2Region(0, ui->getEditor()->getAlignmentLen());
    }

    CHECK(ui->isCollapsibleMode(), );

    MaCollapseModel* model = ui->getCollapseModel();
    SAFE_POINT(model != NULL, tr("MSA Collapsible Model is NULL"), );
    msaSettings.seqIdx.clear();
    for (qint64 i = 0; i < ui->getEditor()->getNumSequences(); i++) {
        if (model->getViewRowIndexByMaRowIndex(i, true) != -1) {
            msaSettings.seqIdx.append(i);
        }
    }
}

} // namespace
