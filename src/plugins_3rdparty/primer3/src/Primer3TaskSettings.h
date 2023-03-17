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

#ifndef PRIMER3TASKSETTINGS_H
#define PRIMER3TASKSETTINGS_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>

#include <U2Core/U2Range.h>
#include <U2Core/U2Region.h>

#include "primer3_core/libprimer3.h"

namespace U2 {

struct SpanIntronExonBoundarySettings {
    bool enabled = false;
    QString exonAnnotationName = "exon";
    bool overlapExonExonBoundary = false;
    int maxPairsToQuery = 1000;
    int minLeftOverlap = 7;
    int minRightOverlap = 7;
    bool spanIntron = false;
    QList<U2Region> regionList;
    U2Range<int> exonRange;
};

class Primer3TaskSettings {
public:
    Primer3TaskSettings();

    Primer3TaskSettings(const Primer3TaskSettings& settings) = delete;
    Primer3TaskSettings(Primer3TaskSettings&& settings) = delete;
    Primer3TaskSettings& operator=(const Primer3TaskSettings& settings) = delete;
    Primer3TaskSettings& operator=(Primer3TaskSettings&& settings) = delete;
    ~Primer3TaskSettings();

    bool getIntProperty(const QString& key, int* outValue) const;
    bool getDoubleProperty(const QString& key, double* outValue) const;

    bool setIntProperty(const QString& key, int value);
    bool setDoubleProperty(const QString& key, double value);

    QList<QString> getIntPropertyList() const;
    QList<QString> getDoublePropertyList() const;

    QByteArray getSequenceName() const;
    QByteArray getSequence() const;
    int getSequenceSize() const;
    QList<U2Region> getTarget() const;
    QList<int> getOverlapJunctionList() const;
    QList<int> getInternalOverlapJunctionList() const;
    QList<U2Region> getProductSizeRange() const;
    QList<QList<int>> getOkRegion() const;
    int getMinProductSize() const;
    task getTask() const;
    QList<U2Region> getInternalOligoExcludedRegion() const;
    QString getStartCodonSequence() const;
    QString getPrimerMustMatchFivePrime() const;
    QString getPrimerMustMatchThreePrime() const;
    QString getInternalPrimerMustMatchFivePrime() const;
    QString getInternalPrimerMustMatchThreePrime() const;
    QByteArray getLeftInput() const;
    QByteArray getRightInput() const;
    QByteArray getInternalInput() const;
    QByteArray getOverhangLeft() const;
    QByteArray getOverhangRight() const;
    QList<U2Region> getExcludedRegion() const;
    U2Region getIncludedRegion() const;
    QVector<int> getSequenceQuality() const;

    int getFirstBaseIndex() const;

    bool isShowDebugging() const;
    bool isFormatOutput() const;
    bool isExplain() const;

    void setSequenceName(const QByteArray& value);
    void setSequence(const QByteArray& value, bool isCircular = false);
    void setCircularity(bool isCircular);
    void setTarget(const QList<U2Region>& value);
    void setOverlapJunctionList(const QList<int>& value);
    void setInternalOverlapJunctionList(const QList<int>& value);
    void setProductSizeRange(const QList<U2Region>& value);
    void setTaskByName(const QString& taskName);
    void setInternalOligoExcludedRegion(const QList<U2Region>& value);
    void setStartCodonSequence(const QByteArray& value) const;
    void setPrimerMustMatchFivePrime(const QByteArray& value) const;
    void setPrimerMustMatchThreePrime(const QByteArray& value) const;
    void setInternalPrimerMustMatchFivePrime(const QByteArray& value) const;
    void setInternalPrimerMustMatchThreePrime(const QByteArray& value) const;
    void setLeftInput(const QByteArray& value);
    void setLeftOverhang(const QByteArray& value);
    void setRightInput(const QByteArray& value);
    void setRightOverhang(const QByteArray& value);
    void setInternalInput(const QByteArray& value);
    void setExcludedRegion(const QList<U2Region>& value);
    void setOkRegion(const QList<QList<int>>& value);
    void setIncludedRegion(const U2Region& value);
    void setIncludedRegion(const qint64& startPos, const qint64& length);
    void setSequenceQuality(const QVector<int>& value);

    void setRepeatLibraryPath(const QByteArray& value);
    void setMishybLibraryPath(const QByteArray& value);
    void setThermodynamicParametersPath(const QByteArray& value);

    void setShowDebugging(bool value);
    void setFormatOutput(bool value);
    void setExplain(bool value);

    const QByteArray& getRepeatLibraryPath() const;
    const QByteArray& getMishybLibraryPath() const;
    const QByteArray& getThermodynamicParametersPath() const;
    p3_global_settings* getPrimerSettings() const;
    seq_args* getSeqArgs() const;
    void setP3RetVal(p3retval* ret);
    p3retval* getP3RetVal() const;

    // span intron/exon boundary settings
    const SpanIntronExonBoundarySettings& getSpanIntronExonBoundarySettings() const;
    void setSpanIntronExonBoundarySettings(const SpanIntronExonBoundarySettings& settings);
    const QList<U2Region>& getExonRegions() const;
    void setExonRegions(const QList<U2Region>& regions);
    bool spanIntronExonBoundaryIsEnabled() const;

    bool isSequenceCircular() const;
    bool isIncludedRegionValid(const U2Region& r) const;
    void setSequenceRange(const U2Region& region);
    const U2Region& getSequenceRange() const;

private:
    void initMaps();

private:
    QMap<QString, int*> intProperties;
    QMap<QString, double*> doubleProperties;
    bool isCircular = false;
    QByteArray repeatLibraryPath;
    QByteArray mishybLibraryPath;
    QByteArray thermodynamicParametersPath;
    SpanIntronExonBoundarySettings spanIntronExonBoundarySettings;
    U2Region sequenceRange;

    bool showDebugging = false;
    bool formatOutput = false;
    bool explain = false;
    p3_global_settings* primerSettings = nullptr;
    seq_args* seqArgs = nullptr;
    p3retval* p3Retval = nullptr; 
};

}  // namespace U2

#endif  // PRIMER3TASKSETTINGS_H
