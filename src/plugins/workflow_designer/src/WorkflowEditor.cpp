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

#include "WorkflowEditor.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollArea>

#include <U2Core/Counter.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DatasetsController.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/MapDatatypeEditor.h>
#include <U2Lang/URLAttribute.h>
#include <U2Lang/WorkflowUtils.h>

#include "ActorCfgFilterProxyModel.h"
#include "ActorCfgModel.h"
#include "WorkflowEditorDelegates.h"
#include "WorkflowViewController.h"

#define MAIN_SPLITTER "main.splitter"
#define TAB_SPLITTER "tab.splitter"

namespace U2 {

WorkflowEditor::WorkflowEditor(WorkflowView* p)
    : QWidget(p),
      owner(p),
      custom(nullptr),
      customWidget(nullptr),
      subject(nullptr),
      onFirstTableShow(true) {
    GCOUNTER(cvar, "WorkflowEditor");
    setupUi(this);

    specialParameters = new SpecialParametersPanel(this);
    tableSplitter->insertWidget(0, specialParameters);
    specialParameters->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    table->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    specialParameters->hide();

    if (isOsMac()) {
        QString style("QGroupBox::title {margin-bottom: 9px;}");
        editorBox->setStyleSheet(style);
    }

    auto inputScrollAreaContainerLayout = new QVBoxLayout();
    inputScrollAreaContainerLayout->setContentsMargins(0, 0, 0, 0);
    inputScrollAreaContainerLayout->setSpacing(0);
    inputScrollAreaContainerLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    inputScrollAreaContainer->setLayout(inputScrollAreaContainerLayout);

    inputPortBox->setEnabled(false);
    inputPortBox->setVisible(true);
    connect(inputPortBox, SIGNAL(toggled(bool)), SLOT(sl_changeVisibleInput(bool)));

    auto outputScrollAreaContainerLayout = new QVBoxLayout();
    outputScrollAreaContainerLayout->setContentsMargins(0, 0, 0, 0);
    outputScrollAreaContainerLayout->setSpacing(0);
    outputScrollAreaContainerLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    outputScrollAreaContainer->setLayout(outputScrollAreaContainerLayout);

    outputPortBox->setEnabled(false);
    outputPortBox->setVisible(true);
    connect(outputPortBox, SIGNAL(toggled(bool)), SLOT(sl_changeVisibleOutput(bool)));

    connect(paramBox, SIGNAL(toggled(bool)), SLOT(sl_changeVisibleParameters(bool)));

    actorModel = new ActorCfgModel(this, owner);
    proxyModel = new ActorCfgFilterProxyModel(this);
    proxyModel->setSourceModel(actorModel);
    table->setModel(proxyModel);

    table->horizontalHeader()->setSectionsClickable(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    table->verticalHeader()->hide();
    table->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 6);
    table->setItemDelegate(new SuperDelegate(this));
    table->installEventFilter(this);

    reset();

    doc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    propDoc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    doc->installEventFilter(this);

    connect(nameEdit, SIGNAL(editingFinished()), SLOT(editingLabelFinished()));

    connect(table->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), SLOT(sl_showPropDoc()));
    connect(table->model(), SIGNAL(dataChanged(QModelIndex, QModelIndex)), SLOT(handleDataChanged(QModelIndex, QModelIndex)));
    // FIXME
    // connect(doc, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_contextMenuForDoc(const QPoint &)));
    table->setTabKeyNavigation(true);
}

void WorkflowEditor::setEditable(bool editable) {
    table->setDisabled(!editable);
    foreach (QWidget* w, inputPortWidget) {
        w->setDisabled(!editable);
    }
    foreach (QWidget* w, outputPortWidget) {
        w->setDisabled(!editable);
    }
}

void WorkflowEditor::sl_updatePortTable() {
    auto a = qobject_cast<Actor*>(sender());
    CHECK(a != nullptr, );

    removePortTable(inputPortWidget);
    removePortTable(outputPortWidget);
    createInputPortTable(a);
    createOutputPortTable(a);
}

