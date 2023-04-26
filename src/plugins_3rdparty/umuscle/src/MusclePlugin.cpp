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

#include "MusclePlugin.h"

#include <QDialog>
#include <QMainWindow>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaCollapseModel.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorSelection.h>

#include "MuscleAlignDialogController.h"
#include "MuscleTask.h"
#include "MuscleWorker.h"
#include "ProfileToProfileWorker.h"
#include "umuscle_tests/umuscleTests.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    MusclePlugin* plug = new MusclePlugin();
    return plug;
}

MusclePlugin::MusclePlugin()
    : Plugin(tr("MUSCLE"),
             tr("A port of MUSCLE package for multiple sequence alignment. Check http://www.drive5.com/muscle/ for the original version")),
      ctx(nullptr) {
    if (AppContext::getMainWindow()) {
        ctx = new MuscleMSAEditorContext(this);
        ctx->init();

        // Add to tools menu for fast run
        QAction* muscleAction = new QAction(tr("Align with MUSCLE…"), this);
        muscleAction->setIcon(QIcon(":umuscle/images/muscle_16.png"));
        muscleAction->setObjectName(ToolsMenu::MALIGN_MUSCLE);
        connect(muscleAction, SIGNAL(triggered()), SLOT(sl_runWithExtFileSpecify()));

        ToolsMenu::addAction(ToolsMenu::MALIGN_MENU, muscleAction);
    }
    LocalWorkflow::MuscleWorkerFactory::init();
    LocalWorkflow::ProfileToProfileWorkerFactory::init();

    // Register MUSCLE tests.
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    if (xmlTestFormat != nullptr) {
        GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
        l->qlist = UMUSCLETests ::createTestFactories();

        for (XMLTestFactory* f : qAsConst(l->qlist)) {
            bool res = xmlTestFormat->registerTestFactory(f);
            SAFE_POINT(res, "Failed to register MUSCLE test factories", );
        }
    }
}

void MusclePlugin::sl_runWithExtFileSpecify() {
    // Call select input file and setup settings dialog
    MuscleTaskSettings settings;
    QObjectScopedPointer<MuscleAlignWithExtFileSpecifyDialogController> muscleRunDialog = new MuscleAlignWithExtFileSpecifyDialogController(AppContext::getMainWindow()->getQMainWindow(), settings);
    muscleRunDialog->exec();
    CHECK(!muscleRunDialog.isNull(), );

    if (muscleRunDialog->result() != QDialog::Accepted) {
        return;
    }
    SAFE_POINT(!settings.inputFilePath.isEmpty(), "sl_runWithExtFileSpecify: no inputFilePath", );

    MuscleWithExtFileSpecifySupportTask* muscleTask = new MuscleWithExtFileSpecifySupportTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(muscleTask);
}

MuscleAction::MuscleAction(QObject* p, GObjectViewController* v, const QString& text, int order, bool isAlignSelectionAction)
    : GObjectViewAction(p, v, text, order) {
    setIcon(QIcon(":umuscle/images/muscle_16.png"));

    auto msaEditor = qobject_cast<MSAEditor*>(getObjectView());
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView", );

    QAction* msaEditorAction = isAlignSelectionAction ? msaEditor->alignSelectedSequencesToAlignmentAction : msaEditor->alignAction;
    connect(msaEditorAction, &QAction::changed, this, [this, msaEditorAction]() { setEnabled(msaEditorAction->isEnabled()); });
    setEnabled(msaEditorAction->isEnabled());
}

MSAEditor* MuscleAction::getMSAEditor() const {
    auto e = qobject_cast<MSAEditor*>(getObjectView());
    SAFE_POINT(e != nullptr, "Can't get an appropriate MSA Editor", nullptr);
    return e;
}

