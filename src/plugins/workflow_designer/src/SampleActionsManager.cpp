/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QMenu>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/PluginModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ToolsMenu.h>

#include "SampleActionsManager.h"

namespace U2 {

namespace {
    const char *ID_PROPERTY = "action_id";
}

SampleAction::SampleAction(const QString &actionName, const QString &toolsMenu, const QString &samplePath, const QString &actionText)
: actionText(actionText), actionName(actionName), toolsMenu(toolsMenu), samplePath(samplePath)
{

}

SampleActionsManager::SampleActionsManager(QObject *parent)
: QObject(parent)
{

}

void SampleActionsManager::registerAction(const SampleAction &action) {
    actions.append(action);
    int id = actions.size() - 1;

    QAction *a = new QAction(action.actionText, this);
    a->setObjectName(action.actionName);
    a->setProperty(ID_PROPERTY, id);
    connect(a, SIGNAL(triggered()), SLOT(sl_clicked()));
    ToolsMenu::addAction(action.toolsMenu, a);
}

int SampleActionsManager::getValidClickedActionId(U2OpStatus &os) const {
    QAction *a = qobject_cast<QAction*>(sender());
    CHECK_EXT(NULL != a, os.setError(L10N::internalError("Unexpected method call")), -1);

    bool ok = false;
    int id = a->property(ID_PROPERTY).toInt(&ok);
    CHECK_EXT(ok, os.setError(L10N::internalError("Wrong action ID")), -1);

    CHECK_EXT(id >=0 && id < actions.size(), os.setError(L10N::internalError("Out of range action ID")), -1);
    return id;
}

SampleAction SampleActionsManager::getClickedAction(U2OpStatus &os) const {
    int id = getValidClickedActionId(os);
    CHECK_OP(os, SampleAction("", "", "", ""));
    return actions[id];
}

QStringList SampleActionsManager::getAbsentPlugins(const QStringList &requiredPlugins) const {
    QStringList result = requiredPlugins;
#ifdef _DEBUG
    for (int i=0; i<result.size(); i++) {
        result[i] += "d";
    }
#endif // _DEBUG
    foreach (Plugin *plugin, AppContext::getPluginSupport()->getPlugins()) {
        result.removeAll(plugin->getId());
    }
    return result;
}

void SampleActionsManager::sl_clicked() {
    U2OpStatusImpl os;
    SampleAction clickedAction = getClickedAction(os);

    if (!os.hasError()) {
        QStringList absentPlugins = getAbsentPlugins(clickedAction.requiredPlugins);
        if (!absentPlugins.isEmpty()) {
            os.setError(tr("This task requires the following plugins: %1. Add them and try again, please.").arg(absentPlugins.join(", ")));
        }
    }

    if (os.hasError()) {
        QMessageBox::warning(QApplication::activeWindow(), L10N::warningTitle(), os.getError());
        return;
    }

    emit si_clicked(clickedAction);
}

} // U2
