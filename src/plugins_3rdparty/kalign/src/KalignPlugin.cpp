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

#include <QMainWindow>
#include <QMessageBox>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Notification.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

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
