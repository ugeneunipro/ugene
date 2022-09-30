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

#include "GenecutOPWidget.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Theme.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVSequenceObjectContext.h>

#include <QCheckBox>
#include <QDesktopServices>
#include <QPushButton>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTableWidgetItem>
#include <QUrlQuery>

namespace U2 {

const QString GenecutOPWidget::HEADER_VALUE = "application/json";
const QString GenecutOPWidget::API_SERVER = "http://192.168.16.230:5000";
const QString GenecutOPWidget::API_REQUEST_URL = API_SERVER + "/api/";
const QString GenecutOPWidget::API_REQUEST_TYPE = "user";
const QString GenecutOPWidget::API_REQUEST_LOGIN = "login";
const QString GenecutOPWidget::API_REQUEST_RESET_PASSWORD = "sendChangePassLink";
const QString GenecutOPWidget::API_REQUEST_LOGOUT = "logout";
const QString GenecutOPWidget::API_REQUEST_REGISTER = "registration";
const QString GenecutOPWidget::API_REQUEST_UPLOAD_SEQUENCE = "uploadSequence";
const QString GenecutOPWidget::API_REQUEST_REPORTS = "reports";
const QString GenecutOPWidget::API_REQUEST_OPEN_REPORT_IN_BROWSER = "showReport";
const QString GenecutOPWidget::API_REQUEST_GET_REPORT = "getReport";
const QString GenecutOPWidget::API_REQUEST_DEL_REPORT = "delReport";
const QString GenecutOPWidget::JSON_EMAIL = "email";
const QString GenecutOPWidget::JSON_PASSWORD = "password";
const QString GenecutOPWidget::JSON_ROLE = "role";
const QString GenecutOPWidget::JSON_ACCESS_TOKEN = "accessToken";
const QString GenecutOPWidget::JSON_REFRESH_TOKEN = "refreshToken";
const QString GenecutOPWidget::JSON_USER_OBJECT = API_REQUEST_TYPE;
const QString GenecutOPWidget::JSON_FIRST_NAME = "firstName";
const QString GenecutOPWidget::JSON_LAST_NAME = "lastName";
const QString GenecutOPWidget::JSON_MESSAGE = "message";
const QString GenecutOPWidget::JSON_DATE = "name";
const QString GenecutOPWidget::JSON_STATUS = "title";
const QString GenecutOPWidget::JSON_ID = "id";
const QString GenecutOPWidget::JSON_COMPLETED = "completed";
const QString GenecutOPWidget::JSON_COMPLETED_WITH_ERROR = "completedWithError";
const QString GenecutOPWidget::JSON_INTERRUPTED = "interrupted";
const QString GenecutOPWidget::JSON_SHORT_DESCRIPTION = "short";
const QString GenecutOPWidget::JSON_SEQUENCE_FILE_NAME = "sequenceFileName";
const QString GenecutOPWidget::JSON_SEQUENCE_FILE_BODY = "sequenceFileBody";
const QString GenecutOPWidget::JSON_REPORT_ID = "reportId";
const QString GenecutOPWidget::JSON_LANG_ID = "langId";

GenecutOPWidget::GenecutOPWidget(AnnotatedDNAView* _annDnaView)
    : QWidget(nullptr),
      annDnaView(_annDnaView)/*,
      savableWidget(this, GObjectViewUtils::findViewByName(annDnaView->getName()))*/ {
    setupUi(this);
    lbLoginWarning->hide();
    lbLoginWarning->setStyleSheet(lbLoginWarning->styleSheet() + "color: " + Theme::errorColorLabelStr());
    lbLoginWarning->setAlignment(Qt::AlignLeft);
    lbLoginWarning->setWordWrap(true);
    lbRegisterWarning->hide();
    lbRegisterWarning->setStyleSheet(lbRegisterWarning->styleSheet() + "color: " + Theme::errorColorLabelStr());
    lbRegisterWarning->setAlignment(Qt::AlignLeft);
    lbRegisterWarning->setWordWrap(true);
    stackedWidget->setCurrentIndex(0);

    mgr = new QNetworkAccessManager(this);

    connect(pbLogin, &QPushButton::clicked, this, &GenecutOPWidget::sl_loginClicked);
    connect(pbForgot, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(1);
    });
    connect(pbReset, &QPushButton::clicked, this, &GenecutOPWidget::sl_resetPasswordClicked);
    connect(pbGoBack2, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(0);
    });
    connect(pbLogout, &QPushButton::clicked, this, &GenecutOPWidget::sl_logoutClicked);
    connect(pbOpenInGenecut, &QPushButton::clicked, this, &GenecutOPWidget::sl_openInGenecut);
    connect(pbRegister, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(3);
    });
    connect(cbShowPass, &QCheckBox::toggled, [this](bool toggled) {
        lePasword->setEchoMode(toggled ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(pbRegisterNew, &QPushButton::clicked, this, &GenecutOPWidget::sl_registerNewClicked);
    connect(pbGoBack, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(0);
    });
    connect(pbFetchResults, &QPushButton::clicked, this, &GenecutOPWidget::sl_fetchResultsClicked);
    connect(pbGetResultSequence, &QPushButton::clicked, this, &GenecutOPWidget::sl_getResultSequenceClicked);
    connect(pbRemoveSelectedResult, &QPushButton::clicked, this, &GenecutOPWidget::sl_removeSelectedResultClicked);
    connect(pbOpenResultInBrowser, &QPushButton::clicked, this, &GenecutOPWidget::sl_openResultInBrowserClicked);
    connect(twResults, &QTableWidget::itemSelectionChanged, [this]() {
        setWidgetsEnabled({ pbRemoveSelectedResult, pbOpenResultInBrowser, pbGetResultSequence }, !twResults->selectedItems().isEmpty());

        QString shortDescription = getSelectedReportData(ResultData::ShortDescription);
        CHECK_EXT(!shortDescription.isEmpty(), tbShortDescription->setText(""), );

        tbShortDescription->setText(shortDescription);
    });
}

void GenecutOPWidget::sl_loginClicked() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_LOGIN);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);

    QJsonObject obj;
    obj[JSON_EMAIL] = leEmail->text();
    obj[JSON_PASSWORD] = lePasword->text();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ pbLogin, pbForgot, pbRegister }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ pbLogin, pbForgot, pbRegister }, true);
        if (reply->error() == QNetworkReply::NoError) {
            lbLoginWarning->hide();
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
            auto jsonObj = doc.object();
            accessToken = jsonObj.value(JSON_ACCESS_TOKEN).toString();
            refreshToken = jsonObj.value(JSON_REFRESH_TOKEN).toString();
            auto userObject = jsonObj.value(JSON_USER_OBJECT).toObject();
            email = userObject.value(JSON_EMAIL).toString();
            firstName = userObject.value(JSON_FIRST_NAME).toString();
            lastName = userObject.value(JSON_LAST_NAME).toString();
            lbWelcome->setText(tr("Welcome, %1").arg(firstName));
            stackedWidget->setCurrentIndex(2);
        } else {
            errorMessage(reply, lbLoginWarning);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_resetPasswordClicked() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_RESET_PASSWORD);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);

    QJsonObject obj;
    obj[JSON_EMAIL] = leResetPassword->text();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ leResetPassword, pbReset }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ leResetPassword, pbReset }, true);
        if (reply->error() == QNetworkReply::NoError) {
            successMessage(tr("Check your email"), lbResetStatus);
        } else {
            errorMessage(reply, lbResetStatus);
        }
        reply->deleteLater();
    });

}

