/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GLASS_VIEW_H_
#define _U2_GLASS_VIEW_H_

#include <QGraphicsView>

#include <U2Core/global.h>

class QTextDocument;

namespace U2 {

class U2GUI_EXPORT GlassPane : public QWidget {
    Q_OBJECT
public:
    GlassPane(QWidget* parent = 0)
        : QWidget(parent) {
    }
    virtual void paint(QPainter*) = 0;
    virtual bool eventFilter(QObject*, QEvent* e) {
        return this->event(e);
    }
};

class U2GUI_EXPORT GlassView : public QGraphicsView {
    Q_OBJECT
public:
    GlassView(QGraphicsScene* scene)
        : QGraphicsView(scene), glass(nullptr) {
    }

public slots:
    void setGlass(GlassPane*);

protected:
    bool viewportEvent(QEvent* event);
    void paintEvent(QPaintEvent*);
    void scrollContentsBy(int dx, int dy);

    GlassPane* glass;
};

}  // namespace U2
#endif
