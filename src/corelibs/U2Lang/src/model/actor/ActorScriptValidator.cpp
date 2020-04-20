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

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowUtils.h>

#include "ActorScriptValidator.h"

namespace U2 {
using namespace WorkflowSerialize;
namespace Workflow {

/************************************************************************/
/* ActorScriptValidator */
/************************************************************************/
bool ActorScriptValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const {
    QScriptEngine engine;
    ValidationContext context(engine, actor);
    {
        QScriptValue ctx = engine.newQObject(&context, QScriptEngine::QtOwnership);
        engine.globalObject().setProperty("ctx", ctx);
    }

    engine.evaluate(options[Constants::V_SCRIPT]);
    if (engine.hasUncaughtException()) {
        coreLog.error(QObject::tr("Exception during script execution! Line: %1, error: %2")
            .arg(engine.uncaughtExceptionLineNumber())
            .arg(engine.uncaughtException().toString()));
        return true;
    }

    notificationList << context.notifications();
    return !context.hasErrors();
}

/************************************************************************/
/* ValidationContext */
/************************************************************************/
ValidationContext::ValidationContext(QScriptEngine &engine, const Actor *actor)
: engine(engine), actor(actor)
{

}

QScriptValue ValidationContext::attributeValue(const QString &attrId) {
    Attribute *attr = actor->getParameter(attrId);
    if (NULL == attr) {
        engine.evaluate("throw \"" + tr("Wrong attribute id: ") + attrId + "\"");
        return QScriptValue::NullValue;
    }

    QVariant value = attr->getAttributePureValue();
    if (value.type() == QVariant::Bool) {
        return QScriptValue(value.toBool());
    } else if (value.canConvert< QList<Dataset> >()) {
        return WorkflowUtils::datasetsToScript(value.value< QList<Dataset> >(), engine);
    }
    return engine.newVariant(value);
}

void ValidationContext::error(const QString &message) {
    errors << message;
}

void ValidationContext::warning(const QString &message) {
    warnings << message;
}

NotificationsList ValidationContext::notifications() const {
    NotificationsList result;
    foreach (const QString &e, errors) {
        result << WorkflowNotification(e, actor->getId(), WorkflowNotification::U2_ERROR);
    }
    foreach (const QString &w, warnings) {
        result << WorkflowNotification(w, actor->getId(), WorkflowNotification::U2_WARNING);
    }
    return result;
}

bool ValidationContext::hasErrors() const {
    return !errors.isEmpty();
}

} // Workflow
} // U2
