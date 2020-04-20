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

#include "FormatSettingsGUIController.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ProjectModel.h>

#include "project_view/ProjectViewImpl.h"

namespace U2 {

FormatSettingsGUIPageController::FormatSettingsGUIPageController(QObject* p) 
: AppSettingsGUIPageController(tr("File Format"), APP_SETTINGS_FORMAT, p)
{
}


AppSettingsGUIPageState *FormatSettingsGUIPageController::getSavedState() {
    FormatSettingsGUIPageState* state = new FormatSettingsGUIPageState();
    FormatAppsSettings *s = AppContext::getAppSettings()->getFormatAppsSettings();
    state->caseMode = s->getCaseAnnotationsMode();

    return state;
}

void FormatSettingsGUIPageController::saveState(AppSettingsGUIPageState* _state) {
    FormatSettingsGUIPageState* state = qobject_cast<FormatSettingsGUIPageState*>(_state);
    FormatAppsSettings *s = AppContext::getAppSettings()->getFormatAppsSettings();
    CaseAnnotationsMode prevMode = s->getCaseAnnotationsMode();
    s->setCaseAnnotationsMode(state->caseMode);
    Project *p = AppContext::getProject();
    if (state->caseMode != prevMode && p != NULL) {
        QList<Document*> docs = p->getDocuments(), toReload;
        foreach(Document *d, docs){
            if(d->isLoaded()){
                QList <GObject*> gobjList = d->findGObjectByType(GObjectTypes::SEQUENCE);
                if(!gobjList.isEmpty()){
                    toReload.append(d);
                }
            }
        }
        DocumentUpdater du;
        du.reloadDocuments(toReload);
    }
}

AppSettingsGUIPageWidget* FormatSettingsGUIPageController::createWidget(AppSettingsGUIPageState* state) {
    FormatSettingsGUIPageWidget* r = new FormatSettingsGUIPageWidget(this);
    r->setState(state);
    return r;
}

const QString FormatSettingsGUIPageController::helpPageId = QString("24748733");

FormatSettingsGUIPageWidget::FormatSettingsGUIPageWidget(FormatSettingsGUIPageController*) {
    setupUi(this);
    caseAnnsModeNames.insert(NO_CASE_ANNS, tr("Don't use case annotations"));
    caseAnnsModeNames.insert(LOWER_CASE, tr("Use lower case annotations"));
    caseAnnsModeNames.insert(UPPER_CASE, tr("Use upper case annotations"));
    caseCombo->addItems(caseAnnsModeNames.values());
}

void FormatSettingsGUIPageWidget::setState(AppSettingsGUIPageState* s) {
    FormatSettingsGUIPageState* state = qobject_cast<FormatSettingsGUIPageState*>(s);

    int caseModeIdx = caseCombo->findText(caseAnnsModeNames.value(state->caseMode), Qt::MatchFixedString);
    if (caseModeIdx!=-1) {
        caseCombo->setCurrentIndex(caseModeIdx);
    }
}

AppSettingsGUIPageState* FormatSettingsGUIPageWidget::getState(QString& err) const {
    Q_UNUSED(err);
    FormatSettingsGUIPageState* state = new FormatSettingsGUIPageState();
    state->caseMode = caseAnnsModeNames.key(caseCombo->currentText(), NO_CASE_ANNS);

    return state;
}

} // U2
