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

#include "ExportSequenceViewItems.h"

#include <QDir>
#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DBXRefRegistry.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/MultiTask.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ExportTasks.h>

#include <U2Gui/ExportAnnotations2CSVTask.h>
#include <U2Gui/ExportAnnotationsDialog.h>
#include <U2Gui/ExportObjectUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OpenViewTask.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "ExportBlastResultDialog.h"
#include "ExportSelectedSeqRegionsTask.h"
#include "ExportSequences2MSADialog.h"
#include "ExportSequencesDialog.h"
#include "ExportUtils.h"
#include "GetSequenceByIdDialog.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// ExportSequenceViewItemsController

ExportSequenceViewItemsController::ExportSequenceViewItemsController(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID),
      av(nullptr) {
}

void ExportSequenceViewItemsController::initViewContext(GObjectViewController* v) {
    av = qobject_cast<AnnotatedDNAView*>(v);
    auto vc = new ADVExportContext(av);
    addViewResource(av, vc);
}

void ExportSequenceViewItemsController::buildStaticOrContextMenu(GObjectViewController* v, QMenu* m) {
    QList<QObject*> resources = viewResources.value(v);
    assert(resources.size() == 1);
    QObject* r = resources.first();
    ADVExportContext* vc = qobject_cast<ADVExportContext*>(r);
    assert(vc != nullptr);
    vc->buildMenu(m);
}

void ExportSequenceViewItemsController::init() {
    GObjectViewWindowContext::init();
    if (!viewResources.value(av).isEmpty()) {
        QMenu* actions = AppContext::getMainWindow()->getTopLevelMenu(MWMENU_ACTIONS);
        SAFE_POINT(actions != nullptr, "Actions menu not found.", );
        actions->clear();
        AppContext::getMainWindow()->getMDIManager()->getActiveWindow()->setupViewMenu(actions);
    }
}

//////////////////////////////////////////////////////////////////////////
// ADV view context

// TODO: define global BLAST text constants in CoreLibs
#define BLAST_ANNOTATION_NAME "blast result"

ADVExportContext::ADVExportContext(AnnotatedDNAView* v)
    : view(v) {
    sequence2SequenceAction = new QAction(tr("Export selected sequence region..."), this);
    sequence2SequenceAction->setObjectName("action_export_selected_sequence_region");
    connect(sequence2SequenceAction, SIGNAL(triggered()), SLOT(sl_saveSelectedSequences()));

    annotations2SequenceAction = new QAction(tr("Export sequence of selected annotations..."), this);
    annotations2SequenceAction->setObjectName("action_export_sequence_of_selected_annotations");
    connect(annotations2SequenceAction, SIGNAL(triggered()), SLOT(sl_saveSelectedAnnotationsSequence()));

    annotations2CSVAction = new QAction(tr("Export annotations..."), this);
    annotations2CSVAction->setObjectName(ACTION_EXPORT_ANNOTATIONS);
    connect(annotations2CSVAction, SIGNAL(triggered()), SLOT(sl_saveSelectedAnnotations()));

    annotationsToAlignmentAction = new QAction(QIcon(":core/images/msa.png"), tr("Align selected annotations..."), this);
    annotationsToAlignmentAction->setObjectName("Align selected annotations");
    connect(annotationsToAlignmentAction, SIGNAL(triggered()), SLOT(sl_saveSelectedAnnotationsToAlignment()));

    annotationsToAlignmentWithTranslatedAction = new QAction(QIcon(":core/images/msa.png"), tr("Align selected annotations (amino acids)..."), this);
    annotationsToAlignmentWithTranslatedAction->setObjectName("Align selected annotations (amino acids)...");
    connect(annotationsToAlignmentWithTranslatedAction, SIGNAL(triggered()), SLOT(sl_saveSelectedAnnotationsToAlignmentWithTranslation()));

    sequenceToAlignmentAction = new QAction(QIcon(":core/images/msa.png"), tr("Align selected sequence regions..."), this);
    sequenceToAlignmentAction->setObjectName("action_align_selected_sequence_regions");
    connect(sequenceToAlignmentAction, SIGNAL(triggered()), SLOT(sl_saveSelectedSequenceToAlignment()));

    sequenceToAlignmentWithTranslationAction = new QAction(QIcon(":core/images/msa.png"), tr("Align selected sequence regions (amino acids)..."), this);
    sequenceToAlignmentWithTranslationAction->setObjectName("Align selected sequence regions (amino acids)");
    connect(sequenceToAlignmentWithTranslationAction, SIGNAL(triggered()), SLOT(sl_saveSelectedSequenceToAlignmentWithTranslation()));

    sequenceById = new QAction(tr("Export sequences by 'id'"), this);
    connect(sequenceById, SIGNAL(triggered()), SLOT(sl_getSequenceById()));
    sequenceByAccession = new QAction(tr("Export sequences by 'accession'"), this);
    connect(sequenceByAccession, SIGNAL(triggered()), SLOT(sl_getSequenceByAccession()));
    sequenceByDBXref = new QAction(tr("Export sequences by 'db_xref'"), this);
    connect(sequenceByDBXref, SIGNAL(triggered()), SLOT(sl_getSequenceByDBXref()));

    blastResultToAlignmentAction = new QAction(tr("Export BLAST result to alignment"), this);
    blastResultToAlignmentAction->setObjectName("export_BLAST_result_to_alignment");
    connect(blastResultToAlignmentAction, SIGNAL(triggered()), SLOT(sl_exportBlastResultToAlignment()));

    connect(view->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)),
            SLOT(updateActions()));

    connect(view->getAnnotationsGroupSelection(),
            SIGNAL(si_selectionChanged(AnnotationGroupSelection*, const QList<AnnotationGroup*>&, const QList<AnnotationGroup*>&)),
            SLOT(updateActions()));

    connect(view, SIGNAL(si_sequenceAdded(ADVSequenceObjectContext*)), SLOT(sl_onSequenceContextAdded(ADVSequenceObjectContext*)));
    connect(view, SIGNAL(si_sequenceRemoved(ADVSequenceObjectContext*)), SLOT(sl_onSequenceContextRemoved(ADVSequenceObjectContext*)));
    foreach (ADVSequenceObjectContext* sCtx, view->getSequenceContexts()) {
        sl_onSequenceContextAdded(sCtx);
    }
}

