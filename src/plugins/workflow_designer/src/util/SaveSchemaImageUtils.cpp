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

#include <QFile>
#include <QPainter>

#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowIOTasks.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "SaveSchemaImageUtils.h"
#include "WorkflowViewController.h"
#include "WorkflowViewItems.h"

namespace U2 {

static Logger log("Save workflow image task");

/********************************
 * ProduceSchemaImageLinkTask
 ********************************/
ProduceSchemaImageLinkTask::ProduceSchemaImageLinkTask(const QString & schemaName)
: Task(tr("Save workflow image"), TaskFlags_NR_FOSCOE), schema(NULL) {

    schemaPath = WorkflowUtils::findPathToSchemaFile( schemaName );
    if( schemaPath.isEmpty() ) {
        setError( tr( "Cannot find workflow: %1" ).arg( schemaName ) );
        return;
    }
}

ProduceSchemaImageLinkTask::~ProduceSchemaImageLinkTask() {
    delete schema;
}

void ProduceSchemaImageLinkTask::prepare() {
    if(hasError() || isCanceled()) {
        return;
    }

    schema = new Schema();
    schema->setDeepCopyFlag(true);
    addSubTask(new LoadWorkflowTask( schema, &meta, schemaPath ));
}

QList<Task*> ProduceSchemaImageLinkTask::onSubTaskFinished(Task* subTask) {
    LoadWorkflowTask * loadTask = qobject_cast<LoadWorkflowTask*>(subTask);
    assert(loadTask != NULL);

    QList<Task*> res;
    if( loadTask->hasError() || loadTask->isCanceled() ) {
        return res;
    }

    GoogleChartImage googleImg(schema, meta);
    imageLink = googleImg.getImageUrl();

    return res;
}

QString ProduceSchemaImageLinkTask::getImageLink() const {
    return imageLink;
}

Task::ReportResult ProduceSchemaImageLinkTask::report() {
    return ReportResult_Finished;
}

/********************************
 * GoogleChartImage
 ********************************/

const QString GoogleChartImage::CHART_TYPE_OPTION = "cht";
const QString GoogleChartImage::GRAPH_OPTION = "chl";
const QString GoogleChartImage::CHART_SIZE_OPTION = "chs";
const QString GoogleChartImage::GRAPH_VIZ_CHART_TYPE = "gv:dot";
const QSize   GoogleChartImage::CHART_SIZE_DEFAULT(500, 500);
const QString GoogleChartImage::GOOGLE_CHART_BASE_URL = "http://chart.apis.google.com/chart?";

GoogleChartImage::GoogleChartImage(Schema * sc, const Metadata& m) : chartSize(CHART_SIZE_DEFAULT), schema(sc), meta(m) {
    assert(schema != NULL);
}

QString GoogleChartImage::getImageUrl() const {
    return GOOGLE_CHART_BASE_URL + getUrlArguments();
}

static QString makeArgumentPair( const QString & argName, const QString & value ) {
    return argName + "=" + value + "&";
}

//static QString getSchemaGraphInSimpleDotNotation(Schema * schema, const Metadata & meta) {
//    assert(schema != NULL);
//    QString graph = "digraph{";
//    graph += QString("label=\"Schema %1\"").arg(meta.name);
//    foreach(Link * link, schema->getFlows()) {
//        assert(link != NULL);
//        Actor * source = link->source()->owner();
//        Actor * destination = link->destination()->owner();
//        assert(source != NULL && destination != NULL);
//        graph += QString("%1->%2;").arg("\"" + source->getLabel() + "\"").arg("\"" + destination->getLabel() + "\"");
//    }
//    graph = graph.mid(0, graph.size() - 1);
//    return graph + "}";
//}

static QString getSchemaGraphInExtendedDotNotation(Schema * schema, const Metadata & meta) {
    assert(schema != NULL);
    QString graph = "digraph{";
    graph += QString("label=\"Workflow %1\";").arg(meta.name);
    graph += QString("compound=true;");
    graph += QString("rankdir=LR;");
    graph += QString("bgcolor=white;");
    graph += QString("edge [arrowsize=1, color=black];");
    graph += QString("node [shape=box,style=\"filled, rounded\",fillcolor=lightblue];");

    // Nodes definition
    foreach(Actor * actor, schema->getProcesses()) {
        graph += QString("%1 [label=\"%2\"];").arg(QString("node_%1").arg(actor->getId())).arg(actor->getLabel());
    }
    // relationships definition
    foreach(Link * link, schema->getFlows()) {
        Actor * source = link->source()->owner();
        Actor * destination = link->destination()->owner();
        graph += QString("node_%1->node_%2;").arg(source->getId()).arg(destination->getId());
    }

    graph = graph.mid(0, graph.size() - 1);
    return graph + "}";
}

//static QString getSizeStr(const QSize & sz) {
//    return QString("%1x%2").arg(sz.width()).arg(sz.height());
//}

QString GoogleChartImage::getUrlArguments() const {
    QString res;
    res += makeArgumentPair(CHART_TYPE_OPTION, GRAPH_VIZ_CHART_TYPE);
    res += makeArgumentPair(GRAPH_OPTION, getSchemaGraphInExtendedDotNotation(schema, meta));
    //res += makeArgumentPair(CHART_SIZE_OPTION, getSizeStr(chartSize));
    return res.mid(0, res.size() - 1);
}

/********************************
 * SaveSchemaImageUtils
 ********************************/
QPixmap SaveSchemaImageUtils::generateSchemaSnapshot(const QString & data) {
    Schema schema;
    Metadata meta;
    QString msg = HRSchemaSerializer::string2Schema(data, &schema, &meta);
    if (!msg.isEmpty()) {
        log.trace(QString("Snapshot issues: cannot read scene: '%1'").arg(msg));
        return QPixmap();
    }
    SceneCreator sc(&schema, meta);
    QScopedPointer<WorkflowScene> scene(sc.createScene(NULL));

    QRectF bounds = scene->itemsBoundingRect();
    CHECK(!bounds.isEmpty(), QPixmap());

    QPixmap pixmap(bounds.size().toSize());
    pixmap.fill();
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter, QRectF(), bounds);
    return pixmap;
}

QString SaveSchemaImageUtils::saveSchemaImageToFile(const QString & schemaPath, const QString & imagePath) {
    log.info(QString("Saving %1 snapshot to %2").arg(schemaPath).arg(imagePath));

    QFile file(schemaPath);
    if(!file.open(QIODevice::ReadOnly)) {
        return L10N::errorOpeningFileRead(schemaPath);
    }

    QByteArray rawData = file.readAll();
    QPixmap image = generateSchemaSnapshot(rawData);
    image.save(imagePath, "png");
    return QString();
}

} // U2
