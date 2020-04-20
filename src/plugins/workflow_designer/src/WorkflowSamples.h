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

#ifndef _U2_WORKFLOW_SAMPLES_H_
#define _U2_WORKFLOW_SAMPLES_H_

#include <U2Lang/Descriptor.h>
#include <U2Gui/GlassView.h>
#include <U2Core/Task.h>

#include <QAction>
#include <QHBoxLayout>
#include <QToolBox>
#include <QButtonGroup>
#include <QTreeWidget>

namespace U2 {
class WorkflowView;
class WorkflowScene;

class Sample {
public:
    Descriptor d;
    QIcon ico;
    QString data;
    QString content;
    QString id; // UWL file name of the sample workflow
};

class SampleCategory {
public:
    SampleCategory(const QString& dir, const QString& name) : d(dir, name, ""){}
    Descriptor d;
    QList<Sample> items;
};

class LoadSamplesTask : public Task {
    Q_OBJECT
public:
    LoadSamplesTask(const QStringList&);
    void run();
    ReportResult report();
private:
    void scanDir(const QString&, int depth = 0);
    QStringList dirs;
    QList<SampleCategory> result;
    static const int maxDepth;
};

class SampleRegistry {
public:
    static Task* init(const QStringList&);
    static QList<SampleCategory> getCategories() {return data;}
private:
    static QList<SampleCategory> data;
    friend class LoadSamplesTask;
};

class SamplePane : public GlassPane {
    Q_OBJECT
public:
    SamplePane(WorkflowScene *scene);
    virtual void paint(QPainter*);
    virtual bool eventFilter(QObject*, QEvent* e) {
        this->event(e);
        return item != NULL;
    }

public slots:
    void setItem(QTreeWidgetItem *it){item = it;};
signals:
    void itemActivated(QTreeWidgetItem * item);
    void cancel();
protected:
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void keyPressEvent ( QKeyEvent * event );
private:
    QTextDocument*      m_document;
    QTreeWidgetItem*    item;
    WorkflowScene *scene;
};


class SamplesWidget : public QTreeWidget {
    Q_OBJECT
public:
    static const QString MIME_TYPE;

    SamplesWidget(WorkflowScene *scene, QWidget *parent = 0);
    void activateSample(const QString &category, const QString &id);
    void loadSample(const QString &category, const QString &id);

public slots:
    void cancelItem();
    void sl_nameFilterChanged(const QString &nameFilter);
protected:
    void resizeEvent(QResizeEvent *e) {
        QTreeWidget::resizeEvent(e);
        doItemsLayout();
    }

private slots:
    void handleTreeItem(QTreeWidgetItem * item);
    void activateItem(QTreeWidgetItem * item);
    void sl_refreshSampesItems();

signals:
    void setupGlass(GlassPane*);
    void sampleSelected(const QString&);
private:
    QTreeWidgetItem * getSampleItem(const QString &category, const QString &id);
    void addCategory(const SampleCategory& cat);
    void revisible(const QString &nameFilter);

    SamplePane* glass;
};

class NameFilterLayout : public QHBoxLayout {
    Q_OBJECT
public:
    NameFilterLayout(QWidget *parent);

    QLineEdit * getNameEdit() const;

    static bool filterMatched(const QString &nameFilter, const QString &name);

private:
    QLineEdit *nameEdit;
    QAction *delTextAction;
};

class SamplesWrapper : public QWidget {
public:
    SamplesWrapper(SamplesWidget *samples, QWidget *parent);
};

}//namespace

#endif
