/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/HRSchemaSerializer.h>

#include "HRWizardSerializer.h"

namespace U2 {
namespace WorkflowSerialize {

const QString HRWizardParser::WIZARD("wizard");
const QString HRWizardParser::NAME("name");
const QString HRWizardParser::AUTORUN("auto-run");
const QString HRWizardParser::PAGE("page");
const QString HRWizardParser::ID("id");
const QString HRWizardParser::NEXT("next");
const QString HRWizardParser::TITLE("title");
const QString HRWizardParser::TEMPLATE("template");
const QString HRWizardParser::TYPE("type");
const QString HRWizardParser::LOGO_PATH("logo-path");
const QString HRWizardParser::DEFAULT("default");
const QString HRWizardParser::HIDEABLE("hideable");
const QString HRWizardParser::LABEL("label");
const QString HRWizardParser::LABEL_SIZE("label-size");
const QString HRWizardParser::ELEMENT_ID("element-id");
const QString HRWizardParser::PROTOTYPE("prototype");
const QString HRWizardParser::VALUE("value");
const QString HRWizardParser::PORT_MAPPING("port-mapping");
const QString HRWizardParser::SLOTS_MAPPRING("slots-mapping");
const QString HRWizardParser::SRC_PORT("src-port");
const QString HRWizardParser::DST_PORT("dst-port");
const QString HRWizardParser::RESULT("result");
const QString HRWizardParser::FINISH_LABEL("finish-label");
const QString HRWizardParser::TOOLTIP("tooltip");
const QString HRWizardParser::HAS_RUN_BUTTON("has-run-button");
const QString HRWizardParser::HAS_DEFAULTS_BUTTON("has-defaults-button");
const QString HRWizardParser::DATASETS_PROVIDER("datasets-provider");
const QString HRWizardParser::TEXT("text");
const QString HRWizardParser::TEXT_COLOR("text-color");
const QString HRWizardParser::BACKGROUND_COLOR("background-color");
const QString HRWizardParser::HELP_PAGE_ID("help-page-id");

HRWizardParser::HRWizardParser(Tokenizer &tokenizer, const QMap<QString, Actor*> &_actorMap)
: tokenizer(tokenizer), actorMap(_actorMap), wizardName(Wizard::DEFAULT_NAME){}

HRWizardParser::~HRWizardParser() {
    qDeleteAll(pagesMap);
}

Wizard * HRWizardParser::takeResult() {
    QList<WizardPage*> retPages = pages;
    pages.clear();
    pagesMap.clear();

    Wizard *wizard = new Wizard(wizardName, retPages, helpPageId);
    foreach (const QString &name, vars.keys()) {
        wizard->addVariable(vars[name]);
    }
    foreach (const QString &result, results.keys()) {
        wizard->addResult(results[result], result);
    }
    wizard->setFinishLabel(finishLabel);
    return wizard;
}

Wizard * HRWizardParser::parseWizard(U2OpStatus &os) {
    bool autoRun = true;
    bool hasRunButton = true;
    bool hasDefaultsButton = true;
    while (tokenizer.look() != Constants::BLOCK_END) {
        QString tok = tokenizer.take();
        if (PAGE == tok) {
            tokenizer.assertToken(Constants::BLOCK_START);
            parsePage(os);
            CHECK_OP(os, NULL);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (NAME == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            wizardName = tokenizer.take();
        } else if (HELP_PAGE_ID == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            helpPageId = tokenizer.take();
        } else if (AUTORUN == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            autoRun = ("true" == tokenizer.take());
        } else if (HAS_RUN_BUTTON == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            hasRunButton = ("false" != tokenizer.take());
        } else if (HAS_DEFAULTS_BUTTON == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            hasDefaultsButton = ("false" != tokenizer.take());
        } else if (RESULT == tok) {
            tokenizer.assertToken(Constants::BLOCK_START);
            parseResult(os);
            CHECK_OP(os, NULL);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (FINISH_LABEL == tok) {
            tokenizer.assertToken(Constants::EQUALS_SIGN);
            finishLabel = tokenizer.take();
        }
    }

    finilizePagesOrder(os);
    CHECK_OP(os, NULL);

    Wizard *result = takeResult();
    CHECK(NULL != result, NULL);
    result->setAutoRun(autoRun);
    result->setHasRunButton(hasRunButton);
    result->setHasDefaultsButton(hasDefaultsButton);
    return result;
}

void HRWizardParser::parsePage(U2OpStatus &os) {
    ParsedPairs pairs(tokenizer);

    QString title = pairs.equalPairs.value(TITLE, "");
    if (!pairs.equalPairs.contains(ID)) {
        os.setError(tr("Wizard page %1 does not contain id").arg(title));
        return;
    }
    QString id = pairs.equalPairs.take(ID);
    if (nextIds.keys().contains(id)) {
        os.setError(tr("Several wizard pages have equal ids: %1").arg(id));
        return;
    }

    QString templateId = pairs.equalPairs.value(TEMPLATE, DefaultPageContent::ID);
    QScopedPointer<TemplatedPageContent> content(PageContentFactory::createContent(templateId, os));
    CHECK_OP(os, );
    PageContentParser pcp(pairs, actorMap, vars, os);
    content->accept(&pcp);
    CHECK_OP(os, );

    QScopedPointer<WizardPage> page(new WizardPage(id, title));
    parseNextIds(pairs, page.data(), os);
    CHECK_OP(os, );
    page->setContent(content.take());
    pagesMap[id] = page.take();
}

void HRWizardParser::parseResult(U2OpStatus &os) {
    ParsedPairs pairs(tokenizer);
    foreach (const QString &result, pairs.equalPairs.keys()) {
        QList<Predicate> preds;
        foreach (const QString &predStr, pairs.equalPairs[result].split(" ")) {
            preds << Predicate::fromString(predStr, os);
            CHECK_OP(os, );
        }
        results[result] = preds;
    }
}

void HRWizardParser::finilizePagesOrder(U2OpStatus &os) {
    // TODO: UGENE-1322
    Q_UNUSED(os);
    pages = pagesMap.values();
    //QString lastId;
    //QList<QString> ids = nextIds.keys();
    //// Check loops, first page id and last page id
    //foreach (const QString &id, nextIds.keys()) {
    //    if (nextIds[id].isEmpty()) {
    //        if (!lastId.isEmpty()) {
    //            os.setError(tr("Two pages of the wizard are defined as last: %1, %2").arg(lastId).arg(id));
    //            return;
    //        }
    //        lastId = id;
    //    } else {
    //        if (!nextIds.keys().contains(nextIds[id])) {
    //            os.setError(tr("Unknown next page id: %1").arg(nextIds[id]));
    //            return;
    //        }
    //        if (!ids.contains(nextIds[id])) {
    //            os.setError(tr("Two pages of the wizard have equal next page ids: %1").arg(nextIds[id]));
    //            return;
    //        }
    //        ids.removeOne(nextIds[id]);
    //    }
    //}
    //if (ids.isEmpty()) {
    //    os.setError(tr("Some pages of the widget are looped"));
    //    return;
    //} else if (ids.size() > 1) {
    //    os.setError(tr("Several pages of the wizard are defined as first: %1, %2, ...").arg(ids[0]).arg(ids[1]));
    //    return;
    //}

    //// Create pages list saving order
    //QString currentId = ids.first();
    //while ("" != currentId) {
    //    QString nextId = nextIds[currentId];

    //    WizardPage *page = pagesMap[currentId];
    //    if (!nextId.isEmpty()) {
    //        page->setNext(pagesMap[nextId]->getId());
    //    }
    //    pages << page;
    //    currentId = nextId;
    //}
}

void HRWizardParser::parseNextIds(ParsedPairs &pairs, WizardPage *page, U2OpStatus &os) {
    if (pairs.equalPairs.contains(NEXT)) {
        QString nextId = pairs.equalPairs.value(NEXT);
        if (page->getId() == nextId) {
            os.setError(tr("Page's id and next id are equal: %1").arg(nextId));
            return;
        }
        if (pairs.blockPairs.contains(NEXT)) {
            os.setError(HRWizardParser::tr("Double definition of next id in the page with id: %1").arg(page->getId()));
            return;
        }
        page->setNext(nextId);
        return;
    }
    if (pairs.blockPairs.contains(NEXT)) {
        ParsedPairs predPairs(pairs.blockPairs[NEXT]);
        foreach (const QString &id, predPairs.equalPairs.keys()) {
            Predicate p = Predicate::fromString(predPairs.equalPairs[id], os);
            CHECK_OP(os, );
            page->setNext(id, p, os);
            CHECK_OP(os, );
        }
    }
}

/************************************************************************/
/* WizardWidgetParser */
/************************************************************************/
WizardWidgetParser::WizardWidgetParser(const QString &_title,
    const QString &_data,
    const QMap<QString, Actor*> &_actorMap,
    QMap<QString, Variable> &_vars,
    U2OpStatus &_os)
: title(_title), data(_data), actorMap(_actorMap), vars(_vars), os(_os)
{

}

void WizardWidgetParser::visit(AttributeWidget *aw) {
    AttributeInfo info = parseInfo(title, data);
    CHECK_OP(os, );
    aw->setInfo(info);
}

void WizardWidgetParser::visit(WidgetsArea *wa) {
    pairs = ParsedPairs(data, 0);

    getTitle(wa);
    getLabelSize(wa);

    foreach (const StrStrPair &pair, pairs.blockPairsList) {
        WizardWidgetParser wParser(pair.first, pair.second, actorMap, vars, os);
        QScopedPointer<WizardWidget> w(createWidget(pair.first));
        CHECK_OP(os, );
        w->accept(&wParser);
        CHECK_OP(os, );
        wa->addWidget(w.take());
    }
}

void WizardWidgetParser::visit(LogoWidget *lw) {
    pairs = ParsedPairs(data, 0);
    if (pairs.equalPairs.contains(HRWizardParser::LOGO_PATH)) {
        lw->setLogoPath(pairs.equalPairs.value(HRWizardParser::LOGO_PATH));
    }
}

void WizardWidgetParser::visit(GroupWidget *gw) {
    visit((WidgetsArea*)gw);
    CHECK_OP(os, );

    QString typeStr = pairs.equalPairs.value(HRWizardParser::TYPE, HRWizardParser::DEFAULT);
    if (HRWizardParser::DEFAULT == typeStr) {
        gw->setType(GroupWidget::DEFAULT);
    } else if (HRWizardParser::HIDEABLE == typeStr) {
        gw->setType(GroupWidget::HIDEABLE);
    }
}

void WizardWidgetParser::visit(ElementSelectorWidget *esw) {
    pairs = ParsedPairs(data, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::ELEMENT_ID)) {
        os.setError(HRWizardParser::tr("Element id is undefined in the element selector"));
        return;
    }
    QString actorId = pairs.equalPairs[HRWizardParser::ELEMENT_ID];
    if (!actorMap.contains(actorId)) {
        os.setError(HRWizardParser::tr("Undefined actor id: %1").arg(actorId));
        return;
    }
    esw->setActorId(actorId);
    if (pairs.equalPairs.contains(AttributeInfo::LABEL)) {
        esw->setLabel(pairs.equalPairs[AttributeInfo::LABEL]);
    }
    ActorPrototype *srcProto = actorMap[actorId]->getProto();
    foreach (const StrStrPair &pair, pairs.blockPairsList) {
        if (pair.first != HRWizardParser::VALUE) {
            os.setError(HRWizardParser::tr("Unknown block name in element selector definition: %1").arg(pair.first));
            return;
        }
        SelectorValue value = parseSelectorValue(srcProto, pair.second);
        CHECK_OP(os, );
        esw->addValue(value);
    }
    addVariable(Variable(actorId));
    CHECK_OP(os, );
}

void WizardWidgetParser::visit(PairedReadsWidget *dsw) {
    pairs = ParsedPairs(data, 0);
    foreach (const StrStrPair &p, pairs.blockPairsList) {
        dsw->addInfo(parseInfo(p.first, p.second));
        CHECK_OP(os, );
    }
}

void WizardWidgetParser::visit(UrlAndDatasetWidget *ldsw) {
    pairs = ParsedPairs(data, 0);
    foreach (const StrStrPair &p, pairs.blockPairsList) {
        ldsw->addInfo(parseInfo(p.first, p.second));
        CHECK_OP(os, );
    }
}

static RadioWidget::Value parseValue(const QString &valueDef, U2OpStatus &os) {
    ParsedPairs pairs(valueDef, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::ID)) {
        os.setError("No id of radio value");
        return RadioWidget::Value("", "");
    }
    QString id = pairs.equalPairs[HRWizardParser::ID];
    QString label = pairs.equalPairs.value(HRWizardParser::LABEL, id);
    RadioWidget::Value result(id, label);

    result.tooltip = pairs.equalPairs.value(HRWizardParser::TOOLTIP, "");
    return result;
}

static QString serializeValue(const RadioWidget::Value &value, int depth) {
    QString vData;
    vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ID, value.id, depth + 1);
    if (!value.label.isEmpty()) {
        vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::LABEL, value.label, depth + 1);
    }
    if (!value.tooltip.isEmpty()) {
        vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TOOLTIP, value.tooltip, depth + 1);
    }

    return HRSchemaSerializer::makeBlock(HRWizardParser::VALUE, Constants::NO_NAME, vData, depth);
}