void ADVExportContext::sl_onSequenceContextAdded(ADVSequenceObjectContext* c) {
    connect(c->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(updateActions()));

    updateActions();
}

void ADVExportContext::sl_onSequenceContextRemoved(ADVSequenceObjectContext* c) {
    c->getSequenceSelection()->disconnect(this);
    updateActions();
}

static bool allNucleic(const QList<ADVSequenceObjectContext*>& seqs) {
    foreach (const ADVSequenceObjectContext* s, seqs) {
        if (!s->getAlphabet()->isNucleic()) {
            return false;
        }
    }
    return true;
}

void ADVExportContext::updateActions() {
    bool hasSelectedAnnotations = !view->getAnnotationsSelection()->isEmpty();
    bool hasSelectedGroups = !view->getAnnotationsGroupSelection()->isEmpty();
    int nSequenceSelections = 0;
    foreach (ADVSequenceObjectContext* c, view->getSequenceContexts()) {
        nSequenceSelections += c->getSequenceSelection()->getSelectedRegions().count();
    }

    sequence2SequenceAction->setEnabled(nSequenceSelections >= 1);
    annotations2SequenceAction->setEnabled(hasSelectedAnnotations);
    annotations2CSVAction->setEnabled(hasSelectedAnnotations || hasSelectedGroups);

    bool _allNucleic = allNucleic(view->getSequenceContexts());

    bool hasMultipleAnnotationsSelected = view->getAnnotationsSelection()->getAnnotations().size() > 1;
    annotationsToAlignmentAction->setEnabled(hasMultipleAnnotationsSelected);
    annotationsToAlignmentWithTranslatedAction->setEnabled(hasMultipleAnnotationsSelected && _allNucleic);

    bool hasMultiSequenceSelection = nSequenceSelections > 1;
    sequenceToAlignmentAction->setEnabled(hasMultiSequenceSelection);
    sequenceToAlignmentWithTranslationAction->setEnabled(hasMultiSequenceSelection && _allNucleic);
}

