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

#ifndef _U2_MSA_GRAPH_CALCULATION_TASK_H_
#define _U2_MSA_GRAPH_CALCULATION_TASK_H_

#include <U2Core/AppResources.h>
#include <U2Core/global.h>
#include <U2Core/BackgroundTaskRunner.h>
#include <U2View/MSAEditorConsensusCache.h>

#include <U2Core/MultipleSequenceAlignment.h>

#include <QPolygonF>

namespace U2 {

class MaEditor;
class MultipleAlignmentObject;
class MSAConsensusAlgorithm;
class MsaColorScheme;
class MsaHighlightingScheme;

class MaGraphCalculationTask : public BackgroundTask<QPolygonF> {
    Q_OBJECT
public:
    MaGraphCalculationTask(MultipleAlignmentObject* msa, int width, int height);

    void run();
signals:
    void si_calculationStarted();
    void si_calculationStoped();
protected:
    void constructPolygon(QPolygonF &polygon);
    virtual int getGraphValue(int) const { return height; }

    MultipleAlignment ma;
    MemoryLocker memLocker;
    int msaLength;
    int seqNumber;
    int width;
    int height;
};

class MaConsensusOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaConsensusOverviewCalculationTask(MultipleAlignmentObject* msa,
                                        int width, int height);
private:
    int getGraphValue(int pos) const;

    MSAConsensusAlgorithm*  algorithm;
};

class MaGapOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaGapOverviewCalculationTask(MultipleAlignmentObject* msa,
                                  int width, int height);
private:
    int getGraphValue(int pos) const;
};

class MaClustalOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaClustalOverviewCalculationTask(MultipleAlignmentObject* msa,
                                      int width, int height);
private:
    int getGraphValue(int pos) const;

    MSAConsensusAlgorithm*  algorithm;
};

class MaHighlightingOverviewCalculationTask : public MaGraphCalculationTask {
    Q_OBJECT
public:
    MaHighlightingOverviewCalculationTask(MaEditor* _editor,
                                           const QString &colorSchemeId,
                                           const QString &highlightingSchemeId,
                                           int width, int height);

    static bool isCellHighlighted(const MultipleAlignment &msa,
                                  MsaHighlightingScheme* highlightingScheme,
                                  MsaColorScheme* colorScheme,
                                  int seq, int pos,
                                  int refSeq);

    static bool isGapScheme(const QString &schemeId);
    static bool isEmptyScheme(const QString &schemeId);

private:
    int getGraphValue(int pos) const;

    bool isCellHighlighted(int seq, int pos) const;

    int refSequenceId;

    MsaColorScheme*         colorScheme;
    MsaHighlightingScheme*  highlightingScheme;
    QString                 schemeId;
};

} // namespace

#endif // _U2_MSA_GRAPH_CALCULATION_TASK_H_
