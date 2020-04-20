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

#ifndef _U2_WORKFLOW_EDITOR_H_
#define _U2_WORKFLOW_EDITOR_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowDebugStatus.h>
#include <ui_WorkflowEditorWidget.h>

#include <QShortcutEvent>

class QSortFilterProxyModel;

namespace U2 {
using namespace Workflow;
class ActorCfgModel;
class AttributeDatasetsController;
class SpecialParametersPanel;
class WorkflowView;

class WorkflowEditor : public QWidget, Ui_WorkflowEditorWidget
{
    Q_OBJECT
public:

    WorkflowEditor(WorkflowView *parent);

    QVariant saveState() const;
    void restoreState(const QVariant&);

    void changeScriptMode(bool _mode);

    void setEditable(bool editable);

    bool eventFilter(QObject* object, QEvent* event);

    void setSpecialPanelEnabled(bool isEnabled);
    void commitDatasets(const QString &attrId, const QList<Dataset> &sets);

public slots:
    void editActor(Actor*);
    void editPort(Port*);
    void setDescriptor(Descriptor* d, const QString& hint = QString());
    void edit(Configuration* subject);
    void reset();
    void commit();
    void sendModified();
    void sl_updatePortTable();
    void sl_resizeSplitter(bool);

private slots:
    void finishPropertyEditing();
    void updateEditingData();
    void handleDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void editingLabelFinished();
    void sl_showPropDoc();
    void sl_changeVisibleParameters(bool);
    void sl_changeVisibleInput(bool);
    void sl_changeVisibleOutput(bool);
    void sl_showDoc(const QString&);
    void sl_linkActivated(const QString&);

private:
    void changeSizes(QWidget *w, int h);
    static void removePortTable(QList<QWidget *> &portWidgets);
    void createInputPortTable(Actor* a);
    void createOutputPortTable(Actor* a);

    SpecialParametersPanel *specialParameters;
    WorkflowView* owner;
    QPointer<ConfigurationEditor> custom;
    QWidget* customWidget;
    Configuration* subject;
    Actor* actor;
    friend class SuperDelegate;
    ActorCfgModel* actorModel;
    QSortFilterProxyModel *proxyModel;
    QList<QWidget *> inputPortWidget;
    QList<QWidget *> outputPortWidget;
    int paramHeight, inputHeight, outputHeight;

    bool onFirstTableShow;
};

class SpecialParametersPanel : public QWidget {
    Q_OBJECT
public:
    SpecialParametersPanel(WorkflowEditor *parent);
    virtual ~SpecialParametersPanel();

    void editActor(Actor *a);
    void reset();
    void setDatasetsEnabled(bool isEnabled);
    int contentHeight() const;

signals:
    void si_dataChanged();

private slots:
    void sl_datasetsChanged();

private:
    WorkflowEditor *editor;
    QMap<QString, AttributeDatasetsController*> controllers; // attrId <-> controller
    QMap<QString, QList<Dataset> > sets; // attrId <-> datasets

private:
    void addWidget(AttributeDatasetsController *controller);
    void removeWidget(AttributeDatasetsController *controller);
};


}//namespace

#endif