void WizardWidgetParser::visit(RadioWidget *rw) {
    pairs = ParsedPairs(data, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::ID)) {
        os.setError("No id of radio");
        return;
    }

    rw->setVar(pairs.equalPairs[HRWizardParser::ID]);
    Variable v(rw->var());
    foreach (const StrStrPair &p, pairs.blockPairsList) {
        if (p.first == HRWizardParser::VALUE) {
            RadioWidget::Value value = parseValue(p.second, os);
            CHECK_OP(os, );
            rw->add(value);
            if (!v.isAssigned()) {
                v.setValue(value.id);
            }
        }
    }

    addVariable(v);
    CHECK_OP(os, );
}

void WizardWidgetParser::visit(SettingsWidget *sw) {
    pairs = ParsedPairs(data, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::ID)) {
        os.setError("No id of settings-widget");
        return;
    }

    sw->setVar(SettingsWidget::SETTING_PREFIX + pairs.equalPairs[HRWizardParser::ID]);
    sw->setType(pairs.equalPairs[HRWizardParser::TYPE]);
    sw->setLabel(pairs.equalPairs[HRWizardParser::LABEL]);
    Variable v(sw->var());

    addVariable(v);
    CHECK_OP(os, );
}

void WizardWidgetParser::visit(BowtieWidget *bw) {
    pairs = ParsedPairs(data, 0);
    if (pairs.blockPairsList.size() != 2) {
        os.setError("Not enough attributes for Bowtie index widget");
        return;
    }
    bw->idxDir = parseInfo(pairs.blockPairsList[0].first, pairs.blockPairsList[0].second);
    CHECK_OP(os, );
    bw->idxName = parseInfo(pairs.blockPairsList[1].first, pairs.blockPairsList[1].second);
    CHECK_OP(os, );
}

