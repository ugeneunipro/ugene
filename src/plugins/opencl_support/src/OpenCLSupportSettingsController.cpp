/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "OpenCLSupportSettingsController.h"

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>

#include <U2Algorithm/OpenCLGpuRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>

namespace U2 {

OpenCLSupportSettingsPageController::OpenCLSupportSettingsPageController(const QString &_displayMsg, QObject *p /* = 0 */)
    : AppSettingsGUIPageController(tr("OpenCL"), OpenCLSupportSettingsPageId, p), displayMsg(_displayMsg) {
}

AppSettingsGUIPageState *OpenCLSupportSettingsPageController::getSavedState() {
    return new OpenCLSupportSettingsPageState(AppContext::getOpenCLGpuRegistry()->getEnabledGpuName());
}

void OpenCLSupportSettingsPageController::saveState(AppSettingsGUIPageState *_s) {
    QList<OpenCLGpuModel *> registeredGpus = AppContext::getOpenCLGpuRegistry()->getRegisteredGpus();
    OpenCLSupportSettingsPageState *s = qobject_cast<OpenCLSupportSettingsPageState *>(_s);

    //saving state of enabled/disabled GPUs into registry
    const QString enabledGpu = s->getEnabledGpuName();
    bool enabledGpuWasFound = false;
    foreach (OpenCLGpuModel *gpu, registeredGpus) {
        if (enabledGpu == gpu->getName()) {
            gpu->setEnabled(true);
            enabledGpuWasFound = true;
        } else {
            gpu->setEnabled(false);
        }
    }
    if (!enabledGpuWasFound) {
        registeredGpus.first()->setEnabled(true);
    }

    //increasing/decreasing maxuse of according resource
    AppResourceSemaphore *gpuResource = dynamic_cast<AppResourceSemaphore *>(AppResourcePool::instance()->getResource(RESOURCE_OPENCL_GPU));
    if (gpuResource) {
        gpuResource->setMaxUse(1); //Only one GPU is in use at each very moment
    }    //else - resource was not registered, nothing to do.
}

AppSettingsGUIPageWidget *OpenCLSupportSettingsPageController::createWidget(AppSettingsGUIPageState *state) {
    OpenCLSupportSettingsPageWidget *w = new OpenCLSupportSettingsPageWidget(displayMsg, this);
    w->setState(state);
    return w;
}

const QString OpenCLSupportSettingsPageController::helpPageId = QString("24742346");

OpenCLSupportSettingsPageState::OpenCLSupportSettingsPageState(const QString& name)
    : enabledGpuName(name) {
}

const QString &OpenCLSupportSettingsPageState::getEnabledGpuName() const {
    return enabledGpuName;
}

const static char *gpusDiscoveredText =
    "The following OpenCL-enabled GPUs are detected.<br>\
    Check the GPUs to use for accelerating algorithms computations.";

const static char *noGpusDiscoveredText = "No OpenCL-enabled GPU detected.";

OpenCLSupportSettingsPageWidget::OpenCLSupportSettingsPageWidget(const QString &_msg, OpenCLSupportSettingsPageController * /*ctrl*/)
    : onlyMsg(_msg) {

    if (!onlyMsg.isEmpty()) {
        //just display the centered warning message
        QHBoxLayout *hLayout = new QHBoxLayout(this);
        QLabel *msgLabel = new QLabel(onlyMsg, this);
        msgLabel->setAlignment(Qt::AlignLeft);

        hLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        hLayout->addWidget(msgLabel);
        hLayout->addStretch();
        setLayout(hLayout);
    } else {
        //everything is OK - adding info about all available GPUs
        QVBoxLayout *vLayout = new QVBoxLayout(this);
        QList<OpenCLGpuModel *> gpus = AppContext::getOpenCLGpuRegistry()->getRegisteredGpus();
        const QString &actualText = gpus.empty() ? tr(noGpusDiscoveredText) : tr(gpusDiscoveredText);
        QLabel *gpusDiscoveredLabel = new QLabel(actualText, this);
        gpusDiscoveredLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        vLayout->addWidget(gpusDiscoveredLabel);

        QButtonGroup* buttonGroup = new QButtonGroup(this);
        foreach (OpenCLGpuModel *m, gpus) {
            vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            QHBoxLayout *hLayout = new QHBoxLayout(this);

            QString gpuText = m->getName() + " " + QString::number(m->getGlobalMemorySizeBytes() / (1024 * 1024)) + " Mb";
            QRadioButton *rb = new QRadioButton(gpuText, this);
            rb->setChecked(m->isEnabled());
            gpuRadioButtons.insert(m->getName(), rb);
            buttonGroup->addButton(rb);
            hLayout->addWidget(rb);
            vLayout->addLayout(hLayout);
        }
        buttonGroup->setExclusive(true);

        setLayout(vLayout);
    }
}

void OpenCLSupportSettingsPageWidget::setState(AppSettingsGUIPageState *_state) {
    CHECK(!gpuRadioButtons.isEmpty(), )

    OpenCLSupportSettingsPageState *state = qobject_cast<OpenCLSupportSettingsPageState *>(_state);
    SAFE_POINT(nullptr != state, "OpenCLSupportSettingsPageState isn't found", );

    const QString enbledGpuName = state->getEnabledGpuName();
    if (gpuRadioButtons.keys().contains(enbledGpuName)) {
        gpuRadioButtons.value(enbledGpuName)->setChecked(true);
    } else {
        gpuRadioButtons.values().first()->setChecked(true);
    }
}

AppSettingsGUIPageState *OpenCLSupportSettingsPageWidget::getState(QString & /*err*/) const {
    CHECK(!gpuRadioButtons.isEmpty(), new OpenCLSupportSettingsPageState(QString()));

    QString enabledGpuName;
    foreach (QRadioButton *rb, gpuRadioButtons.values()) {
        CHECK_CONTINUE(rb->isChecked());

        enabledGpuName = gpuRadioButtons.key(rb);
        break;
    }

    return new OpenCLSupportSettingsPageState(enabledGpuName);
}


}    // namespace U2
