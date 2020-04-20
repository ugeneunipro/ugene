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

#include <U2Lang/WizardPage.h>

#include "Wizard.h"

namespace U2 {

const QString Wizard::DEFAULT_NAME("Wizard");

Wizard::Wizard(const QString &_name, const QList<WizardPage*> &_pages, const QString &_helpPageId)
: name(_name), pages(_pages), autoRun(false), withRunButton(true), withDefaultsButton(true), helpPageId(_helpPageId)
{

}

Wizard::~Wizard() {
    foreach (WizardPage *page, pages) {
        delete page;
    }
    pages.clear();
}

const QString & Wizard::getName() const {
    return name;
}

const QList<WizardPage*> & Wizard::getPages() const {
    return pages;
}

void Wizard::validate(const Workflow::Schema *schema, U2OpStatus &os) const {
    foreach (WizardPage *page, pages) {
        page->validate(schema->getProcesses(), os);
        CHECK_OP(os, );
    }
}

void Wizard::addVariable(const Variable &v) {
    vars[v.getName()] = v;
}

QMap<QString, Variable> Wizard::getVariables() const {
    return vars;
}

bool Wizard::isAutoRun() const {
    return autoRun;
}

void Wizard::setAutoRun(bool value) {
    autoRun = value;
}

void Wizard::addResult(const QList<Predicate> &preds, const QString &result) {
    results[result] = preds;
}

QMap<QString, QList<Predicate> > Wizard::getResults() const {
    return results;
}

QString Wizard::getResult(const QMap<QString, Variable> &vars) const {
    foreach (const QString &result, results.keys()) {
        const QList<Predicate> &preds = results[result];
        bool match = true;
        foreach (const Predicate &p, preds) {
            match &= p.isTrue(vars);
        }
        if (match) {
            return result;
        }
    }
    return "";
}

QString Wizard::getFinishLabel() const {
    return finishLabel;
}

void Wizard::setFinishLabel(const QString &value) {
    finishLabel = value;
}

bool Wizard::hasRunButton() const {
    return withRunButton;
}

void Wizard::setHasRunButton(bool value) {
    withRunButton = value;
}

bool Wizard::hasDefaultsButton() const {
    return withDefaultsButton;
}

void Wizard::setHasDefaultsButton(bool value) {
    withDefaultsButton = value;
}

const QString Wizard::getHelpPageId()const {
    return helpPageId;
}

} // U2
