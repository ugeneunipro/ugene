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

#ifndef _U2_SEARCH_BOX_H_
#define _U2_SEARCH_BOX_H_

#include <U2Core/global.h>

#include <QLineEdit>

class QLabel;
class QMovie;
class QToolButton;

namespace U2 {

class U2GUI_EXPORT SearchBox : public QLineEdit {
    Q_OBJECT
public:
    SearchBox(QWidget *p);

public slots:
    void sl_filteringStarted();
    void sl_filteringFinished();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private slots:
    void sl_filterCleared();
    void sl_textChanged(const QString &text);

private:
    void initStyle();
    void updateInternalControlsPosition();

    bool firstShow;
    QLabel *progressLabel;
    QMovie *progressMovie;
    QLabel *searchIconLabel;
    QToolButton *clearButton;
};

} // namespace U2
#endif // _U2_SEARCH_BOX_H_