void GenecutOPWidget::sl_logoutClicked() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_LOGOUT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);

    QJsonObject obj;
    obj[JSON_REFRESH_TOKEN] = refreshToken;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ wtMainForm }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (reply->error() == QNetworkReply::NoError) {
            accessToken.clear();
            refreshToken.clear();
            lbTestInfo->clear();
            twResults->clearContents();
            twResults->setRowCount(0);
            stackedWidget->setCurrentIndex(0);
        } else {
            errorMessage(reply, lbTestInfo);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_openInGenecut() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_UPLOAD_SEQUENCE);
    QNetworkRequest request(url);
    auto aut = QString("Bearer %1").arg(accessToken);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    request.setRawHeader("Authorization", aut.toLocal8Bit());

    auto seqObj = annDnaView->getActiveSequenceContext()->getSequenceObject();
    U2::U2OpStatus2Log os;
    QJsonObject obj;
    obj[JSON_SEQUENCE_FILE_BODY] = QString(seqObj->getWholeSequenceData(os));
    SAFE_POINT_OP(os, );

    obj[JSON_SEQUENCE_FILE_NAME] = seqObj->getSequenceName();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, true);
        if (reply->error() == QNetworkReply::NoError) {
            QFile f(":genecut/template/hidden_login.html");
            SAFE_POINT(f.open(QIODevice::ReadOnly), L10N::errorReadingFile(f.fileName()), );

            QString hiddenLoginHtml = f.readAll();
            hiddenLoginHtml = hiddenLoginHtml.arg(L10N::getActiveLanguageCode()).arg(email).arg(accessToken).arg(refreshToken);

            QString tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("genecut");
            U2OpStatus2Log os;
            GUrlUtils::prepareDirLocation(tmpDir, os);
            CHECK_OP(os, );

            QFile tmpFile(tmpDir + QDir::separator() + "genecut_template.html");
            SAFE_POINT(!tmpFile.exists() || tmpFile.remove(), "Can't reuse tmp file", );
            SAFE_POINT(tmpFile.open(QIODevice::WriteOnly), L10N::errorOpeningFileRead(tmpFile.fileName()), );

            QTextStream out(&tmpFile);
            out << hiddenLoginHtml;
            tmpFile.close();
            QDesktopServices::openUrl(QUrl::fromLocalFile(tmpFile.fileName()));
        } else {
            errorMessage(reply, lbTestInfo);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_fetchResultsClicked() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_REPORTS);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    auto aut = QString("Bearer %1").arg(accessToken);
    request.setRawHeader("Authorization", aut.toLocal8Bit());

    QJsonObject obj;
    obj[JSON_LANG_ID] = L10N::getActiveLanguageCode();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->sendCustomRequest(request, "GET", data);
    setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, true);
        if (reply->error() == QNetworkReply::NoError) {
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
            auto jsonArray = doc.array();
            twResults->clearContents();
            int rowCount = jsonArray.size();
            twResults->setRowCount(rowCount);
            for (int i = 0; i < rowCount; i++) {
                const auto& arrayObj = jsonArray.at(i).toObject();
                auto dateWgtItem = new QTableWidgetItem(arrayObj.value(JSON_DATE).toString(), (int)ResultData::Date);
                dateWgtItem->setData((int)ResultData::Id, arrayObj.value(JSON_ID).toString());
                dateWgtItem->setData((int)ResultData::Completed, arrayObj.value(JSON_COMPLETED).toBool());
                dateWgtItem->setData((int)ResultData::CompletedWithError, arrayObj.value(JSON_COMPLETED_WITH_ERROR).toBool());
                dateWgtItem->setData((int)ResultData::Interrupted, arrayObj.value(JSON_INTERRUPTED).toBool());
                dateWgtItem->setData((int)ResultData::ShortDescription, arrayObj.value(JSON_SHORT_DESCRIPTION).toString());
                twResults->setItem(i, (int)TableColumns::Date, dateWgtItem);
                twResults->setItem(i, (int)TableColumns::Status, new QTableWidgetItem(arrayObj.value(JSON_STATUS).toString(), (int)ResultData::Status));
            }
            successMessage(tr("results have been fetched"), lbTestInfo);
        } else {
            errorMessage(reply, lbTestInfo);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_registerNewClicked() {
    CHECK(areRegistrationDataValid(), );

    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_REGISTER);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);

    QJsonObject obj;
    obj[JSON_EMAIL] = leEmailNew->text();
    obj[JSON_PASSWORD] = lePasswordNew->text();
    obj[JSON_ROLE] = "USER";
    obj[JSON_FIRST_NAME] = leFirstName->text();
    obj[JSON_LAST_NAME] = leLastName->text();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ pbRegisterNew }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ pbRegisterNew }, true);
        if (reply->error() == QNetworkReply::NoError) {
            successMessage(tr("user created! Check your email"), lbRegisterWarning);
        } else {
            errorMessage(reply, lbRegisterWarning);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_getResultSequenceClicked() {
    QString resultId = getSelectedReportData(ResultData::Id);
    CHECK(!resultId.isEmpty(), );

    const QUrl url(API_REQUEST_URL + API_REQUEST_GET_REPORT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    auto aut = QString("Bearer %1").arg(accessToken);
    request.setRawHeader("Authorization", aut.toLocal8Bit());

    QJsonObject obj;
    obj[JSON_REPORT_ID] = resultId;
    obj[JSON_LANG_ID] = L10N::getActiveLanguageCode();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->sendCustomRequest(request, "GET", data);
    setWidgetsEnabled({ wtMainForm }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (reply->error() == QNetworkReply::NoError) {
            //TODO: handle it
            successMessage(tr("the result has been recieved"), lbTestInfo);
        } else {
            errorMessage(reply, lbTestInfo);
        }
        reply->deleteLater();
    });

}

void GenecutOPWidget::sl_removeSelectedResultClicked() {
    QString resultId = getSelectedReportData(ResultData::Id);
    CHECK(!resultId.isEmpty(), );

    const QUrl url(API_REQUEST_URL + API_REQUEST_DEL_REPORT);
    QNetworkRequest request(url);
    auto aut = QString("Bearer %1").arg(accessToken);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    request.setRawHeader("Authorization", aut.toLocal8Bit());

    auto seqObj = annDnaView->getActiveSequenceContext()->getSequenceObject();
    QJsonObject obj;
    obj[JSON_REPORT_ID] = resultId;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);
    setWidgetsEnabled({ wtMainForm }, false);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (reply->error() == QNetworkReply::NoError) {
            auto selected = twResults->selectedItems();
            CHECK(!selected.isEmpty(), );

            twResults->removeRow(twResults->row(selected.first()));

            int i = 0;
        } else {
            errorMessage(reply, lbTestInfo);
        }
        reply->deleteLater();
    });

}

