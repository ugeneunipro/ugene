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

#ifndef _U2_SCHEMA_ESTIMATION_TASK_H_
#define _U2_SCHEMA_ESTIMATION_TASK_H_

#include <U2Core/Task.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/Schema.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT EstimationResult {
public:
    EstimationResult();

    int timeSec;
    int ramMb;
    int hddMb;
    int cpuCount;
};

class U2LANG_EXPORT SchemaEstimationTask : public Task {
    Q_OBJECT
public:
    SchemaEstimationTask(const Schema *schema, const Metadata *meta);

    void run();

    EstimationResult result() const;

private:
    const Schema *schema;
    const Metadata *meta;
    EstimationResult er;
};

class ExtimationsUtilsClass : public QObject {
    Q_OBJECT
public:
    ExtimationsUtilsClass(QScriptEngine &engine, const Schema *schema);

public slots:
    QScriptValue attributeValue(const QString &attrStr);
    void test(QScriptValue v);
    qint64 fileSize(const QString &url);
    QString fileFormat(const QString &url);
    bool testAttr(const QString &attrId);
    qint64 bowtieIndexSize(const QString &dir, const QString &name, int versionId);

private:
    QStringList parseTokens(const QString &attrStr, U2OpStatus &os);
    Attribute * getAttribute(const QString &attrStr, U2OpStatus &os);
    QScriptValue prepareDatasets(const QList<Dataset> &sets);
    void checkFile(const QString &url, U2OpStatus &os);

private:
    QScriptEngine &engine;
    const Schema *schema;
};

} // Workflow
} // U2

#endif // _U2_SCHEMA_ESTIMATION_TASK_H_
