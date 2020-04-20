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

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowUtils.h>

#include "SchemaEstimationTask.h"

namespace U2 {
namespace Workflow {

EstimationResult::EstimationResult() {
    timeSec = -1;
    ramMb = -1;
    hddMb = -1;
    cpuCount = -1;
}

SchemaEstimationTask::SchemaEstimationTask(const Schema *_schema, const Metadata *_meta)
: Task(tr("Workflow estimation task"), TaskFlag_None), schema(_schema), meta(_meta)
{

}

void SchemaEstimationTask::run() {
    QScriptEngine engine;
    {
        ExtimationsUtilsClass *utilsClass = new ExtimationsUtilsClass(engine, schema);
        QScriptValue utils = engine.newQObject(utilsClass, QScriptEngine::ScriptOwnership);
        engine.globalObject().setProperty("utils", utils);
    }

    QScriptValue result = engine.evaluate(meta->estimationsCode);
    if (engine.hasUncaughtException()) {
        setError(tr("Exception during script execution! Line: %1, error: %2")
            .arg(engine.uncaughtExceptionLineNumber())
            .arg(engine.uncaughtException().toString()));
        return;
    }

    er.timeSec = result.property(0).toInt32();
    if (result.property("length").toInt32() > 1) {
        er.ramMb = result.property(1).toInt32();
    }
    //er.hddMb = result.property(2).toInt32();
    //er.cpuCount = result.property(3).toInt32();
}

EstimationResult SchemaEstimationTask::result() const {
    return er;
}

/************************************************************************/
/* Utils */
/************************************************************************/
ExtimationsUtilsClass::ExtimationsUtilsClass(QScriptEngine &_engine, const Schema *_schema)
: engine(_engine), schema(_schema)
{

}

QStringList ExtimationsUtilsClass::parseTokens(const QString &attrStr, U2OpStatus &os) {
    QStringList tokens = attrStr.split(".");
    if (2 != tokens.size()) {
        os.setError("Bad attribute string: " + attrStr);
    }
    return tokens;
}

Attribute * ExtimationsUtilsClass::getAttribute(const QString &attrStr, U2OpStatus &os) {
    QStringList tokens = parseTokens(attrStr, os);
    CHECK_OP(os, NULL);

    Actor *actor = schema->actorById(tokens[0]);
    if (NULL == actor) {
        os.setError("Unknown actor id: " + tokens[0]);
        return NULL;
    }

    Attribute *attr = actor->getParameter(tokens[1]);
    if (NULL == attr) {
        os.setError("Wrong attribute string: " + attrStr);
        return NULL;
    }
    return attr;
}

QScriptValue ExtimationsUtilsClass::prepareDatasets(const QList<Dataset> &sets) {
    QScriptValue setsArray = engine.newArray(sets.size());

    for (int setIdx = 0; setIdx < sets.size(); setIdx++) {
        Dataset set = sets[setIdx];
        QScriptValue urls = engine.newArray(set.getUrls().size());
        for (int urlIdx = 0; urlIdx < set.getUrls().size(); urlIdx++) {
            QString url = set.getUrls()[urlIdx]->getUrl();
            urls.setProperty(urlIdx, engine.newVariant(url));
        }
        setsArray.setProperty(setIdx, urls);
    }

    return setsArray;
}

void ExtimationsUtilsClass::checkFile(const QString &url, U2OpStatus &os) {
    QFileInfo info(url);
    if (!info.exists()) {
        os.setError(QObject::tr("The file does not exist: ") + url);
        return;
    }
    if (!info.isFile()) {
        os.setError(QObject::tr("It is not a file: ") + url);
        return;
    }
}

#define CHECK_JS_OP(os, result) \
    if (os.hasError()) { \
        engine.evaluate("throw \"" + os.getError() + "\""); \
        return result; \
    }

#define CHECK_JS(condition, message, result) \
    if (!(condition)) { \
        engine.evaluate("throw \"" + QString(message) + "\""); \
        return result; \
    }

QScriptValue ExtimationsUtilsClass::attributeValue(const QString &attrStr) {
    U2OpStatusImpl os;
    Attribute *attr = getAttribute(attrStr, os);
    CHECK_JS_OP(os, QScriptValue::NullValue);

    QVariant value = attr->getAttributePureValue();
    if (!value.canConvert< QList<Dataset> >()) {
        return engine.newVariant(value);
    }

    return WorkflowUtils::datasetsToScript(value.value< QList<Dataset> >(), engine);
}

qint64 ExtimationsUtilsClass::fileSize(const QString &url) {
    U2OpStatusImpl os;
    checkFile(url, os);
    CHECK_JS_OP(os, -1);

    return QFileInfo(url).size();
}

QString ExtimationsUtilsClass::fileFormat(const QString &url) {
    U2OpStatusImpl os;
    checkFile(url, os);
    CHECK_JS_OP(os, "");

    FormatDetectionConfig cfg;
    cfg.bestMatchesOnly = false;
    cfg.excludeHiddenFormats = false;
    cfg.useImporters = true;
    QList<FormatDetectionResult> result = DocumentUtils::detectFormat(url, cfg);
    CHECK_JS(!result.isEmpty(), tr("Unknown file format: %1").arg(url), "");

    FormatDetectionResult r = result.first();
    if (NULL != r.format) {
        return r.format->getFormatId();
    } else {
        CHECK_JS(NULL != r.importer, "NULL importer", "");
        return r.importer->getId();
    }
}

#define BT1_ID 1
#define BT2_ID 0

qint64 ExtimationsUtilsClass::bowtieIndexSize(const QString &dir, const QString &name, int versionId) {
    QString ext = (BT1_ID == versionId) ? ".ebwt" : ".bt2";
    qint64 result = 0;
    result += fileSize(dir + "/" + name + ".1" + ext);
    result += fileSize(dir + "/" + name + ".2" + ext);
    result += fileSize(dir + "/" + name + ".3" + ext);
    result += fileSize(dir + "/" + name + ".4" + ext);
    result += fileSize(dir + "/" + name + ".rev.1" + ext);
    result += fileSize(dir + "/" + name + ".rev.2" + ext);
    return result;
}

void ExtimationsUtilsClass::test(QScriptValue v) {
    if (v.isNumber()) {
        coreLog.info(QString::number(v.toNumber()));
        return;
    } else if (v.isString()) {
        coreLog.info(v.toString());
        return;
    } else if (v.isBoolean()) {
        bool bv = v.toBoolean();
        coreLog.info(bv ? "true" : "false");
    }
    for (int i=0; i<v.property("length").toInt32(); i++) {
        coreLog.info(v.property(i).toString());
    }
}

bool ExtimationsUtilsClass::testAttr(const QString &attrStr) {
    U2OpStatusImpl os;
    QStringList tokens = parseTokens(attrStr, os);
    CHECK_OP(os, false);

    Actor *actor = schema->actorById(tokens[0]);
    if (NULL == actor) {
        os.setError("Unknown actor id: " + tokens[0]);
        return false;
    }

    Attribute *attr = actor->getParameter(tokens[1]);
    if (NULL == attr) {
        os.setError("Wrong attribute string: " + attrStr);
        return false;
    }
    return WorkflowUtils::isUrlAttribute(attr, actor);
}

} // Workflow
} // U2