void WizardWidgetParser::visit(TophatSamplesWidget *tsw) {
    pairs = ParsedPairs(data, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::DATASETS_PROVIDER)) {
        os.setError(HRWizardParser::tr("Not enough attributes for Tophat samples widget"));
        return;
    }
    if (1 != pairs.blockPairsList.size()) {
        os.setError(HRWizardParser::tr("Not enough attributes for Tophat samples widget"));
        return;
    }
    tsw->datasetsProvider = pairs.equalPairs[HRWizardParser::DATASETS_PROVIDER];
    tsw->samplesAttr = parseInfo(pairs.blockPairsList[0].first, pairs.blockPairsList[0].second);
}

void WizardWidgetParser::visit(LabelWidget *lw) {
    pairs = ParsedPairs(data, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::TEXT)) {
        os.setError(HRWizardParser::tr("Not enough attributes for the label widget"));
        return;
    }
    lw->text = pairs.equalPairs[HRWizardParser::TEXT];

    if (pairs.equalPairs.contains(HRWizardParser::TEXT_COLOR)) {
        lw->textColor = pairs.equalPairs[HRWizardParser::TEXT_COLOR];
    }
    if (pairs.equalPairs.contains(HRWizardParser::BACKGROUND_COLOR)) {
        lw->backgroundColor = pairs.equalPairs[HRWizardParser::BACKGROUND_COLOR];
    }
}

