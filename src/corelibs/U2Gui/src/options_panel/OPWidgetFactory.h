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

#ifndef _U2_OP_WIDGET_FACTORY_H_
#define _U2_OP_WIDGET_FACTORY_H_

#include <U2Gui/ObjectViewModel.h>

#include <QPixmap>

namespace U2 {

enum ObjectViewType {
    ObjViewType_SequenceView,
    ObjViewType_AlignmentEditor,
    ObjViewType_AssemblyBrowser,
    ObjViewType_PhylogeneticTree,
    ObjViewType_ChromAlignmentEditor
};

struct U2GUI_EXPORT OPGroupParameters
{
public:
    OPGroupParameters(QString groupId, QPixmap headerImage, QString title, QString documentationPage);

    inline QString getGroupId() { return groupId; }
    inline QPixmap getIcon() { return groupIcon; }
    inline QString getTitle() { return groupTitle; }
    inline QString getDocumentationPage() { return groupDocumentationPage; }

private:
    QString groupId;
    QPixmap groupIcon;
    QString groupTitle;
    QString groupDocumentationPage;
};

class U2GUI_EXPORT OPFactoryFilterVisitorInterface{
public:
    OPFactoryFilterVisitorInterface(){}
    virtual ~OPFactoryFilterVisitorInterface();

    virtual bool typePass(ObjectViewType factoryViewType) = 0;
    virtual bool alphabetPass(DNAAlphabetType factoryAlphabetType) = 0;
    virtual bool atLeastOneAlphabetPass(DNAAlphabetType factoryAlphabetType) = 0;

};

class U2GUI_EXPORT OPFactoryFilterVisitor : public OPFactoryFilterVisitorInterface{
public:
    OPFactoryFilterVisitor(ObjectViewType _objectViewType)
        :OPFactoryFilterVisitorInterface()
        ,objectViewType(_objectViewType)
        ,objectAlphabetType(DNAAlphabet_RAW){}
    OPFactoryFilterVisitor(DNAAlphabetType _objectAlphabetType)
        :OPFactoryFilterVisitorInterface()
        ,objectViewType(ObjViewType_SequenceView)
        ,objectAlphabetType(_objectAlphabetType){}
    OPFactoryFilterVisitor(ObjectViewType _objectViewType, DNAAlphabetType _objectAlphabetType)
        :OPFactoryFilterVisitorInterface()
        ,objectViewType(_objectViewType)
        ,objectAlphabetType(_objectAlphabetType){}
    OPFactoryFilterVisitor(ObjectViewType _objectViewType, QList<DNAAlphabetType> _objectListAlphabet)
        :OPFactoryFilterVisitorInterface()
        ,objectViewType(_objectViewType)
        ,objectAlphabetType(DNAAlphabet_RAW)
        ,objectAlphabets(_objectListAlphabet){}

    virtual bool typePass(ObjectViewType factoryViewType){ return factoryViewType == objectViewType;}
    virtual bool alphabetPass(DNAAlphabetType factoryAlphabetType) { return factoryAlphabetType == objectAlphabetType;}
    virtual bool atLeastOneAlphabetPass(DNAAlphabetType factoryAlphabetType);

private:
    ObjectViewType objectViewType;
    DNAAlphabetType objectAlphabetType;
    QList<DNAAlphabetType> objectAlphabets;

};


class U2GUI_EXPORT OPWidgetFactory : public QObject
{
    Q_OBJECT

public:
    OPWidgetFactory();
    virtual ~OPWidgetFactory();

    virtual QWidget* createWidget(GObjectView* objView) = 0;

    virtual OPGroupParameters getOPGroupParameters() = 0;

    virtual bool passFiltration (OPFactoryFilterVisitorInterface* filter);

protected:
    virtual ObjectViewType getObjectViewType() { return objectViewOfWidget; }

protected:
    GObjectView*    objView;
    ObjectViewType  objectViewOfWidget;
};


class U2GUI_EXPORT OPCommonWidgetFactory : public QObject
{
    Q_OBJECT
public:
    OPCommonWidgetFactory(QList<QString> groupIds);
    virtual ~OPCommonWidgetFactory();

    virtual QWidget* createWidget(GObjectView *objView) = 0;

    bool isInGroup(QString groupId) { return groupIds.contains(groupId); }

protected:
    QList<QString> groupIds; // groups where the common widget is placed
};

} // namespace

#endif

