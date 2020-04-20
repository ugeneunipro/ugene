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

#ifndef _U2_SAVE_DOCUMENT_CONTROLLER_H_
#define _U2_SAVE_DOCUMENT_CONTROLLER_H_

#include <U2Core/DocumentModel.h>

class QAbstractButton;
class QCheckBox;
class QComboBox;
class QLineEdit;

namespace U2 {

class U2GUI_EXPORT SaveDocumentControllerConfig {
public:
    SaveDocumentControllerConfig();

    QLineEdit*                  fileNameEdit;       // edit for file name
    QAbstractButton*            fileDialogButton;   // a button to open save file dialog
    QComboBox*                  formatCombo;        // combo widget to list formats
    QCheckBox*                  compressCheckbox;   // checkbox for the compress option

    QString                     defaultFileName;    // filename set by default
    DocumentFormatId            defaultFormatId;    // format selected by default
    QString                     defaultDomain;      // domain for the last folder

    QString                     saveTitle;          // a title for save file dialog
    QWidget*                    parentWidget;       // parent widget for file dialog

    QString                     rollSuffix;
    bool                        rollFileName;       // roll filename
    bool                        rollOutProjectUrls; // filename roll policy, that allows to create project-unique URLs
};

class U2GUI_EXPORT SaveDocumentController : public QObject {
    Q_OBJECT
public:
    class U2GUI_EXPORT SimpleFormatsInfo {
    public:
        void addFormat(const QString& name, const QStringList& extensions);
        void addFormat(const QString& id, const QString& name, const QStringList& extensions);

        QStringList getNames() const;
        QStringList getExtensionsByName(const QString& formatName) const;
        QString getFirstExtensionByName(const QString& formatName) const;

        QString     getFormatNameById(const QString& id) const;
        QString     getFormatNameByExtension(const QString& ext) const;

        QString     getIdByName(const QString& name) const;

    private:
        QMap<QString, QStringList>  extensions;
        QMap<QString, QString>      names;

    };

    SaveDocumentController(const SaveDocumentControllerConfig& config,
                           const DocumentFormatConstraints& formatConstraints,
                           QObject* parent);
    SaveDocumentController(const SaveDocumentControllerConfig& config,
                           const QList<DocumentFormatId>& formats,
                           QObject* parent);
    SaveDocumentController(const SaveDocumentControllerConfig& config,
                           const SimpleFormatsInfo& formatsDesc,
                           QObject* parent);

    void addFormat(const QString &id, const QString &name, const QStringList &extenstions);

    void setPath(const QString &path, const QSet<QString>& excludeList = QSet<QString>());
    void setFormat(const QString &formatId);

    QString getSaveFileName() const;
    DocumentFormatId getFormatIdToSave() const;

    void forceRoll(const QSet<QString>& excludeList = QSet<QString>());

signals:
    void si_formatChanged(const QString &newFormatId);
    void si_pathChanged(const QString newPath);

private slots:
    void sl_fileNameChanged(const QString& newName);
    void sl_fileDialogButtonClicked();
    void sl_formatChanged(const QString& newFormat);
    void sl_compressToggled(bool enable);

private:
    void init();
    void initSimpleFormatInfo(DocumentFormatConstraints formatConstraints);
    void initSimpleFormatInfo(const QList<DocumentFormatId>& formats);
    void initFormatComboBox();
    bool cutGzExtension(QString &path) const;
    void addGzExtension(QString &path) const;
    void addFormatExtension(QString &path) const;
    QString prepareDefaultFileFilter() const;
    QString prepareFileFilter() const;

    SaveDocumentControllerConfig    conf;
    SimpleFormatsInfo               formatsInfo;
    QString                         currentFormat;
    bool                            overwritingConfirmed;
};

}   // namespace U2

#endif // _U2_SAVE_DOCUMENT_CONTROLLER_H_
