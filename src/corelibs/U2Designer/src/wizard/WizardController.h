/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QWizard>

#include <U2Lang/ElementSelectorWidget.h>
#include <U2Lang/Schema.h>
#include <U2Lang/SchemaConfig.h>
#include <U2Lang/Wizard.h>
#include <U2Lang/WizardPage.h>
#include <U2Lang/WizardWidget.h>
#include <U2Lang/WizardWidgetVisitor.h>

#include "SelectorActors.h"

namespace U2 {

class PropertyWizardController;
class WidgetController;
class WizardPageController;

class U2DESIGNER_EXPORT WizardController : public QObject, public SchemaConfig {
    Q_OBJECT
public:
    enum U2DESIGNER_EXPORT ApplyResult {
        OK,
        ACTORS_REPLACED,
        BROKEN
    };

public:
    WizardController(const QSharedPointer<Schema>& s, const Wizard* w);
    virtual ~WizardController();

    QWizard* createGui();
    ApplyResult applyChanges(Metadata& meta);
    const QList<Actor*>& getCurrentActors() const;
    bool isRunAfterApply() const;

    QVariant getVariableValue(const QString& var);
    void setVariableValue(const QString& var, const QString& value);

    QVariant getSelectorValue(ElementSelectorWidget* widget);
    void setSelectorValue(ElementSelectorWidget* widget, const QVariant& value);

    /** Returns id of the Qt page object by @hrId.
     * @hrId - human-readable id of the page.
     */
    int getQtPageId(const QString& hrId) const;
    const QMap<QString, Variable>& getVariables() const;

    /** Wizard pages are validated in runtime. If some page is not validated
     * then wizard becomes broken
     */
    void setBroken();
    bool isBroken() const;

    bool isRejected() const;

    void addPropertyController(const AttributeInfo& info, PropertyWizardController* ctrl);
    void clearControllers();

    // SchemaConfig
    RunFileSystem* getRFS() override;
    QVariant getAttributeValue(const AttributeInfo& info) const override;
    void setAttributeValue(const AttributeInfo& info, const QVariant& value) override;

    Attribute* getAttribute(const AttributeInfo& info) const;
    DelegateTags* getTags(const AttributeInfo& info, bool returnNewTags = false);
    DelegateTags* getTagsWithoutController(const AttributeInfo& info) const;

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    bool rejected;
    bool broken;
    const QSharedPointer<Schema> schema;
    const Wizard* wizard;
    QList<WizardPageController*> pageControllers;
    QList<Actor*> currentActors;
    QMap<QString, int> pageIdMap;  // hr-id <-> qt-id
    QMap<QString, Variable> vars;
    QMap<QString, SelectorActors> selectors;  // varName <-> actors
    QVariantMap values;
    QMap<QString, PropertyWizardController*> propertyControllers;
    QMap<QString, DelegateTags*> tagsWithoutController;
    bool runAfterApply;

private:
    void setupButtons(QWizard* wizard);
    QWizardPage* createPage(WizardPage* page);
    void registerSelector(ElementSelectorWidget* widget);
    void replaceCurrentActor(const QString& actorId, const QString& selectorValue);
    void assignParameters();
    void applySettings();
    void saveDelegateTags();
    void run();
    void defaults(QWizardPage* wPage);
    WizardPage* findPage(QWizardPage* wPage);

private slots:
    void sl_customButtonClicked(int num);
    void sl_pageChanged(int num);
};

/************************************************************************/
/* Utilities */
/************************************************************************/
class GroupBox;
class WidgetCreator : public WizardWidgetVisitor {
public:
    WidgetCreator(WizardController* wc);
    WidgetCreator(WizardController* wc, int labelSize);

    void visit(AttributeWidget* aw) override;
    void visit(WidgetsArea* wa) override;
    void visit(GroupWidget* gw) override;
    void visit(LogoWidget* lw) override;
    void visit(ElementSelectorWidget* esw) override;
    void visit(PairedReadsWidget* dsw) override;
    void visit(UrlAndDatasetWidget* dsw) override;
    void visit(RadioWidget* rw) override;
    void visit(SettingsWidget* sw) override;
    void visit(BowtieWidget* sw) override;
    void visit(TophatSamplesWidget* tsw) override;
    void visit(LabelWidget* lw) override;

    QWidget* getResult();
    QList<WidgetController*>& getControllers();
    QBoxLayout* getLayout();

    bool hasFullWidth();

private:
    WizardController* wc;
    int labelSize;
    QWidget* result;
    QList<WidgetController*> controllers;
    QBoxLayout* layout;
    QScrollArea* widgetsArea;
    bool fullWidth;

private:
    void setGroupBoxLayout(GroupBox* gb);
    void setupScrollArea(QWidget* scrollContent);
};

class PageContentCreator : public TemplatedPageVisitor {
public:
    PageContentCreator(WizardController* wc);

    void visit(DefaultPageContent* content) override;
    void setPageTitle(const QString& title);
    void setPageSubtitle(const QString& subtitle);

    QLayout* getResult();
    QList<WidgetController*>& getControllers();

private:
    WizardController* wc;
    QLayout* result;
    QList<WidgetController*> controllers;
    QLabel* pageTitle;
    QLabel* pageSubtitle;

private:
    void createTitle(QVBoxLayout* contentLayout);
    void createSubTitle(QVBoxLayout* contentLayout);
};

/************************************************************************/
/* GroupBox */
/************************************************************************/
class GroupBox : public QGroupBox {
    Q_OBJECT
public:
    GroupBox(bool collapsible, const QString& title, bool fullWidth = false);

    void setLayout(QLayout* l);

private slots:
    void sl_collapse();
    void sl_expand();
    void sl_onCheck();

private:
    QWidget* ui;
    QHBoxLayout* hLayout;
    QLabel* tip;
    QToolButton* showHideButton;

    static const int MARGIN;

private:
    void changeView(const QString& buttonText, const QString& showHide);
};

}  // namespace U2
