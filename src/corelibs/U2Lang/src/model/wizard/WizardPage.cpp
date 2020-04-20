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

#include <U2Core/U2SafePoints.h>

#include "WizardPage.h"

namespace U2 {

/**********************************
* WizardPage
*********************************/
WizardPage::WizardPage(const QString &_id, const QString &_title)
: id(_id), title(_title)
{
}

WizardPage::~WizardPage() {
    delete content;
}

void WizardPage::validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const {
    if (NULL == content) {
        os.setError(QObject::tr("NULL page content"));
        return;
    }
    content->validate(actors, os);
    CHECK_OP(os, );
}

void WizardPage::setNext(const QString &nextId) {
    this->nextId = nextId;
    nextIds.clear();
}

void WizardPage::setNext(const QString &nextId, const Predicate &predicate, U2OpStatus &os) {
    if (nextIds.contains(predicate)) {
        os.setError(QObject::tr("Double condition: %1").arg(predicate.toString()));
        return;
    }
    this->nextId.clear();
    nextIds[predicate] = nextId;
}

QString WizardPage::getNextId(const QMap<QString, Variable> &vars) const {
    if (nextIds.isEmpty()) {
        return nextId;
    }
    foreach (const Predicate &p, nextIds.keys()) {
        if (p.isTrue(vars)) {
            return nextIds[p];
        }
    }
    return "";
}

bool WizardPage::isFinal() const {
    return (nextId.isEmpty() && nextIds.isEmpty());
}

const QString & WizardPage::getId() const {
    return id;
}

const QString & WizardPage::getTitle() const {
    return title;
}

void WizardPage::setContent(TemplatedPageContent *value) {
    content = value;
}

TemplatedPageContent * WizardPage::getContent() {
    return content;
}

/** for serializing */
const QMap<Predicate, QString> & WizardPage::nextIdMap() const {
    return nextIds;
}

const QString & WizardPage::plainNextId() const {
    return nextId;
}

/**********************************
* TemplatedPage
*********************************/
TemplatedPageContent::TemplatedPageContent(const QString &_templateId)
: templateId(_templateId)
{

}

TemplatedPageContent::~TemplatedPageContent() {

}

const QString & TemplatedPageContent::getTemplateId() const {
    return templateId;
}

TemplatedPageContent * PageContentFactory::createContent(const QString &id, U2OpStatus &os) {
    if (DefaultPageContent::ID == id) {
        return new DefaultPageContent();
    }
    os.setError(QObject::tr("Unknown page template id: %1").arg(id));
    return NULL;
}

/**********************************
* DefaultTemplatePage
*********************************/
const QString DefaultPageContent::ID("default");
const QString DefaultPageContent::PARAMETERS("parameters-area");
const int DefaultPageContent::HEIGHT = 400;
const int DefaultPageContent::WIDTH = 720;

DefaultPageContent::DefaultPageContent()
: TemplatedPageContent(ID)
{
    logoArea = new LogoWidget();
    paramsArea = new WidgetsArea(PARAMETERS);
}

DefaultPageContent::~DefaultPageContent() {
    delete logoArea;
    delete paramsArea;
}

void DefaultPageContent::accept(TemplatedPageVisitor *visitor) {
    visitor->visit(this);
}

void DefaultPageContent::validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const {
    if (NULL == logoArea) {
        os.setError(QObject::tr("NULL logo area"));
        return;
    }
    if (NULL == paramsArea) {
        os.setError(QObject::tr("NULL parameters area"));
        return;
    }

    logoArea->validate(actors, os);
    CHECK_OP(os, );
    paramsArea->validate(actors, os);
    CHECK_OP(os, );
}

void DefaultPageContent::addParamWidget(WizardWidget *widget) {
    paramsArea->addWidget(widget);
}

void DefaultPageContent::setLogoPath(const QString &path) {
    logoArea->setLogoPath(path);
}

LogoWidget * DefaultPageContent::getLogoArea() {
    return logoArea;
}

WidgetsArea * DefaultPageContent::getParamsArea() {
    return paramsArea;
}

int DefaultPageContent::getPageDefaultHeight() const {
    return HEIGHT;
}

int DefaultPageContent::getPageWidth() const {
    return WIDTH;
}

} // U2
