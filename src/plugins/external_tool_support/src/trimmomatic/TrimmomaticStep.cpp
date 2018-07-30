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

#include <QDir>

#include <U2Core/U2SafePoints.h>

#include "TrimmomaticStep.h"

namespace U2 {
namespace LocalWorkflow {

QScopedPointer<TrimmomaticStepsRegistry> TrimmomaticStepsRegistry::instance;

TrimmomaticStepFactory::TrimmomaticStepFactory(const QString &_id)
    : id(_id)
{

}

TrimmomaticStepFactory::~TrimmomaticStepFactory() {

}

const QString &TrimmomaticStepFactory::getId() const {
    return id;
}

TrimmomaticStepsRegistry *TrimmomaticStepsRegistry::getInstance() {
    if (NULL == instance) {
        instance.reset(new TrimmomaticStepsRegistry());
    }
    return instance.data();
}

void TrimmomaticStepsRegistry::releaseInstance() {
    delete instance.take();
}

TrimmomaticStep::TrimmomaticStep(const QString &_id)
    : id(_id),
      settingsWidget(NULL)
{

}

TrimmomaticStep::~TrimmomaticStep() {
    delete settingsWidget;
}

const QString &TrimmomaticStep::getId() const {
    return id;
}

const QString &TrimmomaticStep::getName() const {
    return name;
}

const QString &TrimmomaticStep::getDescription() const {
    return description;
}

QString TrimmomaticStep::getCommand() const {
    const QString serializedState = serializeState(getSettingsWidget()->getState());
    return getId() + (serializedState.isEmpty() ? "" : ":" + serializedState);
}

void TrimmomaticStep::setCommand(const QString &command) {
    const QString stepId = command.left(command.indexOf(":"));
    CHECK(stepId == id, );
    widgetState = parseState(command);
}

bool TrimmomaticStep::validate() const {
    return getSettingsWidget()->validate();
}

TrimmomaticStepSettingsWidget *TrimmomaticStep::getSettingsWidget() const {
    if (NULL == settingsWidget) {
        settingsWidget = createWidget();
        settingsWidget->setState(widgetState);
        settingsWidget->setVisible(false);
        connect(settingsWidget, SIGNAL(destroyed(QObject *)), SLOT(sl_widgetDestroyed()));
        connect(settingsWidget, SIGNAL(si_valueChanged()), SIGNAL(si_valueChanged()));
    }
    return settingsWidget;
}

void TrimmomaticStep::sl_widgetDestroyed() {
    settingsWidget = NULL;
}

void TrimmomaticStep::sl_widgetIsAboutToBeDestroyed(const QVariantMap &state) {
    widgetState = state;
}

TrimmomaticStepSettingsWidget::TrimmomaticStepSettingsWidget()
    : QWidget(NULL)
{

}

}   // namespace LocalWorkflow
}   // namespace U2
