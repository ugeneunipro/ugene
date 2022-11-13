/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GENECUT_OP_WIDGET_H_
#define _U2_GENECUT_OP_WIDGET_H_

//#include "PCRPrimerDesignForDNAAssemblyOPSavableTab.h"
#include "ui_GenecutOPWidget.h"

#include <QWidget>

#include <U2Core/GUrl.h>

namespace U2 {

class AnnotatedDNAView;
class GenecutHttpFileAdapter;
class GenecutHttpFileAdapterFactory;

class GenecutOPWidget : public QWidget, private Ui_GenecutOPWidget {
    Q_OBJECT
public:
    GenecutOPWidget(AnnotatedDNAView* annDnaView);

private slots:
    void sl_loginClicked();
    void sl_resetPasswordClicked();
    void sl_logoutClicked();
    void sl_openInGenecut();
    void sl_fetchResultsClicked();
    void sl_registerNewClicked();
    void sl_getInputSequenceClicked();
    void sl_getResultSequenceClicked();
    void sl_removeSelectedResultClicked();
    void sl_openResultInBrowserClicked();
    void sl_compareInputAndOutput();

private:
    enum class TableColumns {
        Date = 0,
        Status = 1
    };

    enum class ResultData {
        Date = 1000,
        Status = 1001,
        Id = 1002,
        Completed = 1003,
        CompletedWithError = 1004,
        Interrupted = 1005,
        ShortDescription = 1006,
        IsAmino = 1007
    };

    enum class ServerFileType {
        Input,
        Result
    };

    static void errorMessage(GenecutHttpFileAdapter* adapter, QLabel* errorLabel);
    static void errorMessage(const QString& message, QLabel* errorLabel);
    static void successMessage(const QString& message, QLabel* label);
    static void warningMessage(const QString& message, QLabel* label);
    static void setWidgetsEnabled(QList<QWidget*> wgts, bool enabled);

    bool areRegistrationDataValid() const;
    bool hasFullReportFile() const;
    bool hasNucleicInput() const;
    QString getSelectedReportData(ResultData datatype) const;
    void downloadAndSaveFileFromServer(ServerFileType fileType, bool silentDownload = false);
    void fileFromServerLoaded(const QString& loadedFile);

    const QString apiServer;

    AnnotatedDNAView* annDnaView = nullptr;
    GenecutHttpFileAdapterFactory* factory = nullptr;

    QString accessToken;
    QString refreshToken;
    QString email;
    QString firstName;
    QString lastName;

    QList<GUrl> loadedFilesPaths;

    static const QString ENV_UGENE_GENECUT_USE_TEST_API_SERVER;

    static const QString HEADER_VALUE;
    static const QString API_REQUEST_API_SUFFIX;
    static const QString API_REQUEST_TYPE;
    static const QString API_REQUEST_LOGIN;
    static const QString API_REQUEST_RESET_PASSWORD;
    static const QString API_REQUEST_LOGOUT;
    static const QString API_REQUEST_REGISTER;
    static const QString API_REQUEST_UPLOAD_SEQUENCE;
    static const QString API_REQUEST_REPORTS;
    static const QString API_REQUEST_OPEN_REPORT_IN_BROWSER;
    static const QString API_REQUEST_GET_INPUT;
    static const QString API_REQUEST_GET_RESULT;
    static const QString API_REQUEST_DEL_RESULT;

    static const QString JSON_EMAIL;
    static const QString JSON_PASSWORD;
    static const QString JSON_ROLE;
    static const QString JSON_ACCESS_TOKEN;
    static const QString JSON_REFRESH_TOKEN;
    static const QString JSON_USER_OBJECT;
    static const QString JSON_FIRST_NAME;
    static const QString JSON_LAST_NAME;
    static const QString JSON_MESSAGE;
    static const QString JSON_DATE;
    static const QString JSON_STATUS;
    static const QString JSON_ID;
    static const QString JSON_COMPLETED;
    static const QString JSON_COMPLETED_WITH_ERROR;
    static const QString JSON_INTERRUPTED;
    static const QString JSON_SHORT_DESCRIPTION;
    static const QString JSON_IS_AMINO;
    static const QString JSON_SEQUENCE_FILE_NAME;
    static const QString JSON_SEQUENCE_FILE_BODY;
    static const QString JSON_RESPOND_SEQUENCE_FILE_NAME;
    static const QString JSON_RESPOND_SEQUENCE_FILE_BODY;
    static const QString JSON_REPORT_ID;
    static const QString JSON_LANG_ID;

    static const QString GENECUT_USER_EMAIL_SETTINGS;
    static const QString GENECUT_USER_PASSWORD_SETTINGS;
};


}    // namespace U2

#endif    // _U2_GENECUT_OP_WIDGET_H_
