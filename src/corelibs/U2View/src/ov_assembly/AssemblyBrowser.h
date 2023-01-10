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

#ifndef __ASSEMBLY_BROWSER_H__
#define __ASSEMBLY_BROWSER_H__

#include <assert.h>

#include <QByteArray>
#include <QListIterator>
#include <QSharedPointer>

#include <U2Core/AssemblyObject.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>

#include "AssemblyModel.h"
#include "CoveredRegionsManager.h"

namespace U2 {

class AssemblyBrowserUi;
class PositionSelector;
class AssemblyCellRendererFactoryRegistry;
class OptionsPanel;

class AssemblyBrowser : public GObjectView {
    Q_OBJECT
public:
    AssemblyBrowser(QString viewName, AssemblyObject* o);
    // some pre-opening checks
    bool checkValid(U2OpStatus& os);

    // from GObjectView
    void buildStaticToolbar(QToolBar* tb) override;
    void buildMenu(QMenu* menu, const QString& type) override;

    QVariantMap saveState() override;
    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;
    OptionsPanel* getOptionsPanel() override;

    void setGlobalCoverageInfo(CoverageInfo info);
    QList<CoveredRegion> getCoveredRegions() const;
    inline bool areCoveredRegionsReady() const {
        return coverageReady;
    }

    // Local coverage cache is where calculated coverage for current visible region is stored
    void setLocalCoverageCache(CoverageInfo coverage);
    // Methods used to optimize getting coverage at any point inside this region:
    bool isInLocalCoverageCache(qint64 position);
    qint32 getCoverageAtPos(qint64 pos);
    // Methods used to draw coverage for cached part of visible region:
    bool intersectsLocalCoverageCache(U2Region region);
    bool isInLocalCoverageCache(U2Region region);
    // If required region is not fully included in cache, other positions are filled with zeroes
    CoverageInfo extractFromLocalCoverageCache(U2Region region);

    // asm coords <-> pix coords functions
    qint64 calcPixelCoord(qint64 asmCoord) const;
    qint64 calcAsmCoordX(qint64 pixCoord) const;
    qint64 calcAsmCoordY(qint64 pixCoord) const;
    qint64 calcAsmPosX(qint64 pixPosX) const;
    qint64 calcAsmPosY(qint64 pixPosY) const;
    qint64 calcPainterOffset(qint64 xAsmCoord) const;

    // cells utility functions
    int getCellWidth() const;
    qint64 basesCanBeVisible() const;
    qint64 rowsCanBeVisible() const;

    qint64 basesVisible() const;
    qint64 rowsVisible() const;

    bool areReadsVisible() const;
    bool areCellsVisible() const;
    bool areLettersVisible() const;

    // offsets in assembly
    inline qint64 getXOffsetInAssembly() const {
        return xOffsetInAssembly;
    }
    inline qint64 getYOffsetInAssembly() const {
        return yOffsetInAssembly;
    }

    U2Region getVisibleBasesRegion() const {
        return U2Region(xOffsetInAssembly, basesVisible());
    }
    U2Region getVisibleRowsRegion() const {
        return U2Region(yOffsetInAssembly, rowsVisible());
    }

    void setXOffsetInAssembly(qint64 x);
    void setYOffsetInAssembly(qint64 y);
    void setOffsetsInAssembly(qint64 x, qint64 y);
    qint64 normalizeXoffset(qint64 x) const;
    qint64 normalizeYoffset(qint64 y) const;

    void adjustOffsets(qint64 dx, qint64 dy);

    void navigateToRegion(const U2Region& region);

    // other
    inline QSharedPointer<AssemblyModel> getModel() const {
        return model;
    }
    inline QFont getFont() const {
        return font;
    }
    void setFocusToPosSelector();
    inline AssemblyBrowserUi* getMainWidget() {
        return ui;
    }

    AssemblyObject* getAssemblyObject() const {
        return gobject;
    }

    AssemblyCellRendererFactoryRegistry* getCellRendererRegistry() {
        return cellRendererRegistry;
    }

    QAction* getReadHintEnabledAction() {
        return readHintEnabledAction;
    }
    QAction* getCoordsOnRulerAction() {
        return showCoordsOnRulerAction;
    }
    QAction* getCoverageOnRulerAction() {
        return showCoverageOnRulerAction;
    }
    QAction* getSetReferenceAction() {
        return setReferenceAction;
    }

    // public utility functions for zooming
    bool canPerformZoomIn() const {
        return zoomInAction->isEnabled();
    }
    bool canPerformZoomOut() const {
        return zoomOutAction->isEnabled();
    }