SelectorValue WizardWidgetParser::parseSelectorValue(ActorPrototype *srcProto, const QString &valueDef) {
    ParsedPairs pairs(valueDef, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::ID)) {
        os.setError(HRWizardParser::tr("Id is undefined in some selector value definition"));
        return SelectorValue("", "");
    }
    QString id = pairs.equalPairs[HRWizardParser::ID];
    if (!pairs.equalPairs.contains(HRWizardParser::PROTOTYPE)) {
        os.setError(HRWizardParser::tr("Prototype is undefined in the selector value definition: %1").arg(id));
        return SelectorValue("", "");
    }
    QString protoId = pairs.equalPairs[HRWizardParser::PROTOTYPE];
    SelectorValue result(id, protoId);
    result.setName(pairs.equalPairs[HRWizardParser::NAME]);
    if (srcProto->getId() == protoId) {
        if (!pairs.blockPairsList.isEmpty()) {
            os.setError(HRWizardParser::tr("The same prototype could not be mapped: %1").arg(protoId));
        }
        return result;
    }
    foreach (const StrStrPair &pair, pairs.blockPairsList) {
        if (pair.first != HRWizardParser::PORT_MAPPING) {
            os.setError(HRWizardParser::tr("Unknown block name in selector value definition: %1").arg(pair.first));
            return result;
        }
        PortMapping mapping = parsePortMapping(pair.second);
        CHECK_OP(os, result);
        result.addPortMapping(mapping);
    }
    return result;
}

