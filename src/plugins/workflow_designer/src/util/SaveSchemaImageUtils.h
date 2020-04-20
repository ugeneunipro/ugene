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

#ifndef _U2_WORKFLOW_SAVE_SCHEMA_IMAGE_UTILS_H_
#define _U2_WORKFLOW_SAVE_SCHEMA_IMAGE_UTILS_H_

#include <QPixmap>
#include <QDomDocument>

#include <U2Core/Task.h>
#include <U2Lang/Schema.h>

namespace U2 {

using namespace Workflow;

class SaveSchemaImageUtils : public QObject {
    Q_OBJECT
public:
    static QPixmap generateSchemaSnapshot(const QString & data);
    static QString saveSchemaImageToFile(const QString & schemaName, const QString & imagePath);
};

class GoogleChartImage {
public:
    static const QString    CHART_TYPE_OPTION;
    static const QString    GRAPH_OPTION;
    static const QString    CHART_SIZE_OPTION;
    static const QString    GRAPH_VIZ_CHART_TYPE;
    static const QSize      CHART_SIZE_DEFAULT;
    static const QString    GOOGLE_CHART_BASE_URL;

public:
    GoogleChartImage(Schema * sc, const Metadata& meta);
    QString getImageUrl() const;

private:
    QString getUrlArguments() const;

private:
    QSize chartSize;
    Schema * schema;
    Metadata meta;
};

/**
 * Produces link to googlechart api of produced image
 */
class ProduceSchemaImageLinkTask : public Task {
    Q_OBJECT
public:
    ProduceSchemaImageLinkTask(const QString & schemaName);
    ~ProduceSchemaImageLinkTask();

    virtual void prepare();
    virtual ReportResult report();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

    QString getImageLink() const;

private:
    ProduceSchemaImageLinkTask();
    ProduceSchemaImageLinkTask(const ProduceSchemaImageLinkTask &);
    ProduceSchemaImageLinkTask & operator=(const ProduceSchemaImageLinkTask &);

private:
    QString schemaPath;
    Schema * schema;
    Metadata meta;
    QString imageLink;

}; // ProduceSchemaImageLinkTask

} // U2

#endif // _U2_WORKFLOW_SAVE_SCHEMA_IMAGE_UTILS_H_
