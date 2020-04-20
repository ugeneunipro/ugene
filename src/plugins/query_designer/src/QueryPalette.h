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

#ifndef _U2_QUERY_PALETTE_H_
#define _U2_QUERY_PALETTE_H_

#include <QAction>
#include <QTreeWidget>

namespace U2 {

class QDDistanceIds {
public:
    static const QString E2S;
    static const QString S2E;
    static const QString E2E;
    static const QString S2S;
};

class QDActorPrototype;
class QueryPalette : public QTreeWidget {
    Q_OBJECT
public:
    static const QString MIME_TYPE;
    QueryPalette(QWidget* parent=NULL);

    QVariant saveState() const;
    void restoreState(const QVariant&);
    void resetSelection();
signals:
    void processSelected(QDActorPrototype*);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);
private slots:
    void sl_selectProcess(bool checked=false);
private:
    void setContent();
    QAction* createItemAction(QDActorPrototype* item);
    QAction* createItemAction(const QString& constraintId);
private:
    QTreeWidgetItem* overItem;
    QAction* currentAction;
    QPoint  dragStartPosition;
    //  < category id, item ids > ids r used as display names
    QMap< QString, QList<QString> > categoryMap;
    QMap<QAction*, QTreeWidgetItem*> actionMap;
    friend class PaletteDelegate;
};

}//namespace

Q_DECLARE_METATYPE(QAction *)

#endif
