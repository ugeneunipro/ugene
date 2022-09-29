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
#include <QNetworkAccessManager>
#include <QNetworkReply>



namespace U2 {

class AnnotatedDNAView;
class PCRPrimerDesignForDNAAssemblyTask;

class GenecutOPWidget : public QWidget, private Ui_GenecutOPWidget {
    Q_OBJECT
public:
    GenecutOPWidget(AnnotatedDNAView* annDnaView);

private slots:
    void sl_loginClicked();
    void sl_logoutClicked();
    void sl_openInGenecut();
    void sl_fetchResultsClicked();
    void sl_registerNewClicked();
    void sl_getResultSequence();
    void sl_removeSelectedResult();

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
        Interrupted = 1005
    };

    static void errorMessage(QNetworkReply* reply, QLabel* errorLabel);
    static void errorMessage(const QString& message, QLabel* errorLabel);
    static void successMessage(QNetworkReply* reply, QLabel* label);
    static void successMessage(const QString& message, QLabel* label);
    static void setWidgetsEnabled(QList<QWidget*> wgts, bool enabled);

    bool areRegistrationDataValid() const;
    QString getSelectedResultId() const;

    AnnotatedDNAView* annDnaView = nullptr;
    QNetworkAccessManager* mgr = nullptr;

    QString accessToken;
    QString refreshToken;
    QString email;
    QString firstName;
    QString lastName;

    static const QString HEADER_VALUE;
    static const QString API_REQUEST_URL;
    static const QString API_REQUEST_TYPE;
    static const QString API_REQUEST_LOGIN;
    static const QString API_REQUEST_LOGOUT;
    static const QString API_REQUEST_REGISTER;
    static const QString API_REQUEST_UPLOAD_SEQUENCE;
    static const QString API_REQUEST_REPORTS;
    static const QString API_REQUEST_GET_REPORT;
    static const QString API_REQUEST_DEL_REPORT;

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
    static const QString JSON_SEQUENCE_FILE_NAME;
    static const QString JSON_SEQUENCE_FILE_BODY;
    static const QString JSON_REPORT_ID;
    static const QString JSON_LANG_ID;

};


}    // namespace U2

#endif    // _U2_GENECUT_OP_WIDGET_H_