PortMapping WizardWidgetParser::parsePortMapping(const QString &mappingDef) {
    ParsedPairs pairs(mappingDef, 0);
    if (!pairs.equalPairs.contains(HRWizardParser::SRC_PORT)) {
        os.setError(HRWizardParser::tr("Undefined source port id for some port mapping"));
        return PortMapping("", "");
    }
    if (!pairs.equalPairs.contains(HRWizardParser::DST_PORT)) {
        os.setError(HRWizardParser::tr("Undefined destination port id for some port mapping"));
        return PortMapping("", "");
    }
    QString srcPortId = pairs.equalPairs[HRWizardParser::SRC_PORT];
    QString dstPortId = pairs.equalPairs[HRWizardParser::DST_PORT];
    PortMapping result(srcPortId, dstPortId);
    foreach (const StrStrPair &pair, pairs.blockPairsList) {
        if (pair.first != HRWizardParser::SLOTS_MAPPRING) {
            os.setError(HRWizardParser::tr("Unknown block name in port mapping definition: %1").arg(pair.first));
            return result;
        }
        parseSlotsMapping(result, pair.second);
        CHECK_OP(os, result);
    }
    return result;
}

void WizardWidgetParser::parseSlotsMapping(PortMapping &pm, const QString &mappingDef) {
    ParsedPairs pairs(mappingDef, 0);
    foreach (const StrStrPair &pair, pairs.equalPairsList) {
        QString srcSlotId = pair.first;
        QString dstSlotId = pair.second;
        pm.addSlotMapping(SlotMapping(srcSlotId, dstSlotId));
    }
}

void WizardWidgetParser::getLabelSize(WidgetsArea *wa) {
    if (pairs.equalPairs.contains(HRWizardParser::LABEL_SIZE)) {
        QString &sizeStr = pairs.equalPairs[HRWizardParser::LABEL_SIZE];
        bool ok = true;
        int size = sizeStr.toInt(&ok);
        if (!ok) {
            os.setError(QObject::tr("Wrong label size value: %1").arg(sizeStr));
            return;
        }
        wa->setLabelSize(size);
    }
}

void WizardWidgetParser::getTitle(WidgetsArea *wa) {
    if (pairs.equalPairs.contains(HRWizardParser::TITLE)) {
        wa->setTitle(pairs.equalPairs[HRWizardParser::TITLE]);
    }
}

WizardWidget * WizardWidgetParser::createWidget(const QString &id) {
    if (LogoWidget::ID == id) {
        return new LogoWidget();
    } else if (GroupWidget::ID == id) {
        return new GroupWidget();
    } else if (ElementSelectorWidget::ID == id) {
        return new ElementSelectorWidget();
    } else if (PairedReadsWidget::ID == id) {
        return new PairedReadsWidget();
    } else if (UrlAndDatasetWidget::ID == id) {
        return new UrlAndDatasetWidget();
    } else if (RadioWidget::ID == id) {
        return new RadioWidget();
    } else if (SettingsWidget::ID == id) {
        return new SettingsWidget();
    } else if (BowtieWidget::ID == id) {
        return new BowtieWidget();
    } else if (TophatSamplesWidget::ID == id) {
        return new TophatSamplesWidget();
    } else if (LabelWidget::ID == id) {
        return new LabelWidget();
    } else {
        return new AttributeWidget();
    }
}

