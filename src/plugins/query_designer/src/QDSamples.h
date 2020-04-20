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

#ifndef _U2_QD_SAMPLES_H_
#define _U2_QD_SAMPLES_H_

#include <U2Core/Task.h>

#include <U2Lang/Descriptor.h>

#include <U2Gui/GlassView.h>

#include <QListWidget>

namespace U2 {

class QDDocument;
class QueryScene;

class QDSample {
public:
    Descriptor d;
    QDDocument* content;
};

class QDLoadSamplesTask : public Task {
    Q_OBJECT
public:
    QDLoadSamplesTask(const QStringList& _dirs);
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    ReportResult report();
private:
    QMap<Task*, QString> idMap;
    QList<QDSample> result;
};

class QDSamplesRegistry {
public:
    static const QList<QDSample>& getSamples() { return data; }
private:
    static QList<QDSample> data;
    friend class QDLoadSamplesTask;
};

class QDSamplePane : public GlassPane {
    Q_OBJECT
public:
    QDSamplePane(QueryScene* _scene);
    virtual void paint(QPainter* painter);
    virtual bool eventFilter(QObject*, QEvent* e) {
        this->event(e);
        return current != NULL;
    }
    void setItem(QListWidgetItem* item) { current=item; }
signals:
    void itemActivated(QListWidgetItem* item);
protected:
    void mouseDoubleClickEvent(QMouseEvent* e);
private:
    QueryScene* scene;
    QListWidgetItem* current;
};

class QDSamplesWidget : public QListWidget {
    Q_OBJECT
public:
    QDSamplesWidget(QueryScene* scene, QWidget* parent=NULL);
signals:
    void itemActivated(QDDocument*);
    void setupGlass(GlassPane* glass);
private:
    void addSample(const QDSample& sample);
private slots:
    void sl_onItemChanged(QListWidgetItem* item);
    void sl_onItemSelected(QListWidgetItem* item);
    void sl_cancel();
private:
    QDSamplePane* glass;

};

}//namespace

#endif
