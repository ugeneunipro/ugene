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

#ifndef _U2_ADV_SEQUENCE_WIDGET_H_
#define _U2_ADV_SEQUENCE_WIDGET_H_

#include <U2Core/global.h>
#include <U2Core/U2Region.h>

#include <QAction>
#include <QWidget>

namespace U2 {

class AnnotatedDNAView;
class U2SequenceObject;
class DNASequenceSelection;
class ADVSequenceWidgetAction;
class ADVSequenceObjectContext;
class GObject;

class U2VIEW_EXPORT ADVSequenceWidget : public QWidget {
    Q_OBJECT
public:
    ADVSequenceWidget(AnnotatedDNAView* ctx);

    AnnotatedDNAView* getAnnotatedDNAView() const {return ctx;}

    virtual QList<U2SequenceObject*> getSequenceObjects() const;
    virtual QList<ADVSequenceObjectContext*> getSequenceContexts() const {return seqContexts;}

    /** actions associated with this block. Automatically deleted with block*/
    virtual void addADVSequenceWidgetAction(ADVSequenceWidgetAction* action);

    ADVSequenceWidgetAction* getADVSequenceWidgetAction(const QString& objName) const;

    QList<ADVSequenceWidgetAction*> getADVSequenceWidgetActions() const { return wActions; }

    virtual ADVSequenceObjectContext* getActiveSequenceContext() const = 0;

    virtual void centerPosition(int pos, QWidget* skipView = NULL) = 0;

    virtual void buildPopupMenu(QMenu& m);

    //TODO: replace with GObjectViewObjectHandler
    virtual bool isWidgetOnlyObject(GObject* o) const { Q_UNUSED(o); return false;}

    virtual void updateState(const QVariantMap& m) {Q_UNUSED(m);}

    virtual void saveState(QVariantMap& m) {Q_UNUSED(m);}

    virtual U2Region getVisibleRange() const = 0;

    virtual void setVisibleRange(const U2Region& r) = 0;

    virtual int getNumBasesVisible() const = 0;

    virtual void setNumBasesVisible(qint64 n) = 0;

    virtual void onSequenceObjectRenamed(const QString& oldName) = 0;

signals:
    void si_sequenceObjectAdded(U2SequenceObject*);
    void si_sequenceObjectRemoved(U2SequenceObject* );

protected:
    AnnotatedDNAView*                   ctx;
    QList<ADVSequenceObjectContext*>    seqContexts;
    QList<ADVSequenceWidgetAction*>     wActions;
};

class U2VIEW_EXPORT ADVSequenceWidgetAction : public QAction {
    Q_OBJECT
public:
    ADVSequenceWidgetAction(const QString& objName, const QString& text)
        : QAction(text, NULL), addToBar(false), addToMenu(false), seqWidget(NULL) { setObjectName(objName); }

    bool                addToBar;
    bool                addToMenu;
    QString             afterMenu;
    ADVSequenceWidget*  seqWidget;
};


}//namespace

#endif
