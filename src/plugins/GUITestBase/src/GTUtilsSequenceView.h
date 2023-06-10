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

#include <GTGlobals.h>

#include <QString>

namespace U2 {

class ADVSingleSequenceWidget;
class DetView;
class PanView;
class U2Region;
class Overview;
class GSequenceGraphView;
class GraphLabelTextBox;

class GTUtilsSequenceView {
public:
    /** Returns active sequence view window or fails if not found. */
    static QWidget* getActiveSequenceViewWindow();

    /** Checks that there is an active sequence view window or fails if not found. */
    static void checkSequenceViewWindowIsActive();

    /** Checks that there is no sequence view window opened: either active or non-active. */
    static void checkNoSequenceViewWindowIsOpened();

    static void getSequenceAsString(QString& sequence);
    static QString getSequenceAsString(int number = 0);
    static QString getBeginOfSequenceAsString(int length);
    static QString getEndOfSequenceAsString(int length);
    static int getLengthOfSequence();
    static int getVisibleStart(int widgetNumber = 0);
    static U2Region getVisibleRange(int widgetNumber = 0);
    static void checkSequence(const QString& expectedSequence);

    /**
     * Calls and fills sequence selection dialog.
     * If 'useHotkey' is false uses context menu.
     * Otherwise clicks to the sequence view (may affect the current annotation selection) and presses Ctrl+A to invoke the dialog.
     **/
    static void selectSequenceRegion(int from, int to, bool useHotkey = true);

    static void selectSeveralRegionsByDialog(const QString& multipleRangeString);

    static void openSequenceView(const QString& sequenceName);
    static void addSequenceView(const QString& sequenceName);

    static void goToPosition(qint64 position);

    /** Moves mouse to the safe sequence view area (Pan or Det view, not a scrollbar) and clicks (brings focus) into it. */
    static void clickMouseOnTheSafeSequenceViewArea();

    /** Opens popup menu safely on the sequence view. The sequence view must have either Det or Pan view widget. */
    static void openPopupMenuOnSequenceViewArea(int number = 0);

    /**
     * Returns currently opened PanView or DetView widget.
     * This method is useful to trigger mouse events over a sequence safely with no chance of hitting scrollbars, etc...
     */
    static QWidget* getPanOrDetView(int number = 0);

    static ADVSingleSequenceWidget* getSeqWidgetByNumber(int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static DetView* getDetViewByNumber(int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static PanView* getPanViewByNumber(int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static Overview* getOverviewByNumber(int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static int getSeqWidgetsNumber();
    static QVector<U2Region> getSelection(int number = 0);
    static QString getSeqName(int number = 0);
    static QString getSeqName(ADVSingleSequenceWidget* seqWidget);

    /** Move mouse (hovers) to the annotation in the DetView. */
    static void moveMouseToAnnotationInDetView(
        const QString& annotationName,
        int annotationRegionStartPos,
        int sequenceWidgetIndex = 0);

    /**
     * Clicks on the center of the annotation region in DetView.
     * Locates the region to click by the annotationName and annotationRegionStartPos(visual, starts with 1) that must be one of the location.region.startPos.
     */
    static void clickAnnotationDet(
        const QString& annotationName,
        int annotationRegionStartPos,
        int sequenceWidgetIndex = 0,
        bool isDoubleClick = false,
        Qt::MouseButton button = Qt::LeftButton);

    static void clickAnnotationPan(const QString& name, int startPos, int number = 0, const bool isDoubleClick = false, Qt::MouseButton button = Qt::LeftButton);

    static GSequenceGraphView* getGraphView();
    static QList<QVariant> getLabelPositions(GSequenceGraphView* graph);
    static QList<GraphLabelTextBox*> getGraphLabels(GSequenceGraphView* graph);
    static QColor getGraphColor(GSequenceGraphView* graph);

    /** Toggle graph visibility by graph name. */
    static void toggleGraphByName(const QString& graphName, int sequenceViewIndex = 0);

    /** Clicks zoom in button. */
    static void zoomIn(int sequenceViewIndex = 0);

    static void enableEditingMode(bool enable = true, int sequenceNumber = 0);

    /** Enables editing mode, sets cursor to the offset, enters the sequence and disables editing mode. */
    static void insertSubsequence(qint64 offset, const QString& subsequence, bool isDirectStrand = true);

    /** It is supposed, that the editing mode is enabled and DetView is visible.
      * The method sets the cursor before the @position (0-based) in the first sequence in the view
      The case with translations and turned off complementary supported bad, let's try to avoid this situation now
      **/
    static void setCursor(qint64 position, bool clickOnDirectLine = false, bool doubleClick = false);

    static qint64 getCursor();

    static QString getRegionAsString(const U2Region& region);

    static void clickOnDetView();

    /** Enables det-view widget if it is not visible. */
    static void makeDetViewVisible();

    /** Enables zoom-view widget if @enable is true, disable otherwise. */
    static void makePanViewVisible(bool enable = true);
};

}  // namespace U2