void WizardWidgetParser::addVariable(const Variable &v) {
    if (vars.contains(v.getName())) {
        os.setError(QObject::tr("The variable is already defined: %1").arg(v.getName()));
        return;
    }
    vars[v.getName()] = v;
}

AttributeInfo WizardWidgetParser::parseInfo(const QString &attrStr, const QString &body) {
    QStringList vals = attrStr.split(Constants::DOT, QString::SkipEmptyParts);
    if (2 != vals.size()) {
        os.setError(HRWizardParser::tr("Unknown widget name: %1").arg(attrStr));
        return AttributeInfo("", "");
    }

    ParsedPairs pairs = ParsedPairs(body, 0);
    QVariantMap hints;
    foreach (const QString &id, pairs.equalPairs.keys()) {
        hints[id] = pairs.equalPairs[id];
    }

    return AttributeInfo(vals[0], vals[1], hints);
}

/************************************************************************/
/* PageContentParser */
/************************************************************************/
PageContentParser::PageContentParser(ParsedPairs &pairs,
    const QMap<QString, Actor*> &actorMap,
    QMap<QString, Variable> &vars,
    U2OpStatus &os)
: pairs(pairs), actorMap(actorMap), vars(vars), os(os)
{

}

void PageContentParser::visit(DefaultPageContent *content) {
    foreach (const StrStrPair &pair, pairs.blockPairsList) {
        WizardWidgetParser wParser(pair.first, pair.second, actorMap, vars, os);
        if (LogoWidget::ID == pair.first) {
            content->getLogoArea()->accept(&wParser);
        } else if (DefaultPageContent::PARAMETERS == pair.first) {
            content->getParamsArea()->accept(&wParser);
        }
        CHECK_OP(os, );
    }
}

/************************************************************************/
/* HRWizardSerializer */
/************************************************************************/
QString HRWizardSerializer::serialize(Wizard *wizard, int depth) {
    QString wizardData;

    if (Wizard::DEFAULT_NAME != wizard->getName()) {
        wizardData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::NAME,
            wizard->getName(), depth + 1);
    }

    if (wizard->isAutoRun()) {
        wizardData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::AUTORUN, "true", depth + 1);
    }

    if (!wizard->hasRunButton()) {
        wizardData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::HAS_RUN_BUTTON, "false", depth + 1);
    }
    if (!wizard->hasDefaultsButton()) {
        wizardData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::HAS_DEFAULTS_BUTTON, "false", depth + 1);
    }

    if (!wizard->getResults().isEmpty()) {
        wizardData += serializeResults(wizard->getResults(), depth + 1);
    }

    foreach (WizardPage *page, wizard->getPages()) {
        wizardData += serializePage(page, depth + 1);
    }

    return HRSchemaSerializer::makeBlock(HRWizardParser::WIZARD,
        Constants::NO_NAME, wizardData, depth);
}

QString HRWizardSerializer::serializePage(WizardPage *page, int depth) {
    QString pageData;

    pageData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ID, page->getId(), depth + 1);
    pageData += HRWizardSerializer::serializeNextId(page, depth + 1);
    if (!page->getTitle().isEmpty()) {
        pageData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TITLE,
            page->getTitle(), depth + 1);
    }
    if (DefaultPageContent::ID != page->getContent()->getTemplateId()) {
        pageData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TEMPLATE,
            page->getContent()->getTemplateId(), depth + 1);
    }

    PageContentSerializer cs(depth + 1);
    page->getContent()->accept(&cs);
    pageData += cs.getResult();

    return HRSchemaSerializer::makeBlock(HRWizardParser::PAGE,
        Constants::NO_NAME, pageData, depth);
}

QString HRWizardSerializer::serializeResults(const QMap<QString, QList<Predicate> > results, int depth) {
    QString rData;

    foreach (const QString &result, results.keys()) {
        QStringList preds;
        foreach (const Predicate &p, results[result]) {
            preds << p.toString();
        }
        QString predsStr = preds.join(" ");
        rData += HRSchemaSerializer::makeEqualsPair(result, predsStr, depth + 1);
    }

    return HRSchemaSerializer::makeBlock(HRWizardParser::RESULT,
        Constants::NO_NAME, rData, depth);
}

