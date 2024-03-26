/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ResourceSettingsGUIController.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>

namespace U2 {
#define TRANSMAP_FILE_NAME "translations.txt"

ResourceSettingsGUIPageController::ResourceSettingsGUIPageController(QObject* p)
    : AppSettingsGUIPageController(tr("Resources"), APP_SETTINGS_RESOURCES, p) {
}

AppSettingsGUIPageState* ResourceSettingsGUIPageController::getSavedState() {
    auto state = new ResourceSettingsGUIPageState();
    AppResourcePool* s = AppContext::getAppSettings()->getAppResourcePool();
    state->nCpus = s->getIdealThreadCount();
    state->nThreads = s->getMaxThreadCount();
    state->maxMem = s->getMaxMemorySizeInMB();
    return state;
}

void ResourceSettingsGUIPageController::saveState(AppSettingsGUIPageState* _state) {
    auto state = qobject_cast<ResourceSettingsGUIPageState*>(_state);
    AppResourcePool* s = AppContext::getAppSettings()->getAppResourcePool();
    assert(state->nCpus <= state->nThreads);
    s->setMaxThreadCount(state->nThreads);
    s->setIdealThreadCount(state->nCpus);
    s->setMaxMemorySizeInMB(state->maxMem);
}

AppSettingsGUIPageWidget* ResourceSettingsGUIPageController::createWidget(AppSettingsGUIPageState* state) {
    auto r = new ResourceSettingsGUIPageWidget(this);
    r->setState(state);
    return r;
}

const QString ResourceSettingsGUIPageController::helpPageId = QString("65929347");

ResourceSettingsGUIPageWidget::ResourceSettingsGUIPageWidget(ResourceSettingsGUIPageController*) {
    setupUi(this);

    int maxMem = AppResourcePool::x64MaxMemoryLimitMb;
    memBox->setMaximum(maxMem);
    connect(threadBox, SIGNAL(valueChanged(int)), SLOT(sl_threadsCountChanged(int)));
    connect(cpuBox, SIGNAL(valueChanged(int)), SLOT(sl_cpuCountChanged(int)));
}

void ResourceSettingsGUIPageWidget::setState(AppSettingsGUIPageState* s) {
    auto state = qobject_cast<ResourceSettingsGUIPageState*>(s);
    cpuBox->setValue(state->nCpus);
    threadBox->setValue(state->nThreads);
    memBox->setValue(state->maxMem);
}

AppSettingsGUIPageState* ResourceSettingsGUIPageWidget::getState(QString& err) const {
    Q_UNUSED(err);
    auto state = new ResourceSettingsGUIPageState();
    state->nCpus = cpuBox->value();
    state->nThreads = threadBox->value();
    state->maxMem = memBox->value();
    return state;
}

void ResourceSettingsGUIPageWidget::sl_threadsCountChanged(int n) {
    if (cpuBox->value() > n) {
        cpuBox->setValue(n);
    }
}

void ResourceSettingsGUIPageWidget::sl_cpuCountChanged(int n) {
    if (threadBox->value() < n) {
        threadBox->setValue(n);
    }
}

}  // namespace U2
