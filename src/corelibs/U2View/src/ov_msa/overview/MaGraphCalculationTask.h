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

#include <QPolygonF>

#include <U2Core/AppResources.h>
#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/Msa.h>
#include <U2Core/global.h>

#include <U2View/MsaEditorConsensusCache.h>

namespace U2 {

class MaEditor;
class MsaObject;
class MSAConsensusAlgorithm;
class MsaColorScheme;
class MsaHighlightingScheme;

class MaGraphCalculationTask : public BackgroundTask<QPolygonF> {
    Q_OBJECT
public:
    MaGraphCalculationTask(MsaObject* msa, int width, int height);

    void run();
signals:
    void si_calculationStarted();
    void si_calculationStoped();

protected:
    void constructPolygon(QPolygonF& polygon);
    virtual int getGraphValue(int) const {
        return height;
    }

    Msa ma;
    MemoryLocker memLocker;
    int msaLength;
    int seqNumber;
    int width;
    int height;
};

class MaConsensusOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaConsensusOverviewCalculationTask(MsaObject* msa,
                                       int width,
                                       int height);

private:
    int getGraphValue(int pos) const;

    MSAConsensusAlgorithm* algorithm;
};

class MaGapOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaGapOverviewCalculationTask(MsaObject* msa,
                                 int width,
                                 int height);

private:
    int getGraphValue(int pos) const;
};

class MaClustalOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaClustalOverviewCalculationTask(MsaObject* msa,
                                     int width,
                                     int height);

private:
    int getGraphValue(int pos) const;

    MSAConsensusAlgorithm* algorithm;
};

class MaHighlightingOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaHighlightingOverviewCalculationTask(MaEditor* _editor,
                                          const QString& colorSchemeId,
                                          const QString& highlightingSchemeId,
                                          int width,
                                          int height);

    static bool isCellHighlighted(const Msa& msa,
                                  MsaHighlightingScheme* highlightingScheme,
                                  MsaColorScheme* colorScheme,
                                  int seq,
                                  int pos,
                                  int refSeq);

    static bool isGapScheme(const QString& schemeId);
    static bool isEmptyScheme(const QString& schemeId);

private:
    int getGraphValue(int pos) const;

    bool isCellHighlighted(int seq, int pos) const;

    int refSequenceId;

    MsaColorScheme* colorScheme;
    MsaHighlightingScheme* highlightingScheme;
    QString schemeId;
};

}  // namespace U2
