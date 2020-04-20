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

#ifndef _U2_EXTERNAL_TOOL_CONFIG_H_
#define _U2_EXTERNAL_TOOL_CONFIG_H_

#include <U2Lang/ConfigurationEditor.h>

#include <QString>
#include <QObject>
#include <QMap>

namespace U2 {

#define SEQ_WITH_ANNS QString("Sequence_with_annotations")

class U2LANG_EXPORT DataConfig {
public:
    QString attributeId;
    QString attrName;
    QString type;
    QString format;
    QString description;

    bool isStringValue() const;
    bool isFileUrl() const;
    bool isSequence() const;
    bool isAnnotations() const;
    bool isAnnotatedSequence() const;
    bool isAlignment() const;
    bool isText() const;

    bool operator ==(const DataConfig &other) const;

    static const DocumentFormatId STRING_VALUE;
    static const DocumentFormatId OUTPUT_FILE_URL;
};

class U2LANG_EXPORT AttributeConfig {
public:
    enum Flag {
        None,
        AddToDashboard,         // only for output URLs
        OpenWithUgene           // only for file URLs that are added to a dashboard
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    AttributeConfig();

    QString attributeId;
    QString attrName;
    QString type;
    QString defaultValue;
    QString description;
    Flags flags;

    static const QString NUMBER_DEPRECATED_TYPE;
    static const QString URL_DEPRECATED_TYPE;

    static const QString BOOLEAN_TYPE;
    static const QString STRING_TYPE;
    static const QString INTEGER_TYPE;
    static const QString DOUBLE_TYPE;
    static const QString INPUT_FILE_URL_TYPE;
    static const QString OUTPUT_FILE_URL_TYPE;
    static const QString INPUT_FOLDER_URL_TYPE;
    static const QString OUTPUT_FOLDER_URL_TYPE;

    void fixTypes();
    bool isOutputUrl() const;
    bool isFile() const;
    bool isFolder() const;
    bool operator ==(const AttributeConfig &other) const;
};

class U2LANG_EXPORT ExternalProcessConfig {
public:
    ExternalProcessConfig();

    QList<DataConfig> inputs;
    QList<DataConfig> outputs;
    QList<AttributeConfig> attrs;
    QString cmdLine;
    QString id;
    QString name;
    QString description;
    QString templateDescription;
    QString filePath;
    bool useIntegratedTool;
    QString customToolPath;
    QString integratedToolId;

    bool operator ==(const ExternalProcessConfig &other) const;
    bool operator !=(const ExternalProcessConfig &other) const;
};

class U2LANG_EXPORT ExternalToolCfgRegistry: public QObject {
    Q_OBJECT
public:
    ExternalToolCfgRegistry(QObject *parent = nullptr);

    bool registerExternalTool(ExternalProcessConfig *cfg);
    void unregisterConfig(const QString &id);

    ExternalProcessConfig *getConfigById(const QString& id) const;
    QList<ExternalProcessConfig *> getConfigs() const;

private:
    QMap<QString, ExternalProcessConfig *> configs;
};

}

Q_DECLARE_METATYPE(U2::AttributeConfig)
Q_DECLARE_METATYPE(U2::DataConfig)
Q_DECLARE_OPERATORS_FOR_FLAGS(U2::AttributeConfig::Flags)

#endif // _U2_EXTERNAL_TOOL_CONFIG_H_
