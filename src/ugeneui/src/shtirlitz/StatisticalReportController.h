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

#ifndef _U2_STATISTICAL_REPORT_CONTROLLER_H_
#define _U2_STATISTICAL_REPORT_CONTROLLER_H_

#include <U2Gui/U2WebView.h>
#include <QtWidgets/QTextBrowser>

#include "ui_StatisticalReport.h"

namespace U2 {

/* HTML browser that autoresizes itself to fit HTML content with no scroll bars. */
class ContentSizeHtmlViewer: public QTextBrowser {
    Q_OBJECT
public:
    ContentSizeHtmlViewer(QWidget* parent, const QString& html);
    virtual QSize sizeHint();

public slots:
    void sl_updateSize();
};

class StatisticalReportController : public QDialog, public Ui_StatisticalReport {
    Q_OBJECT
public:
    StatisticalReportController(const QString &htmlContent, QWidget *parent);
    bool isInfoSharingAccepted() const;
    void resizeEvent( QResizeEvent* event );
public slots:
    void accept();
private:
    ContentSizeHtmlViewer* htmlView;
};

}

#endif