void ADVExportContext::buildMenu(QMenu* m) {
    QMenu* alignMenu = GUIUtils::findSubMenu(m, ADV_MENU_ALIGN);
    SAFE_POINT(alignMenu != nullptr, "alignMenu", );
    alignMenu->addAction(sequenceToAlignmentAction);
    alignMenu->addAction(sequenceToAlignmentWithTranslationAction);
    alignMenu->addAction(annotationsToAlignmentAction);
    alignMenu->addAction(annotationsToAlignmentWithTranslatedAction);

    QMenu* exportMenu = GUIUtils::findSubMenu(m, ADV_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu", );
    exportMenu->addAction(sequence2SequenceAction);
    exportMenu->addAction(annotations2SequenceAction);
    exportMenu->addAction(annotations2CSVAction);

    bool isShowId = false;
    bool isShowAccession = false;
    bool isShowDBXref = false;
    bool isBlastResult = false;

    QString name;
    if (!view->getAnnotationsSelection()->getAnnotations().isEmpty()) {
        name = view->getAnnotationsSelection()->getAnnotations().first()->getName();
    }
    foreach (const Annotation* annotation, view->getAnnotationsSelection()->getAnnotations()) {
        if (name != annotation->getName()) {
            name = "";
        }

        if (!isShowId && !annotation->findFirstQualifierValue("id").isEmpty()) {
            isShowId = true;
        } else if (!isShowAccession && !annotation->findFirstQualifierValue("accession").isEmpty()) {
            isShowAccession = true;
        } else if (!isShowDBXref && !annotation->findFirstQualifierValue("db_xref").isEmpty()) {
            isShowDBXref = true;
        }

        isBlastResult = name == BLAST_ANNOTATION_NAME;
    }

    if (isShowId || isShowAccession || isShowDBXref) {
        name = name.isEmpty() ? "" : tr("from '") + name + "'";
        auto fetchMenu = new QMenu(tr("Fetch sequences from remote database"));
        m->insertMenu(exportMenu->menuAction(), fetchMenu);
        if (isShowId) {
            sequenceById->setText(tr("Fetch sequences by 'id' %1").arg(name));
            fetchMenu->addAction(sequenceById);
        }
        if (isShowAccession) {
            sequenceByAccession->setText(tr("Fetch sequences by 'accession' %1").arg(name));
            fetchMenu->addAction(sequenceByAccession);
        }
        if (isShowDBXref) {
            sequenceByDBXref->setText(tr("Fetch sequences by 'db_xref' %1").arg(name));
            fetchMenu->addAction(sequenceByDBXref);
        }
    }
    if (isBlastResult) {
        exportMenu->addAction(blastResultToAlignmentAction);
    }
}

void ADVExportContext::sl_saveSelectedAnnotationsSequence() {
    AnnotationSelection* as = view->getAnnotationsSelection();
    AnnotationGroupSelection* ags = view->getAnnotationsGroupSelection();

    QList<Annotation*> annotations = as->getAnnotations();
    const QList<AnnotationGroup*> groups = ags->getSelection();
    foreach (AnnotationGroup* g, groups) {
        g->findAllAnnotationsInGroupSubTree(annotations);
    }

    if (annotations.isEmpty()) {
        QMessageBox::warning(view->getWidget(), L10N::warningTitle(), tr("No annotations selected!"));
        return;
    }

    bool allowComplement = true;
    bool allowTranslation = true;
    bool allowBackTranslation = true;

    QMap<const ADVSequenceObjectContext*, QList<SharedAnnotationData>> annotationsPerSeq;
    foreach (Annotation* a, annotations) {
        ADVSequenceObjectContext* seqCtx = view->getSequenceContext(a->getGObject());
        if (seqCtx == nullptr) {
            continue;
        }

        QList<SharedAnnotationData>& annsPerSeq = annotationsPerSeq[seqCtx];
        annsPerSeq.append(a->getData());
        if (annsPerSeq.size() > 1) {
            continue;
        }
        U2SequenceObject* seqObj = seqCtx->getSequenceObject();
        if (GObjectUtils::findComplementTT(seqObj->getAlphabet()) == nullptr) {
            allowComplement = false;
        }
        if (GObjectUtils::findAminoTT(seqObj, false) == nullptr) {
            allowTranslation = false;
        }
        if (GObjectUtils::findBackTranslationTT(seqObj) == nullptr) {
            allowBackTranslation = false;
        }
    }

    QString fileExt = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA)->getSupportedDocumentFileExtensions().first();
    QString dirPath;
    QString fileBaseName;

    GUrl seqUrl = view->getActiveSequenceContext()->getSequenceGObject()->getDocument()->getURL();
    GUrlUtils::getLocalPathFromUrl(seqUrl, view->getActiveSequenceContext()->getSequenceGObject()->getGObjectName(), dirPath, fileBaseName);
    GUrl defaultUrl = GUrlUtils::rollFileName(dirPath + QDir::separator() + fileBaseName + "_annotation." + fileExt, DocumentUtils::getNewDocFileNameExcludesHint());

    QObjectScopedPointer<ExportSequencesDialog> d = new ExportSequencesDialog(true,
                                                                              allowComplement,
                                                                              allowTranslation,
                                                                              allowBackTranslation,
                                                                              defaultUrl.getURLString(),
                                                                              fileBaseName,
                                                                              BaseDocumentFormats::FASTA,
                                                                              AppContext::getMainWindow()->getQMainWindow());
    d->setWindowTitle(tr("Export Sequence of Selected Annotations"));
    d->disableAllFramesOption(true);  // only 1 frame is suitable
    d->disableStrandOption(true);  // strand is already recorded in annotation
    d->disableAnnotationsOption(true);  // here we do not export annotations for sequence under another annotations
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc == QDialog::Rejected) {
        return;
    }
    assert(d->file.length() > 0);

    ExportAnnotationSequenceTaskSettings s;
    ExportUtils::loadDNAExportSettingsFromDlg(s.exportSequenceSettings, d.data());
    foreach (const ADVSequenceObjectContext* seqCtx, annotationsPerSeq.keys()) {
        ExportSequenceAItem ei;
        ei.sequence = seqCtx->getSequenceObject();
        ei.complTT = seqCtx->getComplementTT();
        ei.aminoTT = d->translate ? seqCtx->getAminoTT() : nullptr;
        if (d->useSpecificTable && ei.sequence->getAlphabet()->isNucleic()) {
            DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
            ei.aminoTT = tr->lookupTranslation(ei.sequence->getAlphabet(), DNATranslationType_NUCL_2_AMINO, d->translationTable);
        }
        ei.annotations = annotationsPerSeq.value(seqCtx);
        s.items.append(ei);
    }
    Task* t = ExportUtils::wrapExportTask(new ExportAnnotationSequenceTask(s), d->addToProject);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void ADVExportContext::sl_saveSelectedSequences() {
    ADVSequenceObjectContext* seqCtx = view->getActiveSequenceContext();
    DNASequenceSelection* sel = nullptr;
    if (seqCtx != nullptr) {
        // TODO: support multi-export..
        sel = seqCtx->getSequenceSelection();
    }
    if (sel == nullptr || sel->isEmpty()) {
        QMessageBox::warning(view->getWidget(), L10N::warningTitle(), tr("No sequence regions selected!"));
        return;
    }

    const QVector<U2Region>& regions = sel->getSelectedRegions();
    bool merge = regions.size() > 1;
    bool complement = seqCtx->getComplementTT() != nullptr;
    bool amino = seqCtx->getAminoTT() != nullptr;
    bool nucleic = GObjectUtils::findBackTranslationTT(seqCtx->getSequenceObject()) != nullptr;

    QString fileExt = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA)->getSupportedDocumentFileExtensions().first();
    QString dirPath;
    QString fileBaseName;

    GUrl seqUrl = seqCtx->getSequenceGObject()->getDocument()->getURL();
    GUrlUtils::getLocalPathFromUrl(seqUrl, seqCtx->getSequenceGObject()->getGObjectName(), dirPath, fileBaseName);
    GUrl defaultUrl = GUrlUtils::rollFileName(dirPath + QDir::separator() + fileBaseName + "_region." + fileExt, DocumentUtils::getNewDocFileNameExcludesHint());

    QObjectScopedPointer<ExportSequencesDialog> d = new ExportSequencesDialog(merge,
                                                                              complement,
                                                                              amino,
                                                                              nucleic,
                                                                              defaultUrl.getURLString(),
                                                                              fileBaseName,
                                                                              BaseDocumentFormats::FASTA,
                                                                              AppContext::getMainWindow()->getQMainWindow());
    d->setWindowTitle(tr("Export Selected Sequence Region"));
    const int rc = d->exec();
    CHECK(!d.isNull(), );
    CHECK(rc != QDialog::Rejected, );
    SAFE_POINT(!d->file.isEmpty(), "Invalid file path", );

    ExportSequenceTaskSettings s;
    ExportUtils::loadDNAExportSettingsFromDlg(s, d.data());

    const DNATranslation* aminoTrans = nullptr;
    if (d->translate) {
        aminoTrans = d->useSpecificTable ? GObjectUtils::findAminoTT(seqCtx->getSequenceObject(), false, d->translationTable) : seqCtx->getAminoTT();
    }
    const DNATranslation* backTrans = d->backTranslate ? GObjectUtils::findBackTranslationTT(seqCtx->getSequenceObject(), d->translationTable) : nullptr;
    const DNATranslation* complTrans = seqCtx->getComplementTT();
    Task* t = ExportUtils::wrapExportTask(new ExportSelectedSeqRegionsTask(seqCtx->getSequenceObject(), seqCtx->getAnnotationObjects(true), regions, s, aminoTrans, backTrans, complTrans), d->addToProject);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void ADVExportContext::sl_saveSelectedAnnotations() {
    // find annotations: selected annotations, selected groups
    AnnotationSelection* as = view->getAnnotationsSelection();
    QList<Annotation*> annotationSet = as->getAnnotations();
    foreach (AnnotationGroup* group, view->getAnnotationsGroupSelection()->getSelection()) {
        group->findAllAnnotationsInGroupSubTree(annotationSet);
    }

    if (annotationSet.isEmpty()) {
        QMessageBox::warning(view->getWidget(), L10N::warningTitle(), tr("No annotations selected!"));
        return;
    }

    Annotation* first = *annotationSet.begin();
    Document* doc = first->getGObject()->getDocument();
    ADVSequenceObjectContext* sequenceContext = view->getActiveSequenceContext();

    GUrl url;
    if (doc != nullptr) {
        url = doc->getURL();
    } else if (sequenceContext != nullptr) {
        url = sequenceContext->getSequenceGObject()->getDocument()->getURL();
    } else {
        url = GUrl("newfile");
    }

    QString fileName = GUrlUtils::getNewLocalUrlByExtension(url, "newfile", ".csv", "_annotations");
    QObjectScopedPointer<ExportAnnotationsDialog> d = new ExportAnnotationsDialog(fileName, AppContext::getMainWindow()->getQMainWindow());
    d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted != d->result()) {
        return;
    }

    // TODO: lock documents or use shared-data objects
    std::stable_sort(annotationSet.begin(), annotationSet.end(), Annotation::annotationLessThan);

    // run task
    Task* t = nullptr;
    if (d->fileFormat() == ExportAnnotationsDialog::CSV_FORMAT_ID) {
        U2OpStatusImpl os;
        QByteArray seqData = sequenceContext->getSequenceObject()->getWholeSequenceData(os);
        CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );
        t = new ExportAnnotations2CSVTask(annotationSet, seqData, sequenceContext->getSequenceObject()->getSequenceName(), sequenceContext->getComplementTT(), d->exportSequence(), d->exportSequenceNames(), d->filePath());
    } else {
        t = ExportObjectUtils::saveAnnotationsTask(d->filePath(), d->fileFormat(), annotationSet, d->addToProject());
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

//////////////////////////////////////////////////////////////////////////
// alignment part

#define MAX_ALI_MODEL (10 * 1000 * 1000)

void ADVExportContext::prepareMAFromBlastAnnotations(Msa& ma, const QString& qualiferId, bool includeRef, U2OpStatus& os) {
    SAFE_POINT_EXT(ma->isEmpty(), os.setError("Illegal parameter: input alignment is not empty!"), );
    const QList<Annotation*>& selection = view->getAnnotationsSelection()->getAnnotations();
    CHECK_EXT(selection.size() >= 2, os.setError(tr("At least 2 annotations are required")), );

    AnnotationTableObject* ao = selection.first()->getGObject();
    ADVSequenceObjectContext* commonSeq = view->getSequenceContext(ao);
    qint64 maxLen = commonSeq->getSequenceLength();
    ma->setAlphabet(commonSeq->getAlphabet());
    QSet<QString> names;
    int rowIdx = 0;

    for (const Annotation* annotation : qAsConst(selection)) {
        SAFE_POINT(annotation->getName() == BLAST_ANNOTATION_NAME, QString("%1 is not a BLAST annotation").arg(annotation->getName()), );

        ADVSequenceObjectContext* seqCtx = view->getSequenceContext(annotation->getGObject());
        CHECK_EXT(seqCtx != nullptr, os.setError(tr("No sequence object found")), );
        CHECK_EXT(seqCtx == commonSeq, os.setError(tr("Can not export BLAST annotations from different sequences")), );

        QString qVal = annotation->findFirstQualifierValue(qualiferId);
        CHECK_EXT(!qVal.isEmpty(), os.setError(tr("Can not find qualifier to set as a name for BLAST sequence")), );

        QString rowName = ExportUtils::genUniqueName(names, qVal);
        U2EntityRef seqRef = seqCtx->getSequenceObject()->getSequenceRef();

        maxLen = qMax(maxLen, annotation->getRegionsLen());
        CHECK_EXT(maxLen * ma->getRowCount() <= MAX_ALI_MODEL, os.setError(tr("Alignment is too large")), );

        QString subjSeq = annotation->findFirstQualifierValue("subj_seq");
        if (!subjSeq.isEmpty()) {
            ma->addRow(rowName, subjSeq.toLatin1());
        } else {
            QByteArray rowSequence = AnnotationSelection::getSequenceUnderAnnotation(seqRef, annotation, nullptr, nullptr, os);
            CHECK_OP(os, );
            ma->addRow(rowName, rowSequence);
        }

        int offset = annotation->getLocation()->regions.first().startPos;
        ma->insertGaps(rowIdx, 0, offset, os);
        CHECK_OP(os, );

        names.insert(rowName);
        ++rowIdx;
    }

    if (includeRef) {
        QByteArray rowSequence = commonSeq->getSequenceObject()->getWholeSequenceData(os);
        CHECK_OP(os, );
        ma->addRow(commonSeq->getSequenceGObject()->getGObjectName(), rowSequence, 0);
    }
}

void ADVExportContext::prepareMAFromAnnotations(Msa& ma, bool translate, U2OpStatus& os) {
    SAFE_POINT_EXT(ma->isEmpty(), os.setError("Illegal parameter: input alignment is not empty!"), );
    const QList<Annotation*>& selection = view->getAnnotationsSelection()->getAnnotations();
    CHECK_EXT(selection.size() >= 2, os.setError(tr("At least 2 annotations are required")), );

    // check that all sequences are present and have the same alphabets
    const DNAAlphabet* al = nullptr;
    foreach (const Annotation* annotation, selection) {
        AnnotationTableObject* ao = annotation->getGObject();
        ADVSequenceObjectContext* seqCtx = view->getSequenceContext(ao);
        CHECK_EXT(seqCtx != nullptr, os.setError(tr("No sequence object found")), );
        if (al == nullptr) {
            al = seqCtx->getAlphabet();
        } else {
            const DNAAlphabet* al2 = seqCtx->getAlphabet();
            // BUG524: support alphabet reduction
            CHECK_EXT(al->getType() == al2->getType(), os.setError(tr("Different sequence alphabets")), );
            al = al->getMap().count(true) >= al2->getMap().count(true) ? al : al2;
        }
    }
    qint64 maxLen = 0;
    ma->setAlphabet(al);
    QSet<QString> names;
    foreach (const Annotation* annotation, selection) {
        QString rowName = annotation->getName();
        AnnotationTableObject* ao = annotation->getGObject();
        ADVSequenceObjectContext* seqCtx = view->getSequenceContext(ao);
        U2EntityRef seqRef = seqCtx->getSequenceObject()->getSequenceRef();

        maxLen = qMax(maxLen, annotation->getRegionsLen());
        CHECK_EXT(maxLen * ma->getRowCount() <= MAX_ALI_MODEL, os.setError(tr("Alignment is too large")), );
        const DNATranslation* complTT = annotation->getStrand().isComplementary() ? seqCtx->getComplementTT() : nullptr;
        const DNATranslation* aminoTT = translate ? seqCtx->getAminoTT() : nullptr;
        QByteArray rowSequence = AnnotationSelection::getSequenceUnderAnnotation(seqRef, annotation, complTT, aminoTT, os);
        CHECK_OP(os, );

        ma->addRow(rowName, rowSequence);

        names.insert(rowName);
    }
}

void ADVExportContext::prepareMAFromSequences(Msa& ma, bool translate, U2OpStatus& os) {
    SAFE_POINT_EXT(ma->isEmpty(), os.setError("Illegal parameter: Input alignment is not empty!"), );

    const DNAAlphabet* al = translate ? AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::AMINO_DEFAULT()) : nullptr;

    // derive alphabet
    int nItems = 0;
    bool forceTranslation = false;
    foreach (ADVSequenceObjectContext* c, view->getSequenceContexts()) {
        if (c->getSequenceSelection()->isEmpty()) {
            continue;
        }
        nItems += c->getSequenceSelection()->getSelectedRegions().count();
        const DNAAlphabet* seqAl = c->getAlphabet();
        if (al == nullptr) {
            al = seqAl;
        } else if (al != seqAl) {
            if (al->isNucleic() && seqAl->isAmino()) {
                forceTranslation = true;
                al = seqAl;
            } else if (al->isAmino() && seqAl->isNucleic()) {
                forceTranslation = true;
            } else {
                os.setError(tr("Can't derive alignment alphabet"));
                return;
            }
        }
    }

    CHECK_EXT(nItems >= 2, os.setError(tr("At least 2 sequences required")), );
    ma->setAlphabet(al);

    // cache sequences
    QSet<QString> names;
    qint64 maxLen = 0;
    foreach (ADVSequenceObjectContext* seqCtx, view->getSequenceContexts()) {
        if (seqCtx->getSequenceSelection()->isEmpty()) {
            continue;
        }
        const DNAAlphabet* seqAl = seqCtx->getAlphabet();
        DNATranslation* aminoTT = ((translate || forceTranslation) && seqAl->isNucleic()) ? seqCtx->getAminoTT() : nullptr;
        QVector<U2Region> regions = seqCtx->getSequenceSelection()->getSelectedRegions();
        for (const U2Region& r : qAsConst(regions)) {
            maxLen = qMax(maxLen, r.length);
            CHECK_EXT(maxLen * ma->getRowCount() <= MAX_ALI_MODEL, os.setError(tr("Alignment is too large")), );
            QByteArray seq = seqCtx->getSequenceData(r, os);
            CHECK_OP(os, );
            if (aminoTT != nullptr) {
                int len = aminoTT->translate(seq.data(), seq.size());
                seq.resize(len);
            }
            QString rowName = ExportUtils::genUniqueName(names, seqCtx->getSequenceGObject()->getGObjectName());
            names.insert(rowName);
            ma->addRow(rowName, seq);
        }
    }
}

