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

#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/MainWindow.h>

#include <U2Lang/WizardPage.h>
#include <U2Lang/WorkflowUtils.h>

#include "BowtieWidgetController.h"
#include "ElementSelectorController.h"
#include "PairedDatasetsController.h"
#include "PropertyWizardController.h"
#include "RadioController.h"
#include "SettingsController.h"
#include "TophatSamplesWidgetController.h"
#include "UrlAndDatasetWizardController.h"
#include "WDWizardPage.h"
#include "WizardPageController.h"

#include "WizardController.h"

#define WIZARD_SAFE_POINT_EXT(condition, message, extraOp, result)  \
    if (!(condition)) { \
    coreLog.error(QString("Wizard error: %1").arg(message)); \
    extraOp; \
    return result; \
    } \

#define WIZARD_SAFE_POINT_OP_EXT(os, extraOp, result)  \
    WIZARD_SAFE_POINT_EXT(!os.hasError(), os.getError(), extraOp, result) \

#define WIZARD_SAFE_POINT(condition, message, result)  \
    WIZARD_SAFE_POINT_EXT(condition, message, setBroken(), result) \

#define WIZARD_SAFE_POINT_OP(os, result)  \
    WIZARD_SAFE_POINT_OP_EXT(os, setBroken(), result) \

namespace U2 {

WizardController::WizardController(Schema *s, const Wizard *w)
: QObject(), schema(s), wizard(w), runAfterApply(false)
{
    rejected = false;
    broken = false;
    currentActors = s->getProcesses();
    vars = w->getVariables();
}

WizardController::~WizardController() {
    qDeleteAll(pageControllers);
}

QWizard * WizardController::createGui() {
    QWizard *result = new QWizard((QWidget*)AppContext::getMainWindow()->getQMainWindow());
    result->setAttribute(Qt::WA_DeleteOnClose);
    setupButtons(result);

    int idx = 0;
    foreach (WizardPage *page, wizard->getPages()) {
        result->setPage(idx, createPage(page));
        pageIdMap[page->getId()] = idx;
        idx++;
    }
    result->setWizardStyle(QWizard::ClassicStyle);
    result->setModal(true);
    result->setAutoFillBackground(true);
    result->setWindowTitle(wizard->getName());
    result->setObjectName(wizard->getName());

    QString finishLabel = wizard->getFinishLabel();
    if (finishLabel.isEmpty()) {
        finishLabel = tr("Apply");
    }
    result->setButtonText(QWizard::FinishButton, finishLabel);
    result->setOption(QWizard::NoBackButtonOnStartPage);

    result->installEventFilter(this);

    connect(result, SIGNAL(currentIdChanged(int)), SLOT(sl_pageChanged(int)));
    return result;
}

void WizardController::setupButtons(QWizard *gui) {
    connect(gui, SIGNAL(customButtonClicked(int)), SLOT(sl_customButtonClicked(int)));
    QList<QWizard::WizardButton> order;
    order << QWizard::Stretch << QWizard::BackButton << QWizard::NextButton << QWizard::FinishButton  << QWizard::CancelButton;
    if (!wizard->getHelpPageId().isEmpty()) {
        //TODO: manually add button as in other layouts (mac, win, kde, gnome)
        order << QWizard::HelpButton;
    }

    if (wizard->hasDefaultsButton()) {
        gui->setOption(QWizard::HaveCustomButton2);
        gui->setButtonText(QWizard::CustomButton2, tr("Defaults"));
        gui->button(QWizard::CustomButton2)->setToolTip(tr("Set page values by default"));
        order.prepend(QWizard::CustomButton2);
    }
    if (wizard->hasRunButton()) {
        gui->setOption(QWizard::HaveCustomButton1);
        gui->setButtonText(QWizard::CustomButton1, tr("Run"));
        connect(gui->button(QWizard::CustomButton1), SIGNAL(clicked()), gui, SLOT(accept()));
        order.insert(order.size() - 1, QWizard::CustomButton1);
    }
    gui->setButtonLayout(order);

    if (!wizard->getHelpPageId().isEmpty()) {
        gui->setOption(QWizard::HaveHelpButton, true);
        new HelpButton(this, gui->button(QWizard::HelpButton), wizard->getHelpPageId());
    }
}

bool WizardController::isRunAfterApply() const {
    return runAfterApply;
}

WizardPage * WizardController::findPage(QWizardPage *wPage) {
    foreach (WizardPageController *ctrl, pageControllers) {
        if (ctrl->getQtPage() == wPage) {
            return ctrl->getPage();
        }
    }
    return NULL;
}

void WizardController::run() {
    runAfterApply = true;
}

namespace {
    class WidgetDefaulter : public WizardWidgetVisitor {
    public:
        WidgetDefaulter(WizardController *wc) : wc(wc) {}
        void visit(AttributeWidget *aw) {
            Attribute *attr = wc->getAttribute(aw->getInfo());
            CHECK(NULL != attr, );
            wc->setAttributeValue(aw->getInfo(), attr->getDefaultPureValue());
        }
        void visit(WidgetsArea *wa) {
            foreach (WizardWidget *w, wa->getWidgets()) {
                WidgetDefaulter defaulter(wc);
                w->accept(&defaulter);
            }
        }
        void visit(GroupWidget *gw) {
            visit((WidgetsArea*)gw);
        }
        void visit(LogoWidget *) {}
        void visit(ElementSelectorWidget *) {}
        void visit(PairedReadsWidget *prw) {
            foreach (const AttributeInfo &info, prw->getInfos()) {
                Attribute *attr = wc->getAttribute(info);
                CHECK(NULL != attr, );
                wc->setAttributeValue(info, attr->getDefaultPureValue());
            }
        }
        void visit(UrlAndDatasetWidget *udw) {
            foreach (const AttributeInfo &info, udw->getInfos()) {
                Attribute *attr = wc->getAttribute(info);
                CHECK(NULL != attr, );
                wc->setAttributeValue(info, attr->getDefaultPureValue());
            }
        }
        void visit(RadioWidget *) {}
        void visit(SettingsWidget *) {}
        void visit(BowtieWidget *bw) {
            Attribute *dirAttr = wc->getAttribute(bw->idxDir);
            CHECK(NULL != dirAttr, );
            wc->setAttributeValue(bw->idxDir, dirAttr->getDefaultPureValue());

            Attribute *nameAttr = wc->getAttribute(bw->idxName);
            CHECK(NULL != nameAttr, );
            wc->setAttributeValue(bw->idxName, nameAttr->getDefaultPureValue());
        }

