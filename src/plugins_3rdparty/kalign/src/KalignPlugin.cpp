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

#include "KalignPlugin.h"

#include <QDialog>
#include <QMainWindow>
#include <QMessageBox>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Notification.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "KalignDialogController.h"
#include "KalignTask.h"
#include "KalignWorker.h"
#include "PairwiseAlignmentHirschbergGUIExtensionFactory.h"
#include "PairwiseAlignmentHirschbergTask.h"
#include "PairwiseAlignmentHirschbergTaskFactory.h"
#include "kalign_tests/KalignTests.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    Kalign2Plugin* plug = new Kalign2Plugin();
    return plug;
}

Kalign2Plugin::Kalign2Plugin()
    : Plugin(tr("Kalign"),
             tr("A port of Kalign package for multiple sequence alignment. Check https://msa.sbc.su.se for the original version"))
{
    bool guiMode = AppContext::getMainWindow() != NULL;

    if (guiMode) {
        ctx = new Kalign2MSAEditorContext(this);
        ctx->init();

        QAction* kalignAction = new QAction(tr("Align with Kalign..."), this);
        kalignAction->setObjectName(ToolsMenu::MALIGN_KALIGN);
        kalignAction->setIcon(QIcon(":kalign/images/kalign_16.png"));
        ToolsMenu::addAction(ToolsMenu::MALIGN_MENU, kalignAction);
        connect(kalignAction, SIGNAL(triggered()), SLOT(sl_runWithExtFileSpecify()));
    }

    LocalWorkflow::Kalign2WorkerFactory::init();  // TODO
    // TODO:
    // Kalign Test

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat != NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = KalignTests ::createTestFactories();

    foreach (XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }

    AppContext::getAlignmentAlgorithmsRegistry()->registerAlgorithm(new Kalign2PairwiseAligmnentAlgorithm());
}

void Kalign2Plugin::sl_runWithExtFileSpecify() {
    // Call select input file and setup settings dialog

    Kalign2TaskSettings settings;
    QObjectScopedPointer<KalignAlignWithExtFileSpecifyDialogController> kalignRunDialog = new KalignAlignWithExtFileSpecifyDialogController(AppContext::getMainWindow()->getQMainWindow(), settings);
    kalignRunDialog->exec();
    CHECK(!kalignRunDialog.isNull(), );

    if (kalignRunDialog->result() != QDialog::Accepted) {
        return;
    }
    Kalign2WithExtFileSpecifySupportTask* kalignTask = new Kalign2WithExtFileSpecifySupportTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(kalignTask);
}

Kalign2Plugin::~Kalign2Plugin() {
    // nothing to do
}

MSAEditor* Kalign2Action::getMSAEditor() const {
    auto e = qobject_cast<MSAEditor*>(getObjectView());
    SAFE_POINT(e != NULL, "Can't get an appropriate MSA Editor", NULL);
    return e;
}

void Kalign2Action::sl_updateState() {
    auto item = qobject_cast<StateLockableItem*>(sender());
    SAFE_POINT(item != NULL, "Unexpected sender: expect StateLockableItem", );
    MSAEditor* msaEditor = getMSAEditor();
    CHECK(msaEditor != NULL, );
    setEnabled(!item->isStateLocked() && !msaEditor->isAlignmentEmpty());
}

Kalign2MSAEditorContext::Kalign2MSAEditorContext(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void Kalign2MSAEditorContext::initViewContext(GObjectViewController* view) {
    auto msaed = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaed != NULL, "Invalid GObjectView", );
    CHECK(msaed->getMaObject() != NULL, );
    msaed->registerActionProvider(this);

    bool objLocked = msaed->getMaObject()->isStateLocked();
    bool isMsaEmpty = msaed->isAlignmentEmpty();

    auto alignAction = new Kalign2Action(this, view, tr("Align with Kalign..."), 4000);
    alignAction->setObjectName("align_with_kalign");
    alignAction->setIcon(QIcon(":kalign/images/kalign_16.png"));
    alignAction->setEnabled(!objLocked && !isMsaEmpty);
    alignAction->setMenuTypes({MsaEditorMenuType::ALIGN});

    connect(alignAction, SIGNAL(triggered()), SLOT(sl_align()));
    connect(msaed->getMaObject(), SIGNAL(si_lockedStateChanged()), alignAction, SLOT(sl_updateState()));
    connect(msaed->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), alignAction, SLOT(sl_updateState()));
    addViewAction(alignAction);
}

void Kalign2MSAEditorContext::sl_align() {
    auto action = qobject_cast<Kalign2Action*>(sender());
    assert(action != NULL);
    MSAEditor* ed = action->getMSAEditor();
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();
    if (!Kalign2Task::isAlphabetSupported(obj->getAlphabet()->getId())) {
        QMessageBox::information(ed->getWidget(),
                                 tr("Unable to align with Kalign"),
                                 tr("Unable to align this Multiple alignment with Kalign.\r\nPlease, convert alignment from %1 alphabet to supported one and try again.")
                                     .arg(obj->getAlphabet()->getName()));
        return;
    }
    Kalign2TaskSettings s;
    QObjectScopedPointer<Kalign2Dialog> dlg = new Kalign2Dialog(ed->getWidget(), obj->getMultipleAlignment(), s);
    const int rc = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }

    AlignGObjectTask* kalignTask = new Kalign2GObjectRunFromSchemaTask(obj, s);
    Task* alignTask = NULL;

    if (dlg->translateToAmino()) {
        alignTask = new AlignInAminoFormTask(obj, kalignTask, dlg->getTranslationId());
    } else {
        alignTask = kalignTask;
    }

    connect(obj, SIGNAL(destroyed()), alignTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(alignTask);

    // Turn off rows collapsing mode.
    ed->resetCollapseModel();
}

Kalign2PairwiseAligmnentAlgorithm::Kalign2PairwiseAligmnentAlgorithm()
    : AlignmentAlgorithm(PairwiseAlignment,
                         "Hirschberg (KAlign)",
                         Kalign2Plugin::tr("Hirschberg (KAlign)"),
                         new PairwiseAlignmentHirschbergTaskFactory(),
                         new PairwiseAlignmentHirschbergGUIExtensionFactory(),
                         "KAlign") {
}

bool Kalign2PairwiseAligmnentAlgorithm::checkAlphabet(const DNAAlphabet* al) const {
    return Kalign2Task::isAlphabetSupported(al->getId());
}

}  // namespace U2
