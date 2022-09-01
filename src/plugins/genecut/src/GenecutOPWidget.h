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


namespace U2 {

class AnnotatedDNAView;
class PCRPrimerDesignForDNAAssemblyTask;

class GenecutOPWidget : public QWidget, private Ui_GenecutOPWidget {
    Q_OBJECT
public:
    GenecutOPWidget(AnnotatedDNAView* annDnaView);

private slots:
    void sl_loginClicked();
    void sl_testClicked();
    void sl_registerNewClicked();

private:
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
    static const QString API_REQUEST_REGISTER;
    static const QString API_REQUEST_TEST;

    static const QString JSON_EMAIL;
    static const QString JSON_PASSWORD;
    static const QString JSON_ROLE;
    static const QString JSON_ACCESS_TOKEN;
    static const QString JSON_REFRESH_TOKEN;
    static const QString JSON_USER_OBJECT;
    static const QString JSON_FIRST_NAME;
    static const QString JSON_LAST_NAME;
    static const QString JSON_MESSAGE;
};


}    // namespace U2

#endif    // _U2_GENECUT_OP_WIDGET_H_
