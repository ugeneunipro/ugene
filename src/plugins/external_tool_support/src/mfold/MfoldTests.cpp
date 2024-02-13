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
#include "MfoldTests.h"

#include <QDomElement>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include "MfoldTask.h"

namespace U2 {
const QString inpAttr = "inp";
const QString regionAttr = "region";
const QString failAttr = "shouldFail";
const QString messageAttr = "message";
const QString temperatureSetting = "T";
const QString naSetting = "NA_CONC";
const QString mgSetting = "MG_CONC";
const QString percentSetting = "P";
const QString maxFoldSetting = "MAX";
const QString windowSetting = "W";
const QString maxBpSetting = "MAXBP";
const QString labFrSetting = "LAB_FR";
const QString rotAngSetting = "ROT_ANG";

void GTest_Mfold::init(XMLTestFormat*, const QDomElement& el) {
    // Input sequence parsing.
    checkNecessaryAttributeExistence(el, inpAttr);
    CHECK_OP(stateInfo, );
    inpCtx = el.attribute(inpAttr);

    // Region parsing.
    checkNecessaryAttributeExistence(el, regionAttr);
    CHECK_OP(stateInfo, );
    auto regionStr = el.attribute(regionAttr);
    auto regionBytes = regionStr.toLocal8Bit();
    U2Location location;
    using Genbank::LocationParser;
    CHECK_EXT(LocationParser::parseLocation(regionBytes.constData(), regionBytes.length(), location) ==
                  LocationParser::ParsingResult::Success,
              wrongValue(regionStr), );
    CHECK_EXT(location->regions.size() == 1, wrongValue(regionStr), );
    settings.region->regions += location->regions[0];

    // shouldFail
    if (el.hasAttribute(failAttr)) {
        checkBooleanAttribute(el, failAttr, false);
        CHECK_OP(stateInfo, );
        shouldFail = el.attribute(failAttr) == "true";
    }

    // Mfold script parameters parsing.
    if (el.hasAttribute(temperatureSetting)) {
        settings.algoSettings.temperature = getInt(el, temperatureSetting);
    }
    if (el.hasAttribute(percentSetting)) {
        settings.algoSettings.percent = getInt(el, percentSetting);
    }
    if (el.hasAttribute(maxFoldSetting)) {
        settings.algoSettings.maxFold = getInt(el, maxFoldSetting);
    }
    if (el.hasAttribute(windowSetting)) {
        settings.algoSettings.window = getInt(el, windowSetting);
    }
    if (el.hasAttribute(maxBpSetting)) {
        settings.algoSettings.maxBp = getInt(el, maxBpSetting);
    }
    if (el.hasAttribute(labFrSetting)) {
        settings.algoSettings.labFr = getInt(el, labFrSetting);
    }
    if (el.hasAttribute(naSetting)) {
        settings.algoSettings.naConc = getDouble(el, naSetting);
    }
    if (el.hasAttribute(mgSetting)) {
        settings.algoSettings.mgConc = getDouble(el, mgSetting);
    }
    if (el.hasAttribute(rotAngSetting)) {
        settings.algoSettings.rotAng = getDouble(el, rotAngSetting);
    }

    // Message for checking in log.
    if (el.hasAttribute(messageAttr)) {
        logExpectedMessages += el.attribute(messageAttr);
    }
}

void GTest_Mfold::prepare() {
    auto seqCtx = getContext<U2SequenceObject>(this, inpCtx);
    CHECK_EXT(seqCtx != nullptr, stateInfo.setError(QString("Context `%1` not found").arg(inpCtx)), );
    settings.outSettings.outPath = env->getVar("TEMP_DATA_DIR");

    if (!logExpectedMessages.isEmpty()) {
        logHelper.initMessages(logExpectedMessages, {});
    }

    // We use wide screen for default image resolution.
    auto t = createMfoldTask(seqCtx, settings, 1920, stateInfo);
    CHECK_OP(stateInfo, );
    addSubTask(t);
}

QList<Task*> GTest_Mfold::onSubTaskFinished(Task* subTask) {
    if (!shouldFail && subTask->hasError()) {
        setError(QString("Mfold failed with error `%1`, but it shouldn't").arg(subTask->getError()));
        return {};
    }
    if (shouldFail && !subTask->hasError()) {
        setError(QString("Mfold completed successfully, but failure was expected").arg(subTask->getError()));
        return {};
    }
    if (!logExpectedMessages.isEmpty() && logHelper.verifyStatus() != GTest_LogHelper_Valid) {
        setError(QString("Messages `%1` not found in log").arg(logExpectedMessages.join("` `")));
        return {};
    }
    return {};
}

Task::ReportResult GTest_Mfold::report() {
    return ReportResult_Finished;
}

QList<XMLTestFactory*> MfoldTests::createTestFactories() {
    return {GTest_Mfold::createFactory()};
}
}  // namespace U2