    bool onCloseEvent();

public slots:
    void sl_zoomIn(const QPoint& pos = QPoint());
    void sl_zoomOut(const QPoint& pos = QPoint());
    void sl_zoomToReads();
    void sl_coveredRegionClicked(const QString link);
    void sl_extractAssemblyRegion();

signals:
    void si_offsetsChanged();
    void si_zoomOperationPerformed();
    void si_coverageReady();

protected:
    virtual QWidget* createWidget();
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void onObjectRenamed(GObject* obj, const QString& oldName);

private slots:
    void sl_onPosChangeRequest(int);
    void sl_changeOverviewType();
    void sl_onShowCoordsOnRulerChanged(bool checked);
    void sl_onShowCoverageOnRulerChanged(bool checked);
    void sl_onReadHintEnabledChanged(bool checked);
    void sl_saveScreenshot();
    void sl_exportToSam();
    void sl_exportCoverage();
    void sl_unassociateReference();
    void sl_referenceChanged();
    void sl_trackRemoved(VariantTrackObject* obj);
    void sl_setReference();
    void sl_onReferenceLoaded();

private:
    void initFont();
    void setupActions();
    void updateOverviewTypeActions();
    void clear();
    // returns error string
    QString tryAddObject(GObject* obj);
    bool isAssemblyObjectLocked(bool showDialog = true) const;

    // utility functions for zooming
    int zoomInFromSize(int oldCellSize);
    int zoomOutFromSize(int oldCellSize);
    void zoomToSize(int reqCellSize);
    void updateZoomingActions();
    void removeReferenceSequence();
    void assemblyLoaded();

    void addObjectToView(GObject* o);
    void removeObjectFromView(GObject* o);

    QString chooseReferenceUrl() const;
    void loadReferenceFromFile();
    void showReferenceLoadingError(const QList<GObject*>& sequenceObjects, const QString& url) const;
    void setReference(const Document* doc);

private:
    AssemblyBrowserUi* ui;

    AssemblyObject* gobject;
    U2OpStatusImpl dbiOpStatus;
    QSharedPointer<AssemblyModel> model;

    double zoomFactor;
    QFont font;

    qint64 xOffsetInAssembly;
    qint64 yOffsetInAssembly;

    CoveredRegionsManager coveredRegionsManager;
    bool coverageReady;

    CoverageInfo localCoverageCache;

    AssemblyCellRendererFactoryRegistry* cellRendererRegistry;

    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* posSelectorAction;
    PositionSelector* posSelector;
    QList<QAction*> overviewScaleTypeActions;
    QAction* showCoordsOnRulerAction = nullptr;
    QAction* showCoverageOnRulerAction;
    QAction* readHintEnabledAction = nullptr;
    QAction* saveScreenShotAction;
    QAction* exportToSamAction;
    QAction* setReferenceAction;
    QAction* extractAssemblyRegionAction;

    Task* loadReferenceTask;

    const static int MAX_CELL_WIDTH = 300;
    const static double INITIAL_ZOOM_FACTOR;
    const static double ZOOM_MULT;

    const static int LETTER_VISIBLE_WIDTH = 7;
    const static int CELL_VISIBLE_WIDTH = 1;
};

class AssemblyOverview;
class ZoomableAssemblyOverview;
class AssemblyReferenceArea;
class AssemblyConsensusArea;
class AssemblyCoverageGraph;
class AssemblyRuler;
class AssemblyReadsArea;
class AssemblyAnnotationsArea;

class U2VIEW_EXPORT AssemblyBrowserUi : public QWidget {
    Q_OBJECT
public:
    AssemblyBrowserUi(AssemblyBrowser* browser);

    inline QSharedPointer<AssemblyModel> getModel() const {
        return browser->getModel();
    }
    inline AssemblyBrowser* getWindow() const {
        return browser;
    }

    inline AssemblyReadsArea* getReadsArea() const {
        return readsArea;
    }
    inline ZoomableAssemblyOverview* getOverview() const {
        return zoomableOverview;
    }
    inline AssemblyRuler* getRuler() const {
        return ruler;
    }
    inline AssemblyReferenceArea* getReferenceArea() const {
        return referenceArea;
    }
    inline AssemblyConsensusArea* getConsensusArea() const {
        return consensusArea;
    }
    inline AssemblyAnnotationsArea* getAnnotationsArea() const {
        return annotationsArea;
    }
    inline bool isCorrectView() const {
        return !nothingToVisualize;
    }

    QColor getCoverageColor(double grayCoeff);

private:
    AssemblyBrowser* browser;
    ZoomableAssemblyOverview* zoomableOverview;
    AssemblyReferenceArea* referenceArea;
    AssemblyConsensusArea* consensusArea;
    AssemblyCoverageGraph* coverageGraph;
    AssemblyRuler* ruler;
    AssemblyReadsArea* readsArea;
    AssemblyAnnotationsArea* annotationsArea;
    bool nothingToVisualize;
};

}  // namespace U2

#endif
