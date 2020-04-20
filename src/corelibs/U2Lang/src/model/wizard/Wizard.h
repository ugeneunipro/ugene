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

#ifndef _U2_WIZARD_H_
#define _U2_WIZARD_H_

#include <U2Core/global.h>
#include <U2Core/U2OpStatus.h>

#include <U2Lang/Schema.h>
#include <U2Lang/Variable.h>

namespace U2 {

class WizardPage;

class U2LANG_EXPORT Wizard {
public:
    Wizard(const QString &name, const QList<WizardPage*> &pages, const QString &helpPageId);
    virtual ~Wizard();

    void validate(const Workflow::Schema *schema, U2OpStatus &os) const;
    void addVariable(const Variable &v);
    QMap<QString, Variable> getVariables() const;
    QString getResult(const QMap<QString, Variable> &vars) const;

    const QString & getName() const;
    const QList<WizardPage*> & getPages() const;

    void addResult(const QList<Predicate> &preds, const QString &result);
    QMap<QString, QList<Predicate> > getResults() const;

    QString getFinishLabel() const;
    void setFinishLabel(const QString &value);

    bool isAutoRun() const;
    void setAutoRun(bool value);

    bool hasRunButton() const;
    void setHasRunButton(bool value);

    bool hasDefaultsButton() const;
    void setHasDefaultsButton(bool value);

    const QString getHelpPageId() const;

    static const QString DEFAULT_NAME;

private:
    QString name;
    QList<WizardPage*> pages;
    QMap<QString, Variable> vars;
    bool autoRun;
    bool withRunButton;
    bool withDefaultsButton;
    QMap<QString, QList<Predicate> > results;
    QString finishLabel;
    const QString helpPageId;
};

} // U2

#endif // _U2_WIZARD_H_
