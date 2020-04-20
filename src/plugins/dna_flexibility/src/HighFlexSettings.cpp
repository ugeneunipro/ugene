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

#include "HighFlexSettings.h"

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>

#include <QVariant>


namespace U2 {

const double HighFlexSettings::DEFAULT_THRESHOLD = 13.7;

const QString HighFlexSettings::SETTINGS_PATH = "high_flex_settings";

const QString HighFlexSettings::SETTING_WINDOW_SIZE = "window_size";
const QString HighFlexSettings::SETTING_WINDOW_STEP = "window_step";
const QString HighFlexSettings::SETTING_THRESHOLD = "threshold";


HighFlexSettings::HighFlexSettings()
    : windowSize(DEFAULT_WINDOW_SIZE),
      windowStep(DEFAULT_WINDOW_STEP),
      threshold(DEFAULT_THRESHOLD)
{
    bool result = restoreRememberedSettings();
    if (!result)
    {
        windowSize = DEFAULT_WINDOW_SIZE;
        windowStep = DEFAULT_WINDOW_STEP;
        threshold = DEFAULT_THRESHOLD;
    }
}


void HighFlexSettings::restoreDefaults()
{
    windowSize = DEFAULT_WINDOW_SIZE;
    windowStep = DEFAULT_WINDOW_STEP;
    threshold = DEFAULT_THRESHOLD;

    AppContext::getSettings()->remove(SETTINGS_PATH + "/" + SETTING_WINDOW_SIZE);
    AppContext::getSettings()->remove(SETTINGS_PATH + "/" + SETTING_WINDOW_STEP);
    AppContext::getSettings()->remove(SETTINGS_PATH + "/" + SETTING_THRESHOLD);
}


void HighFlexSettings::rememberSettings()
{
    AppContext::getSettings()->setValue(SETTINGS_PATH + "/" + SETTING_WINDOW_SIZE, windowSize);
    AppContext::getSettings()->setValue(SETTINGS_PATH + "/" + SETTING_WINDOW_STEP, windowStep);
    AppContext::getSettings()->setValue(SETTINGS_PATH + "/" + SETTING_THRESHOLD, threshold);
}


bool HighFlexSettings::restoreRememberedSettings()
{
    bool convertionSuccessful = false;

    // Window size
    QVariant windowSizeVariant = AppContext::getSettings()->getValue(
        SETTINGS_PATH + "/" + SETTING_WINDOW_SIZE,
        DEFAULT_WINDOW_SIZE); // The second parameter has no actual meaning in this case

    windowSize = windowSizeVariant.toInt(&convertionSuccessful);
    if (!convertionSuccessful)
    {
        ioLog.error(QObject::tr("Invalid high flex setting window size is stored in the application settings."));
        return false;
    }

    // Window step
    QVariant windowStepVariant = AppContext::getSettings()->getValue(
        SETTINGS_PATH + "/" + SETTING_WINDOW_STEP,
        DEFAULT_WINDOW_STEP);

    windowStep = windowStepVariant.toInt(&convertionSuccessful);
    if (!convertionSuccessful)
    {
        ioLog.error(QObject::tr("Invalid high flex setting window step is stored in the application settings."));
        return false;
    }

    // Threshold
    QVariant thresholdVariant = AppContext::getSettings()->getValue(
        SETTINGS_PATH + "/" + SETTING_THRESHOLD,
        DEFAULT_THRESHOLD);

    threshold = thresholdVariant.toDouble(&convertionSuccessful);
    if (!convertionSuccessful)
    {
        ioLog.error(QObject::tr("Invalid high flex setting threshold is stored in the application settings."));
        return false;
    }

    // All values have been successfully converted
    return true;
}


} // namespace
