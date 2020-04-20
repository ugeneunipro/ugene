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

#include <U2Lang/URLContainer.h>

#include "Dataset.h"

namespace U2 {

const QString Dataset::DEFAULT_NAME("Dataset 1");

Dataset::Dataset(const QString &_name)
: name(_name)
{

}

Dataset::Dataset() {
    name = DEFAULT_NAME;
}

Dataset::Dataset(const Dataset &other) {
    copy(other);
}

Dataset::~Dataset() {
    clear();
}

const QString & Dataset::getName() const {
    return name;
}

void Dataset::setName(const QString &value) {
    name = value;
}

void Dataset::addUrl(URLContainer *url) {
    if (NULL != url) {
        urls << url;
    }
}

void Dataset::removeUrl(URLContainer *url) {
    urls.removeOne(url);
}

QList<URLContainer*> Dataset::getUrls() const {
    return urls;
}

QList<URLContainer*> & Dataset::getUrls() {
    return urls;
}

QList<Dataset> Dataset::getDefaultDatasetList() {
    return QList<Dataset>() << Dataset();
}

Dataset &Dataset::operator =(const Dataset &other) {
    if (this == &other) {
        return *this;
    }
    copy(other);
    return *this;
}

void Dataset::copy(const Dataset &other) {
    clear();
    name = other.name;
    foreach (URLContainer *url, other.urls) {
        urls << url->clone();
    }
}

bool Dataset::contains(const QString &url) const {
    foreach (URLContainer *cont, urls) {
        if (cont->getUrl() == url) {
            return true;
        }
    }
    return false;
}

void Dataset::clear() {
    qDeleteAll(urls);
    urls.clear();
}

/************************************************************************/
/* DatasetFilesIterator */
/************************************************************************/
DatasetFilesIterator::DatasetFilesIterator(const QList<Dataset> &_sets)
: FilesIterator(), currentIter(NULL)
{
    foreach (const Dataset &dSet, _sets) {
        sets << dSet;
    }
}

DatasetFilesIterator::~DatasetFilesIterator() {
    delete currentIter;
}

QString DatasetFilesIterator::getNextFile() {
    if (!hasNext()) {
        return "";
    }
    if (NULL != currentIter) {
        assert(!sets.isEmpty());
        lastDatasetName = sets.first().getName();
        return currentIter->getNextFile();
    }
    return "";
}

bool DatasetFilesIterator::hasNext() {
    if (sets.isEmpty()) {
        return false;
    }

    do {
        if (NULL != currentIter && currentIter->hasNext()) {
            return true;
        }
        while (!sets.isEmpty() && sets.first().getUrls().isEmpty()) {
            sets.removeFirst();
            emit si_datasetEnded();
        }
        if (sets.isEmpty()) {
            return false;
        }
        URLContainer *url = sets.first().getUrls().takeFirst();
        sets.first().removeUrl(url);
        delete currentIter;
        currentIter = url->getFileUrls();
    } while(!currentIter->hasNext());

    return (NULL != currentIter && currentIter->hasNext());
}

QString DatasetFilesIterator::getLastDatasetName() const {
    return lastDatasetName;
}

void DatasetFilesIterator::tryEmitDatasetEnded() {
    hasNext();
}

} // U2
