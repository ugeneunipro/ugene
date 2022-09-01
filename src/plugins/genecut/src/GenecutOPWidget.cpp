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

#include <U2Core/Log.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <QCheckBox>
#include <QPushButton>

#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

namespace U2 {

const QString GenecutOPWidget::HEADER_VALUE = "application/json";
const QString GenecutOPWidget::API_REQUEST_URL = "http://192.168.16.230:5000/api/";
const QString GenecutOPWidget::API_REQUEST_TYPE = "user";
const QString GenecutOPWidget::API_REQUEST_LOGIN = "login";
const QString GenecutOPWidget::API_REQUEST_REGISTER = "registration";
const QString GenecutOPWidget::API_REQUEST_TEST = "test";
const QString GenecutOPWidget::JSON_EMAIL = "email";
const QString GenecutOPWidget::JSON_PASSWORD = "password";
const QString GenecutOPWidget::JSON_ROLE = "role";
const QString GenecutOPWidget::JSON_ACCESS_TOKEN = "accessToken";
const QString GenecutOPWidget::JSON_REFRESH_TOKEN = "refreshToken";
const QString GenecutOPWidget::JSON_USER_OBJECT = API_REQUEST_TYPE;
const QString GenecutOPWidget::JSON_FIRST_NAME = "firstName";
const QString GenecutOPWidget::JSON_LAST_NAME = "lastName";
const QString GenecutOPWidget::JSON_MESSAGE = "message";

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
    connect(pbTest, &QPushButton::clicked, this, &GenecutOPWidget::sl_testClicked);
    connect(pbRegister, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(2);
    });
    connect(cbShowPass, &QCheckBox::toggled, [this](bool toggled) {
        lePasword->setEchoMode(toggled ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(pbRegisterNew, &QPushButton::clicked, this, &GenecutOPWidget::sl_registerNewClicked);



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

    connect(reply, &QNetworkReply::finished, [this, reply]() {
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
            stackedWidget->setCurrentIndex(1);
        } else {
            QString err = reply->errorString();
            lbLoginWarning->show();
            lbLoginWarning->setText("Warning: " + err);
            coreLog.error(err);
        }
        reply->deleteLater();
    });
}

void GenecutOPWidget::sl_testClicked() {
    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_TEST);
    QNetworkRequest request(url);
    auto aut = QString("Bearer %1").arg(accessToken);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    request.setRawHeader("Authorization", aut.toLocal8Bit());

    QNetworkReply* reply = mgr->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            lbTestInfo->setStyleSheet("font-weight: 600;color: " + Theme::successColorLabelStr());
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
            auto value = doc.object().value(JSON_MESSAGE).toString();
            lbTestInfo->setText(tr("Success: %1").arg(value));
        } else {
            lbTestInfo->setStyleSheet("font-weight: 600;color: " + Theme::errorColorLabelStr());
            lbLoginWarning->setText("Warning: " + reply->errorString());
        }
        reply->deleteLater();
    });


}

void GenecutOPWidget::sl_registerNewClicked() {
    bool valid = true;
    QString emailNew = leEmailNew->text();
    {
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
            lbRegisterWarning->show();
            lbRegisterWarning->setText("Warning: passwords do not match");
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
    CHECK(valid, );

    const QUrl url(API_REQUEST_URL + API_REQUEST_TYPE + "/" + API_REQUEST_REGISTER);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);

    QJsonObject obj;
    obj[JSON_EMAIL] = emailNew;
    obj[JSON_PASSWORD] = passwordNew;
    obj[JSON_ROLE] = "USER";
    obj[JSON_FIRST_NAME] = firstName;
    obj[JSON_LAST_NAME] = lastName;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = mgr->post(request, data);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {

        } else {
            QString err = reply->errorString();
            lbLoginWarning->show();
            lbLoginWarning->setText("Warning: " + err);
            coreLog.error(err);
        }
        reply->deleteLater();
    });
}

}
