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

#include "WebWindow.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QTextBrowser>

#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

/**
 * Maximum HTML content length supported by QTextBrowser.
 * The size above this value may trigger memory overflow in Qts HTML parser.
 */
static constexpr int MAX_SUPPORTED_HTML_CONTENT_LENGTH = 25'000'000;

/**
 * Maximum HTML tags count length supported by QTextBrowser.
 * The number of tags above this value may trigger memory overflow in Qts HTML parser.
 */
static constexpr int MAX_SUPPORTED_TAGS_IN_HTML_CONTENT = 1'000'000;

WebWindow::WebWindow(const QString& title, const QString& htmlContent)
    : MWMDIWindow(title) {
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    setLayout(layout);

    textBrowser = new QTextBrowser(this);
    textBrowser->setObjectName("textBrowser");
    textBrowser->setOpenExternalLinks(true);
    // Simple heuristic based on closing HTML tags pattern. Works for the most tags except tags like <input> or <br>.
    int countOfHtmlTags = htmlContent.count("</");
    bool isContentIsTooBig = htmlContent.length() > MAX_SUPPORTED_HTML_CONTENT_LENGTH || countOfHtmlTags > MAX_SUPPORTED_TAGS_IN_HTML_CONTENT;
    if (isContentIsTooBig) {
        QString errorHtmlContent = "<br><br><br><br><br><center>";
        errorHtmlContent += "<h2>" + tr("HTML content is too large to be safely displayed in UGENE.") + "</h2>";
        errorHtmlContent += "<br><br><br>" + tr("Save HTML document to %1 file  %2").arg("<a href=#saveToFile>").arg("</a>");
        errorHtmlContent += "</center>";
        textBrowser->setHtml(errorHtmlContent);
        connect(textBrowser, &QTextBrowser::anchorClicked, this, [this, htmlContent] {
            LastUsedDirHelper lod("HTML");
            QString htmlFilter = FileFilters::createFileFilter(tr("HTML Files"), {"html"});
            QString fileName = GUrlUtils::rollFileName(lod.dir + "/report.html", "_");
            lod.url = U2FileDialog::getSaveFileName(this, tr("Save as"), fileName, htmlFilter, htmlFilter);
            CHECK(!lod.url.isEmpty(), );
            FileAndDirectoryUtils::storeTextToFile(lod.url, htmlContent);
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(lod.url).absolutePath()));
        });
    } else {
        textBrowser->setHtml(htmlContent);
    }
    layout->addWidget(textBrowser);
}

}  // namespace U2
