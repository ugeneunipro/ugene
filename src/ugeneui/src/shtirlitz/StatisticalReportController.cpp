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

#include <U2Core/Version.h>
#include "StatisticalReportController.h"

namespace U2 {

StatisticalReportController::StatisticalReportController(const QString &htmlContent, QWidget *parent) : QDialog(parent) {
    setupUi(this);
    lblStat->setText(tr("<b>Optional:</b> Help make UGENE better by automatically sending anonymous usage statistics."));

    Version v = Version::appVersion();
    setWindowTitle(tr("Welcome to UGENE %1.%2").arg(v.major).arg(v.minor));

    htmlView = new ContentSizeHtmlViewer(this, htmlContent);
    htmlView->document()->setDocumentMargin(15);
    dialogLayout->insertWidget(0, htmlView);

    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
}

bool StatisticalReportController::isInfoSharingAccepted() const {
    return chkStat->isChecked();
}

void StatisticalReportController::accept() {
    QDialog::close();
}

void StatisticalReportController::resizeEvent( QResizeEvent* event ) {
    htmlView->sl_updateSize();
    QDialog::resizeEvent( event );
}

ContentSizeHtmlViewer::ContentSizeHtmlViewer(QWidget* parent, const QString& html)
    : QTextBrowser(parent)
{
    setOpenExternalLinks(true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHtml(html);
    connect(this, SIGNAL(textChanged), this, SLOT(sl_updateSize));
}

QSize ContentSizeHtmlViewer::sizeHint() {
    sl_updateSize();
    return QTextBrowser::sizeHint();
}

void ContentSizeHtmlViewer::sl_updateSize() {
    document()->setTextWidth(viewport()->size().width());
    QSize docSize = document()->size().toSize();
    setMinimumWidth(docSize.width());
    setMinimumHeight(docSize.height() + 10);
}

} // namespace