void GenecutOPWidget::sl_openResultInBrowserClicked() {
    auto reportId = getSelectedReportData(ResultData::Id);
    CHECK(!reportId.isEmpty(), );

    QFile f(":genecut/template/show_report.html");
    SAFE_POINT(f.open(QIODevice::ReadOnly), L10N::errorReadingFile(f.fileName()), );

    QString showReportHtml = f.readAll();
    showReportHtml = showReportHtml.arg(reportId).arg(L10N::getActiveLanguageCode()).arg(email).arg(accessToken).arg(refreshToken).arg(API_SERVER);

    QString tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("genecut");
    U2OpStatus2Log os;
    GUrlUtils::prepareDirLocation(tmpDir, os);
    CHECK_OP(os, );

    QFile tmpFile(tmpDir + QDir::separator() + "genecut_template.html");
    SAFE_POINT(!tmpFile.exists() || tmpFile.remove(), "Can't reuse tmp file", );
    SAFE_POINT(tmpFile.open(QIODevice::WriteOnly), L10N::errorOpeningFileRead(tmpFile.fileName()), );

    QTextStream out(&tmpFile);
    out << showReportHtml;
    tmpFile.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(tmpFile.fileName()));
}

void GenecutOPWidget::errorMessage(QNetworkReply* reply, QLabel* errorLabel) {
    QString contents = QString::fromUtf8(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    auto value = doc.object().value(JSON_MESSAGE).toString();
    QString err;
    if (!value.isEmpty()) {
        err = value;
    } else {
        err = reply->errorString();
    }
    errorMessage(err, errorLabel);
}

void GenecutOPWidget::errorMessage(const QString& message, QLabel* errorLabel) {
    errorLabel->setStyleSheet("font-weight: 600;color: " + Theme::errorColorLabelStr());
    errorLabel->show();
    errorLabel->setText(tr("Error: ") + message);
    coreLog.error(message);
}

void GenecutOPWidget::successMessage(QNetworkReply* reply, QLabel* label) {
    QString contents = QString::fromUtf8(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    auto value = doc.object().value(JSON_MESSAGE).toString();
    successMessage(value, label);
}

void GenecutOPWidget::successMessage(const QString& message, QLabel* label) {
    label->setStyleSheet("font-weight: 600;color: " + Theme::successColorLabelStr());
    label->show();
    label->setText(tr("Success: ") + message);
}

void GenecutOPWidget::setWidgetsEnabled(QList<QWidget*> wgts, bool enabled) {
    for (auto wgt : wgts) {
        wgt->setEnabled(enabled);
    }
}

bool GenecutOPWidget::areRegistrationDataValid() const {
    bool valid = true;
    QString emailNew = leEmailNew->text();
    {
        //TODO: regex
        bool valieEmail = !emailNew.isEmpty();
        GUIUtils::setWidgetWarningStyle(leEmailNew, !valieEmail);
        valid &= valieEmail;
    }

    QString passwordNew = lePasswordNew->text();
    {
        QString passwordConformationNew = lePasswordConformationNew->text();
        bool validPassword = !passwordNew.isEmpty();
        GUIUtils::setWidgetWarningStyle(lePasswordNew, !validPassword);
        bool validPasswordConfirmation = !passwordConformationNew.isEmpty();
        GUIUtils::setWidgetWarningStyle(lePasswordConformationNew, !validPasswordConfirmation);
        bool match = passwordNew == passwordConformationNew;
        if (!match) {
            errorMessage(tr("passwords do not match"), lbRegisterWarning);
            GUIUtils::setWidgetWarningStyle(lePasswordNew, true);
            GUIUtils::setWidgetWarningStyle(lePasswordConformationNew, true);
        } else {
            lbRegisterWarning->hide();
        }
        valid &= validPassword;
        valid &= validPasswordConfirmation;
        valid &= match;
    }

    QString firstName = leFirstName->text();
    {
        bool validFirstName = !firstName.isEmpty();
        GUIUtils::setWidgetWarningStyle(leFirstName, !validFirstName);
        valid &= validFirstName;
    }

    QString lastName = leLastName->text();
    {
        bool validLastName = !lastName.isEmpty();
        GUIUtils::setWidgetWarningStyle(leLastName, !validLastName);
        valid &= validLastName;
    }

    return valid;
}

QString GenecutOPWidget::getSelectedReportData(ResultData datatype) const {
    CHECK(!twResults->selectedItems().isEmpty(), QString());

    auto selection = twResults->selectedItems();
    SAFE_POINT(selection.size() == 2, "Unexpected selection size", QString());

    auto dataItem = selection.first()->type() == (int)ResultData::Date ? selection.first() : selection.last();
    auto resultId = dataItem->data((int)datatype).toString();
    SAFE_POINT(!resultId.isEmpty(), "Result data ID is empty", QString());

    return resultId;
}

}
