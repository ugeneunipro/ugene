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

#ifndef __ASSEMBLY_REFERENCE_AREA_H__
#define __ASSEMBLY_REFERENCE_AREA_H__

#include <QWidget>
#include <QMenu>
#include <QSharedPointer>

#include "AssemblyCellRenderer.h"
#include "AssemblyModel.h"

namespace U2 {

class AssemblyBrowserUi;
class AssemblyBrowser;
class U2OpStatus;

// TODO: move class to separate file
class AssemblySequenceArea : public QWidget {
    Q_OBJECT
public:
    AssemblySequenceArea(AssemblyBrowserUi * ui, char skipChar = '\0');

protected:
    virtual QByteArray getSequenceRegion(U2OpStatus &os) = 0;
    virtual bool canDrawSequence() = 0;
    virtual void drawSequence(QPainter & p);

    QSharedPointer<AssemblyModel> getModel() const { return model; }
    bool areCellsVisible() const;
    U2Region getVisibleRegion() const;

    void setNormalCellRenderer();
    void setDiffCellRenderer();

    void paintEvent(QPaintEvent * e);
    void resizeEvent(QResizeEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

signals:
    void si_mouseMovedToPos(const QPoint &);

protected slots:
    void sl_redraw();
    virtual void sl_offsetsChanged();
    virtual void sl_zoomPerformed();

private:
    void initCellRenderer(QString id);
    void connectSlots();
    void drawAll();

protected:
    AssemblyBrowser *browser;

private:
    AssemblyBrowserUi * ui;
    QSharedPointer<AssemblyModel> model;

    QPixmap cachedView;
    bool redraw;
    QScopedPointer<AssemblyCellRenderer> cellRenderer;
    bool needsReference;
    char skipChar;

    const static int FIXED_HEIGHT = 25;
};

class AssemblyReferenceArea: public AssemblySequenceArea {
    Q_OBJECT
public:
    AssemblyReferenceArea(AssemblyBrowserUi * ui);

protected:
    virtual QByteArray getSequenceRegion(U2OpStatus &os);
    virtual bool canDrawSequence();
    virtual void drawSequence(QPainter & p);

    void mousePressEvent(QMouseEvent* e);

signals:
    void si_unassociateReference();

private slots:
    void sl_onReferenceChanged();

private:
    QMenu * referenceAreaMenu;
    QAction * unassociateReferenceAction;
};

} //ns

#endif
