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

#ifndef _U2_GRAPH_DOCUMENT_H_
#define _U2_GRAPH_DOCUMENT_H_

#include <U2Lang/QDConstraint.h>

#include <QPair>
#include <QString>


namespace U2 {

class QDDocument;
class QDDocStatement {
public:
    virtual ~QDDocStatement();

    const QList< QPair<QString, QString> >& getAttributes() const { return attributes; }
    QString getAttribute(const QString& name) const;
    void setAttribute(const QString& name, const QString& value);
    void setComment(const QString& com) { comment = com; }
    const QString& getComment() const { return comment; }
    virtual QString toString() const;
protected:
    int evalStringLen() const;
protected:
    QString comment;
    QList< QPair<QString, QString> > attributes;
};

enum QDStatementType { Element, Group };

class QDElementStatement : public QDDocStatement {
    friend class QDDocument;
public:
    QDElementStatement(const QString& _id, QDStatementType _type)
        : id(_id), type(_type), document(NULL) {}
    const QString& getId() const { return id; }
    QDStatementType getType() const { return type; }
    QString toString() const;
    QString definedIn() const;
    QDDocument* getDocument() const { return document; }
public:
    static const QString GEOMETRY_ATTR_NAME;
    static const QString ALGO_ATTR_NAME;
private:
    QString id;
    QDStatementType type;
    QDDocument* document;
};

class QDLinkStatement : public QDDocStatement {
    friend class QDDocument;
public:
    QDLinkStatement(const QList<QString>& ids) : elementIds(ids), document(NULL) {}
    const QList<QString>& getElementIds() const { return elementIds; }
    QString toString() const;
    QDDocument* getDocument() const { return document; }
public:
    static const QString TYPE_ATTR_NAME;
private:
    QList<QString> elementIds;
    QDDocument* document;
};

class QDDocument : public QObject {
    Q_OBJECT
public:
    QDDocument() : schemaStrand(QDStrand_Both) {}
    ~QDDocument();
    bool setContent(const QString& content);
    QByteArray toByteArray() const;
    void setName(const QString& name) { docName = name; }
    const QString& getName() const { return docName; }
    bool addElement(QDElementStatement* el);
    void addLink(QDLinkStatement* lnk) {
        assert(lnk->document==NULL);
        links.append(lnk);
        lnk->document = this;
    }
    const QList<QDElementStatement*>& getElements() const { return elements; }
    QList<QDElementStatement*> getElements(QDStatementType type) const;
    QDElementStatement* getElement(const QString& id) const;
    const QList<QDLinkStatement*>& getLinks() const { return links; }
    const QList<QString>& getImportedUrls() const { return importedUrls; }
    static QString definedIn(const QString& id);
    static QString getLocalName(const QString& id);
    QDElementStatement* findElementByUnitName(QDElementStatement* parent, const QString& unitName) const;

    const QString& getDocDesc() const { return docDesc; }
    void setDocDesc(const QString& newDesc) { docDesc = newDesc; }
    const QStringList& getOrder() const { return order; }
    void saveOrder(const QList<QDActor*>& actors);
    void parseSchemaStrand(const QString& str);
    QDStrandOption getSchemaStrand() const { return schemaStrand; }
    void setSchemaStrand(QDStrandOption stOp) { schemaStrand = stOp; }

    static bool isHeaderLine(const QString &line);

public:
    static const QString HEADER_LINE;
    static const QString DEPRECATED_HEADER_LINE;
    static const QString GROUPS_SECTION;
    static const QString ID_PATTERN;
private:
    void findImportedUrls(const QString& str);
    bool findElementStatements(const QString& str);
    bool findLinkStatements(const QString& str);
    void findComments(const QString& str);
    void parseOrder(const QString& str);
    static QMap<QString, QString> string2attributesMap(const QString& str);
    static QList<QString> idsFromString(const QString& str);
private:
    QString docName;
    QList<QDElementStatement*> elements;
    QList<QDLinkStatement*> links;
    QList<QString> importedUrls;
    QString docDesc;
    //ID, comment
    QMap<QString, QString> comments;
    QStringList order;
    QDStrandOption schemaStrand;
};

class QDIdMapper {
public:
    static QString distance2string(QDDistanceType type);
    static int string2distance(const QString& str);
    static QDConstraintType string2constraintType(const QString& str);
    static QString constraintType2string(const QDConstraintType& type);
};

}//namespace

Q_DECLARE_METATYPE(U2::QDDocument*)

#endif
