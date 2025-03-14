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

#include <U2Core/FilesIterator.h>
#include <U2Core/global.h>

namespace U2 {

class URLContainer;

class U2LANG_EXPORT Dataset {
public:
    Dataset();
    Dataset(const QString& name);
    Dataset(const Dataset& other);
    ~Dataset();

    const QString& getName() const;
    void setName(const QString& value);

    void addUrl(URLContainer* url);
    void removeUrl(URLContainer* url);
    QList<URLContainer*> getUrls() const;
    QList<URLContainer*>& getUrls();

    bool contains(const QString& url) const;
    void clear();

    static QList<Dataset> getDefaultDatasetList();

    Dataset& operator=(const Dataset& other);

public:
    static const QString DEFAULT_NAME;

private:
    void copy(const Dataset& other);

    QString name;
    QList<URLContainer*> urls;
};

class U2LANG_EXPORT DatasetFilesIterator : public QObject, public FilesIterator {
    Q_OBJECT
public:
    DatasetFilesIterator(const QList<Dataset>& exps);
    virtual ~DatasetFilesIterator();

    QString getNextFile() override;
    bool hasNext() override;

    QString getLastDatasetName() const;
    void tryEmitDatasetEnded();

signals:
    void si_datasetEnded();

private:
    QList<Dataset> sets;
    FilesIterator* currentIter;
    QString lastDatasetName;
};

}  // namespace U2

Q_DECLARE_METATYPE(QList<U2::Dataset>);
