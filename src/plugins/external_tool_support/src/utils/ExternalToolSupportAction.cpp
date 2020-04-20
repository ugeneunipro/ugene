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

#include "ExternalToolSupportAction.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>


namespace U2 {

ExternalToolSupportAction::ExternalToolSupportAction(QObject* p, GObjectView* v, const QString& _text, int order, const QStringList& _toolIds)
    : GObjectViewAction(p,v,_text,order),
      toolIds(_toolIds) {
    bool isAnyToolConfigured = checkTools(true);
    setState(isAnyToolConfigured);
}

ExternalToolSupportAction::ExternalToolSupportAction(const QString& _text, QObject* p, const QStringList& _toolIds)
    : GObjectViewAction(p, NULL, _text),
      toolIds(_toolIds) {
    bool isAnyToolConfigured = checkTools(true);
    setState(isAnyToolConfigured);
}

void ExternalToolSupportAction::sl_pathChanged() {
    bool isAnyToolConfigured = checkTools();
    setState(isAnyToolConfigured);
}

bool ExternalToolSupportAction::checkTools(bool connectSignals) {
    bool result = false;
    foreach (QString toolId, toolIds) {
        if (!AppContext::getExternalToolRegistry()->getById(toolId)->getPath().isEmpty()) {
            result = true;
        }
        ExternalTool* exTool=AppContext::getExternalToolRegistry()->getById(toolId);
        if (connectSignals == true) {
            connect(exTool, SIGNAL(si_pathChanged()), SLOT(sl_pathChanged()));
            connect(exTool, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolStateChanged(bool)));
        }
    }

    if (connectSignals == true) {
        connect(AppContext::getAppSettings()->getUserAppsSettings(), SIGNAL(si_temporaryPathChanged()), SLOT(sl_pathChanged()));
    }

    return result;
}

void ExternalToolSupportAction::setState(bool isAnyToolConfigured) {
    QFont isConfiguredToolFont;

    if (!isAnyToolConfigured ||
            (AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath().isEmpty())) {
        isConfiguredToolFont.setItalic(true);
        setIcon(AppContext::getExternalToolRegistry()->getById(toolIds.at(0))->getGrayIcon());
    } else {
        isConfiguredToolFont.setItalic(false);
        if (AppContext::getExternalToolRegistry()->getById(toolIds.at(0))->isValid()) {
            setIcon(AppContext::getExternalToolRegistry()->getById(toolIds.at(0))->getIcon());
        } else {
            setIcon(AppContext::getExternalToolRegistry()->getById(toolIds.at(0))->getWarnIcon());
        }
    }

#ifndef Q_OS_MAC
    // On Mac OS X native menu bar ignores font style changes providing via Qt.
    // Result is not an italic font, it has normal style but less size.
    // Turning off the italic style doesn't return the font's previous state.
    setFont(isConfiguredToolFont);
#endif
}
}//namespace
