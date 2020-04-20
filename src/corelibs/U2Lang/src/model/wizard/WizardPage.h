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

#ifndef _U2_WIZARDPAGE_H_
#define _U2_WIZARDPAGE_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/Schema.h>
#include <U2Lang/Variable.h>
#include <U2Lang/WizardWidget.h>

namespace U2 {

class TemplatedPageContent;
class TemplatedPageVisitor;

class U2LANG_EXPORT WizardPage {
public:
    WizardPage(const QString &id, const QString &title);
    virtual ~WizardPage();

    void validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const;

    void setNext(const QString &id);
    void setNext(const QString &id, const Predicate &predicate, U2OpStatus &os);
    QString getNextId(const QMap<QString, Variable> &vars) const;
    bool isFinal() const;

    const QString & getId() const;
    const QString & getTitle() const;
    void setContent(TemplatedPageContent *value);
    TemplatedPageContent * getContent();

    /** for serializing */
    const QMap<Predicate, QString> & nextIdMap() const;
    const QString & plainNextId() const;

private:
    QString id;
    QString nextId;
    QMap<Predicate, QString> nextIds; // predicate <-> id
    QString title;

    TemplatedPageContent *content;
};

class U2LANG_EXPORT TemplatedPageContent {
public:
    TemplatedPageContent(const QString &templateId);
    virtual ~TemplatedPageContent();

    virtual void accept(TemplatedPageVisitor *visitor) = 0;
    virtual void validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const = 0;

    const QString & getTemplateId() const;

private:
    QString templateId;
};

class PageContentFactory {
public:
    static TemplatedPageContent * createContent(const QString &id, U2OpStatus &os);
};

/**
 * Default page template consists of two areas (horizontal layout):
 * logo area (1/3 of the page)and parameters area(2/3).
 */
class U2LANG_EXPORT DefaultPageContent : public TemplatedPageContent {
public:
    DefaultPageContent();
    virtual ~DefaultPageContent();

    virtual void accept(TemplatedPageVisitor *visitor);
    virtual void validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const;

    void addParamWidget(WizardWidget *widget);
    void setLogoPath(const QString &path);

    /** Default height of the page in pixels. Default height can be changed
     * to another one by logo widget.
     */
    int getPageDefaultHeight() const;
    /** The width in pixels of whole page: logo + parameters */
    int getPageWidth() const;

    LogoWidget * getLogoArea();
    WidgetsArea * getParamsArea();

private:
    LogoWidget *logoArea;
    WidgetsArea *paramsArea;

    static const int HEIGHT; // px
    static const int WIDTH; // px

public:
    static const QString ID;
    static const QString PARAMETERS;
};

/////////////////////////////////////////////////////
class U2LANG_EXPORT TemplatedPageVisitor {
public:
    virtual void visit(DefaultPageContent *) = 0;
};

} // U2

#endif // _U2_WIZARDPAGE_H_
