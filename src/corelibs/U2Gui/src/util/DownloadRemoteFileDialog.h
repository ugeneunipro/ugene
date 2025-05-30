/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QDialog>
#include <QList>
#include <QNetworkReply>
#include <QString>
#include <QXmlSimpleReader>

#include <U2Core/global.h>

class QNetworkReply;
class QNetworkAccessManager;
class Ui_DownloadRemoteFileDialog;

namespace U2 {

class U2GUI_EXPORT DownloadRemoteFileDialog : public QDialog {
    Q_OBJECT
    bool isQueryDB;
    QString resUrl;
    QString resUrls;
    static QString defaultDB;

public slots:
    void sl_onDbChanged();
    void sl_formatChanged(const QString& format);
    void sl_saveFilenameButtonClicked();
    void sl_linkActivated(const QString& link);

public:
    DownloadRemoteFileDialog(QWidget* p = nullptr);
    DownloadRemoteFileDialog(const QString& ids, const QString& dbId, QWidget* p = nullptr);
    ~DownloadRemoteFileDialog();
    QString getDBId() const;
    QString getResourceId() const;
    QString getFullpath() const;
    void accept() override;

private:
    void setSaveFilename();
    bool isNcbiDb(const QString& dbId) const;
    void setupHintText(const QString& sampleText = QString());

    Ui_DownloadRemoteFileDialog* ui;
};

}  // namespace U2
