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

#ifndef PRIMER3TASKSETTINGS_H
#define PRIMER3TASKSETTINGS_H

#include <QMap>
#include <QList>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QPair>
#include <U2Core/U2Region.h>
#include <U2Core/U2Range.h>
#include "primer3_core/primer3.h"

namespace U2 {


struct SpanIntronExonBoundarySettings {

    SpanIntronExonBoundarySettings() {
        enabled = false;
        minLeftOverlap = 7;
        minRightOverlap = 7;
        maxPairsToQuery = 1000;
        spanIntron = false;
        overlapExonExonBoundary = false;
        exonAnnotationName = "exon";

    }

    bool enabled;
    QString exonAnnotationName;
    bool overlapExonExonBoundary;
    int maxPairsToQuery;
    int minLeftOverlap, minRightOverlap;
    bool spanIntron;
    QList<U2Region> regionList;
    U2Range<int> exonRange;

};


class Primer3TaskSettings
{
public:
    Primer3TaskSettings();
    Primer3TaskSettings(const Primer3TaskSettings &settings);
    Primer3TaskSettings &operator=(const Primer3TaskSettings &settings);
    ~Primer3TaskSettings();

    bool getIntProperty(const QString &key, int *outValue)const;
    bool getDoubleProperty(const QString &key, double *outValue)const;
    bool getAlignProperty(const QString &key, short *outValue)const;

    bool setIntProperty(const QString &key,int value);
    bool setDoubleProperty(const QString &key,double value);
    bool setAlignProperty(const QString &key,short value);

    QList<QString> getIntPropertyList()const;
    QList<QString> getDoublePropertyList()const;
    QList<QString> getAlignPropertyList()const;

    QByteArray getSequenceName()const;
    QByteArray getSequence()const;
    int getSequenceSize() const;
    QList< U2Region > getTarget()const;
    QList< U2Region > getProductSizeRange()const;
    int getMinProductSize() const;
    task getTask()const;
    QList< U2Region > getInternalOligoExcludedRegion()const;
    QByteArray getLeftInput()const;
    QByteArray getRightInput()const;
    QByteArray getInternalInput()const;
    QList< U2Region > getExcludedRegion()const;
    U2Region getIncludedRegion()const;
    QVector<int> getSequenceQuality()const;

    QByteArray getError()const;
    int getFirstBaseIndex()const;

    void setSequenceName(const QByteArray &value);
    void setSequence(const QByteArray &value, bool isCircular = false);
    void setCircularity(bool isCircular);
    void setTarget(const QList< U2Region > &value);
    void setProductSizeRange(const QList< U2Region > &value);
    void setTask(const task &value);
    void setInternalOligoExcludedRegion(const QList< U2Region > &value);
    void setLeftInput(const QByteArray &value);
    void setRightInput(const QByteArray &value);
    void setInternalInput(const QByteArray &value);
    void setExcludedRegion(const QList< U2Region > &value);
    void setIncludedRegion(const U2Region &value);
    void setIncludedRegion(const qint64 &startPos, const qint64 &length);
    void setSequenceQuality(const QVector<int> &value);

    void setRepeatLibrary(const QByteArray &value);
    void setMishybLibrary(const QByteArray &value);

    QByteArray getRepeatLibrary()const;
    QByteArray getMishybLibrary()const;

    primer_args *getPrimerArgs();
    seq_args *getSeqArgs();

    // span intron/exon boundary settings

    const SpanIntronExonBoundarySettings& getSpanIntronExonBoundarySettings() const {
        return spanIntronExonBoundarySettings;
    }

    void setSpanIntronExonBoundarySettings(const SpanIntronExonBoundarySettings& settings) {
        spanIntronExonBoundarySettings = settings;
    }


    const QList<U2Region>& getExonRegions() const {
        return spanIntronExonBoundarySettings.regionList;
    }

    void setExonRegions(const QList<U2Region>& regions) {
        spanIntronExonBoundarySettings.regionList = regions;
    }


    bool spanIntronExonBoundaryIsEnabled() const { return spanIntronExonBoundarySettings.enabled; }

    bool isSequenceCircular() const { return isCircular; }

    bool checkIncludedRegion(const U2Region &r) const;

private:
    void initMaps();

private:
    QMap<QString, int *> intProperties;
    QMap<QString, double *> doubleProperties;
    QMap<QString, short *> alignProperties;

    // don't forget to change copy constructor and assignment operator when changing this!
    QByteArray sequenceName;
    QByteArray sequence;
    bool isCircular;
    QByteArray leftInput;
    QByteArray rightInput;
    QByteArray internalInput;
    QVector<int> sequenceQuality;

    QByteArray repeatLibrary;
    QByteArray mishybLibrary;
    SpanIntronExonBoundarySettings spanIntronExonBoundarySettings;

    primer_args primerArgs;
    seq_args seqArgs;
};

} // namespace U2

#endif // PRIMER3TASKSETTINGS_H
