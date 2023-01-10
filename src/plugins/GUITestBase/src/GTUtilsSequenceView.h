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

#ifndef _U2_GT_UTILS_SEQUENCE_VIEW_H
#define _U2_GT_UTILS_SEQUENCE_VIEW_H

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
    static QWidget* getActiveSequenceViewWindow(HI::GUITestOpStatus& os);

    /** Checks that there is an active sequence view window or fails if not found. */
    static void checkSequenceViewWindowIsActive(HI::GUITestOpStatus& os);

    /** Checks that there is no sequence view window opened: either active or non-active. */
    static void checkNoSequenceViewWindowIsOpened(HI::GUITestOpStatus& os);

    static void getSequenceAsString(HI::GUITestOpStatus& os, QString& sequence);
    static QString getSequenceAsString(HI::GUITestOpStatus& os, int number = 0);
    static QString getBeginOfSequenceAsString(HI::GUITestOpStatus& os, int length);
    static QString getEndOfSequenceAsString(HI::GUITestOpStatus& os, int length);
    static int getLengthOfSequence(HI::GUITestOpStatus& os);
    static int getVisibleStart(HI::GUITestOpStatus& os, int widgetNumber = 0);
    static U2Region getVisibleRange(HI::GUITestOpStatus& os, int widgetNumber = 0);
    static void checkSequence(HI::GUITestOpStatus& os, const QString& expectedSequence);

    /**
     * Calls and fills sequence selection dialog.
     * If 'useHotkey' is false uses context menu.
     * Otherwise clicks to the sequence view (may affect the current annotation selection) and presses Ctrl+A to invoke the dialog.
     **/
    static void selectSequenceRegion(HI::GUITestOpStatus& os, int from, int to, bool useHotkey = true);

    static void selectSeveralRegionsByDialog(HI::GUITestOpStatus& os, const QString& multipleRangeString);

    static void openSequenceView(HI::GUITestOpStatus& os, const QString& sequenceName);
    static void addSequenceView(HI::GUITestOpStatus& os, const QString& sequenceName);

    static void goToPosition(HI::GUITestOpStatus& os, qint64 position);

    /** Moves mouse to the safe sequence view area (Pan or Det view, not a scrollbar) and clicks (brings focus) into it. */
    static void clickMouseOnTheSafeSequenceViewArea(HI::GUITestOpStatus& os);

    /** Opens popup menu safely on the sequence view. The sequence view must have either Det or Pan view widget. */
    static void openPopupMenuOnSequenceViewArea(HI::GUITestOpStatus& os, int number = 0);

    /**
     * Returns currently opened PanView or DetView widget.
     * This method is useful to trigger mouse events over a sequence safely with no chance of hitting scrollbars, etc...
     */
    static QWidget* getPanOrDetView(HI::GUITestOpStatus& os, int number = 0);

    static ADVSingleSequenceWidget* getSeqWidgetByNumber(HI::GUITestOpStatus& os, int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static DetView* getDetViewByNumber(HI::GUITestOpStatus& os, int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static PanView* getPanViewByNumber(HI::GUITestOpStatus& os, int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static Overview* getOverviewByNumber(HI::GUITestOpStatus& os, int number = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static int getSeqWidgetsNumber(HI::GUITestOpStatus& os);
    static QVector<U2Region> getSelection(HI::GUITestOpStatus& os, int number = 0);
    static QString getSeqName(HI::GUITestOpStatus& os, int number = 0);
    static QString getSeqName(HI::GUITestOpStatus& os, ADVSingleSequenceWidget* seqWidget);

    /** Move mouse (hovers) to the annotation in the DetView. */
    static void moveMouseToAnnotationInDetView(HI::GUITestOpStatus& os,
                                               const QString& annotationName,
                                               int annotationRegionStartPos,
                                               int sequenceWidgetIndex = 0);

    /**
     * Clicks on the center of the annotation region in DetView.
     * Locates the region to click by the annotationName and annotationRegionStartPos(visual, starts with 1) that must be one of the location.region.startPos.
     */
    static void clickAnnotationDet(HI::GUITestOpStatus& os,
                                   const QString& annotationName,
                                   int annotationRegionStartPos,
                                   int sequenceWidgetIndex = 0,
                                   bool isDoubleClick = false,
                                   Qt::MouseButton button = Qt::LeftButton);

    static void clickAnnotationPan(HI::GUITestOpStatus& os, const QString& name, int startPos, int number = 0, const bool isDoubleClick = false, Qt::MouseButton button = Qt::LeftButton);

    static GSequenceGraphView* getGraphView(HI::GUITestOpStatus& os);
    static QList<QVariant> getLabelPositions(HI::GUITestOpStatus& os, GSequenceGraphView* graph);
    static QList<GraphLabelTextBox*> getGraphLabels(HI::GUITestOpStatus& os, GSequenceGraphView* graph);
    static QColor getGraphColor(HI::GUITestOpStatus& os, GSequenceGraphView* graph);

    /** Toggle graph visibility by graph name. */
    static void toggleGraphByName(HI::GUITestOpStatus& os, const QString& graphName, int sequenceViewIndex = 0);

    /** Clicks zoom in button. */
    static void zoomIn(HI::GUITestOpStatus& os, int sequenceViewIndex = 0);

    static void enableEditingMode(HI::GUITestOpStatus& os, bool enable = true, int sequenceNumber = 0);

    /** Enables editing mode, sets cursor to the offset, enters the sequence and disables editing mode. */
    static void insertSubsequence(HI::GUITestOpStatus& os, qint64 offset, const QString& subsequence, bool isDirectStrand = true);

    /** It is supposed, that the editing mode is enabled and DetView is visible.
      * The method sets the cursor before the @position (0-based) in the first sequence in the view
      The case with translations and turned off complementary supported bad, let's try to avoid this situation now
      **/
    static void setCursor(HI::GUITestOpStatus& os, qint64 position, bool clickOnDirectLine = false, bool doubleClick = false);

    static qint64 getCursor(HI::GUITestOpStatus& os);

    static QString getRegionAsString(HI::GUITestOpStatus& os, const U2Region& region);

    static void clickOnDetView(HI::GUITestOpStatus& os);

    /** Enables det-view widget if it is not visible. */
    static void makeDetViewVisible(HI::GUITestOpStatus& os);

    /** Enables zoom-view widget if @enable is true, disable otherwise. */
    static void makePanViewVisible(HI::GUITestOpStatus& os, bool enable = true);
};

}  // namespace U2

#endif  // _U2_GT_UTILS_SEQUENCE_VIEW_H
