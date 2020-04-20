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

#ifndef _ASSEMBLY_VARIANT_ROW_
#define _ASSEMBLY_VARIANT_ROW_

#include <U2Core/U2Variant.h>

#include "AssemblyCellRenderer.h"
#include "AssemblyModel.h"
#include "AssemblyVariantHint.h"

#include <QWidget>
#include <QMenu>
#include <qevent.h>

namespace U2 {

class AssemblyBrowser;
class AssemblyBrowserUi;
class VariantTrackObject;

class AssemblyVariantRow : public QWidget {
    Q_OBJECT
public:
    AssemblyVariantRow(QWidget *parent, VariantTrackObject *trackObj, AssemblyBrowser *browser);

    VariantTrackObject *getTrackObject() const;

    static bool isSNP(const U2Variant &v);

public slots:
    void sl_zoomPerformed();
    void sl_offsetsChanged();
    void sl_hideHint();

signals:
    void si_mouseMovedToPos(const QPoint &);
    void si_removeRow();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    VariantTrackObject *trackObj;
    AssemblyBrowser *browser;
    QPixmap cachedView;
    bool redraw;
    QMenu *contextMenu;
    QScopedPointer<AssemblyCellRenderer> nuclRenderer;
    QScopedPointer<AssemblyCellRenderer> snpRenderer;

    class CurrentData {
    public:
        QList<U2Variant> variants;
        QPoint pos;
        U2Region region;
        int snpWidth;
        bool updateHint;
    } currentData;
    AssemblyVariantHint hint;

    void draw();
    void prepareRenderers(int snpWidth, int snpHeight);
    void updateHint();
    bool findVariantOnPos(QList<U2Variant> &variants);

    static const int FIXED_HEIGHT;
    static const int TOP_OFFSET;

private slots:
    void sl_redraw();
};

class AssemblyVariantRowManager : public QObject {
    Q_OBJECT
public:
    AssemblyVariantRowManager(AssemblyBrowserUi *ui);

public slots:
    void sl_trackAdded(VariantTrackObject *trackObj);
    void sl_trackRemoved(VariantTrackObject *trackObj);

private:
    AssemblyBrowserUi *ui;
    AssemblyBrowser *browser;
    QSharedPointer<AssemblyModel> model;

private slots:
    void sl_removeRow();
};

} // U2

#endif // _ASSEMBLY_VARIANT_ROW_