        void visit(TophatSamplesWidget *tsw) {
            QList<TophatSample> defSamples;
            defSamples << TophatSample("Sample1", QStringList());
            defSamples << TophatSample("Sample2", QStringList());
            QString defaultSamples = WorkflowUtils::packSamples(defSamples);
            wc->setAttributeValue(tsw->samplesAttr, defaultSamples);
        }

        void visit(LabelWidget *) {}

    private:
        WizardController *wc;
    };

    class PageDefaulter : public TemplatedPageVisitor {
    public:
        PageDefaulter(WizardController *wc) : wc(wc) {}
        virtual void visit(DefaultPageContent *dp) {
            WidgetDefaulter defaulter(wc);
            dp->getParamsArea()->accept(&defaulter);
        }

    private:
        WizardController *wc;
    };
}

void WizardController::defaults(QWizardPage *wPage) {
    WizardPage *page = findPage(wPage);
    CHECK(NULL != page, );
    TemplatedPageContent *content = page->getContent();

    PageDefaulter defaulter(this);
    content->accept(&defaulter);
    wPage->initializePage();
}

void WizardController::sl_customButtonClicked(int which) {
    if (QWizard::CustomButton1 == which) {
        run();
    } else if (QWizard::CustomButton2 == which) {
        QWizard *w = dynamic_cast<QWizard*>(sender());
        CHECK(NULL != w, );
        defaults(w->currentPage());
    }
}

void WizardController::sl_pageChanged(int num) {
    CHECK(-1 != num, );

    QWizard *wizard = dynamic_cast<QWizard*>(sender());
    CHECK(NULL != wizard, );

    QWizardPage *page = wizard->currentPage();
    CHECK(NULL != page, );

    page->cleanupPage();
    page->initializePage();
}

bool WizardController::eventFilter(QObject *watched, QEvent *event) {
    CHECK(NULL != event, false);

    if (event->type() == QEvent::Close) { // if close button is pressed
        rejected = true;
    } else if (event->type() == QEvent::KeyPress) { // if ESC is pressed
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        CHECK(NULL != keyEvent, QObject::eventFilter(watched, event));

        if ((keyEvent->key() == Qt::Key_Escape) && (keyEvent->modifiers() == Qt::NoModifier)) {
            rejected = true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void WizardController::assignParameters() {
    foreach (const QString &attrId, values.keys()) {
        U2OpStatus2Log os;
        AttributeInfo info = AttributeInfo::fromString(attrId, os);
        Attribute *attr = getAttribute(info);
        if (NULL == attr) {
            continue;
        }
        attr->setAttributeValue(values[attrId]);
    }
}

void WizardController::applySettings() {
    foreach (QString varId, vars.keys()) {
        if (varId.startsWith(SettingsWidget::SETTING_PREFIX)) {
            QString settingName = varId;
            settingName.remove(0, SettingsWidget::SETTING_PREFIX.length());
            QVariant value;
            if (vars[varId].isAssigned()) {
                value = vars[varId].getValue();
            }
            AppContext::getSettings()->setValue(settingName, value);
        }
    }
}

void WizardController::saveDelegateTags() {
    foreach (const QString &attrId, propertyControllers.keys()) {
        U2OpStatus2Log os;
        AttributeInfo info = AttributeInfo::fromString(attrId, os);
        DelegateTags *tags = propertyControllers[attrId]->tags();
        if (NULL == tags) {
            continue;
        }
        Actor *actor = WorkflowUtils::actorById(currentActors, info.actorId);
        if (NULL == actor->getEditor()) {
            continue;
        }
        PropertyDelegate *delegate = actor->getEditor()->getDelegate(info.attrId);
        if (NULL == delegate) {
            continue;
        }
        delegate->tags()->set(*tags);
    }
}

const QList<Actor*> & WizardController::getCurrentActors() const {
    return currentActors;
}

DelegateTags * WizardController::getTags(const AttributeInfo &info, bool returnNewTags) {
    if (!propertyControllers.contains(info.toString())) {
        if (returnNewTags) {
            DelegateTags* t = new DelegateTags();
            tagsWithoutController[info.toString()] = t;
            return t;
        } else {
            return NULL;
        }
    }

    return propertyControllers[info.toString()]->tags();
}

DelegateTags * WizardController::getTagsWithoutController(const AttributeInfo &info) const {
    CHECK(tagsWithoutController.contains(info.toString()), NULL);
    return tagsWithoutController[info.toString()];
}

Attribute * WizardController::getAttribute(const AttributeInfo &info) const {
    U2OpStatusImpl os;
    info.validate(currentActors, os);
    CHECK_OP(os, NULL);
    Actor *actor = WorkflowUtils::actorById(currentActors, info.actorId);
    return actor->getParameter(info.attrId);
}

QWizardPage * WizardController::createPage(WizardPage *page) {
    WizardPageController *controller = new WizardPageController(this, page);
    WDWizardPage *result = new WDWizardPage(controller);

    pageControllers << controller;

    return result;
}

int WizardController::getQtPageId(const QString &hrId) const {
    return pageIdMap[hrId];
}

const QMap<QString, Variable> & WizardController::getVariables() const {
    return vars;
}

QVariant WizardController::getVariableValue(const QString &var) {
    WIZARD_SAFE_POINT(vars.contains(var),
        QObject::tr("Undefined variable: %1").arg(var), QVariant());
    return vars[var].getValue();
}

void WizardController::setVariableValue(const QString &var, const QString &value) {
    WIZARD_SAFE_POINT(vars.contains(var),
        QObject::tr("Undefined variable: %1").arg(var), );
    vars[var].setValue(value);
}

QVariant WizardController::getSelectorValue(ElementSelectorWidget *widget) {
    WIZARD_SAFE_POINT(vars.contains(widget->getActorId()),
        QObject::tr("Undefined variable: %1").arg(widget->getActorId()), QVariant());
    Variable &v = vars[widget->getActorId()];
    if (v.isAssigned()) {
        return v.getValue();
    }

    // variable is not assigned yet => selector is not registered
    registerSelector(widget);
    SelectorValue sv = widget->getValues().first();
    v.setValue(sv.getValue());
    // now variable is assigned, selector is registered
    setSelectorValue(widget, sv.getValue());
    return sv.getValue();
}

void WizardController::setSelectorValue(ElementSelectorWidget *widget, const QVariant &value) {
    WIZARD_SAFE_POINT(vars.contains(widget->getActorId()),
        QObject::tr("Undefined variable: %1").arg(widget->getActorId()), );
    Variable &v = vars[widget->getActorId()];
    v.setValue(value.toString());
    replaceCurrentActor(widget->getActorId(), value.toString());
}

void WizardController::registerSelector(ElementSelectorWidget *widget) {
    if (selectors.contains(widget->getActorId())) {
        WIZARD_SAFE_POINT(false, QObject::tr("Actors selector is already defined: %1").arg(widget->getActorId()), );
    }
    U2OpStatusImpl os;
    SelectorActors actors(widget, currentActors, os);
    WIZARD_SAFE_POINT_OP(os, );
    selectors[widget->getActorId()] = actors;
}

void WizardController::replaceCurrentActor(const QString &actorId, const QString &selectorValue) {
    if (!selectors.contains(actorId)) {
        WIZARD_SAFE_POINT(false, QObject::tr("Unknown actors selector: %1").arg(actorId), );
    }
    Actor *currentA = WorkflowUtils::actorById(currentActors, actorId);
    WIZARD_SAFE_POINT(NULL != currentA, QObject::tr("Unknown actor id: %1").arg(actorId), );
    Actor *newA = selectors[actorId].getActor(selectorValue);
    WIZARD_SAFE_POINT(NULL != newA, QObject::tr("Unknown actors selector value id: %1").arg(selectorValue), );

    int idx = currentActors.indexOf(currentA);
    currentActors.replace(idx, newA);
}

void WizardController::setBroken() {
    broken = true;
}

bool WizardController::isBroken() const {
    return broken;
}

bool WizardController::isRejected() const {
    return rejected;
}

WizardController::ApplyResult WizardController::applyChanges(Metadata &meta) {
    if (isBroken()) {
        return BROKEN;
    }
    assignParameters();
    applySettings();
    saveDelegateTags();
    if (selectors.isEmpty()) {
        return OK;
    }

    ApplyResult result = OK;
    foreach (const QString &varName, selectors.keys()) {
        WIZARD_SAFE_POINT(vars.contains(varName),
            QObject::tr("Undefined variable: %1").arg(varName), BROKEN);
        Variable &v = vars[varName];
        SelectorActors &s = selectors[varName];
        Actor *newActor = s.getActor(v.getValue());
        Actor *oldActor = s.getSourceActor();
        if (newActor != oldActor) {
            result = ACTORS_REPLACED;
            schema->replaceProcess(oldActor, newActor, s.getMappings(v.getValue()));
            meta.replaceProcess(oldActor->getId(), newActor->getId(), s.getMappings(v.getValue()));
        }
    }
    return result;
}

void WizardController::addPropertyController(const AttributeInfo &info, PropertyWizardController *ctrl) {
    propertyControllers[info.toString()] = ctrl;
}

void WizardController::clearControllers() {
    propertyControllers.clear();
}

RunFileSystem * WizardController::getRFS() {
    RunFileSystem *result = new RunFileSystem(this);
    RFSUtils::initRFS(*result, schema->getProcesses(), this);
    return result;
}

QVariant WizardController::getAttributeValue(const AttributeInfo &info) const {
    if (values.contains(info.toString())) {
        return values[info.toString()];
    }
    Attribute *attr = getAttribute(info);
    CHECK(NULL != attr, QVariant());

    return attr->getAttributePureValue();
}

void WizardController::setAttributeValue(const AttributeInfo &info, const QVariant &value) {
    values[info.toString()] = value;

    Actor *actor = WorkflowUtils::actorById(currentActors, info.actorId);
    // Check attribute relations
    Attribute *attr = getAttribute(info);
    CHECK(NULL != attr, );
    foreach (const AttributeRelation *relation, attr->getRelations()) {
        if (!relation->valueChangingRelation()) {
            continue;
        }
        AttributeInfo related(info.actorId, relation->getRelatedAttrId());
        QVariant newValue = relation->getAffectResult(value, getAttributeValue(related), getTags(info),
                                                      getTags(related, true));
        Attribute *attr = getAttribute(info);

        bool canSetValue = false;
        bool dir = false;
        bool isOutUrlAttr = RFSUtils::isOutUrlAttribute(attr, actor, dir);
        if (!isOutUrlAttr) {
            canSetValue = true;
        } else {
            canSetValue = getRFS()->canAdd(value.toString(), dir);
        }

        if (canSetValue) {
            setAttributeValue(related, newValue);
            if (propertyControllers.contains(related.toString())) {
                propertyControllers[related.toString()]->updateGUI(newValue);
            }
        }
    }
    foreach (Attribute* otherAttr, actor->getParameters().values()) {
        if (otherAttr == attr) {
            continue;
        }
        foreach (const AttributeRelation *relation, otherAttr->getRelations()) {
            if (relation->getType() != VISIBILITY || relation->getRelatedAttrId() != attr->getId()) {
                continue;
            }
            AttributeInfo related(info.actorId, otherAttr->getId());
            if (propertyControllers.contains(related.toString())) {
                bool isVisible = true;
                //isVisible &= isAttributeVisible(masterAttribute);
                isVisible &= relation->getAffectResult(value, getAttributeValue(related)).toBool();
                propertyControllers[related.toString()]->updateVisibility(isVisible);
            }
        }
    }
}

/************************************************************************/
/* WidgetCreator */
/************************************************************************/
WidgetCreator::WidgetCreator(WizardController *_wc)
: wc(_wc), labelSize(0), result(NULL), layout(NULL), widgetsArea(NULL), fullWidth(false)
{

}

WidgetCreator::WidgetCreator(WizardController *_wc, int _labelSize)
: wc(_wc), labelSize(_labelSize), result(NULL), layout(NULL), widgetsArea(NULL), fullWidth(false)
{

}

bool WidgetCreator::hasFullWidth() {
    return fullWidth;
}

void WidgetCreator::visit(AttributeWidget *aw) {
    QString type = aw->getProperty(AttributeInfo::TYPE);
    PropertyWizardController *controller = NULL;

    if (AttributeInfo::DEFAULT == type) {
        controller = new DefaultPropertyController(wc, aw, labelSize);
    } else if (AttributeInfo::DATASETS == type) {
        controller = new InUrlDatasetsController(wc, aw);
        fullWidth = true;
    } else {
        WIZARD_SAFE_POINT_EXT(false, QString("Unknown widget type: %1").arg(type), wc->setBroken(), );
    }

    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
    WIZARD_SAFE_POINT_OP_EXT(os, wc->setBroken(), );
}

void WidgetCreator::visit(WidgetsArea *wa) {
    QWidget *scrollContent = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    scrollContent->setLayout(layout);
    foreach (WizardWidget *w, wa->getWidgets()) {
        int labelSize = wa->getLabelSize();
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
        labelSize *= 1.3;
#elif defined(Q_OS_UNIX)
        labelSize *= 1.4;
#endif
        WidgetCreator wcr(wc, labelSize);
        w->accept(&wcr);
        if (NULL != wcr.getResult()) {
            if (!wcr.hasFullWidth()) {
                wcr.getResult()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
            } else {
                fullWidth = true;
            }
            layout->addWidget(wcr.getResult());
            controllers << wcr.getControllers();
        }
    }
    if (!fullWidth) {
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);
        layout->addSpacerItem(spacer);
    }
    setupScrollArea(scrollContent);
}

void WidgetCreator::setupScrollArea(QWidget *scrollContent) {
    scrollContent->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    widgetsArea = new QScrollArea();
    widgetsArea->setWidget(scrollContent);
    widgetsArea->setWidgetResizable(true);
    widgetsArea->setFrameShape(QFrame::NoFrame);
    widgetsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    result = widgetsArea;
}

void WidgetCreator::visit(GroupWidget *gw) {
    visit((WidgetsArea*)gw);
    result = widgetsArea->takeWidget();
    delete widgetsArea;
    widgetsArea = NULL;

    bool collapsible = (gw->getType() == GroupWidget::HIDEABLE);
    GroupBox *gb = new GroupBox(collapsible, gw->getTitle(), fullWidth);
    setGroupBoxLayout(gb);
}

void WidgetCreator::visit(LogoWidget *lw) {
    result = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    result->setLayout(layout);

    QLabel *label = new QLabel(result);
    QPixmap pix;
    if (lw->isDefault()) {
        pix = QPixmap(QString(":U2Designer/images/logo.png"));
    } else {
        pix = QPixmap(lw->getLogoPath());
    }
    pix = pix.scaled(255, 450, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    label->setPixmap(pix);
    layout->addWidget(label);
    result->setFixedSize(pix.size());
}

void WidgetCreator::visit(ElementSelectorWidget *esw) {
    ElementSelectorController *controller = new ElementSelectorController(wc, esw, labelSize);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
}

void WidgetCreator::visit(PairedReadsWidget *dsw) {
    PairedDatasetsController *controller = new PairedDatasetsController(wc, dsw);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
    fullWidth = true;
}

void WidgetCreator::visit(UrlAndDatasetWidget *ldsw) {
    UrlAndDatasetWizardController *controller = new UrlAndDatasetWizardController(wc, ldsw);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
    fullWidth = true;
}

void WidgetCreator::visit(RadioWidget *rw) {
    RadioController *controller = new RadioController(wc, rw);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
}

void WidgetCreator::visit(SettingsWidget *sw) {
    SettingsController *controller = new SettingsController(wc, sw);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
}

void WidgetCreator::visit(BowtieWidget *bw) {
    BowtieWidgetController *controller = new BowtieWidgetController(wc, bw, labelSize);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
}

void WidgetCreator::visit(TophatSamplesWidget *tsw) {
    TophatSamplesWidgetController *controller = new TophatSamplesWidgetController(wc, tsw);
    controllers << controller;
    U2OpStatusImpl os;
    result = controller->createGUI(os);
    fullWidth = true;
}

void WidgetCreator::visit(LabelWidget *lw) {
    QString text = lw->text;
    text.replace("\\n", "\n");
    QLabel *label = new QLabel(text);
    QString style = "\
                    border-width: 0px;\
                    border-style: solid;\
                    border-radius: 4px;\
                    ";
    style += "color: " + lw->textColor + ";";
    style += "background-color: " + lw->backgroundColor + ";";
    style += "padding: 6px;";
    style += "margin-bottom: 12px;";
    label->setStyleSheet(style);
    label->setAlignment(Qt::AlignJustify);
    label->setWordWrap(true);

#ifdef Q_OS_WIN
    QFont font = label->font();
    font.setPointSize(font.pointSize() + 2);
    label->setFont(font);
#endif

    result = label;
}

QWidget * WidgetCreator::getResult() {
    return result;
}

QList<WidgetController*> & WidgetCreator::getControllers() {
    return controllers;
}

QBoxLayout * WidgetCreator::getLayout() {
    return layout;
}

void WidgetCreator::setGroupBoxLayout(GroupBox *gb) {
    gb->setLayout(layout);
    delete result;
    result = gb;
}

/************************************************************************/
/* PageContentCreator */
/************************************************************************/
PageContentCreator::PageContentCreator(WizardController *_wc)
    : wc(_wc), result(NULL), pageTitle(NULL), pageSubtitle(NULL)
{

}

void PageContentCreator::visit(DefaultPageContent *content) {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    int paramsHeight = content->getPageDefaultHeight();
    int paramsWidth = content->getPageWidth();
    { // create logo
        WidgetCreator logoWC(wc);
        content->getLogoArea()->accept(&logoWC);
        if (NULL != logoWC.getResult()) {
            paramsHeight = logoWC.getResult()->height();
            paramsWidth -= logoWC.getResult()->width();
            layout->addWidget(logoWC.getResult());
            controllers << logoWC.getControllers();
        }
    }
    createTitle(contentLayout);
    createSubTitle(contentLayout);
    { //TODO: compute real title and subtitle height
        paramsHeight = 0;
    }
    { // create parameters
        WidgetCreator paramsWC(wc);
        content->getParamsArea()->accept(&paramsWC);
        if (NULL != paramsWC.getResult()) {
            if (NULL != paramsWC.getLayout() && !paramsWC.hasFullWidth()) {
                QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
                paramsWC.getLayout()->addSpacerItem(spacer);
            }
            contentLayout->addWidget(paramsWC.getResult());
            paramsWC.getResult()->setMinimumSize(paramsWidth, paramsHeight);
            controllers << paramsWC.getControllers();

            //let it process attribute relations
            foreach (WidgetController *wcc, controllers) {
                PropertyWizardController *pwc = qobject_cast<PropertyWizardController*>(wcc);
                if (pwc) {
                    wc->setAttributeValue(pwc->attributeWidget()->getInfo(), wc->getAttributeValue(pwc->attributeWidget()->getInfo()));
                }
            }
        }
    }
    layout->addLayout(contentLayout);
    layout->setAlignment(Qt::AlignBottom);
    result = layout;
}

void PageContentCreator::setPageTitle(const QString& title) {
    if (NULL != pageTitle && false == title.isEmpty()) {
        pageTitle->setText(title);
        pageTitle->show();
        pageTitle->setObjectName("pageTitle");
    }
}

void PageContentCreator::setPageSubtitle(const QString& subtitle) {
    if (NULL != pageSubtitle && false == subtitle.isEmpty()) {
        pageSubtitle->setText(subtitle);
        pageSubtitle->show();
    }
}

QLayout * PageContentCreator::getResult() {
    return result;
}

QList<WidgetController*> & PageContentCreator::getControllers() {
    return controllers;
}

void PageContentCreator::createTitle(QVBoxLayout *contentLayout) {
    pageTitle = new QLabel();
    pageTitle->setWordWrap(true);
#ifdef Q_OS_MAC
    pageTitle->setStyleSheet("QLabel {font-size: 20pt; padding-bottom: 10px; color: #0c3762}");
#else
    pageTitle->setStyleSheet("QLabel {font-size: 16pt; padding-bottom: 10px; color: #0c3762}");
#endif
    pageTitle->resize(0, 0);
    pageTitle->hide();
    contentLayout->addWidget(pageTitle);
}

void PageContentCreator::createSubTitle(QVBoxLayout *contentLayout) {
    pageSubtitle = new QLabel();
    pageSubtitle->setWordWrap(true);
    pageSubtitle->resize(0, 0);
    pageSubtitle->hide();
    contentLayout->addWidget(pageSubtitle);
}

/************************************************************************/
/* GroupBox */
/************************************************************************/
const int GroupBox::MARGIN = 5;

GroupBox::GroupBox(bool collapsible, const QString &title, bool fullWidth)
: QGroupBox(title), hLayout(NULL), tip(NULL), showHideButton(NULL)
{
    ui = new QWidget(this);
    QSizePolicy::Policy vPolicy = fullWidth ? QSizePolicy::MinimumExpanding : QSizePolicy::Maximum;
    ui->setSizePolicy(QSizePolicy::Minimum, vPolicy);
    QVBoxLayout *layout = new QVBoxLayout();
    QGroupBox::setLayout(layout);

#ifdef Q_OS_MAC
    layout->setContentsMargins(0, 0, 0, 0);
    QString style = "QGroupBox  {"
                    "margin-top: 30px;" // leave space at the top for the title
                    "padding: 5 5 5 5px;"
                    "}"

                    "QGroupBox::title  {"
                    "subcontrol-origin: margin;"
                    "padding-top: 12px;"
                    "padding-left: 15px;"
                    "}";
    setStyleSheet(style);
#else
    layout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
#endif

    if (collapsible) {
        hLayout = new QHBoxLayout();
        tip = new QLabel(this);
        hLayout->addWidget(tip);
        hLayout->setContentsMargins(0, 0, 0, 0);
        showHideButton = new QToolButton(this);
        showHideButton->setObjectName("showHideButton");
        showHideButton->setFixedSize(19, 19);
        hLayout->addWidget(showHideButton);
        layout->addLayout(hLayout);
    }

    layout->addWidget(ui);

    if (collapsible) {
        sl_collapse();
        connect(showHideButton, SIGNAL(clicked()), this, SLOT(sl_onCheck()));
    }
}

void GroupBox::sl_collapse() {
    ui->hide();
    changeView("+", tr("Show"));
}

void GroupBox::sl_expand() {
    ui->show();
    changeView("-", tr("Hide"));
}

void GroupBox::setLayout(QLayout *l) {
    ui->setLayout(l);
}

void GroupBox::sl_onCheck() {
    if (ui->isVisible()) {
        sl_collapse();
    } else {
        sl_expand();
    }
}

void GroupBox::changeView(const QString &buttonText, const QString &showHide) {
    CHECK(NULL != showHideButton, );
    showHideButton->setText(buttonText);

    CHECK(NULL != tip, );
    QString parametersStr = tr("additional");
    if (!title().isEmpty()) {
        parametersStr = title().toLower();
    }
    tip->setText(showHide + " " + parametersStr + tr(" settings"));
    showHideButton->setToolTip(tip->text());

    CHECK(NULL != hLayout, );
    if (!title().isEmpty()) {
        QMargins m = layout()->contentsMargins();
        m.setTop(0);
        layout()->setContentsMargins(m);
    }
}

} // U2
