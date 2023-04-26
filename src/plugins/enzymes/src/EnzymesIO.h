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

#include <QList>
#include <QObject>
#include <QSet>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/Task.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class IOAdapterFactory;

enum EnzymeFileFormat {
    EnzymeFileFormat_Unknown,
    EnzymeFileFormat_Bairoch
};

class EnzymesIO : public QObject {
    Q_OBJECT
public:
    static QList<SEnzymeData> readEnzymes(const QString& url, U2OpStatus& os);
    static void writeEnzymes(const QString& url, const QString& source, const QSet<QString>& enzymes, U2OpStatus& os);
    static QString getFileDialogFilter();
    static QList<SEnzymeData> getDefaultEnzymesList();
    static SEnzymeData findEnzymeById(const QString& id, const QList<SEnzymeData>& enzymes);

    static const char* NOT_DEFINED_SIGN;
private:
    static EnzymeFileFormat detectFileFormat(const QString& url);
    static QList<SEnzymeData> readBairochFile(const QString& url, IOAdapterFactory* iof, U2OpStatus& os);
    static void writeBairochFile(const QString& url, IOAdapterFactory* iof, const QString& source, IOAdapterFactory* srciof, const QSet<QString>& enzymes, U2OpStatus& os);
};

class LoadEnzymeFileTask : public Task {
    Q_OBJECT
public:
    LoadEnzymeFileTask(const QString& url);
    void run();
    void cleanup() {
        enzymes.clear();
    }

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

#define DEFAULT_ENZYMES_FILE QString("2023_02_25.bairoch.gz")

}  // namespace U2
