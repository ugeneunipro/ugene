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

#ifndef _U2_HRWIZARDSERIALIZER_H_
#define _U2_HRWIZARDSERIALIZER_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/Variable.h>
#include <U2Lang/Wizard.h>
#include <U2Lang/WizardPage.h>
#include <U2Lang/WizardWidget.h>
#include <U2Lang/WizardWidgetVisitor.h>

#include "Tokenizer.h"

namespace U2 {
namespace WorkflowSerialize {

class HRWizardParser : public QObject {
    Q_OBJECT
public:
    HRWizardParser(Tokenizer &tokenizer, const QMap<QString, Actor*> &actorMap);
    virtual ~HRWizardParser();

    Wizard * parseWizard(U2OpStatus &os);

private:
    Tokenizer &tokenizer;
    const QMap<QString, Actor*> &actorMap;
    QString wizardName;
    QString finishLabel;
    QString helpPageId;
    QList<WizardPage*> pages;
    QMap<QString, Variable> vars;

    QMap<QString, QString> nextIds; // id <-> nextId
    QMap<QString, WizardPage*> pagesMap; // id <-> page
    QMap<QString, QList<Predicate> > results;

private:
    void parsePage(U2OpStatus &os);
    void parseResult(U2OpStatus &os);
    void finilizePagesOrder(U2OpStatus &os);
    Wizard * takeResult();
    void parseNextIds(ParsedPairs &pairs, WizardPage *page, U2OpStatus &os);

public:
    static const QString WIZARD;
    static const QString NAME;
    static const QString AUTORUN;
    static const QString PAGE;
    static const QString ID;
    static const QString NEXT;
    static const QString TITLE;
    static const QString TEMPLATE;
    static const QString TYPE;
    static const QString LOGO_PATH;
    static const QString DEFAULT;
    static const QString HIDEABLE;
    static const QString LABEL;
    static const QString LABEL_SIZE;
    static const QString ELEMENT_ID;
    static const QString PROTOTYPE;
    static const QString VALUE;
    static const QString PORT_MAPPING;
    static const QString SLOTS_MAPPRING;
    static const QString SRC_PORT;
    static const QString DST_PORT;
    static const QString RESULT;
    static const QString FINISH_LABEL;
    static const QString TOOLTIP;
    static const QString HAS_RUN_BUTTON;
    static const QString HAS_DEFAULTS_BUTTON;
    static const QString BWT_DIR;
    static const QString BWT_IDX_NAME;
    static const QString DATASETS_PROVIDER;
    static const QString TEXT;
    static const QString TEXT_COLOR;
    static const QString BACKGROUND_COLOR;
    static const QString HELP_PAGE_ID;
};

class U2LANG_EXPORT HRWizardSerializer {
public:
    QString serialize(Wizard *wizard, int depth);

private:
    QString serializePage(WizardPage *page, int depth);
    QString serializeResults(const QMap<QString, QList<Predicate> > results, int depth);
    QString serializeNextId(WizardPage *page, int depth);
};

/************************************************************************/
/* Parsing utilities */
/************************************************************************/
class WizardWidgetParser : public WizardWidgetVisitor {
public:
    WizardWidgetParser(const QString &title,
        const QString &data,
        const QMap<QString, Actor*> &actorMap,
        QMap<QString, Variable> &vars,
        U2OpStatus &os);

    virtual void visit(AttributeWidget *aw);
    virtual void visit(WidgetsArea *w);
    virtual void visit(LogoWidget *lw);
    virtual void visit(GroupWidget *gw);
    virtual void visit(ElementSelectorWidget *esw);
    virtual void visit(PairedReadsWidget *dsw);
    virtual void visit(UrlAndDatasetWidget *ldsw);
    virtual void visit(RadioWidget *rw);
    virtual void visit(SettingsWidget *sw);
    virtual void visit(BowtieWidget *bw);
    virtual void visit(TophatSamplesWidget *tsw);
    virtual void visit(LabelWidget *lw);

private:
    QString title;
    QString data;
    const QMap<QString, Actor*> &actorMap;
    QMap<QString, Variable> &vars;
    U2OpStatus &os;

    ParsedPairs pairs;

private:
    void getLabelSize(WidgetsArea *wa);
    void getTitle(WidgetsArea *wa);
    WizardWidget * createWidget(const QString &id);
    SelectorValue parseSelectorValue(ActorPrototype *srcProto, const QString &valueDef);
    PortMapping parsePortMapping(const QString &mappingDef);
    void parseSlotsMapping(PortMapping &pm, const QString &mappingDef);
    void addVariable(const Variable &v);

    AttributeInfo parseInfo(const QString &attrStr, const QString &body);
};

class PageContentParser : public TemplatedPageVisitor {
public:
    PageContentParser(ParsedPairs &pairs,
        const QMap<QString, Actor*> &actorMap,
        QMap<QString, Variable> &vars,
        U2OpStatus &os);

    virtual void visit(DefaultPageContent *content);

private:
    ParsedPairs &pairs;
    const QMap<QString, Actor*> &actorMap;
    QMap<QString, Variable> &vars;
    U2OpStatus &os;
};

/************************************************************************/
/* Serializing utilities */
/************************************************************************/
class WizardWidgetSerializer : public WizardWidgetVisitor {
public:
    WizardWidgetSerializer(int depth);

    virtual void visit(AttributeWidget *aw);
    virtual void visit(WidgetsArea *wa);
    virtual void visit(GroupWidget *gw);
    virtual void visit(LogoWidget *lw);
    virtual void visit(ElementSelectorWidget *esw);
    virtual void visit(PairedReadsWidget *dsw);
    virtual void visit(UrlAndDatasetWidget *ldsw);
    virtual void visit(RadioWidget *rw);
    virtual void visit(SettingsWidget *sw);
    virtual void visit(BowtieWidget *bw);
    virtual void visit(TophatSamplesWidget *tsw);
    virtual void visit(LabelWidget *lw);

    const QString & getResult();

private:
    int depth;
    QString result;
    QString addInfo;

private:
    QString serializeSlotsMapping(const QList<SlotMapping> &mappings, int depth) const;
    QString serializePortMapping(const PortMapping &mapping, int depth) const;
    QString serializeSelectorValue(const SelectorValue &value, int depth) const;
    QString serializeInfo(const AttributeInfo &info, int depth) const;
};

class PageContentSerializer : public TemplatedPageVisitor {
public:
    PageContentSerializer(int depth);

    virtual void visit(DefaultPageContent *content);

    const QString & getResult() const;

private:
    int depth;
    QString result;
};

} // WorkflowSerialize
} // U2

#endif // _U2_HRWIZARDSERIALIZER_H_