void WorkflowEditor::sl_resizeSplitter(bool b) {
    auto w = qobject_cast<QWidget*>(sender());
    int ind = splitter->indexOf(w);
    if (ind != -1) {
        if (!b) {
            splitter->setStretchFactor(ind, 0);
            QList<int> sizes = splitter->sizes();
            sizes[ind] = 0;
            splitter->setSizes(sizes);
        } else {
            if (paramBox == w) {
                changeSizes(paramBox, paramHeight);
            } else {
                int h = w->minimumHeight();
                QList<int> sizes = splitter->sizes();
                sizes[ind] = h;
                sizes[splitter->indexOf(propDoc)] -= h;
                splitter->setSizes(sizes);
            }
        }
    }
}

void WorkflowEditor::changeSizes(QWidget* w, int h) {
    int ind = splitter->indexOf(w);
    if (ind == -1) {
        return;
    } else {
        QList<int> sizes = splitter->sizes();
        sizes[ind] = h;
        sizes[splitter->indexOf(propDoc)] -= h / 2;
        sizes[splitter->indexOf(doc)] -= h / 2;
        splitter->setSizes(sizes);
    }
}

void WorkflowEditor::removePortTable(QList<QWidget*>& portWidgets) {
    qDeleteAll(portWidgets);
    portWidgets.clear();
}

void WorkflowEditor::createInputPortTable(Actor* a) {
    const QList<Port*> enabledPorts = a->getEnabledInputPorts();

    if (!enabledPorts.isEmpty()) {
        inputPortBox->setEnabled(true);
        inputPortBox->setVisible(true);
        inputScrollArea->setVisible(true);
        inputHeight = 0;
        foreach (Port* p, enabledPorts) {
            auto ed = new BusPortEditor(qobject_cast<IntegralBusPort*>(p));
            ed->setParent(p);
            p->setEditor(ed);
            QWidget* w = ed->getWidget();
            inputScrollAreaContainer->layout()->addWidget(w);
            if (!ed->isEmpty()) {
                inputHeight += ed->getOptimalHeight();
            }

            connect(ed, SIGNAL(si_showDoc(const QString&)), SLOT(sl_showDoc(const QString&)));
            inputPortWidget << w;
        }

        if (inputPortBox->isChecked()) {
            changeSizes(inputPortBox, inputHeight);
        } else {
            sl_changeVisibleInput(false);
        }
    } else {
        inputPortBox->setEnabled(false);
        inputPortBox->setVisible(false);
        inputPortBox->resize(0, 0);
    }
}

void WorkflowEditor::createOutputPortTable(Actor* a) {
    const QList<Port*> enabledPorts = a->getEnabledOutputPorts();

    if (!enabledPorts.isEmpty()) {
        outputPortBox->setEnabled(true);
        outputPortBox->setVisible(true);
        outputScrollArea->setVisible(true);
        outputHeight = 0;
        foreach (Port* p, enabledPorts) {
            auto ed = new BusPortEditor(qobject_cast<IntegralBusPort*>(p));
            ed->setParent(p);
            p->setEditor(ed);
            QWidget* w = ed->getWidget();
            outputScrollAreaContainer->layout()->addWidget(w);
            if (!ed->isEmpty()) {
                outputHeight += ed->getOptimalHeight();
            }

            connect(ed, SIGNAL(si_showDoc(const QString&)), SLOT(sl_showDoc(const QString&)));
            outputPortWidget << w;
        }

        if (outputPortBox->isChecked()) {
            changeSizes(outputPortBox, outputHeight);
        } else {
            sl_changeVisibleOutput(false);
        }
    } else {
        outputPortBox->setEnabled(false);
        outputPortBox->setVisible(false);
        outputPortBox->resize(0, 0);
    }
}

