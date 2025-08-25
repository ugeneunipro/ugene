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
#include "MfoldSupport.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/AppSettingsGUI.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "MfoldDialog.h"
#include "MfoldTask.h"

namespace U2 {
const QString MfoldSupport::ET_MFOLD_ID = "USUPP_MFOLD";

MfoldSupport::MfoldSupport()
    : ExternalTool(ET_MFOLD_ID, "mfold", "mfold") {
    if (AppContext::getMainWindow() != nullptr) {
        viewCtx = new MfoldContext(this);
        iconPath = ":external_tool_support/images/mfold.png";
        grayIconPath = ":external_tool_support/images/mfold_gray.png";
        warnIconPath = ":external_tool_support/images/mfold_warn.png";
        viewCtx->init();
    }
    description = tr(
        "The <i>mfold</i> software computes a collection of optimal and suboptimal foldings "
        "<a href=\"http://www.unafold.org/\">http://www.unafold.org/</a>.<br>"
        "Secondary structure images are generated using Ghostscript "
        "<a href=\"https://ghostscript.com/\">https://ghostscript.com/</a>.");
    executableFileName = "mfold" + QString(isOsWindows() ? ".bat" : ".sh");
    validationArguments += "-v";
    version = "3.6";
    toolKitName = "mfold";
    pathChecks += {PathChecks::NonLatinArguments,
                   PathChecks::NonLatinTemporaryDirPath,
                   PathChecks::NonLatinToolPath,
                   PathChecks::SpacesArguments,
                   PathChecks::SpacesTemporaryDirPath};

    static const QString MFOLD_VERSION_REGEXP = "(\\d+\\.\\d+)";
    validationMessageRegExp = "mfold version " + MFOLD_VERSION_REGEXP;
    versionRegExp = QRegExp(MFOLD_VERSION_REGEXP);
}

GObjectViewWindowContext* MfoldSupport::getViewContext() const {
    return viewCtx;
}

MfoldContext::MfoldContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
}

void MfoldContext::initViewContext(GObjectViewController* controller) {
    auto adv = qobject_cast<AnnotatedDNAView*>(controller);
    SAFE_POINT_NN(adv, );
    ADVGlobalAction* a = new ADVGlobalAction(adv,
                                             ":/external_tool_support/images/mfold.png",
                                             "Mfold...",
                                             2002,
                                             ADVGlobalActionFlags(ADVGlobalActionFlag_AddToToolbar) |
                                                 ADVGlobalActionFlag_AddToAnalyseMenu);
    a->setObjectName("mfold_action");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, &ADVGlobalAction::triggered, this, &MfoldContext::sl_showDialog);
}

void MfoldContext::sl_showDialog() {
    if (AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID)->getPath().isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("mfold");
        msgBox->setText(tr("Path for mfold tool not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        CHECK(msgBox->exec() == QMessageBox::Yes, );
        AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
    }
    CHECK(!AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID)->getPath().isEmpty(), );
    U2OpStatus2Log os;
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    auto objView = qobject_cast<GObjectViewAction*>(sender())->getObjectView();
    SAFE_POINT_NN(objView, );
    auto seqCtx = qobject_cast<AnnotatedDNAView*>(objView)->getActiveSequenceContext();
    SAFE_POINT_NN(seqCtx, );
    QScopedPointer<MfoldDialog, QScopedPointerDeleteLater> dialog(new MfoldDialog(*seqCtx));
    SAFE_POINT_NN(dialog, );
    CHECK(dialog->exec() == QDialog::Accepted, );

    auto t = createMfoldTask(seqCtx->getSequenceObject(),
                             dialog->getSettings(),
                             seqCtx->getAnnotatedDNAView()->getWidget()->width(),
                             os);
    CHECK_OP(os, );
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}
}  // namespace U2
