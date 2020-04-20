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

#ifndef _U2_GRAPH_MENU_H_
#define _U2_GRAPH_MENU_H_

#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVGraphModel.h>
#include <U2View/GSequenceGraphViewWithFactory.h>
#include <U2View/ADVSingleSequenceWidget.h>


namespace U2 {

class DNAAlphabet;

/**
 * Action for drawing a graph
 */
class U2VIEW_EXPORT GraphAction : public QAction
{
    Q_OBJECT
public:
    GraphAction(GSequenceGraphFactory*);

private:
    GSequenceGraphFactory*  factory;
    GSequenceGraphView*     view;
    bool                    isBookmarkUpdate;
    QList<QVariant>         positions;

private slots:
    void sl_handleGraphAction();
    void sl_updateGraphView(const QStringList &, const QVariantMap&);
    void sl_renderError();
};


/**
 * Menu of actions for drawing graphs
 */
class U2VIEW_EXPORT GraphMenuAction : public ADVSequenceWidgetAction
{
    Q_OBJECT
public:
    GraphMenuAction(const DNAAlphabet* a);
    static void addGraphAction(ADVSequenceObjectContext*, GraphAction*);
public slots:
    void sl_closeAllGraphs();
private:
    static const QString ACTION_NAME;
    QMenu* menu;
    QAction* separator;

    static GraphMenuAction* findGraphMenuAction(ADVSequenceObjectContext*);
};


}//namespace


#endif