void WorkflowEditor::handleDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    if (topLeft == bottomRight) {
        sendModified();
    }
}

void WorkflowEditor::changeScriptMode(bool _mode) {
    if (_mode) {
        GCOUNTER(cvar, "Script. Scripting mode enabled");
    } else {
        GCOUNTER(cvar, "Script. Scripting mode disabled");
    }

    if (table->currentIndex().column() == 2) {
        table->clearSelection();
        table->setCurrentIndex(QModelIndex());
    }
    bool updateRequired = _mode != actorModel->getScriptMode();
    actorModel->changeScriptMode(_mode);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    if ((updateRequired && _mode)) {
        table->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    }
}

void WorkflowEditor::updateEditingData() {
    if (sender()) {
        finishPropertyEditing();
    }
    actorModel->update();
}

void WorkflowEditor::sl_showDoc(const QString& str) {
    propDoc->setText(str);
}

void WorkflowEditor::sl_showPropDoc() {
    QModelIndex current = table->selectionModel()->currentIndex();
    if (current.isValid()) {
        propDoc->setText(WorkflowUtils::getRichDoc(current.data(DescriptorRole).value<Descriptor>()));
    } else {
        propDoc->setText("");
    }
}

void WorkflowEditor::editingLabelFinished() {
    QString newLabel = nameEdit->text();
    if (!actor.isNull() && !newLabel.isEmpty() && newLabel != actor->getLabel()) {
        actor->setLabel(newLabel);
        owner->getScene()->setModified(true);
        owner->refreshView();
    }
}

void WorkflowEditor::reset() {
    caption->setText("");
    nameEdit->hide();
    paramBox->setTitle(tr("Parameters"));
    if (!actor.isNull()) {
        disconnect(actor.data(), &Actor::si_modified, this, &WorkflowEditor::sl_updatePortTable);
    }
    setDescriptor(nullptr);
    edit(nullptr);
    actor = nullptr;
    actorModel->setActor(nullptr);
    propDoc->setText("");

    inputPortBox->setEnabled(false);
    inputPortBox->setVisible(true);
    inputScrollArea->setVisible(false);
    inputPortBox->adjustSize();

    outputPortBox->setEnabled(false);
    outputPortBox->setVisible(true);
    outputScrollArea->setVisible(false);
    outputPortBox->adjustSize();

    paramBox->setEnabled(false);
    paramBox->setVisible(true);
    paramBox->adjustSize();

    QList<int> sizes = splitter->sizes();
    int splitterHeight = splitter->height();
    int indDoc = splitter->indexOf(doc);
    int indPropDoc = splitter->indexOf(propDoc);
    int ind = splitter->indexOf(inputPortBox);
    splitter->setStretchFactor(ind, 0);
    sizes[ind] = 0;
    ind = splitter->indexOf(outputPortBox);
    splitter->setStretchFactor(ind, 0);
    sizes[ind] = 0;
    ind = splitter->indexOf(paramBox);
    splitter->setStretchFactor(ind, 0);
    sizes[ind] = 0;

    sizes[indDoc] = splitterHeight / 2;
    splitter->setStretchFactor(indDoc, 1);
    sizes[indPropDoc] = splitterHeight / 2;
    splitter->setStretchFactor(indPropDoc, 1);
    splitter->setSizes(sizes);

    paramHeight = 0;
    inputHeight = 0;
    outputHeight = 0;
    if (specialParameters != nullptr) {
        specialParameters->setEnabled(false);
        specialParameters->reset();
    }
}

void WorkflowEditor::commitDatasets(const QString& attrId, const QList<Dataset>& sets) {
    assert(!actor.isNull());
    Attribute* attr = actor->getParameter(attrId);
    attr->setAttributeValue(qVariantFromValue<QList<Dataset>>(sets));
    sendModified();
}

void WorkflowEditor::sendModified() {
    uiLog.trace("committing workflow data");
    owner->onModified();
}

