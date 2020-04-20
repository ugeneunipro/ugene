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

#include <U2Core/U2SafePoints.h>

#include "trimmomatic/util/LineEditHighlighter.h"
#include "SlidingWindowStep.h"

namespace U2 {
namespace LocalWorkflow {

const QString SlidingWindowStepFactory::ID = "SLIDINGWINDOW";

SlidingWindowStep::SlidingWindowStep()
    : TrimmomaticStep(SlidingWindowStepFactory::ID)
{
    name = "SLIDINGWINDOW";
    description = tr("<html><head></head><body>"
                     "<h4>SLIDINGWINDOW</h4>"
                     "<p>This step performs a sliding window trimming, cutting once the average "
                     "quality within the window falls below a threshold. By considering multiple "
                     "bases, a single poor quality base will not cause the removal of high quality "
                     "data later in the read.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Window size</b>: the number of bases to average across.</li>"
                     "<li><b>Quality threshold</b>: the average quality required.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *SlidingWindowStep::createWidget() const {
    return new SlidingWindowSettingsWidget();
}

QString SlidingWindowStep::serializeState(const QVariantMap &widgetState) const {
    QString serializedState;
    if (widgetState.contains(SlidingWindowSettingsWidget::WINDOW_SIZE)) {
        serializedState += QString::number(widgetState.value(SlidingWindowSettingsWidget::WINDOW_SIZE).toInt());
    }
    serializedState += ":";
    if (widgetState.contains(SlidingWindowSettingsWidget::REQUIRED_QUALITY)) {
        serializedState += QString::number(widgetState.value(SlidingWindowSettingsWidget::REQUIRED_QUALITY).toInt());
    }
    return serializedState;
}

QVariantMap SlidingWindowStep::parseState(const QString &command) const {
    QVariantMap state;
    QRegExp regExp(id + ":" + "(\\d*)" + ":" + "(\\d*)");

    const bool matched = regExp.exactMatch(command);
    CHECK(matched, state);

    const QString windowSize = regExp.cap(1);
    if (!windowSize.isEmpty()) {
        state[SlidingWindowSettingsWidget::WINDOW_SIZE] = windowSize.toInt();
    }

    const QString requiredQuality = regExp.cap(2);
    if (!requiredQuality.isEmpty()) {
        state[SlidingWindowSettingsWidget::REQUIRED_QUALITY] = requiredQuality.toInt();
    }

    return state;
}

const QString SlidingWindowSettingsWidget::WINDOW_SIZE = "windowSize";
const QString SlidingWindowSettingsWidget::REQUIRED_QUALITY = "requiredQuality";

SlidingWindowSettingsWidget::SlidingWindowSettingsWidget() {
    setupUi(this);

    leWindowSize->setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    new LineEditHighlighter(leWindowSize);

    connect(leWindowSize, SIGNAL(textChanged(QString)), SIGNAL(si_valueChanged()));
    connect(sbQualityThreshold, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
}

SlidingWindowSettingsWidget::~SlidingWindowSettingsWidget() {
    emit si_widgetIsAboutToBeDestroyed(getState());
}

bool SlidingWindowSettingsWidget::validate() const {
    return !leWindowSize->text().isEmpty();
}

QVariantMap SlidingWindowSettingsWidget::getState() const {
    QVariantMap state;

    const QString windowSizeString = leWindowSize->text();
    const bool isEmpty = windowSizeString.isEmpty();
    bool valid = false;
    const int windowSize = windowSizeString.toInt(&valid);

    if (!isEmpty && valid) {
        state[WINDOW_SIZE] = windowSize;
    }
    state[REQUIRED_QUALITY] = sbQualityThreshold->value();

    return state;
}

void SlidingWindowSettingsWidget::setState(const QVariantMap &state) {
    bool contains = state.contains(WINDOW_SIZE);
    bool valid = false;
    const int windowSize = state.value(WINDOW_SIZE).toInt(&valid);
    if (contains && valid) {
        leWindowSize->setText(QString::number(windowSize));
    }

    contains = state.contains(REQUIRED_QUALITY);
    const int requiredQuality = state.value(REQUIRED_QUALITY).toInt(&valid);
    if (contains && valid) {
        sbQualityThreshold->setValue(requiredQuality);
    }
}

SlidingWindowStepFactory::SlidingWindowStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

SlidingWindowStep *SlidingWindowStepFactory::createStep() const {
    return new SlidingWindowStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
