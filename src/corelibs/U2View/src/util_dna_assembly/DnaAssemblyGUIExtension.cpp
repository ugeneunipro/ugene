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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Core/QObjectScopedPointer.h>

#include "DnaAssemblyGUIExtension.h"

namespace U2 {

/************************************************************************/
/* DnaAssemblyAlgorithmBaseWidget */
/************************************************************************/
DnaAssemblyAlgorithmBaseWidget::DnaAssemblyAlgorithmBaseWidget(QWidget *parent)
: QWidget(parent)
{

}

void DnaAssemblyAlgorithmBaseWidget::validateReferenceSequence(const GUrl &) const {

}

/************************************************************************/
/* DnaAssemblyAlgorithmMainWidget */
/************************************************************************/
DnaAssemblyAlgorithmMainWidget::DnaAssemblyAlgorithmMainWidget(QWidget *parent)
: DnaAssemblyAlgorithmBaseWidget(parent)
{

}

bool DnaAssemblyAlgorithmMainWidget::isParametersOk(QString & /*error*/) const {
    return requiredToolsAreOk();
}

bool DnaAssemblyAlgorithmMainWidget::buildIndexUrl(const GUrl & /*url*/, bool /*prebuiltIndex*/, QString & /*error*/) const {
    return true;
}

bool DnaAssemblyAlgorithmMainWidget::isIndexOk(const GUrl & /*url*/, QString & /*error*/) const {
    return true;
}

bool DnaAssemblyAlgorithmMainWidget::isIndex(const QString &url) const {
    return DnaAssemblyToReferenceTask::isIndexUrl(url, indexSuffixes);
}

bool DnaAssemblyAlgorithmMainWidget::isValidIndex(const QString &oneIndexFileUrl) const {
    QString baseUrl = DnaAssemblyToReferenceTask::getBaseUrl(oneIndexFileUrl, indexSuffixes);
    return DnaAssemblyToReferenceTask::isPrebuiltIndex(baseUrl, indexSuffixes);
}

bool DnaAssemblyAlgorithmMainWidget::requiredToolsAreOk() const {
    QStringList missedExtTools;
    ExternalToolRegistry *extToolRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(NULL != extToolRegistry, L10N::nullPointerError("External tool subsystem"), false);
    foreach (const QString &toolId, requiredExtToolIds) {
        ExternalTool *tool = extToolRegistry->getById(toolId);
        if (NULL == tool || tool->getPath().isEmpty()) {
            missedExtTools.append(extToolRegistry->getToolNameById(toolId));
        }
    }

    if (!missedExtTools.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("DNA Assembly"));
        msgBox->setInformativeText(tr("Do you want to specify it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);

        QString msgText(tr("Path for "));
        for (int i = 0, n = missedExtTools.size(); i < n; ++i) {
            QString &toolName = missedExtTools[i];
            msgText.append(i > 0 ? (i == n - 1 ? tr(" and ") : ", ") : "");
            msgText.append(QString("<i>%1</i>").arg(toolName));
        }
        msgText.append(tr(" is not set."));
        msgBox->setText(msgText);

        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        if (ret == QMessageBox::Yes) {
            AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_EXTERNAL_TOOLS);
        }
        return false;
    } else {
        return true;
    }
}

/************************************************************************/
/* DnaAssemblyAlgorithmBuildIndexWidget */
/************************************************************************/
DnaAssemblyAlgorithmBuildIndexWidget::DnaAssemblyAlgorithmBuildIndexWidget(QWidget *parent)
: DnaAssemblyAlgorithmBaseWidget(parent)
{

}

/************************************************************************/
/* DnaAssemblyGUIExtensionsFactory */
/************************************************************************/
DnaAssemblyGUIExtensionsFactory::~DnaAssemblyGUIExtensionsFactory() {

}

/************************************************************************/
/* GenomeAssemblyAlgorithmMainWidget */
/************************************************************************/
GenomeAssemblyAlgorithmMainWidget::GenomeAssemblyAlgorithmMainWidget(QWidget *parent)
: QWidget(parent)
{

}

} // U2