QString HRWizardSerializer::serializeNextId(WizardPage *page, int depth) {
    if (page->nextIdMap().isEmpty()) {
        if (page->plainNextId().isEmpty()) {
            return "";
        }
        return HRSchemaSerializer::makeEqualsPair(HRWizardParser::NEXT, page->plainNextId(), depth);
    }
    QString nextData;
    foreach (const Predicate &p, page->nextIdMap().keys()) {
        QString id = page->nextIdMap()[p];
        nextData += HRSchemaSerializer::makeEqualsPair(id, p.toString(), depth + 1);
    }
    return HRSchemaSerializer::makeBlock(HRWizardParser::NEXT,
        Constants::NO_NAME, nextData, depth);
}

/************************************************************************/
/* WizardWidgetSerializer */
/************************************************************************/
WizardWidgetSerializer::WizardWidgetSerializer(int _depth)
: depth(_depth)
{

}

void WizardWidgetSerializer::visit(AttributeWidget *aw) {
    result = serializeInfo(aw->getInfo(), depth);
}

void WizardWidgetSerializer::visit(WidgetsArea *wa) {
    QString wData;
    // write title
    if (!wa->getTitle().isEmpty()) {
        wData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TITLE,
            wa->getTitle(), depth + 1);
    }
    // write label size
    if (wa->hasLabelSize()) {
        wData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::LABEL_SIZE,
            QString::number(wa->getLabelSize()), depth + 1);
    }
    // write additional info
    wData += addInfo;
    // write widgets
    foreach (WizardWidget *w, wa->getWidgets()) {
        WizardWidgetSerializer ws(depth + 1);
        w->accept(&ws);
        wData += ws.getResult();
    }

    result = HRSchemaSerializer::makeBlock(wa->getName(),
        Constants::NO_NAME, wData, depth);
}

void WizardWidgetSerializer::visit(GroupWidget *gw) {
    if (GroupWidget::HIDEABLE == gw->getType()) {
        addInfo = HRSchemaSerializer::makeEqualsPair(HRWizardParser::TYPE,
            HRWizardParser::HIDEABLE, depth + 1);
    }
    visit((WidgetsArea*)gw);
}

void WizardWidgetSerializer::visit(LogoWidget *lw) {
    QString wData;
    if (!lw->isDefault()) {
        wData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::LOGO_PATH,
            lw->getLogoPath(), depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(LogoWidget::ID,
        Constants::NO_NAME, wData, depth);
}

void WizardWidgetSerializer::visit(ElementSelectorWidget *esw) {
    QString wData;
    wData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ELEMENT_ID,
        esw->getActorId(), depth + 1);
    if (!esw->getLabel().isEmpty()) {
        wData += HRSchemaSerializer::makeEqualsPair(AttributeInfo::LABEL,
            esw->getLabel(), depth + 1);
    }
    foreach (const SelectorValue &value, esw->getValues()) {
        wData += serializeSelectorValue(value, depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(ElementSelectorWidget::ID,
        Constants::NO_NAME, wData, depth);
}

void WizardWidgetSerializer::visit(PairedReadsWidget *dsw) {
    QString dData;
    foreach (const AttributeInfo &info, dsw->getInfos()) {
        dData += serializeInfo(info, depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(PairedReadsWidget::ID,
        Constants::NO_NAME, dData, depth);
}

void WizardWidgetSerializer::visit(UrlAndDatasetWidget *ldsw) {
    QString dData;
    foreach (const AttributeInfo &info, ldsw->getInfos()) {
        dData += serializeInfo(info, depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(UrlAndDatasetWidget::ID,
        Constants::NO_NAME, dData, depth);
}

void WizardWidgetSerializer::visit(RadioWidget *rw) {
    QString rData;
    rData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ID, rw->var(), depth + 1);
    foreach (const RadioWidget::Value &value, rw->values()) {
        rData += serializeValue(value, depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(RadioWidget::ID,
        Constants::NO_NAME, rData, depth);
}

void WizardWidgetSerializer::visit(SettingsWidget *sw) {
    QString rData;
    QString var = sw->var();
    if (var.startsWith(SettingsWidget::SETTING_PREFIX)) {
        var.remove(0, SettingsWidget::SETTING_PREFIX.length());
    }

    rData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ID, var, depth + 1);
    rData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TYPE, sw->type(), depth + 1);
    rData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::LABEL, sw->label(), depth + 1);
    result = HRSchemaSerializer::makeBlock(SettingsWidget::ID,
        Constants::NO_NAME, rData, depth);
}

void WizardWidgetSerializer::visit(BowtieWidget *bw) {
    QString bData;
    bData += serializeInfo(bw->idxDir, depth + 1);
    bData += serializeInfo(bw->idxName, depth + 1);
    result = HRSchemaSerializer::makeBlock(BowtieWidget::ID,
        Constants::NO_NAME, bData, depth);
}

void WizardWidgetSerializer::visit(TophatSamplesWidget *tsw) {
    QString tsData;
    tsData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::DATASETS_PROVIDER,
        tsw->datasetsProvider, depth + 1);
    tsData += serializeInfo(tsw->samplesAttr, depth + 1);

    result = HRSchemaSerializer::makeBlock(TophatSamplesWidget::ID,
        Constants::NO_NAME, tsData, depth);
}

void WizardWidgetSerializer::visit(LabelWidget *lw) {
    QString lData;
    lData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TEXT, lw->text, depth + 1);
    if (lw->backgroundColor != LabelWidget::DEFAULT_BG_COLOR) {
        lData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::BACKGROUND_COLOR, lw->backgroundColor, depth + 1);
    }
    if (lw->textColor != LabelWidget::DEFAULT_TEXT_COLOR) {
        lData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::TEXT_COLOR, lw->textColor, depth + 1);
    }
    result = HRSchemaSerializer::makeBlock(LabelWidget::ID, Constants::NO_NAME, lData, depth);
}

