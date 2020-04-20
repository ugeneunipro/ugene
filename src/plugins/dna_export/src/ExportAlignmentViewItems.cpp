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

#include <QDir>
#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>
//#include <U2View/ma

#include "ExportAlignmentViewItems.h"
#include "ExportMSA2MSADialog.h"
#include "ExportTasks.h"
#include "ExportUtils.h"

namespace U2 {


//////////////////////////////////////////////////////////////////////////
// ExportAlignmentViewItemsController

ExportAlignmentViewItemsController::ExportAlignmentViewItemsController(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID)
{
}


void ExportAlignmentViewItemsController::initViewContext(GObjectView* v) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    SAFE_POINT(msaed != NULL, "Invalid GObjectView", );
    MSAExportContext* mc= new MSAExportContext(msaed);
    addViewResource(msaed, mc);
}


void ExportAlignmentViewItemsController::buildMenu(GObjectView* v, QMenu* m) {
    QList<QObject*> resources = viewResources.value(v);
    assert(resources.size() == 1);
    QObject* r = resources.first();
    MSAExportContext* mc = qobject_cast<MSAExportContext*>(r);
    assert(mc!=NULL);
    mc->buildMenu(m);
}


//////////////////////////////////////////////////////////////////////////
// MSA view context

MSAExportContext::MSAExportContext(MSAEditor* e) : editor(e) {
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
    SAFE_POINT(exportMenu != NULL, "exportMenu", );
    MultipleSequenceAlignmentObject* mObject = editor->getMaObject();
    if (mObject->getAlphabet()->isNucleic()) {
        exportMenu->addAction(translateMSAAction);
    }
}

void MSAExportContext::sl_exportNucleicMsaToAmino() {
    const MultipleSequenceAlignment ma = editor->getMaObject()->getMultipleAlignment();
    assert(ma->getAlphabet()->isNucleic());

    GUrl msaUrl = editor->getMaObject()->getDocument()->getURL();
    QString defaultUrl = GUrlUtils::getNewLocalUrlByFormat(msaUrl, editor->getMaObject()->getGObjectName(), BaseDocumentFormats::CLUSTAL_ALN, "_transl");

    QObjectScopedPointer<ExportMSA2MSADialog> d = new ExportMSA2MSADialog(defaultUrl, BaseDocumentFormats::CLUSTAL_ALN, editor->getSelectionRect().height() < 1, AppContext::getMainWindow()->getQMainWindow());
    d->setWindowTitle(tr("Export Amino Translation"));
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc == QDialog::Rejected) {
        return;
    }

    QList<DNATranslation*> trans;
    trans << AppContext::getDNATranslationRegistry()->lookupTranslation(d->translationTable);

    int offset = d->exportWholeAlignment ? 0 : editor->getSelectionRect().top();
    int len = d->exportWholeAlignment ? ma->getNumRows() : editor->getSelectionRect().height();

    Task* t = ExportUtils::wrapExportTask(new ExportMSA2MSATask(ma, offset, len, d->file, trans, d->formatId), d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

} //namespace
