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

#ifndef _U2_MA_EDITOR_UTILS_H_
#define _U2_MA_EDITOR_UTILS_H_

#include <QLabel>
#include <QSplitter>
#include <QWidget>


namespace U2 {

class MaEditorWgt;
class MSAEditorSequenceArea;

/************************************************************************/
/* MaSplitterController */
/************************************************************************/
class MaSplitterController : public QObject {
    Q_OBJECT
public:
    MaSplitterController();
    MaSplitterController(QSplitter *spliter);

    void setSequenceArea(MSAEditorSequenceArea* _seqArea);

    QSplitter* getSplitter();

    void addWidget(QWidget *wgt, int index, qreal coef);
    void addWidget(QWidget *neighboringWidget, QWidget *wgt, qreal coef, int neighboringShift = 0);

    void removeWidget(QWidget *wgt);

private:
    MSAEditorSequenceArea* seqArea;
    QSplitter*             splitter;

    QList<QWidget *>       widgets;
    QList<int>             widgetSizes;
};

/************************************************************************/
/* MaUtilsWidget */
/************************************************************************/
class MaUtilsWidget : public QWidget {
    Q_OBJECT
public:
    MaUtilsWidget(MaEditorWgt* _ui, QWidget* heightWidget);
    virtual ~MaUtilsWidget() {}
    const QFont& getMsaEditorFont();
    void setHeightMargin(int _heightMargin);

protected slots:
    void sl_fontChanged();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);

    MaEditorWgt*    ui;
    QWidget*        heightWidget;
    int             heightMargin;
};

/************************************************************************/
/* MaLabelWidget */
/************************************************************************/
class MaLabelWidget : public MaUtilsWidget {
    Q_OBJECT
public:
    // SANGER_TODO: rename the class and reconsider the usage of it and its parent
    MaLabelWidget(MaEditorWgt* ui, QWidget* heightWidget, const QString & text, Qt::Alignment alignment, bool proxyMouseEventsToNameList = true);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    void sendEventToNameList(QMouseEvent* e) const;

    QLabel* label;
    bool proxyMouseEventsToNameList;
};

} // namespace

#endif // _U2_MA_EDITOR_UTILS_H_

