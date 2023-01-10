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
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ExportTasks.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaCollapseModel.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorSelection.h>

#include "ExportMSA2MSADialog.h"
#include "ExportUtils.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// ExportAlignmentViewItemsController

ExportAlignmentViewItemsController::ExportAlignmentViewItemsController(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void ExportAlignmentViewItemsController::initViewContext(GObjectView* v) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    SAFE_POINT(msaed != nullptr, "Invalid GObjectView", );
    MSAExportContext* mc = new MSAExportContext(msaed);
    addViewResource(msaed, mc);
}

void ExportAlignmentViewItemsController::buildStaticOrContextMenu(GObjectView* v, QMenu* m) {
    QList<QObject*> resources = viewResources.value(v);
    assert(resources.size() == 1);
    QObject* r = resources.first();
    MSAExportContext* mc = qobject_cast<MSAExportContext*>(r);
    assert(mc != nullptr);
    mc->buildMenu(m);
}

//////////////////////////////////////////////////////////////////////////
// MSA view context

MSAExportContext::MSAExportContext(MSAEditor* e)
    : editor(e) {
    translateMSAAction = new QAction(tr("Amino translation..."), this);
    translateMSAAction->setObjectName("amino_translation_of_alignment_rows");
    translateMSAAction->setEnabled(!e->isAlignmentEmpty());
    connect(e->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), translateMSAAction, SLOT(setDisabled(bool)));
    connect(translateMSAAction, SIGNAL(triggered()), SLOT(sl_exportNucleicMsaToAmino()));
}

void MSAExportContext::updateActions() {
    translateMSAAction->setEnabled(editor->getMaObject()->getAlphabet()->isNucleic() &&
                                   !editor->isAlignmentEmpty());
}

void MSAExportContext::buildMenu(QMenu* m) {
    QMenu* exportMenu = GUIUtils::findSubMenu(m, MSAE_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu is not found", );
    MultipleSequenceAlignmentObject* mObject = editor->getMaObject();
    if (mObject->getAlphabet()->isNucleic()) {
        exportMenu->addAction(translateMSAAction);
    }
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
        QList<int> maRowIndexes = editor->getCollapseModel()->getMaRowIndexesFromSelectionRects(selection.getRectList());
        rowIds = ma->getRowIdsByRowIndexes(maRowIndexes);
        SAFE_POINT(!rowIds.isEmpty(), "No rows to export!", );
    }

    bool convertUnknowToGaps = d->unknownAmino == ExportMSA2MSADialog::UnknownAmino::Gap;
    bool reverseComplement = d->translationFrame < 0;
    int baseOffset = qAbs(d->translationFrame) - 1;
    auto exportTask = ExportUtils::wrapExportTask(new ExportMSA2MSATask(ma,
                                                                        rowIds,
                                                                        columnRegion,
                                                                        d->file,
                                                                        translationTable,
                                                                        d->formatId,
                                                                        !d->includeGaps,
                                                                        convertUnknowToGaps,
                                                                        reverseComplement,
                                                                        baseOffset),
                                                  d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(exportTask);
}

}  // namespace U2