void ADVExportContext::selectionToAlignment(const QString& title, bool annotations, bool translate) {
    Msa ma(MA_OBJECT_NAME);
    U2OpStatusImpl os;
    if (annotations) {
        prepareMAFromAnnotations(ma, translate, os);
    } else {
        prepareMAFromSequences(ma, translate, os);
    }
    if (os.hasError()) {
        QMessageBox::critical(nullptr, L10N::errorTitle(), os.getError());
        return;
    }

    DocumentFormatConstraints c;
    c.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;

    QObjectScopedPointer<ExportSequences2MSADialog> d = new ExportSequences2MSADialog(view->getWidget());
    d->setWindowTitle(title);
    d->setOkButtonText(tr("Create alignment"));
    d->setFileLabelText(tr("Save alignment to file"));
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    Task* t = ExportUtils::wrapExportTask(new ExportAlignmentTask(ma, d->url, d->format), d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

QString ADVExportContext::getDbByCurrentAlphabet() const {
    const DNAAlphabet* seqAl = view->getSequenceObjectsWithContexts().first()->getAlphabet();
    QString db;
    if (seqAl->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()) {
        db = "NCBI GenBank (DNA sequence)";
    } else if (seqAl->getId() == BaseDNAAlphabetIds::AMINO_DEFAULT()) {
        db = "NCBI rotein sequence database";
    }

    return db;
}

void ADVExportContext::sl_saveSelectedAnnotationsToAlignment() {
    selectionToAlignment(annotationsToAlignmentAction->text(), true, false);
}

void ADVExportContext::sl_saveSelectedAnnotationsToAlignmentWithTranslation() {
    selectionToAlignment(annotationsToAlignmentAction->text(), true, true);
}

void ADVExportContext::sl_saveSelectedSequenceToAlignment() {
    selectionToAlignment(sequenceToAlignmentAction->text(), false, false);
}

void ADVExportContext::sl_saveSelectedSequenceToAlignmentWithTranslation() {
    selectionToAlignment(sequenceToAlignmentWithTranslationAction->text(), false, true);
}

void ADVExportContext::sl_getSequenceByDBXref() {
    const QList<Annotation*>& selection = view->getAnnotationsSelection()->getAnnotations();
    auto dbEntries = AppContext::getDBXRefRegistry()->getEntries();
    QMap<QString, QStringList> databaseAccessionsMap;
    for (const Annotation* ann : qAsConst(selection)) {
        QList<U2Qualifier> res;
        ann->findQualifiers("db_xref", res);
        for (const auto& qual : qAsConst(res)) {
            auto value = qual.value.split(":");
            CHECK_CONTINUE(value.size() == 2);

            auto database = value.first();
            CHECK_CONTINUE(dbEntries.contains(database));
            CHECK_CONTINUE(!dbEntries.value(database).fileUrl.isEmpty());

            auto accession = value.last();
            auto dbAccessions = databaseAccessionsMap.value(database);
            dbAccessions.append(accession);
            databaseAccessionsMap.insert(database, dbAccessions);
        }
    }

    fetchSequencesFromRemoteDB(databaseAccessionsMap);
}

void ADVExportContext::sl_getSequenceByAccession() {
    const QList<Annotation*>& selection = view->getAnnotationsSelection()->getAnnotations();

    QStringList genbankID;
    for (const Annotation* ann : qAsConst(selection)) {
        QList<U2Qualifier> res;
        ann->findQualifiers("accession", res);
        for (const auto& qual : qAsConst(res)) {
            genbankID << qual.value;
        }
    }
    auto db = getDbByCurrentAlphabet();
    CHECK(!db.isEmpty(), )

    fetchSequencesFromRemoteDB({ {db, genbankID} });
}

void ADVExportContext::sl_getSequenceById() {
    const QList<Annotation*>& selection = view->getAnnotationsSelection()->getAnnotations();

    QStringList genbankID;
    for (const Annotation* ann : qAsConst(selection)) {
        QList<U2Qualifier> res;
        ann->findQualifiers("id", res);
        for (const auto& qual : qAsConst(res)) {
            if (!qual.value.isEmpty()) {
                int off = qual.value.indexOf("|");
                int off1 = qual.value.indexOf("|", off + 1);
                genbankID << qual.value.mid(off + 1, off1 - off - 1);
            }
        }
    }
    auto db = getDbByCurrentAlphabet();
    CHECK(!db.isEmpty(), )

    fetchSequencesFromRemoteDB({ {db, genbankID} });
}

void ADVExportContext::fetchSequencesFromRemoteDB(const QMap<QString, QStringList>& databaseAccessionsMap) {
    QObjectScopedPointer<GetSequenceByIdDialog> dlg = new GetSequenceByIdDialog(view->getWidget());
    dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(dlg->result() == QDialog::Accepted, );

    bool addToProject = dlg->isAddToProject();
    QString dir = dlg->getDirectory();
    auto databases = databaseAccessionsMap.keys();
    QList<Task*> tasks;
    for (const auto& database : qAsConst(databases)) {
        const auto& accessions = databaseAccessionsMap.value(database);
        for (const auto& acc : qAsConst(accessions)) {
            if (addToProject) {
                tasks << new LoadRemoteDocumentAndAddToProjectTask(acc, database, dir);
            } else {
                tasks << new LoadRemoteDocumentTask(acc, database, dir);
            }
        }
    }

    TaskFlags flags = TaskFlag_NoRun | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled;
    Task* topLevelTask = new MultiTask(tr("Download remote documents by qualifiers"), tasks, false, flags);
    AppContext::getTaskScheduler()->registerTopLevelTask(topLevelTask);
}

void ADVExportContext::sl_exportBlastResultToAlignment() {
    DocumentFormatConstraints c;
    c.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;

    QObjectScopedPointer<ExportBlastResultDialog> d = new ExportBlastResultDialog(view->getWidget());
    const int rc = d->exec();
    CHECK(!d.isNull(), );
    if (rc != QDialog::Accepted) {
        return;
    }

    Msa ma(MA_OBJECT_NAME);
    U2OpStatusImpl os;

    prepareMAFromBlastAnnotations(ma, d->qualifierId, d->addRefFlag, os);

    if (os.hasError()) {
        QMessageBox::critical(nullptr, L10N::errorTitle(), os.getError());
        return;
    }

    Task* t = ExportUtils::wrapExportTask(new ExportAlignmentTask(ma, d->url, d->format), d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

}  // namespace U2