MuscleMSAEditorContext::MuscleMSAEditorContext(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void MuscleMSAEditorContext::initViewContext(GObjectViewController* view) {
    view->registerActionProvider(this);

    auto alignAction = new MuscleAction(this, view, tr("Align with MUSCLE…"), 1000);
    alignAction->setObjectName("Align with muscle");
    alignAction->setMenuTypes({MsaEditorMenuType::ALIGN});
    connect(alignAction, &QAction::triggered, this, &MuscleMSAEditorContext::sl_align);
    addViewAction(alignAction);

    auto addSequencesAction = new MuscleAction(this, view, tr("Align sequences to alignment with MUSCLE…"), 1001);
    addSequencesAction->setObjectName("Align sequences to profile with MUSCLE");
    addSequencesAction->setMenuTypes({MsaEditorMenuType::ALIGN_NEW_SEQUENCES_TO_ALIGNMENT});
    connect(addSequencesAction, &QAction::triggered, this, &MuscleMSAEditorContext::sl_alignSequencesToProfile);
    addViewAction(addSequencesAction);

    auto alignProfilesAction = new MuscleAction(this, view, tr("Align alignment to alignment with MUSCLE…"), 1002);
    alignProfilesAction->setObjectName("Align profile to profile with MUSCLE");
    alignProfilesAction->setMenuTypes({MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT});
    connect(alignProfilesAction, &QAction::triggered, this, &MuscleMSAEditorContext::sl_alignProfileToProfile);
    addViewAction(alignProfilesAction);

    auto alignSelectionToAlignmentAction = new MuscleAction(this, view, tr("Align selected sequences to alignment with MUSCLE…"), 3003, true);
    alignSelectionToAlignmentAction->setObjectName("align_selection_to_alignment_muscle");
    alignSelectionToAlignmentAction->setMenuTypes({MsaEditorMenuType::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT});
    connect(alignSelectionToAlignmentAction, &QAction::triggered, this, &MuscleMSAEditorContext::sl_alignSelectedSequences);
    addViewAction(alignSelectionToAlignmentAction);
}

void MuscleMSAEditorContext::sl_align() {
    auto action = qobject_cast<MuscleAction*>(sender());
    SAFE_POINT(action != nullptr, "sl_align: not a MuscleAction", );
    MSAEditor* msaEditor = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = msaEditor->getMaObject();

    QRect selection = msaEditor->getSelection().toRect();
    MuscleTaskSettings s;
    if (!selection.isNull()) {
        int width = selection.width();
        // it doesn't make sense to align one column!
        if ((width > 1) && (width < obj->getLength())) {
            s.regionToAlign = U2Region(selection.x() + 1, selection.width() - 1);
            s.alignRegion = true;
        }
    }

    QObjectScopedPointer<MuscleAlignDialogController> dlg = new MuscleAlignDialogController(msaEditor->getWidget(), obj->getMultipleAlignment(), s);
    int rc = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }

    AlignGObjectTask* muscleTask = new MuscleGObjectRunFromSchemaTask(obj, s);
    Task* alignTask;

    if (dlg->translateToAmino()) {
        QString trId = dlg->getTranslationId();
        alignTask = new AlignInAminoFormTask(obj, muscleTask, trId);
    } else {
        alignTask = muscleTask;
    }

    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

void MuscleMSAEditorContext::sl_alignSequencesToProfile() {
    auto action = qobject_cast<MuscleAction*>(sender());
    SAFE_POINT(action != nullptr, "Not a MuscleAction!", );
    MSAEditor* msaEditor = action->getMSAEditor();
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();

    DocumentFormatConstraints c;
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GObjectTypes::SEQUENCE});

    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(nullptr, tr("Select file with sequences"), lod, filter);
    CHECK(!lod.url.isEmpty(), );

    auto alignTask = new MuscleAddSequencesToProfileTask(msaObject, lod.url, MuscleAddSequencesToProfileTask::Sequences2Profile);
    connect(msaObject, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

void MuscleMSAEditorContext::sl_alignProfileToProfile() {
    auto action = qobject_cast<MuscleAction*>(sender());
    SAFE_POINT(action != nullptr, "sl_alignProfileToProfile: not a MuscleAction", );
    MSAEditor* ed = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GObjectTypes::SEQUENCE});
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(nullptr, tr("Select file with alignment"), lod, filter);

    if (lod.url.isEmpty()) {
        return;
    }

    auto alignTask = new MuscleAddSequencesToProfileTask(obj, lod.url, MuscleAddSequencesToProfileTask::Profile2Profile);
    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing mode.
    ed->resetCollapseModel();
}

void MuscleMSAEditorContext::sl_alignSelectedSequences() {
    auto action = qobject_cast<MuscleAction*>(sender());
    SAFE_POINT(action != nullptr, "Not a MuscleAction!", );
    MSAEditor* msaEditor = action->getMSAEditor();
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();

    QList<int> selectedMaRowIndexes = msaEditor->getSelection().getSelectedRowIndexes();

    auto alignTask = new MuscleAlignOwnSequencesToSelfAction(msaObject, selectedMaRowIndexes);
    connect(msaObject, &QObject::destroyed, alignTask, &Task::cancel);
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);
}

}  // namespace U2
