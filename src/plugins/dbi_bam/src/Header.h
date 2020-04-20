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

#ifndef _U2_BAM_HEADER_H_
#define _U2_BAM_HEADER_H_

#include <QByteArray>
#include <QMap>
#include <QVariant>
#include <U2Core/Version.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace BAM {

class Header
{
public:
    enum SortingOrder {
        Unknown,
        Unsorted,
        QueryName,
        Coordinate
    };

    Header();

    class Reference {
    public:
        Reference(const QByteArray &name, int length);
        const QByteArray &getName()const;
        int getLength()const;
        const QByteArray &getAssemblyId()const;
        const QByteArray &getMd5()const;
        const QByteArray &getSpecies()const;
        const QString &getUri()const;
        void setName(const QByteArray &name);
        void setLength(int length);
        void setAssemblyId(const QByteArray &assemblyId);
        void setMd5(const QByteArray &md5);
        void setSpecies(const QByteArray &species);
        void setUri(const QString &uri);
    private:
        QByteArray name;
        int length;
        QByteArray assemblyId;
        QByteArray md5;
        QByteArray species;
        QString uri;
    };
    class ReadGroup {
    public:
        ReadGroup();
        const QByteArray &getSequencingCenter()const;
        const QByteArray &getDescription()const;
        const QVariant &getDate()const;
        const QByteArray &getLibrary()const;
        const QByteArray &getPrograms()const;
        int getPredictedInsertSize()const;
        const QByteArray &getPlatform()const;
        const QByteArray &getPlatformUnit()const;
        const QByteArray &getSample()const;
        void setSequencingCenter(const QByteArray &sequencingCenter);
        void setDescription(const QByteArray &description);
        void setDate(const QVariant &date);
        void setLibrary(const QByteArray &library);
        void setPrograms(const QByteArray &programs);
        void setPredictedInsertSize(int medianInsertSize);
        void setPlatform(const QByteArray &platform);
        void setPlatformUnit(const QByteArray &platformUnit);
        void setSample(const QByteArray &sample);
    private:
        QByteArray sequencingCenter;
        QByteArray description;
        QVariant date;
        QByteArray library;
        QByteArray programs;
        int predictedInsertSize;
        QByteArray platform;
        QByteArray platformUnit;
        QByteArray sample;
    };
    class Program {
    public:
        Program();
        const QByteArray &getName()const;
        const QByteArray &getCommandLine()const;
        int getPreviousId()const;
        const QByteArray &getVersion()const;
        void setName(const QByteArray &name);
        void setCommandLine(const QByteArray &commandLine);
        void setPreviousId(int previousId);
        void setVersion(const QByteArray &version);
    private:
        QByteArray name;
        QByteArray commandLine;
        int previousId;
        QByteArray version;
    };
    const Version &getFormatVersion()const;
    SortingOrder getSortingOrder()const;
    const QList<Reference> &getReferences()const;
    const QList<ReadGroup> &getReadGroups()const;
    const QList<Program> &getPrograms()const;
    const QString& getText()const;
    void setFormatVersion(const Version &formatVersion);
    void setSortingOrder(SortingOrder sortingOrder);
    void setReferences(const QList<Reference> &references);
    void setReadGroups(const QList<ReadGroup> &readGroups);
    void setPrograms(const QList<Program> &programs);
    void setText(const QString& text);
private:
    Version formatVersion;
    SortingOrder sortingOrder;
    QList<Reference> references;
    QList<ReadGroup> readGroups;
    QList<Program> programs;
    QString text;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_HEADER_H_