void WorkflowEditor::finishPropertyEditing() {
    table->setCurrentIndex(QModelIndex());
}

void WorkflowEditor::commit() {
    finishPropertyEditing();
}

void WorkflowEditor::editActor(Actor* a, const QList<Actor*>& allActors) {
    reset();
    actor = a;
    CHECK(actor, );

    connect(actor.data(), &Actor::si_modified, this, &WorkflowEditor::sl_updatePortTable, Qt::QueuedConnection);

    caption->setText(tr("Element name:"));
    nameEdit->setText(a->getLabel());
    nameEdit->show();
    setDescriptor(a->getProto(), tr("To configure the parameters of the element go to \"Parameters\" area below."));
    edit(a);
    if (specialParameters != nullptr) {
        specialParameters->editActor(a, allActors);
    }

    createInputPortTable(a);
    createOutputPortTable(a);
    paramHeight = table->rowHeight(0) * (table->model()->rowCount() + 3);
    if (specialParameters != nullptr && specialParameters->isVisible()) {
        paramHeight += specialParameters->contentHeight();
    }
    paramBox->setTitle(tr("Parameters"));
    if (paramBox->isChecked()) {
        changeSizes(paramBox, paramHeight);
    }
}

void WorkflowEditor::sl_changeVisibleParameters(bool isChecked) {
    this->tableSplitter->setVisible(isChecked);
    if (!isChecked) {
        paramBox->resize(0, 0);
        changeSizes(paramBox, 0);
    } else {
        changeSizes(paramBox, paramHeight);
    }
    this->paramBox->adjustSize();
}

void WorkflowEditor::sl_changeVisibleInput(bool isChecked) {
    CHECK(!inputPortWidget.isEmpty(), );
    inputScrollArea->setVisible(isChecked);
    if (!isChecked) {
        inputPortBox->resize(0, 0);
        changeSizes(inputPortBox, 0);
    } else {
        changeSizes(inputPortBox, inputHeight);
    }
    inputPortBox->adjustSize();
}

void WorkflowEditor::sl_changeVisibleOutput(bool isChecked) {
    CHECK(!outputPortWidget.isEmpty(), );
    outputScrollArea->setVisible(isChecked);
    if (!isChecked) {
        outputPortBox->resize(0, 0);
        changeSizes(outputPortBox, 0);
    } else {
        changeSizes(outputPortBox, outputHeight);
    }
    outputPortBox->adjustSize();
}

void WorkflowEditor::editPort(Port* p) {
    reset();
    if (p) {
        // caption->setText(formatPortCaption(p));
        QString portDoc = tr("<b>%1 \"%2\"</b> of task \"%3\":<br>%4<br><br>%5")
                              .arg(p->isOutput() ? tr("Output port") : tr("Input port"))
                              .arg(p->getDisplayName())
                              .arg(p->owner()->getLabel())
                              .arg(p->getDocumentation())
                              .arg(tr("You can observe data slots of the port and configure connections if any in the \"Parameters\" widget suited below."));
        doc->setText(portDoc);

        inputPortBox->setEnabled(false);
        outputPortBox->setEnabled(false);
        inputPortBox->setVisible(false);
        outputPortBox->setVisible(false);

        auto ed = new BusPortEditor(qobject_cast<IntegralBusPort*>(p));
        ed->setParent(p);
        p->setEditor(ed);
        paramHeight = ed->getOptimalHeight();

        edit(p);
        bool invisible = (ed->isEmpty() || !p->isEnabled());
        paramBox->setVisible(!invisible);
        if (invisible) {
            paramHeight = 0;
        }
        if (paramBox->isChecked()) {
            changeSizes(paramBox, paramHeight);
        }

        if (p->isInput()) {
            paramBox->setTitle(tr("Input data"));
        } else {
            paramBox->setTitle(tr("Output data"));
        }
    }
}

