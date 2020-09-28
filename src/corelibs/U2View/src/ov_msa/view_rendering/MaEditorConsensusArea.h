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

#ifndef _U2_MA_EDITOR_CONSENSUS_AREA_H_
#define _U2_MA_EDITOR_CONSENSUS_AREA_H_

#include <QWidget>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2Region.h>

#include "../MSAEditorConsensusCache.h"
#include "../MaEditorConsensusAreaSettings.h"

class QHelpEvent;
class QMenu;
class QPainter;
class QToolBar;

namespace U2 {

class GObjectView;
class MSAConsensusAlgorithm;
class MSAConsensusAlgorithmFactory;

class MaConsensusAreaRenderer;
class MaEditor;
class MaEditorSelection;
class MaEditorWgt;
class MaModificationInfo;

class U2VIEW_EXPORT MaEditorConsensusArea : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(MaEditorConsensusArea)
    friend class MaConsensusAreaRenderer;

public:
    MaEditorConsensusArea(MaEditorWgt *ui);
    virtual ~MaEditorConsensusArea();

    MaEditorWgt *getEditorWgt() const;
    virtual QString getDefaultAlgorithmId() const = 0;

    QSize getCanvasSize(const U2Region &region, const MaEditorConsElements &elements) const;

    QSharedPointer<MSAEditorConsensusCache> getConsensusCache();

    U2Region getRullerLineYRange() const;

    void setConsensusAlgorithm(MSAConsensusAlgorithmFactory *algo);
    void setConsensusAlgorithmConsensusThreshold(int val);

    const MaEditorConsensusAreaSettings &getDrawSettings() const;
    void setDrawSettings(const MaEditorConsensusAreaSettings &settings);

    MSAConsensusAlgorithm *getConsensusAlgorithm() const;

    void drawContent(QPainter &painter);
    void drawContent(QPainter &painter,
                     const QList<int> &seqIdx,
                     const U2Region &region,
                     const MaEditorConsensusAreaSettings &consensusAreaSettings);

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    virtual QString getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const = 0;

signals:
    void si_consensusAlgorithmChanged(const QString &algoId);
    void si_consensusThresholdChanged(int value);
    // SANGER_TODO: should be moved to McaEditorConsensusArea
    void si_mismatchRedrawRequired();

protected slots:
    void sl_selectionChanged(const MaEditorSelection &current, const MaEditorSelection &prev);
    void sl_alignmentChanged();
    void sl_changeConsensusAlgorithm(const QString &algoId);
    void sl_changeConsensusThreshold(int val);
    void sl_onConsensusThresholdChanged(int newValue);
    void sl_visibleAreaChanged();

    void sl_copyConsensusSequence();
    void sl_copyConsensusSequenceWithGaps();
    void sl_configureConsensusAction();
    void sl_zoomOperationPerformed(bool resizeModeChanged);

    void sl_completeRedraw();

    void setupFontAndHeight();

protected:
    void initCache();
    QString createToolTip(QHelpEvent *he) const;
    void restoreLastUsedConsensusThreshold();
    virtual QString getLastUsedAlgoSettingsKey() const = 0;
    QString getThresholdSettingsKey(const QString &factoryId) const;

    virtual void initRenderer() = 0;
    virtual bool highlightConsensusChar(int pos);

    void updateSelection(int newPos);

    MSAConsensusAlgorithmFactory *getConsensusAlgorithmFactory();
    void updateConsensusAlgorithm();

    MaEditor *editor;
    MaEditorWgt *ui;
    QFont rulerFont;
    int rulerFontHeight;
    QAction *copyConsensusAction;
    QAction *copyConsensusWithGapsAction;
    QAction *configureConsensusAction;
    int curPos;
    bool scribbling;
    bool selecting;

    MaEditorConsensusAreaSettings consensusSettings;
    MaConsensusAreaRenderer *renderer;

    QSharedPointer<MSAEditorConsensusCache> consensusCache;

    bool completeRedraw;
    QPixmap *cachedView;
};

}    // namespace U2

#endif    // _U2_MA_EDITOR_CONSENSUS_AREA_H_
