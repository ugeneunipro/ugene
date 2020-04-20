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

#include "AbstractAlignmentTask.h"

namespace U2 {

const QString AbstractAlignmentTaskSettings::RESULT_FILE_NAME("resultFileName");
const QString AbstractAlignmentTaskSettings::ALGORITHM_NAME("algorithmName");
const QString AbstractAlignmentTaskSettings::REALIZATION_NAME("realizationName");
const QString AbstractAlignmentTaskSettings::IN_NEW_WINDOW("inNewWindow");
const QString AbstractAlignmentTaskSettings::ALPHABET("alphabet");

AbstractAlignmentTaskSettings::AbstractAlignmentTaskSettings() : inNewWindow(true) {
}

AbstractAlignmentTaskSettings::AbstractAlignmentTaskSettings(const QVariantMap& someSettings) : inNewWindow(true), customSettings(someSettings) {
}

AbstractAlignmentTaskSettings::AbstractAlignmentTaskSettings(const AbstractAlignmentTaskSettings &s) :
    algorithmName(s.algorithmName),
    realizationName(s.realizationName),
    inNewWindow(s.inNewWindow),
    msaRef(s.msaRef),
    alphabet(s.alphabet),
    resultFileName(s.resultFileName),
    customSettings(s.customSettings) {
}

AbstractAlignmentTaskSettings::~AbstractAlignmentTaskSettings() {
}

QVariant AbstractAlignmentTaskSettings::getCustomValue(const QString& optionName, const QVariant& defaultVal) const
{
    return customSettings.value(optionName, defaultVal);
}

void AbstractAlignmentTaskSettings::setCustomValue(const QString& optionName, const QVariant& val)
{
    customSettings.insert(optionName,val);
}

bool AbstractAlignmentTaskSettings::convertCustomSettings() {
    if (customSettings.contains(ALGORITHM_NAME)) {
        algorithmName = customSettings.value(ALGORITHM_NAME).toString();
        customSettings.remove(ALGORITHM_NAME);
    }
    if (customSettings.contains(REALIZATION_NAME)) {
        realizationName = customSettings.value(REALIZATION_NAME).toString();
        customSettings.remove(REALIZATION_NAME);
    }
    if (customSettings.contains(RESULT_FILE_NAME) && customSettings.value(RESULT_FILE_NAME).type() == QVariant::String) {
        resultFileName = GUrl(customSettings.value(RESULT_FILE_NAME).toString());
        customSettings.remove(RESULT_FILE_NAME);
    }
    if (customSettings.contains(IN_NEW_WINDOW)) {
        inNewWindow = customSettings.value(IN_NEW_WINDOW).toBool();
        customSettings.remove(IN_NEW_WINDOW);
    }
    return true;
}

bool AbstractAlignmentTaskSettings::containsValue(const QString& optionName) const {
    return customSettings.contains(optionName);
}

void AbstractAlignmentTaskSettings::setResultFileName(const QString& newFileName) {
    resultFileName = GUrl(newFileName);
}

void AbstractAlignmentTaskSettings::setCustomSettings(const QMap<QString, QVariant>& settings) {
    customSettings = settings;
}

void AbstractAlignmentTaskSettings::appendCustomSettings(const QVariantMap& settings) {
    foreach (const QString& key, settings.keys()) {
        customSettings.insert(key, settings.value(key));
    }
}

bool AbstractAlignmentTaskSettings::isValid() const {
    return msaRef.isValid() && alphabet.isValid() &&
            (!resultFileName.isEmpty() || inNewWindow == false);
}

AbstractAlignmentTask::AbstractAlignmentTask(const QString& taskName, TaskFlags flags) : Task(taskName, flags) {
}


AbstractAlignmentTaskFactory::~AbstractAlignmentTaskFactory() {}

}   //namespace