void WorkflowEditor::setDescriptor(Descriptor* d, const QString& hint) {
    QString text = d ? WorkflowUtils::getRichDoc(*d) + "<br><br>" + hint : hint;
    if (text.isEmpty()) {
        text = tr("Select an element to inspect.");
    }
    doc->setText(text);
}

void WorkflowEditor::edit(Configuration* cfg) {
    paramBox->setEnabled(true);
    if (specialParameters != nullptr) {
        specialParameters->setEnabled(true);
    }
    disconnect(paramBox, SIGNAL(toggled(bool)), tableSplitter, SLOT(setVisible(bool)));

    if (custom != nullptr) {
        custom->commit();
    }
    delete customWidget;

    removePortTable(inputPortWidget);
    removePortTable(outputPortWidget);

    subject = cfg;
    custom = cfg ? cfg->getEditor() : nullptr;
    customWidget = custom ? custom->getWidget() : nullptr;

    if (customWidget) {
        connect(paramBox, SIGNAL(toggled(bool)), customWidget, SLOT(setVisible(bool)));
    }

    if (subject && !customWidget) {
        assert(!actor.isNull());
        actorModel->setActor(actor);
        updateEditingData();
        tableSplitter->setVisible(paramBox->isChecked());
    } else {
        tableSplitter->hide();
        if (customWidget) {
            paramBox->layout()->addWidget(customWidget);
            paramBox->setVisible(!custom->isEmpty());
        }
    }
}

QVariant WorkflowEditor::saveState() const {
    QVariantMap m;
    m.insert(MAIN_SPLITTER, splitter->saveState());
    m.insert(TAB_SPLITTER, tableSplitter->saveState());
    return m;
}

void WorkflowEditor::restoreState(const QVariant& v) {
    QVariantMap m = v.toMap();
    splitter->restoreState(m.value(MAIN_SPLITTER).toByteArray());
    tableSplitter->restoreState(m.value(TAB_SPLITTER).toByteArray());
}

