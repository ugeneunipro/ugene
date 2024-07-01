/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <QBitArray>
#include <QObject>
#include <QVector>

#include <U2Core/Msa.h>

namespace U2 {

class MaEditor;

class MsaConsensusAlgorithm;
class MsaConsensusAlgorithmFactory;
class MaModificationInfo;
class MsaObject;
class U2OpStatus;

class U2VIEW_EXPORT MsaEditorConsensusCache : public QObject {
    friend class MaConsensusMismatchController;
    Q_OBJECT
    Q_DISABLE_COPY(MsaEditorConsensusCache)
public:
    MsaEditorConsensusCache(QObject* p, MsaObject* aliObj, MsaConsensusAlgorithmFactory* algo);
    ~MsaEditorConsensusCache() override;

    char getConsensusChar(int pos);

    int getConsensusCharPercent(int pos);
    QList<int> getConsensusPercents(const U2Region& region);

    void setConsensusAlgorithm(MsaConsensusAlgorithmFactory* algo);

    MsaConsensusAlgorithm* getConsensusAlgorithm() const {
        return algorithm;
    }

    QByteArray getConsensusLine(const U2Region& region, bool withGaps);
    QByteArray getConsensusLine(bool withGaps);

signals:
    void si_cachedItemUpdated(int pos, char c);
    void si_cacheResized(int newSize);

private slots:
    void sl_alignmentChanged();
    void sl_thresholdChanged(int newValue);
    void sl_invalidateAlignmentObject();

private:
    struct CacheItem {
        CacheItem(char c = '-', int tc = 0)
            : topChar(c), topPercent((char)tc) {
        }
        char topChar;
        char topPercent;
    };

    void updateCacheItem(int pos);

    int curCacheSize;
    QVector<CacheItem> cache;
    QBitArray updateMap;
    MsaObject* aliObj;
    MsaConsensusAlgorithm* algorithm;
};

}  // namespace U2
