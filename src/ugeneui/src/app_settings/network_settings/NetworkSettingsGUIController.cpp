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

#include "NetworkSettingsGUIController.h"
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Gui/U2FileDialog.h>

namespace U2
{

NetworkSettingsPageState::NetworkSettingsPageState()
: config(*AppContext::getAppSettings()->getNetworkConfiguration()), useDefaultWebBrowser (true)
{
}

NetworkSettingsPageController::NetworkSettingsPageController(QObject* p) 
: AppSettingsGUIPageController(tr("Network"), APP_SETTINGS_GUI_NETWORK, p)
{
}


AppSettingsGUIPageState* NetworkSettingsPageController::getSavedState() {
    NetworkSettingsPageState* state = new NetworkSettingsPageState();
    state->config = *AppContext::getAppSettings()->getNetworkConfiguration();
    UserAppsSettings* s = AppContext::getAppSettings()->getUserAppsSettings();
    state->webBrowserUrl = s->getWebBrowserURL();
    state->useDefaultWebBrowser = s->useDefaultWebBrowser();
    return state;
}

void NetworkSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    NetworkSettingsPageState* state = qobject_cast<NetworkSettingsPageState*>(s);
    NetworkConfiguration* dst = AppContext::getAppSettings()->getNetworkConfiguration();
    dst->copyFrom(state->config);
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setWebBrowserURL(state->webBrowserUrl);
    st->setUseDefaultWebBrowser(state->useDefaultWebBrowser);
}

AppSettingsGUIPageWidget* NetworkSettingsPageController::createWidget(AppSettingsGUIPageState* data) {
    NetworkSettingsPageWidget* r =  new NetworkSettingsPageWidget();
    r->setState(data);
    return r;
}

const QString NetworkSettingsPageController::helpPageId = QString("24748732");

NetworkSettingsPageWidget::NetworkSettingsPageWidget() {
    setupUi( this );
#ifndef QT_NO_OPENSSL
	sslGroup->setEnabled(true);
#endif
    connect( httpProxyCheck, SIGNAL(stateChanged(int)), SLOT(sl_HttpChecked(int)) );
    connect( proxyExceptionsCheck, SIGNAL(stateChanged(int)), SLOT(sl_ExceptionsChecked(int)) );
    connect( webBrowserButton, SIGNAL(clicked()), SLOT(sl_changeWebBrowserPathButtonClicked()));
}


void NetworkSettingsPageWidget::sl_HttpChecked( int state ) {
    httpProxyAddrEdit->setEnabled( state == Qt::Checked );
    httpProxyPortEdit->setEnabled( state == Qt::Checked );
    sl_ExceptionsChecked(state);
}

void NetworkSettingsPageWidget::sl_ExceptionsChecked(int) {
    proxyExceptionsEdit->setEnabled(httpProxyCheck->isChecked() && proxyExceptionsCheck->isChecked());
}

void NetworkSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    NetworkSettingsPageState* state = qobject_cast<NetworkSettingsPageState*>(s);
    const NetworkConfiguration& set = state->config;
    QNetworkProxy httpProxy = set.getProxy( QNetworkProxy::HttpProxy );
    if( QNetworkProxy::DefaultProxy != httpProxy.type() ) {
        httpProxyAddrEdit->insert( httpProxy.hostName() );
        httpProxyPortEdit->setValue( httpProxy.port() );
        httpProxyCheck->setCheckState( Qt::Checked );
        QString user = httpProxy.user();
        if (!user.isEmpty()) {
            httpAuthBox->setChecked(true);
            httpAuthLoginEdit->setText(user);
            httpAuthPasswordEdit->setText(httpProxy.password());
        }
    } 
    if( !set.isProxyUsed(QNetworkProxy::HttpProxy) ){
        httpProxyAddrEdit->setDisabled( true );
        httpProxyPortEdit->setDisabled( true );
        httpProxyCheck->setCheckState( Qt::Unchecked );
    }

    proxyExceptionsEdit->setPlainText( set.getExceptionsList().join("\n") );
    if( set.exceptionsEnabled() ) {
        proxyExceptionsCheck->setCheckState( Qt::Checked );
    } else {
        proxyExceptionsEdit->setDisabled( true );
    }

    sslBox->addItems(set.getSslProtocolNames());
    int index = sslBox->findText(set.getSslProtocolName());
    sslBox->setCurrentIndex(index);
	remoteRequestBox->setValue(set.remoteRequestTimeout());
    defaultWebBrowser->setChecked(state->useDefaultWebBrowser);
    customWebBrowser->setChecked(!state->useDefaultWebBrowser);
    webBrowserButton->setEnabled(!state->useDefaultWebBrowser);
    webBrowserEdit->setEnabled(!state->useDefaultWebBrowser);
    webBrowserEdit->setText(state->webBrowserUrl);
}

AppSettingsGUIPageState* NetworkSettingsPageWidget::getState(QString& err) const {
    Q_UNUSED(err);
    NetworkSettingsPageState* state = new NetworkSettingsPageState();
    NetworkConfiguration& set = state->config;

    QString httpProxyAddress = httpProxyAddrEdit->text();
    if (!httpProxyAddress.isEmpty()) {
        QNetworkProxy httpProxy( QNetworkProxy::HttpProxy, httpProxyAddrEdit->text());
        
        if (!httpProxyPortEdit->text().isEmpty()) {
            int port = httpProxyPortEdit->value();
            httpProxy.setPort(port);
        }
        
        QString login = httpAuthLoginEdit->text();
        if (httpAuthBox->isChecked() && !login.isEmpty()) {
            QString password = httpAuthPasswordEdit->text();
            httpProxy.setUser(login);
            httpProxy.setPassword(password);
        }
        set.addProxy(httpProxy);
    }

    set.setExceptionsList( proxyExceptionsEdit->toPlainText().split( "\n", QString::SkipEmptyParts ) );
    set.setProxyUsed( QNetworkProxy::HttpProxy, httpProxyCheck->isChecked() );
    set.setExceptionsEnabled( proxyExceptionsCheck->isChecked() );
    set.setSslProtocol(sslBox->currentText());
	set.setRequestTimeout(remoteRequestBox->value());
    if (defaultWebBrowser->isChecked()){
        state->useDefaultWebBrowser=true;
    } else {
        QString wbUrl = webBrowserEdit->text();
        QFile wbFile(wbUrl);
        if (!wbFile.exists()) {
            webBrowserEdit->setFocus();
            err = tr("File not exists");
            delete state;
            return NULL;
        }
        state->webBrowserUrl = wbUrl;
        state->useDefaultWebBrowser=false;
    }
    return state;
}
void NetworkSettingsPageWidget::sl_changeWebBrowserPathButtonClicked() {
    QString file = U2FileDialog::getOpenFileName(this, tr("Select Web browser program"), QString(), 0);
    if (!file.isEmpty()) {
        webBrowserEdit->setText(file);
    }
}


} //namespace