bool WorkflowEditor::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::Show && object == table && onFirstTableShow) {
        // the workaround for correct columns width
        onFirstTableShow = false;
        table->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    }
    if (event->type() == QEvent::Shortcut ||
        event->type() == QEvent::ShortcutOverride) {
        if (object == doc) {
            event->accept();
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}

void WorkflowEditor::sl_linkActivated(const QString& url) {
    const QString& id = WorkflowUtils::getParamIdFromHref(url);

    QModelIndex modelIndex = proxyModel->mapFromSource(actorModel->modelIndexById(id));
    QModelIndex prev = table->selectionModel()->currentIndex();
    if (modelIndex == prev) {
        table->selectionModel()->reset();
    }
    table->setCurrentIndex(modelIndex);
    QWidget* w = table->indexWidget(modelIndex);
    auto pw = dynamic_cast<PropertyWidget*>(w);
    CHECK(pw != nullptr, );
    pw->activate();
}

void WorkflowEditor::setSpecialPanelEnabled(bool isEnabled) {
    if (specialParameters != nullptr) {
        specialParameters->setDatasetsEnabled(isEnabled);
    }
}

/************************************************************************/
/* SpecialParametersPanel */
/************************************************************************/
SpecialParametersPanel::SpecialParametersPanel(WorkflowEditor* parent)
    : QWidget(parent), editor(parent) {
    auto l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    this->setLayout(l);
}

SpecialParametersPanel::~SpecialParametersPanel() {
    qDeleteAll(controllers);
    controllers.clear();
}

void SpecialParametersPanel::editActor(Actor* a, const QList<Actor*>& allActors) {
    reset();
    this->allActors = allActors;
    bool visible = false;
    foreach (const QString& attrId, a->getParameters().keys()) {
        Attribute* attr = a->getParameter(attrId);
        CHECK(attr != nullptr, );
        auto urlAttr = dynamic_cast<URLAttribute*>(attr);
        if (urlAttr == nullptr) {
            continue;
        }
        sets[attrId] = urlAttr->getAttributePureValue().value<QList<Dataset>>();
        controllers[attrId] = new AttributeDatasetsController(sets[attrId], urlAttr->getCompatibleObjectTypes());
        connect(controllers[attrId], SIGNAL(si_attributeChanged()), SLOT(sl_datasetsChanged()));
        connect(controllers[attrId], &DatasetsController::si_datasetRenamed, this, &SpecialParametersPanel::sl_datasetRenamed);
        addWidget(controllers[attrId]);
        visible = true;
    }

    if (visible) {
        this->show();
    }
}

void SpecialParametersPanel::sl_datasetsChanged() {
    auto ctrl = dynamic_cast<AttributeDatasetsController*>(sender());
    CHECK(ctrl != nullptr, );
    CHECK(controllers.values().contains(ctrl), );
    QString attrId = controllers.key(ctrl);
    sets[attrId] = ctrl->getDatasets();
    editor->commitDatasets(attrId, sets[attrId]);
}

void SpecialParametersPanel::sl_datasetRenamed(QPair<QString, QString>& oldNewNamePair) {
    auto ctrl = qobject_cast<AttributeDatasetsController*>(sender());
    CHECK(ctrl != nullptr, );
    CHECK(controllers.values().contains(ctrl), );
    QString attrId = controllers.key(ctrl);
    sets[attrId] = ctrl->getDatasets();
    editor->commitDatasets(attrId, sets[attrId]);
    for (Actor* actor : qAsConst(allActors)) {
        const QStringList keys = actor->getParameters().keys();
        for (const QString& key : qAsConst(keys)) {
            auto urlAttr = dynamic_cast<URLAttribute*>(actor->getParameter(key));
            if (urlAttr == nullptr) {
                continue;
            }
            QList<Dataset> datasetList = urlAttr->getAttributePureValue().value<QList<Dataset>>();
            for (QList<Dataset>::iterator it(datasetList.begin()); it != datasetList.end(); it++) {
                if (it->getName() == oldNewNamePair.first) {
                    it->setName(oldNewNamePair.second);
                    urlAttr->setAttributeValue(qVariantFromValue<QList<Dataset>>(datasetList));
                    break;
                }
            }
        }
    }
}

void SpecialParametersPanel::reset() {
    foreach (AttributeDatasetsController* controller, controllers.values()) {
        removeWidget(controller);
        delete controller;
        controller = nullptr;
    }
    controllers.clear();
    sets.clear();
    this->hide();
}

void SpecialParametersPanel::addWidget(AttributeDatasetsController* controller) {
    CHECK(controller != nullptr, );
    QWidget* newWidget = controller->getWidget();
    if (!editor->isEnabled()) {
        newWidget->setEnabled(false);
    }
    this->layout()->addWidget(newWidget);
}

void SpecialParametersPanel::removeWidget(AttributeDatasetsController* controller) {
    CHECK(controller != nullptr, );
    disconnect(controller, SIGNAL(si_attributeChanged()), this, SLOT(sl_datasetsChanged()));
    disconnect(controller, SIGNAL(si_datasetRenamed(QPair<QString, QString>&)), this, SLOT(sl_datasetRenamed(QPair<QString, QString>&)));
    this->layout()->removeWidget(controller->getWidget());
}

void SpecialParametersPanel::setDatasetsEnabled(bool isEnabled) {
    setEnabled(isEnabled);
    foreach (AttributeDatasetsController* dataset, controllers.values()) {
        dataset->getWidget()->setEnabled(isEnabled);
    }
}

int SpecialParametersPanel::contentHeight() const {
    int result = 0;
    for (int i = 0; i < layout()->count(); i++) {
        QLayoutItem* item = layout()->itemAt(i);
        result += item->widget()->height();
    }
    return result;
}

}  // namespace U2
