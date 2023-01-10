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
#include "io/GenecutHttpFileAdapter.h"
#include "io/GenecutHttpFileAdapterFactory.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
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

const QString GenecutOPWidget::ENV_UGENE_GENECUT_USE_TEST_API_SERVER = "UGENE_GENECUT_USE_TEST_API_SERVER";

const QString GenecutOPWidget::HEADER_VALUE = "application/json";
const QString GenecutOPWidget::API_REQUEST_API_SUFFIX = "api/";
const QString GenecutOPWidget::API_REQUEST_TYPE = "user";
const QString GenecutOPWidget::API_REQUEST_LOGIN = "login";
const QString GenecutOPWidget::API_REQUEST_RESET_PASSWORD = "sendChangePassLink";
const QString GenecutOPWidget::API_REQUEST_LOGOUT = "logout";
const QString GenecutOPWidget::API_REQUEST_REGISTER = "registration";
const QString GenecutOPWidget::API_REQUEST_UPLOAD_SEQUENCE = "uploadSequence";
const QString GenecutOPWidget::API_REQUEST_REPORTS = "reports";
const QString GenecutOPWidget::API_REQUEST_OPEN_REPORT_IN_BROWSER = "showReport";
const QString GenecutOPWidget::API_REQUEST_GET_INPUT = "getInputFile";
const QString GenecutOPWidget::API_REQUEST_GET_RESULT = "getReport";
const QString GenecutOPWidget::API_REQUEST_DEL_RESULT = "delReport";
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
const QString GenecutOPWidget::JSON_IS_AMINO = "is_amino";
const QString GenecutOPWidget::JSON_SEQUENCE_FILE_NAME = "sequenceFileName";
const QString GenecutOPWidget::JSON_SEQUENCE_FILE_BODY = "sequenceFileBody";
const QString GenecutOPWidget::JSON_RESPOND_SEQUENCE_FILE_NAME = "file_name";
const QString GenecutOPWidget::JSON_RESPOND_SEQUENCE_FILE_BODY = "file_body";
const QString GenecutOPWidget::JSON_REPORT_ID = "reportId";
const QString GenecutOPWidget::JSON_LANG_ID = "langId";

const QString GenecutOPWidget::GENECUT_USER_EMAIL_SETTINGS = "/genecut/email";
const QString GenecutOPWidget::GENECUT_USER_PASSWORD_SETTINGS = "/genecut/password";


GenecutOPWidget::GenecutOPWidget(AnnotatedDNAView* _annDnaView)
    : QWidget(nullptr),
      apiServer(qgetenv("UGENE_GENECUT_USE_TEST_API_SERVER") == "1" ? "http://genecut-test.unipro.ru/" : "http://genecut.unipro.ru/"),
      annDnaView(_annDnaView)/*,
      savableWidget(this, GObjectViewUtils::findViewByName(annDnaView->getName()))*/ {
    setupUi(this);
    lbLoginWarning->clear();
    lbLoginWarning->setStyleSheet(lbLoginWarning->styleSheet() + "color: " + Theme::errorColorLabelStr());
    lbLoginWarning->setAlignment(Qt::AlignLeft);
    lbRegisterWarning->clear();
    lbRegisterWarning->setStyleSheet(lbRegisterWarning->styleSheet() + "color: " + Theme::errorColorLabelStr());
    lbRegisterWarning->setAlignment(Qt::AlignLeft);
    stackedWidget->setCurrentIndex(0);

    if (apiServer.contains("test")) {
        coreLog.details(tr("GeneCut test server is in use"));
    }

    auto settings = AppContext::getSettings();
    if (settings->contains(GENECUT_USER_EMAIL_SETTINGS)) {
        cbRememberMe->setChecked(true);
        leEmail->setText(settings->getValue(GENECUT_USER_EMAIL_SETTINGS).toString());
        lePasword->setText(settings->getValue(GENECUT_USER_PASSWORD_SETTINGS).toString());
    }

    factory = new GenecutHttpFileAdapterFactory(this);

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
    connect(pbGetInputSequence, &QPushButton::clicked, this, &GenecutOPWidget::sl_getInputSequenceClicked);
    connect(pbGetResultSequence, &QPushButton::clicked, this, &GenecutOPWidget::sl_getResultSequenceClicked);
    connect(pbRemoveSelectedResult, &QPushButton::clicked, this, &GenecutOPWidget::sl_removeSelectedResultClicked);
    connect(pbOpenResultInBrowser, &QPushButton::clicked, this, &GenecutOPWidget::sl_openResultInBrowserClicked);
    connect(pbCompare, &QPushButton::clicked, this, &GenecutOPWidget::sl_compareInputAndOutput);
    connect(twResults, &QTableWidget::itemSelectionChanged, [this]() {
        bool hasSelected = !twResults->selectedItems().isEmpty();
        setWidgetsEnabled({ pbRemoveSelectedResult, pbGetInputSequence, pbOpenResultInBrowser, pbGetResultSequence, pbCompare }, hasSelected);
        CHECK(hasSelected, );

        QString warning;
        if (!hasFullReportFile()) {
            setWidgetsEnabled({ pbGetResultSequence, pbCompare }, false);
            warning = tr("calculation wasn't finished correctly, the result sequence and the comparison with the input sequence are not available");
        } else if (!hasNucleicInput()) {
            setWidgetsEnabled({ pbCompare }, false);
            warning = tr("the input sequence is amino, can't compare with the nucleic result");
        }
        if (!warning.isEmpty()) {
            warningMessage(warning, lbTestInfo);
        } else {
            lbTestInfo->clear();
        }

        tbShortDescription->setText(getSelectedReportData(ResultData::ShortDescription));
    });
}

