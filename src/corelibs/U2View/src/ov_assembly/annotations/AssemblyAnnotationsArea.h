/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#ifndef _ASSEMBLY_ANNOTATIONS_AREA_
#define _ASSEMBLY_ANNOTATIONS_AREA_

#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>

namespace U2 {

class AssemblyAnnotationsAreaWidget;
class AssemblyBrowserUi;
class SequenceObjectContext;

class AssemblyAnnotationsArea : public QWidget {
    Q_OBJECT
public:
    AssemblyAnnotationsArea(AssemblyBrowserUi* ui);
    ~AssemblyAnnotationsArea() override;

signals:
    void si_mouseMovedToPos(const QPoint& p);

private slots:
    void sl_contextChanged(SequenceObjectContext* ctx);
    void sl_createWidget();

private:
    void connectSignals();

    AssemblyBrowserUi* browserUi;
    SequenceObjectContext* seqCtx;
    QPointer<AssemblyAnnotationsAreaWidget> widget;
    QVBoxLayout* vertLayout;
};

} // U2

#endif // _ASSEMBLY_ANNOTATIONS_AREA_
