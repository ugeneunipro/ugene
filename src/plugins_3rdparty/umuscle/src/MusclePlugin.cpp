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

#include <QDialog>
#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Lang/WorkflowSettings.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "MuscleAlignDialogController.h"
#include "MusclePlugin.h"
#include "MuscleTask.h"
#include "MuscleWorker.h"
#include "ProfileToProfileWorker.h"
#include "umuscle_tests/umuscleTests.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    MusclePlugin * plug = new MusclePlugin();
    return plug;
}


MusclePlugin::MusclePlugin()
: Plugin(tr("MUSCLE"),
         tr("A port of MUSCLE package for multiple sequence alignment. Check http://www.drive5.com/muscle/ for the original version")),
         ctx(NULL)
{
    if (AppContext::getMainWindow()) {
        ctx = new MuscleMSAEditorContext(this);
        ctx->init();

        //Add to tools menu for fast run
        QAction* muscleAction = new QAction(tr("Align with MUSCLE..."), this);
        muscleAction->setIcon(QIcon(":umuscle/images/muscle_16.png"));
        muscleAction->setObjectName(ToolsMenu::MALIGN_MUSCLE);
        connect(muscleAction,SIGNAL(triggered()),SLOT(sl_runWithExtFileSpecify()));

        ToolsMenu::addAction(ToolsMenu::MALIGN_MENU, muscleAction);
    }
    LocalWorkflow::MuscleWorkerFactory::init();
    LocalWorkflow::ProfileToProfileWorkerFactory::init();
    //uMUSCLE Test
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = UMUSCLETests ::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

void MusclePlugin::sl_runWithExtFileSpecify(){
    //Call select input file and setup settings dialog
    MuscleTaskSettings settings;
    QObjectScopedPointer<MuscleAlignWithExtFileSpecifyDialogController> muscleRunDialog = new MuscleAlignWithExtFileSpecifyDialogController(AppContext::getMainWindow()->getQMainWindow(), settings);
    muscleRunDialog->exec();
    CHECK(!muscleRunDialog.isNull(), );

    if (muscleRunDialog->result() != QDialog::Accepted){
        return;
    }
    assert(!settings.inputFilePath.isEmpty());

    MuscleWithExtFileSpecifySupportTask* muscleTask=new MuscleWithExtFileSpecifySupportTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(muscleTask);
}

MusclePlugin::~MusclePlugin() {
    //nothing to do
}

MSAEditor* MuscleAction::getMSAEditor() const {
    MSAEditor* e = qobject_cast<MSAEditor*>(getObjectView());
    SAFE_POINT(e != NULL, "Can't get an appropriate MSA Editor", NULL);
    return e;
}

void MuscleAction::sl_updateState() {
    StateLockableItem* item = qobject_cast<StateLockableItem*>(sender());
    SAFE_POINT(item != NULL, "Unexpected sender: expect StateLockableItem", );
    MSAEditor* msaEditor = getMSAEditor();
    CHECK(msaEditor != NULL, );
    setEnabled(!item->isStateLocked() && !msaEditor->isAlignmentEmpty());
}

MuscleMSAEditorContext::MuscleMSAEditorContext(QObject* p) : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}


void MuscleMSAEditorContext::initViewContext(GObjectView* view) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaed != NULL, "Invalid GObjectView", );
    CHECK(msaed->getMaObject() != NULL, );

    bool objLocked = msaed->getMaObject()->isStateLocked();
    bool isMsaEmpty = msaed->isAlignmentEmpty();

    MuscleAction* alignAction = new MuscleAction(this, view, tr("Align with MUSCLE..."), 1000);
    alignAction->setIcon(QIcon(":umuscle/images/muscle_16.png"));
    alignAction->setEnabled(!objLocked && !isMsaEmpty);
    alignAction->setObjectName("Align with muscle");
    connect(alignAction, SIGNAL(triggered()), SLOT(sl_align()));
    connect(msaed->getMaObject(), SIGNAL(si_lockedStateChanged()), alignAction, SLOT(sl_updateState()));
    connect(msaed->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), alignAction, SLOT(sl_updateState()));
    addViewAction(alignAction);

    MuscleAction* addSequencesAction = new MuscleAction(this, view, tr("Align sequences to profile with MUSCLE..."), 1001);
    addSequencesAction->setIcon(QIcon(":umuscle/images/muscle_16.png"));
    addSequencesAction->setEnabled(!objLocked && !isMsaEmpty);
    addSequencesAction->setObjectName("Align sequences to profile with MUSCLE");
    connect(addSequencesAction, SIGNAL(triggered()), SLOT(sl_alignSequencesToProfile()));
    connect(msaed->getMaObject(), SIGNAL(si_lockedStateChanged()), addSequencesAction, SLOT(sl_updateState()));
    connect(msaed->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), addSequencesAction, SLOT(sl_updateState()));
    addViewAction(addSequencesAction);

    MuscleAction* alignProfilesAction = new MuscleAction(this, view, tr("Align profile to profile with MUSCLE..."), 1002);
    alignProfilesAction->setIcon(QIcon(":umuscle/images/muscle_16.png"));
    alignProfilesAction->setEnabled(!objLocked && !isMsaEmpty);
    alignProfilesAction->setObjectName("Align profile to profile with MUSCLE");
    connect(alignProfilesAction, SIGNAL(triggered()), SLOT(sl_alignProfileToProfile()));
    connect(msaed->getMaObject(), SIGNAL(si_lockedStateChanged()), alignProfilesAction, SLOT(sl_updateState()));
    connect(msaed->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), alignProfilesAction, SLOT(sl_updateState()));
    addViewAction(alignProfilesAction);
}

