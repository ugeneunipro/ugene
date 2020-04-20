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

#ifndef _U2_GSEQUENCE_GRAPH_VIEW_WITH_FACTORY_H_
#define _U2_GSEQUENCE_GRAPH_VIEW_WITH_FACTORY_H_


#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVGraphModel.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/ADVSingleSequenceWidget.h>


namespace U2 {


/**
 * Factory that can be used to draw a graph
 */
class U2VIEW_EXPORT GSequenceGraphFactory : public QObject
{
public:
    GSequenceGraphFactory(const QString& _name, QObject* p)
        : QObject(p), graphName(_name){}

    virtual QList<QSharedPointer<GSequenceGraphData> > createGraphs(GSequenceGraphView* v) = 0;

    virtual GSequenceGraphDrawer* getDrawer(GSequenceGraphView* v);

    virtual bool isEnabled(const U2SequenceObject* o) const  = 0;

    const QString& getGraphName() const {return graphName;}

protected:
    QString graphName;
};


/**
 * Graph view for a sequence with the access to the graph factory
 * NOTE: The access to factory would be required for saving cutoffs as annotations
 */
class U2VIEW_EXPORT GSequenceGraphViewWithFactory : public GSequenceGraphView
{
public:
    GSequenceGraphViewWithFactory(ADVSingleSequenceWidget*, GSequenceGraphFactory*);
    GSequenceGraphFactory* getFactory() const {return factory;}

protected:
    virtual void addActionsToGraphMenu(QMenu* graphMenu);

private:
    GSequenceGraphFactory* factory;
};

} // namespace


#endif
