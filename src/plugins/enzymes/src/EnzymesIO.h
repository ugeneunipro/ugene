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

#ifndef _U2_ENZYMES_IO_H_
#define _U2_ENZYMES_IO_H_

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/Task.h>

#include <QObject>
#include <QList>
#include <QSet>

namespace U2 {

class IOAdapterFactory;

enum EnzymeFileFormat {
    EnzymeFileFormat_Unknown,
    EnzymeFileFormat_Bairoch
};

class EnzymesIO : public QObject {
    Q_OBJECT
public:
    static QList<SEnzymeData> readEnzymes(const QString& url, TaskStateInfo& ti);
    static void writeEnzymes(const QString& url, const QString& source, const QSet<QString>& enzymes, TaskStateInfo& ti);
    static QString getFileDialogFilter();
    static QList<SEnzymeData> getDefaultEnzymesList();
    static SEnzymeData findEnzymeById(const QString& id, const QList<SEnzymeData>& enzymes);

private:
    static EnzymeFileFormat detectFileFormat(const QString& url);
    static QList<SEnzymeData> readBairochFile(const QString& url, IOAdapterFactory* iof, TaskStateInfo& ti);
    static void writeBairochFile(const QString& url, IOAdapterFactory* iof, const QString& source, IOAdapterFactory* srciof, const QSet<QString>& enzymes, TaskStateInfo& ti);
};

class LoadEnzymeFileTask : public Task {
    Q_OBJECT
public:
    LoadEnzymeFileTask(const QString& url);
    void run();
    void cleanup() {enzymes.clear();}

    QString url;
    QList<SEnzymeData> enzymes;
};

class SaveEnzymeFileTask : public Task {
    Q_OBJECT
public:
    SaveEnzymeFileTask(const QString& url, const QString& source, const QSet<QString>& enzymes);
    void run();

    QString url;
    QString source;
    QSet<QString> enzymes;
};

#define DEFAULT_ENZYMES_FILE    QString("2013_08_01.bairoch.gz")


} //namespace

#endif