void MuscleMSAEditorContext::buildMenu(GObjectView* v, QMenu* m) {
    QList<GObjectViewAction *> actions = getViewActions(v);
    QMenu* alignMenu = GUIUtils::findSubMenu(m, MSAE_MENU_ALIGN);
    SAFE_POINT(alignMenu != NULL, "alignMenu", );
    foreach(GObjectViewAction* a, actions) {
        a->addToMenuWithOrder(alignMenu);
    }
}

void MuscleMSAEditorContext::sl_align() {
    MuscleAction* action = qobject_cast<MuscleAction*>(sender());
    assert(action!=NULL);
    MSAEditor* ed = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();

    const QRect selection = action->getMSAEditor()->getSelectionRect();
    MuscleTaskSettings s;
    if (!selection.isNull() ) {
        int width = selection.width();
        // it doesn't make sense to align one column!
        if ( (width > 1) && ( width < obj->getLength() ) ) {
            s.regionToAlign = U2Region(selection.x() + 1, selection.width() - 1);
            s.alignRegion = true;
        }
    }

    QObjectScopedPointer<MuscleAlignDialogController> dlg = new MuscleAlignDialogController(ed->getWidget(), obj->getMultipleAlignment(), s);
    const int rc = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }


    AlignGObjectTask* muscleTask = new MuscleGObjectRunFromSchemaTask(obj, s);
    Task *alignTask = NULL;

    if (dlg->translateToAmino()) {
        QString trId = dlg->getTranslationId();
        alignTask = new AlignInAminoFormTask(obj, muscleTask, trId);
    } else {
        alignTask = muscleTask;
    }

    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing
    ed->resetCollapsibleModel();
}

void MuscleMSAEditorContext::sl_alignSequencesToProfile() {
    MuscleAction* action = qobject_cast<MuscleAction*>(sender());
    assert(action!=NULL);
    MSAEditor* ed = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();
    if (obj == NULL)
        return;
    assert(!obj->isStateLocked());

    DocumentFormatConstraints c;
    QString f1 = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, false);
    QString f2 = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    QString filter = f2 + "\n" + f1;

    LastUsedDirHelper lod;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        lod.url = U2FileDialog::getOpenFileName(NULL, tr("Select file with sequences"), lod, filter, 0, QFileDialog::DontUseNativeDialog );
    } else
#endif
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Select file with sequences"), lod, filter);
    if (lod.url.isEmpty()) {
        return;
    }

    Task *alignTask = new MuscleAddSequencesToProfileTask(obj, lod.url, MuscleAddSequencesToProfileTask::Sequences2Profile);
    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing
    ed->resetCollapsibleModel();
}

void MuscleMSAEditorContext::sl_alignProfileToProfile() {
    MuscleAction* action = qobject_cast<MuscleAction*>(sender());
    assert(action!=NULL);
    MSAEditor* ed = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();
    if (obj == NULL)
        return;
    assert(!obj->isStateLocked());

    LastUsedDirHelper lod;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        lod.url = U2FileDialog::getOpenFileName(NULL, tr("Select file with alignment"), lod,
            DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true), 0, QFileDialog::DontUseNativeDialog );
    } else
#endif
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Select file with alignment"), lod,
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true));

    if (lod.url.isEmpty()) {
        return;
    }

    Task *alignTask = new MuscleAddSequencesToProfileTask(obj, lod.url, MuscleAddSequencesToProfileTask::Profile2Profile);
    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing
    ed->resetCollapsibleModel();
}

}//namespace

