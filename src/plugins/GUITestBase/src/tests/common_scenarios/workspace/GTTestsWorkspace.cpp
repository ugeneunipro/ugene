/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "GTTestsWorkspace.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "GTUtilsCloudStorageView.h"

namespace U2 {

namespace GUITest_common_scenarios_workspace {
using namespace HI;

static void resetTestUserData() {
    QString secret = qEnvironmentVariable("UGENE_TEST_WORKSPACE_API_SECRET");
    CHECK_SET_ERR(!secret.isEmpty(), "UGENE_TEST_WORKSPACE_API_SECRET is not set");

    QString apiUrl = "https://workspace.ugene.net";
    QNetworkRequest request(apiUrl + "/api/e2e/resetTestUserData");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonBody;
    jsonBody["secret"] = secret;
    QJsonDocument doc(jsonBody);

    QEventLoop loop;
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.post(request, doc.toJson());
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        CHECK_SET_ERR(statusCode == 200, "Got status code: " + QString::number(statusCode))
    } else {
        GT_FAIL("Failed to reset test user data: " + reply->errorString(), );
    }
    reply->deleteLater();
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    resetTestUserData();
    GTUtilsCloudStorageView::toggleDockView();

    // Check login.
    GTUtilsCloudStorageView::clickLogin();
    GTUtilsCloudStorageView::checkItemIsPresent({"Sequences"});

    // Check rename.
    GTUtilsCloudStorageView::renameItem({"about.txt"}, "renamed-about.txt"); // Top level.
    GTUtilsCloudStorageView::renameItem({"Alignments", "cytb.aln"}, "renamed-cytb.aln"); // In folder.

    // Check create dir.
    // await createDir(page, 'New Folder');
    // await changeDir(page, 'New Folder');
    // await createDir(page, 'New Sub Folder');

    // Check delete file or dir.
    // await deleteFile(page, 'Documents');
    // await changeDir(page, 'Alignments');
    // await deleteFile(page, 'hemoglobin_alpha_alignment.sto');

    // Check upload.
    // await uploadFile(page, 'Appendix_Fig3.pdf');
    // await changeDir(page, 'Alignments');
    // await uploadFile(page, 'HA_discrete_MCC.tre');

    // Check download file
    // await changeDir(page, 'Sequences');
    // await downloadFile(page, 'D-loop.fasta', 'Sequences/D-loop.fasta');

    GTUtilsCloudStorageView::clickLogout();
}

}  // namespace GUITest_common_scenarios_workspace
}  // namespace U2
