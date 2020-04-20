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

#include "QDSamples.h"
#include "QDDocument.h"
#include "QDSceneIOTasks.h"
#include "QueryViewController.h"

#include <U2Designer/WorkflowGUIUtils.h>

#include <QDir>
#include <QMouseEvent>
#include <QTextDocument>


Q_DECLARE_METATYPE(QTextDocument*);

namespace U2 {

QList<QDSample> QDSamplesRegistry::data;

QDLoadSamplesTask::QDLoadSamplesTask(const QStringList& _dirs)
: Task(tr("Load query samples"), TaskFlag_NoRun) {
    foreach(const QString& s, _dirs) {
        QDir dir(s);
        QStringList names(QString("*.%1").arg(QUERY_SCHEME_EXTENSION));
        foreach(const QFileInfo& fi, dir.entryInfoList(names, QDir::Files|QDir::NoSymLinks)) {
            const QString& file = fi.absoluteFilePath();
            QDLoadDocumentTask* t = new QDLoadDocumentTask(file);
            addSubTask(t);
            idMap[t] = file;
        }
    }
}

QList<Task*> QDLoadSamplesTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> st;
    if (isCanceled()) {
        return st;
    }
    QDLoadDocumentTask* loadTask = qobject_cast<QDLoadDocumentTask*>(subTask);
    assert(loadTask);
    QDSample sample;
    sample.content = loadTask->getDocument();
    if (sample.content) {
        sample.d.setId(idMap.value(loadTask));
        sample.d.setDisplayName(sample.content->getName());
        sample.d.setDocumentation(sample.content->getDocDesc());
        result.append(sample);
    }
    return st;
}

Task::ReportResult QDLoadSamplesTask::report() {
    QDSamplesRegistry::data = result;
    return ReportResult_Finished;
}

#define DATA_ROLE Qt::UserRole
#define DOC_ROLE Qt::UserRole + 1

QDSamplesWidget::QDSamplesWidget(QueryScene* scene, QWidget* parent/* =NULL */)
: QListWidget(parent) {
    setWordWrap(true);
    foreach(const QDSample& sample, QDSamplesRegistry::getSamples()) {
        addSample(sample);
    }

    glass = new QDSamplePane(scene);

    connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(sl_onItemChanged(QListWidgetItem*)));
    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(sl_onItemSelected(QListWidgetItem*)));
    connect(glass, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(sl_onItemSelected(QListWidgetItem*)));
}

void QDSamplesWidget::addSample(const QDSample& sample) {
    QListWidgetItem* item = new QListWidgetItem(sample.d.getDisplayName(), this);
    item->setData(DATA_ROLE, qVariantFromValue<QDDocument*>(sample.content));
    QTextDocument* txtDoc = new QTextDocument(this);
    QIcon ico;
    ico.addPixmap(QDUtils::generateSnapShot(sample.content, QRect()));
    DesignerGUIUtils::setupSamplesDocument(sample.d, ico, txtDoc);
    item->setData(DOC_ROLE, qVariantFromValue<QTextDocument*>(txtDoc));
}

void QDSamplesWidget::sl_onItemChanged(QListWidgetItem* item) {
    if (item && !item->data(DATA_ROLE).isValid()) {
        item = NULL;
    }
    assert(glass);
    glass->setItem(item);
    emit setupGlass(glass);
}

void QDSamplesWidget::sl_onItemSelected(QListWidgetItem* item) {
    QDDocument* doc = item->data(DATA_ROLE).value<QDDocument *>();
    assert(doc);
    emit itemActivated(doc);
}

void QDSamplesWidget::sl_cancel() {
    selectionModel()->clear();
    if (isHidden()) {
        emit setupGlass(NULL);
        glass->setItem(NULL);
    } else {
        emit setupGlass(glass);
    }
}

QDSamplePane::QDSamplePane(QueryScene* _scene) : scene(_scene), current(NULL) {}

void QDSamplePane::paint(QPainter* painter) {
    if (!current && scene->getScheme()->getActors().isEmpty()) {
        DesignerGUIUtils::paintSamplesArrow(painter);
        return;
    }

    if (current) {
        QTextDocument* doc = current->data(DOC_ROLE).value<QTextDocument*>();
        DesignerGUIUtils::paintSamplesDocument(painter, doc, width(), height(), palette());
    }
}

void QDSamplePane::mouseDoubleClickEvent(QMouseEvent* e) {
    if (current == NULL) {
        return;
    }
    QTextDocument* doc = current->data(DOC_ROLE).value<QTextDocument*>();
    int pageWidth = qMax(width() - 100, 100);
    int pageHeight = qMax(height() - 100, 100);
    if (pageWidth != doc->pageSize().width()) {
        doc->setPageSize(QSize(pageWidth, pageHeight));
    }

    QSize ts = doc->size().toSize();
    QRect textRect;
    textRect.setSize(ts);

    QPoint position = e->pos();
    if(textRect.contains(position)) {
        emit itemActivated(current);
    }
    else {
        current = NULL;
        scene->update();
    }
}

}//namespace