void GenecutOPWidget::sl_loginClicked() {
    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addDataValue(JSON_EMAIL, leEmail->text());
    adapter->addDataValue(JSON_PASSWORD, lePasword->text());
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_TYPE + "/" + API_REQUEST_LOGIN);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ pbLogin, pbForgot, pbRegister }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ pbLogin, pbForgot, pbRegister }, true);
        if (!adapter->hasError()) {
            GCOUNTER(cvar, "GeneCut login");
            lbLoginWarning->clear();

            QByteArray contents(DocumentFormat::READ_BUFF_SIZE, '\0');
            int readSize = adapter->readBlock(contents.data(), DocumentFormat::READ_BUFF_SIZE);
            SAFE_POINT(readSize != -1, "Cannot read request data", );

            contents.resize(readSize);
            //coreLog.info(contents);
            QJsonDocument doc = QJsonDocument::fromJson(contents);
            auto jsonObj = doc.object();
            accessToken = jsonObj.value(JSON_ACCESS_TOKEN).toString();
            refreshToken = jsonObj.value(JSON_REFRESH_TOKEN).toString();
            auto userObject = jsonObj.value(JSON_USER_OBJECT).toObject();
            email = userObject.value(JSON_EMAIL).toString();
            firstName = userObject.value(JSON_FIRST_NAME).toString();
            lastName = userObject.value(JSON_LAST_NAME).toString();
            lbWelcome->setText(tr("Welcome, %1").arg(firstName));
            stackedWidget->setCurrentIndex(2);
            auto settings = AppContext::getSettings();
            if (cbRememberMe->isChecked()) {
                settings->setValue(GENECUT_USER_EMAIL_SETTINGS, leEmail->text());
                settings->setValue(GENECUT_USER_PASSWORD_SETTINGS, lePasword->text());
            } else {
                settings->remove(GENECUT_USER_EMAIL_SETTINGS);
                settings->remove(GENECUT_USER_PASSWORD_SETTINGS);
            }
        } else {
            errorMessage(adapter, lbLoginWarning);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_resetPasswordClicked() {
    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addDataValue(JSON_EMAIL, leResetPassword->text());
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_TYPE + "/" + API_REQUEST_RESET_PASSWORD);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ leResetPassword, pbReset }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ leResetPassword, pbReset }, true);
        if (!adapter->hasError()) {
            successMessage(tr("check your email"), lbResetStatus);
        } else {
            errorMessage(adapter, lbResetStatus);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_logoutClicked() {
    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addDataValue(JSON_REFRESH_TOKEN, refreshToken);
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_TYPE + "/" + API_REQUEST_LOGOUT);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ wtMainForm }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (!adapter->hasError()) {
            accessToken.clear();
            refreshToken.clear();
            lbTestInfo->clear();
            twResults->clearContents();
            twResults->setRowCount(0);
            stackedWidget->setCurrentIndex(0);
            tbShortDescription->clear();
        } else {
            errorMessage(adapter, lbTestInfo);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_openInGenecut() {
    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addRawHeader("Authorization", "Bearer " + accessToken.toLocal8Bit());
    auto seqObj = annDnaView->getActiveSequenceContext()->getSequenceObject();
    {
        U2OpStatus2Log os;
        adapter->addDataValue(JSON_SEQUENCE_FILE_BODY, seqObj->getWholeSequenceData(os));
        SAFE_POINT_OP(os, );
    }

    adapter->addDataValue(JSON_SEQUENCE_FILE_NAME, seqObj->getSequenceName());
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_UPLOAD_SEQUENCE);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, true);
        if (!adapter->hasError()) {
            QFile f(":genecut/template/hidden_login.html");
            SAFE_POINT(f.open(QIODevice::ReadOnly), L10N::errorReadingFile(f.fileName()), );

            QString hiddenLoginHtml = f.readAll();
            hiddenLoginHtml = hiddenLoginHtml.arg(L10N::getActiveLanguageCode()).arg(email).arg(accessToken).arg(refreshToken).arg(apiServer + API_REQUEST_TYPE + "/");

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
            errorMessage(adapter, lbTestInfo);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_fetchResultsClicked() {
    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Get);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addRawHeader("Authorization", "Bearer " + accessToken.toLocal8Bit());
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_REPORTS);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ pbOpenInGenecut, pbFetchResults }, true);
        if (!adapter->hasError()) {
            QByteArray contents(DocumentFormat::READ_BUFF_SIZE, '\0');
            int readSize = adapter->readBlock(contents.data(), DocumentFormat::READ_BUFF_SIZE);
            SAFE_POINT(readSize != -1, "Cannot read request data", );

            contents.resize(readSize);
            QJsonDocument doc = QJsonDocument::fromJson(contents);
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
                dateWgtItem->setData((int)ResultData::IsAmino, arrayObj.value(JSON_IS_AMINO).toBool());
                twResults->setItem(i, (int)TableColumns::Date, dateWgtItem);
                twResults->setItem(i, (int)TableColumns::Status, new QTableWidgetItem(arrayObj.value(JSON_STATUS).toString(), (int)ResultData::Status));
            }
            successMessage(tr("results have been fetched"), lbTestInfo);
        } else {
            errorMessage(adapter, lbTestInfo);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_registerNewClicked() {
    CHECK(areRegistrationDataValid(), );

    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addDataValue(JSON_EMAIL, leEmailNew->text());
    adapter->addDataValue(JSON_PASSWORD, lePasswordNew->text());
    adapter->addDataValue(JSON_ROLE, "USER");
    adapter->addDataValue(JSON_FIRST_NAME, leFirstName->text());
    adapter->addDataValue(JSON_LAST_NAME, leLastName->text());
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());

    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_TYPE + "/" + API_REQUEST_REGISTER);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ pbRegisterNew }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ pbRegisterNew }, true);
        if (!adapter->hasError()) {
            successMessage(tr("user created! Check your email"), lbRegisterWarning);
        } else {
            errorMessage(adapter, lbRegisterWarning);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_getInputSequenceClicked() {
    downloadAndSaveFileFromServer(ServerFileType::Input);
}

void GenecutOPWidget::sl_getResultSequenceClicked() {
    downloadAndSaveFileFromServer(ServerFileType::Result);
}

void GenecutOPWidget::sl_removeSelectedResultClicked() {
    QString resultId = getSelectedReportData(ResultData::Id);
    CHECK(!resultId.isEmpty(), );

    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Post);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addRawHeader("Authorization", "Bearer " + accessToken.toLocal8Bit());
    adapter->addDataValue(JSON_REPORT_ID, resultId);
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + API_REQUEST_DEL_RESULT);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ wtMainForm }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (!adapter->hasError()) {
            auto selected = twResults->selectedItems();
            CHECK(!selected.isEmpty(), );

            tbShortDescription->clear();
            lbTestInfo->clear();
            twResults->removeRow(twResults->row(selected.first()));
        } else {
            errorMessage(adapter, lbTestInfo);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::sl_openResultInBrowserClicked() {
    auto reportId = getSelectedReportData(ResultData::Id);
    CHECK(!reportId.isEmpty(), );

    QFile f(":genecut/template/show_report.html");
    SAFE_POINT(f.open(QIODevice::ReadOnly), L10N::errorReadingFile(f.fileName()), );

    QString showReportHtml = f.readAll();
    showReportHtml = showReportHtml.arg(reportId).arg(L10N::getActiveLanguageCode()).arg(email).arg(accessToken).arg(refreshToken).arg(apiServer);

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

void GenecutOPWidget::sl_compareInputAndOutput() {
    loadedFilesPaths.clear();
    downloadAndSaveFileFromServer(ServerFileType::Input, true);
    downloadAndSaveFileFromServer(ServerFileType::Result, true);
}

void GenecutOPWidget::errorMessage(GenecutHttpFileAdapter* adapter, QLabel* errorLabel) {
    QByteArray contents(DocumentFormat::READ_BUFF_SIZE, '\0');
    int readSize = adapter->readBlock(contents.data(), DocumentFormat::READ_BUFF_SIZE);
    SAFE_POINT(readSize != -1, "Cannot read request data", );

    contents.resize(readSize);
    QJsonDocument doc = QJsonDocument::fromJson(contents);
    auto value = doc.object().value(JSON_MESSAGE).toString();
    QString err;
    if (!value.isEmpty()) {
        err = value;
    } else {
        err = adapter->errorString();
    }
    errorMessage(err, errorLabel);
}

void GenecutOPWidget::errorMessage(const QString& message, QLabel* errorLabel) {
    errorLabel->setStyleSheet("font-weight: 600;color: " + Theme::errorColorLabelStr());
    errorLabel->setText(tr("Error: ") + message);
    coreLog.error(message);
}

void GenecutOPWidget::successMessage(const QString& message, QLabel* label) {
    label->setStyleSheet("font-weight: 600;color: " + Theme::successColorLabelStr());
    label->setText(tr("Success: ") + message);
}

void GenecutOPWidget::warningMessage(const QString& message, QLabel* label) {
    label->setStyleSheet("font-weight: 600;color: " + Theme::warningColorLabelHtmlStr());
    label->setText(tr("Warning: ") + message);
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
        bool validPassword = 5 < passwordNew.size() && passwordNew.size() < 32;
        bool validPasswordConfirmation = 5 < passwordConformationNew.size() && passwordConformationNew.size() < 32;
        bool match = passwordNew == passwordConformationNew;
        if (!match) {
            errorMessage(tr("passwords do not match"), lbRegisterWarning);
            GUIUtils::setWidgetWarningStyle(lePasswordNew, true);
            GUIUtils::setWidgetWarningStyle(lePasswordConformationNew, true);
        } else if (!validPassword) {
            errorMessage(tr("password should be between 6 and 31 symbols length"), lbRegisterWarning);
            GUIUtils::setWidgetWarningStyle(lePasswordNew, true);
            GUIUtils::setWidgetWarningStyle(lePasswordConformationNew, true);
        } else {
            lbRegisterWarning->clear();
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

bool GenecutOPWidget::hasFullReportFile() const {
    CHECK(!twResults->selectedItems().isEmpty(), false);

    auto selection = twResults->selectedItems();
    SAFE_POINT(selection.size() == 2, "Unexpected selection size", false);

    auto dataItem = selection.first()->type() == (int)ResultData::Date ? selection.first() : selection.last();
    auto completed = dataItem->data((int)ResultData::Completed).toBool();
    auto completedWithError = dataItem->data((int)ResultData::CompletedWithError).toBool();
    auto interrupted = dataItem->data((int)ResultData::Interrupted).toBool();

    return completed && !completedWithError && !interrupted;
}

bool GenecutOPWidget::hasNucleicInput() const {
    CHECK(!twResults->selectedItems().isEmpty(), false);

    auto selection = twResults->selectedItems();
    SAFE_POINT(selection.size() == 2, "Unexpected selection size", false);

    auto dataItem = selection.first()->type() == (int)ResultData::Date ? selection.first() : selection.last();
    auto isAmino = dataItem->data((int)ResultData::IsAmino).toBool();

    return !isAmino;

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

void GenecutOPWidget::downloadAndSaveFileFromServer(ServerFileType fileType, bool silentDownload) {
    QString endpoint;
    QString textFileType;
    switch (fileType) {
    case ServerFileType::Input:
        endpoint = API_REQUEST_GET_INPUT;
        textFileType = tr("input");
        break;
    case ServerFileType::Result:
        endpoint = API_REQUEST_GET_RESULT;
        textFileType = tr("result");
        break;
    default:
        FAIL("Unexpected ServerFileType", );
        break;
    }

    QString resultId = getSelectedReportData(ResultData::Id);
    CHECK(!resultId.isEmpty(), );

    auto adapter = qobject_cast<GenecutHttpFileAdapter*>(factory->createIOAdapter());
    adapter->setRequestType(GenecutHttpFileAdapter::RequestType::Get);
    adapter->addHeader(QNetworkRequest::ContentTypeHeader, HEADER_VALUE);
    adapter->addRawHeader("Authorization", "Bearer " + accessToken.toLocal8Bit());
    adapter->addDataValue(JSON_REPORT_ID, resultId);
    adapter->addDataValue(JSON_LANG_ID, L10N::getActiveLanguageCode());
    QString url(apiServer + API_REQUEST_API_SUFFIX + endpoint);
    SAFE_POINT(adapter->open(url), QString("HttpFileAdapter unexpectedly wasn't opened, url: %1").arg(url), );

    setWidgetsEnabled({ wtMainForm }, false);
    connect(adapter, &GenecutHttpFileAdapter::si_done, [this, adapter, textFileType, silentDownload]() {
        setWidgetsEnabled({ wtMainForm }, true);
        if (!adapter->hasError()) {
            GCOUNTER(cvar, "GeneCut get file");
            QByteArray contents(DocumentFormat::READ_BUFF_SIZE, '\0');
            int readSize = adapter->readBlock(contents.data(), DocumentFormat::READ_BUFF_SIZE);
            SAFE_POINT(readSize != -1, "Cannot read request data", );

            contents.resize(readSize);
            QJsonDocument doc = QJsonDocument::fromJson(contents);
            auto jsonObj = doc.object();
            auto fileName = jsonObj.value(JSON_RESPOND_SEQUENCE_FILE_NAME).toString();
            auto fileBody = jsonObj.value(JSON_RESPOND_SEQUENCE_FILE_BODY).toString();
            QString dataDir = GUrlUtils::getDefaultDataPath();
            QString resultFilePath = QDir::toNativeSeparators(dataDir + "/" + fileName);
            resultFilePath = GUrlUtils::rollFileName(resultFilePath, "_");
            QFile resultFile(resultFilePath);
            bool opened = resultFile.open(QIODevice::OpenModeFlag::WriteOnly | QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::Text);
            CHECK_EXT(opened, coreLog.error(tr("Can't save the %1 file, probably, no permissions to write to the data directory: %2").arg(textFileType).arg(dataDir)), );

            resultFile.write(fileBody.toLocal8Bit());
            resultFile.close();
            if (!silentDownload) {
                successMessage(tr("the %1 file has been saved").arg(textFileType), lbTestInfo);
                coreLog.details(tr("The %1 file has been saved to %2").arg(textFileType).arg(resultFilePath));
            }

            if (!silentDownload) {
                auto loadTask = AppContext::getProjectLoader()->openWithProjectTask({ GUrl(resultFilePath) });
                CHECK_EXT(loadTask != nullptr, coreLog.error(tr("Can't load the %1 file %2").arg(textFileType).arg(resultFilePath)), );

                AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
            } else {
                fileFromServerLoaded(resultFilePath);
            }
        } else {
            errorMessage(adapter, lbTestInfo);
        }
        adapter->deleteLater();
    });
}

void GenecutOPWidget::fileFromServerLoaded(const QString& loadedFile) {
    loadedFilesPaths << loadedFile;
    CHECK(loadedFilesPaths.size() == 2, );
    GCOUNTER(cvar, "GeneCut compare");

    QString dataDir = GUrlUtils::getDefaultDataPath();
    QString resultFilePath = QDir::toNativeSeparators(dataDir + "/" + loadedFilesPaths.first().baseFileName()+ "_compare_" + loadedFilesPaths.last().baseFileName() + ".aln");
    resultFilePath = GUrlUtils::rollFileName(resultFilePath, "_");
    QVariantMap hints;
    hints[ProjectLoaderHint_SkipGuiAndMergeSequeencesIntoAlignmentPath] = resultFilePath;
    auto loadTask = AppContext::getProjectLoader()->openWithProjectTask(loadedFilesPaths, hints);
    CHECK_EXT(loadTask != nullptr, coreLog.error(tr("Can't merge input and result into alignment")), );

    AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
}

}
