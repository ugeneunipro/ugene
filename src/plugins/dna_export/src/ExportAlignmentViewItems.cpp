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

#include "ExportAlignmentViewItems.h"

#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExportSequencesTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ExportTasks.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorSelection.h>

#include "ExportMSA2MSADialog.h"
#include "ExportMSA2SequencesDialog.h"
#include "ExportUtils.h"
#include "dialogs/SaveSelectedSequenceFromMSADialogController.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// ExportAlignmentViewItemsController

ExportAlignmentViewItemsController::ExportAlignmentViewItemsController(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void ExportAlignmentViewItemsController::initViewContext(GObjectViewController* v) {
    auto msaEditor = qobject_cast<MSAEditor*>(v);
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView", );
    auto msaExportContext = new MSAExportContext(msaEditor);
    addViewResource(msaEditor, msaExportContext);
}

void ExportAlignmentViewItemsController::buildStaticOrContextMenu(GObjectViewController* v, QMenu* m) {
    QList<QObject*> resources = viewResources.value(v);
    assert(resources.size() == 1);
    QObject* r = resources.first();
    auto mc = qobject_cast<MSAExportContext*>(r);
    assert(mc != nullptr);
    mc->buildMenu(m);
}

//////////////////////////////////////////////////////////////////////////
// MSA view context

MSAExportContext::MSAExportContext(MSAEditor* e)
    : editor(e) {
    exportNucleicMsaToAminoAction = new QAction(tr("Export amino acid translated alignment..."), this);
    exportNucleicMsaToAminoAction->setObjectName("exportNucleicMsaToAminoAction");
    connect(exportNucleicMsaToAminoAction, &QAction::triggered, this, &MSAExportContext::sl_exportNucleicMsaToAmino);

    exportMsaToSequenceFileFormatAction = new QAction(tr("Export whole alignment to a sequence file format..."), this);
    exportMsaToSequenceFileFormatAction->setObjectName("exportMsaToSequenceFileFormatAction");
    connect(exportMsaToSequenceFileFormatAction, &QAction::triggered, [e] { ExportMSA2SequencesDialog::showDialogAndStartExportTask(e->getMaObject()); });

    exportSelectedMsaRowsToSeparateFilesAction = new QAction(tr("Export selected rows to separate sequence files..."), this);
    exportSelectedMsaRowsToSeparateFilesAction->setObjectName("exportSelectedMsaRowsToSeparateFilesAction");
    connect(exportSelectedMsaRowsToSeparateFilesAction, &QAction::triggered, this, &MSAExportContext::sl_exportSelectedMsaRowsToSeparateFiles);

    connect(e->getMaObject(), &MultipleSequenceAlignmentObject::si_alignmentChanged, this, [this] { updateActions(); });

    updateActions();
}

void MSAExportContext::updateActions() {
    exportNucleicMsaToAminoAction->setEnabled(editor->getMaObject()->getAlphabet()->isNucleic() && !editor->isAlignmentEmpty());
    exportMsaToSequenceFileFormatAction->setEnabled(!editor->isAlignmentEmpty());
    exportSelectedMsaRowsToSeparateFilesAction->setEnabled(!editor->isAlignmentEmpty());
}

void MSAExportContext::buildMenu(QMenu* m) {
    QMenu* exportMenu = GUIUtils::findSubMenu(m, MSAE_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu is not found", );
    MultipleSequenceAlignmentObject* mObject = editor->getMaObject();
    if (mObject->getAlphabet()->isNucleic()) {
        exportMenu->addAction(exportNucleicMsaToAminoAction);
    }
    exportMenu->addAction(exportSelectedMsaRowsToSeparateFilesAction);
    exportMenu->addAction(exportMsaToSequenceFileFormatAction);
}

void MSAExportContext::sl_exportSelectedMsaRowsToSeparateFiles() {
    auto parentWidget = AppContext::getMainWindow()->getQMainWindow();
    QString suggestedFileName = editor->getMaObject()->getGObjectName() + "_sequence";
    QObjectScopedPointer<SaveSelectedSequenceFromMSADialogController> d = new SaveSelectedSequenceFromMSADialogController(parentWidget, suggestedFileName);
    int rc = d->exec();
    CHECK(!d.isNull() && rc != QDialog::Rejected, );

    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(d->getFormat());
    SAFE_POINT(df != nullptr, "Unknown document format", );
    QString extension = df->getSupportedDocumentFileExtensions().first();

    QList<int> selectedMaRowIndexes = editor->getSelection().getSelectedRowIndexes();
    const MultipleSequenceAlignment& msa = editor->getMaObject()->getMsa();
    QSet<qint64> selectedMaRowIds = msa->getRowIdsByRowIndexes(selectedMaRowIndexes).toSet();
    auto exportTask = new ExportSequencesTask(msa,
                                              selectedMaRowIds,
                                              d->getTrimGapsFlag(),
                                              d->getAddToProjectFlag(),
                                              d->getUrl(),
                                              d->getFormat(),
                                              extension,
                                              d->getCustomFileName());
    AppContext::getTaskScheduler()->registerTopLevelTask(exportTask);
}

void MSAExportContext::sl_exportNucleicMsaToAmino() {
    MultipleSequenceAlignmentObject* maObject = editor->getMaObject();
    const MultipleSequenceAlignment& ma = maObject->getMultipleAlignment();
    SAFE_POINT(ma->getAlphabet()->isNucleic(), "Alignment alphabet is not nucleic", );

    GUrl msaUrl = maObject->getDocument()->getURL();
    QString defaultUrl = GUrlUtils::getNewLocalUrlByFormat(msaUrl, maObject->getGObjectName(), BaseDocumentFormats::CLUSTAL_ALN, "_transl");

    bool isWholeAlignmentMode = editor->getSelection().isEmpty();
    QObjectScopedPointer<ExportMSA2MSADialog> d = new ExportMSA2MSADialog(defaultUrl,
                                                                          BaseDocumentFormats::CLUSTAL_ALN,
                                                                          isWholeAlignmentMode,
                                                                          AppContext::getMainWindow()->getQMainWindow());
    int rc = d->exec();
    CHECK(!d.isNull() && rc != QDialog::Rejected, );

    DNATranslation* translationTable = AppContext::getDNATranslationRegistry()->lookupTranslation(d->translationTable);
    const MaEditorSelection& selection = editor->getSelection();
    U2Region columnRegion(0, editor->getAlignmentLen());
    QList<qint64> rowIds = ma->getRowsIds();
    if (!selection.isEmpty() && d->exportWholeAlignment) {
        columnRegion = selection.getColumnRegion();
        QList<int> maRowIndexes = editor->getSelection().getSelectedRowIndexes();
        rowIds = ma->getRowIdsByRowIndexes(maRowIndexes);
        SAFE_POINT(!rowIds.isEmpty(), "No rows to export!", );
    }

    bool convertUnknownToGaps = d->unknownAmino == ExportMSA2MSADialog::UnknownAmino::Gap;
    bool reverseComplement = d->translationFrame < 0;
    int baseOffset = qAbs(d->translationFrame) - 1;
    auto exportTask = ExportUtils::wrapExportTask(new ExportMSA2MSATask(ma,
                                                                        rowIds,
                                                                        columnRegion,
                                                                        d->file,
                                                                        translationTable,
                                                                        d->formatId,
                                                                        !d->includeGaps,
                                                                        convertUnknownToGaps,
                                                                        reverseComplement,
                                                                        baseOffset),
                                                  d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(exportTask);
}

}  // namespace U2