QString WizardWidgetSerializer::serializeSlotsMapping(const QList<SlotMapping> &mappings, int depth) const {
    QString smData;
    foreach (const SlotMapping &mapping, mappings) {
        smData += HRSchemaSerializer::makeEqualsPair(mapping.getSrcId(),
            mapping.getDstId(), depth + 1);
    }
    return HRSchemaSerializer::makeBlock(HRWizardParser::SLOTS_MAPPRING,
        Constants::NO_NAME, smData, depth);
}

QString WizardWidgetSerializer::serializePortMapping(const PortMapping &mapping, int depth) const {
    QString pmData;
    pmData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::SRC_PORT,
        mapping.getSrcId(), depth + 1);
    pmData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::DST_PORT,
        mapping.getDstId(), depth + 1);
    pmData += serializeSlotsMapping(mapping.getMappings(), depth + 1);
    return HRSchemaSerializer::makeBlock(HRWizardParser::PORT_MAPPING,
        Constants::NO_NAME, pmData, depth);
}

QString WizardWidgetSerializer::serializeSelectorValue(const SelectorValue &value, int depth) const {
    QString vData;
    vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::ID,
        value.getValue(), depth + 1);
    vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::PROTOTYPE,
        value.getProtoId(), depth + 1);
    if (!value.getName().isEmpty()) {
        vData += HRSchemaSerializer::makeEqualsPair(HRWizardParser::NAME,
            value.getName(), depth + 1);
    }
    foreach (const PortMapping &mapping, value.getMappings()) {
        vData += serializePortMapping(mapping, depth + 1);
    }
    return HRSchemaSerializer::makeBlock(HRWizardParser::VALUE,
        Constants::NO_NAME, vData, depth);
}

QString WizardWidgetSerializer::serializeInfo(const AttributeInfo &info, int depth) const {
    QString iData;

    foreach (const QString &id, info.hints.keys()) {
        QString value = info.hints[id].toString();
        if (!value.isEmpty()) {
            iData += HRSchemaSerializer::makeEqualsPair(id, value, depth + 1);
        }
    }

    QString name = info.actorId + Constants::DOT + info.attrId;
    return HRSchemaSerializer::makeBlock(name, Constants::NO_NAME,
        iData, depth);
}

const QString & WizardWidgetSerializer::getResult() {
    return result;
}

/************************************************************************/
/* PageContentSerializer */
/************************************************************************/
PageContentSerializer::PageContentSerializer(int _depth)
: depth(_depth)
{

}

void PageContentSerializer::visit(DefaultPageContent *content) {
    WizardWidgetSerializer pws(depth);
    content->getParamsArea()->accept(&pws);
    result += pws.getResult();

    if (!content->getLogoArea()->isDefault()) {
        WizardWidgetSerializer lws(depth);
        content->getLogoArea()->accept(&lws);
        result += lws.getResult();
    }
}

const QString & PageContentSerializer::getResult() const {
    return result;
}

} // WorkflowSerialize
} // U2
