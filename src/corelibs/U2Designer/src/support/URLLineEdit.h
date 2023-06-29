/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QLineEdit>
#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class CompletionFiller;
class RunFileSystem;
class SchemaConfig;
class URLWidget;

class URLLineEdit : public QLineEdit {
    Q_OBJECT
public:
    URLLineEdit(const QString& type,
                bool multi,
                bool isPath,
                bool saveFile,
                URLWidget* parent);
    bool isMulti() const;
    CompletionFiller* getCompletionFillerInstance() const;

public slots:
    void sl_onBrowse();

private slots:
    void sl_onBrowseWithAdding();

signals:
    void si_finished();

public:
    SchemaConfig* schemaConfig;
    QString type;
    bool multi;
    bool isPath;
    bool saveFile;
    URLWidget* parent;

private:
    void browse(bool addFiles = false);
    void checkExtension(QString& name) const;
};

}  // namespace U2
