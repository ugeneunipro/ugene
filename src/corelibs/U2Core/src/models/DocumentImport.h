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

#ifndef _U2_DOCUMENT_IMPORT_H_
#define _U2_DOCUMENT_IMPORT_H_

#include <U2Core/DocumentModel.h>

#include <QDialog>

namespace U2 {

class DocumentImporter;
class DocumentProviderTask;
class FormatDetectionResult;
class ImportWidget;

class U2CORE_EXPORT ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(const QVariantMap& _settings) : settings(_settings) {}

    const QVariantMap& getSettings() const { return settings; }

public slots:
    virtual void accept();

protected:
    virtual bool isValid() { return true; }     // it is not const method: derived class can do something non-const
    virtual void applySettings() = 0;

    QVariantMap settings;
};

class ImportWidgetFactory {
public:
    virtual ~ImportWidgetFactory() {}

    virtual ImportWidget* getWidget(const GUrl& url, const QVariantMap& settings) const = 0;
};

/** Registry for all DocumentImportHandlers */
class U2CORE_EXPORT DocumentImportersRegistry: public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(DocumentImportersRegistry)
public:
    DocumentImportersRegistry(QObject* p = NULL) : QObject(p) {}
    ~DocumentImportersRegistry();

    /** returns handler by its id */
    DocumentImporter* getDocumentImporter(const QString& importerId) const;

    /** registers new document import handler */
    void addDocumentImporter(DocumentImporter* i);

    const QList<DocumentImporter*>& getImporters() const {return importers;}

private:
    QList<DocumentImporter*> importers;
};


class U2CORE_EXPORT DocumentImporter : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(DocumentImporter)
public:
    DocumentImporter(const QString& _id, const QString& _name, QObject* o = NULL) : QObject(o), id(_id), name(_name), widgetFactory(NULL) {}

    virtual ~DocumentImporter() { delete widgetFactory; }

    virtual FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& url) = 0;

    virtual DocumentProviderTask* createImportTask(const FormatDetectionResult& res, bool showGui, const QVariantMap &hints) = 0;

    virtual QString getRadioButtonText() const;

    virtual ImportWidget* createImportWidget(const GUrl& url, const QVariantMap& settings) const;

    virtual QString getImporterDescription() const {return importerDescription;}

    const QString& getImporterName() const {return name;}

    const QString& getId() const {return id;}

    const QStringList& getFormatIds() const {return formatIds;}

    const QList<QString>& getSupportedFileExtensions() const {return extensions;}

    void setWidgetFactory(ImportWidgetFactory* factory);

    const QSet<GObjectType> &getSupportedObjectTypes() const;

    // NOTE: it is hint, it should be true by default
    static const QString LOAD_RESULT_DOCUMENT;

protected:
    QString                 id;
    QString                 name;
    QStringList             formatIds;
    QList<QString>          extensions;
    QString                 importerDescription;
    ImportWidgetFactory*    widgetFactory;
    QSet<GObjectType>       supportedObjectTypes;
};

} //namespace

#endif
