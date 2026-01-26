/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <GTGlobals.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QPlainTextEdit>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Gui/MainWindow.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/DetView.h>
#include <U2View/DetViewRenderer.h>
#include <U2View/DetViewSequenceEditor.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/Overview.h>

#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTSequenceReader"
class GTSequenceReader : public Filler {
public:
    GTSequenceReader(QString* _str)
        : Filler("EditSequenceDialog"), str(_str) {
    }
    void commonScenario() override {
        QWidget* widget = GTWidget::getActiveModalWidget();

        auto textEdit = widget->findChild<QPlainTextEdit*>();
        GT_CHECK(textEdit != nullptr, "PlainTextEdit not found");

        *str = textEdit->toPlainText();

        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
    }

private:
    QString* str;
};
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsSequenceView"

QWidget* GTUtilsSequenceView::getActiveSequenceViewWindow() {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(AnnotatedDNAViewFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}

void GTUtilsSequenceView::checkSequenceViewWindowIsActive() {
    getActiveSequenceViewWindow();
}

void GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened() {
    GTUtilsMdi::checkNoObjectViewWindowIsOpened(AnnotatedDNAViewFactory::ID);
}

void GTUtilsSequenceView::getSequenceAsString(QString& sequence) {
    QWidget* sequenceWidget = getPanOrDetView();
    GTWidget::click(sequenceWidget);

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller());
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new GTSequenceReader(&sequence));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));
    GTMenu::showContextMenu(sequenceWidget);
    GTUtilsDialog::checkNoActiveWaiters();
}

QString GTUtilsSequenceView::getSequenceAsString(int number) {
    getActiveSequenceViewWindow();
    GTWidget::click(getSeqWidgetByNumber(number));

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller());
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(500);
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}));
    // Use PanView or DetView but not the sequence widget itself: there are internal scrollbars in the SequenceWidget that may affect popup menu content.
    QWidget* panOrDetView = getDetViewByNumber(number, {false});
    if (panOrDetView == nullptr) {
        panOrDetView = getPanViewByNumber(number);
    }
    GTWidget::click(panOrDetView, Qt::RightButton);
    QString result = GTClipboard::text();
    return result;
}


QString GTUtilsSequenceView::getBeginOfSequenceAsString(int length) {
    checkSequenceViewWindowIsActive();
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(length));
    GTKeyboardUtils::selectAll();
    GTThread::waitForMainThread();

    QString sequence;
    GTUtilsDialog::waitForDialog(new GTSequenceReader(&sequence));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));
    openPopupMenuOnSequenceViewArea();
    GTUtilsDialog::checkNoActiveWaiters();

    return sequence;
}

QString GTUtilsSequenceView::getEndOfSequenceAsString(int length) {
    QWidget* mdiWindow = getActiveSequenceViewWindow();
    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    Runnable* filler = new SelectSequenceRegionDialogFiller(length, false);
    GTUtilsDialog::waitForDialog(filler);

    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);  // don't touch

    QString sequence;
    GTUtilsDialog::waitForDialog(new GTSequenceReader(&sequence));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));

    GTMenu::showContextMenu(mdiWindow);
    GTGlobals::sleep(1000);

    return sequence;
}

int GTUtilsSequenceView::getLengthOfSequence() {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != nullptr, "MainWindow == NULL", 0);

    MWMDIWindow* mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK_RESULT(mdiWindow != nullptr, "MDI window == NULL", 0);

    GTGlobals::sleep();

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    int length = -1;
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(&length));
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(1000);

    return length;
}

int GTUtilsSequenceView::getVisibleStart(int widgetNumber) {
    return getSeqWidgetByNumber(widgetNumber)->getDetView()->getVisibleRange().startPos;
}

U2Region GTUtilsSequenceView::getVisibleRange(int widgetNumber) {
    ADVSingleSequenceWidget* seqWgt = getSeqWidgetByNumber(widgetNumber);
    GT_CHECK_RESULT(seqWgt != nullptr, "Cannot find sequence view", U2Region());
    return seqWgt->getDetView()->getVisibleRange();
}

void GTUtilsSequenceView::checkSequence(const QString& expectedSequence) {
    QString actualSequence;
    getSequenceAsString(actualSequence);

    GT_CHECK(expectedSequence == actualSequence, "Actual sequence does not match with expected sequence");
}

void GTUtilsSequenceView::selectSequenceRegion(int from, int to, bool useHotkey) {
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(from, to));
    if (useHotkey) {
        clickMouseOnTheSafeSequenceViewArea();
        GTKeyboardUtils::selectAll();
    } else {
        GTUtilsDialog::waitForDialog(new PopupChooser({"select_range_action"}, GTGlobals::UseMouse));
        GTMenu::showContextMenu(getPanOrDetView());
    }
}

void GTUtilsSequenceView::selectSeveralRegionsByDialog(const QString& multipleRangeString) {
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(multipleRangeString));
    clickMouseOnTheSafeSequenceViewArea();
    GTKeyboardUtils::selectAll();
}

void GTUtilsSequenceView::openSequenceView(const QString& sequenceName) {
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
}

void GTUtilsSequenceView::addSequenceView(const QString& sequenceName) {
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTUtilsDialog::waitForDialog(new PopupChooser({"submenu_add_view", "action_add_view"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
}

void GTUtilsSequenceView::goToPosition(qint64 position) {
    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    GTLineEdit::setText("go_to_pos_line_edit", QString::number(position), toolbar);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

void GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea() {
    QWidget* panOrDetView = getPanOrDetView();
    GT_CHECK(panOrDetView != nullptr, "No pan or det-view found!");
    GTMouseDriver::moveTo(panOrDetView->mapToGlobal(panOrDetView->rect().center()));
    GTMouseDriver::click();
}

void GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(int number) {
    QWidget* panOrDetView = getPanOrDetView(number);
    GT_CHECK(panOrDetView != nullptr, "No pan or det-view found!");
    GT_CHECK(panOrDetView->isVisible(), "Pan or det-view is not visible!");
    GTWidget::click(panOrDetView, Qt::RightButton);
}

QWidget* GTUtilsSequenceView::getPanOrDetView(int number) {
    QWidget* panOrDetView = getDetViewByNumber(number, {false});
    if (panOrDetView == nullptr) {
        panOrDetView = getPanViewByNumber(number);
    }
    return panOrDetView;
}

ADVSingleSequenceWidget* GTUtilsSequenceView::getSeqWidgetByNumber(int number, const GTGlobals::FindOptions& options) {
    auto widget = GTWidget::findWidget(
        QString("ADV_single_sequence_widget_%1").arg(number),
        getActiveSequenceViewWindow(),
        options);

    auto seqWidget = qobject_cast<ADVSingleSequenceWidget*>(widget);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(widget != nullptr, QString("Sequence widget %1 was not found!").arg(number), nullptr);
    }

    return seqWidget;
}

DetView* GTUtilsSequenceView::getDetViewByNumber(int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    auto result = seq->findChild<DetView*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(result != nullptr, QString("det view with number %1 not found").arg(number), nullptr);
    }
    return result;
}

PanView* GTUtilsSequenceView::getPanViewByNumber(int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    auto result = seq->findChild<PanView*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("pan view with number %1 not found").arg(number), nullptr)
    }

    return result;
}

Overview* GTUtilsSequenceView::getOverviewByNumber(int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    auto result = seq->findChild<Overview*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("pan view with number %1 not found").arg(number), nullptr)
    }

    return result;
}

int GTUtilsSequenceView::getSeqWidgetsNumber() {
    QList<ADVSingleSequenceWidget*> seqWidgets = getActiveSequenceViewWindow()->findChildren<ADVSingleSequenceWidget*>();
    return seqWidgets.size();
}

QVector<U2Region> GTUtilsSequenceView::getSelection(int number) {
    PanView* panView = getPanViewByNumber(number);
    QVector<U2Region> result = panView->getSequenceContext()->getSequenceSelection()->getSelectedRegions();
    return result;
}

QString GTUtilsSequenceView::getSeqName(int number) {
    return getSeqName(getSeqWidgetByNumber(number));
}

QString GTUtilsSequenceView::getSeqName(ADVSingleSequenceWidget* seqWidget) {
    GT_CHECK_RESULT(seqWidget != nullptr, "Sequence widget is NULL!", "");
    auto nameLabel = GTWidget::findLabel("nameLabel", seqWidget);

    QString labelText = nameLabel->text();
    QString result = labelText.left(labelText.indexOf("[") - 1);  // detachment of name from label text
    return result;
}

#define MIN_ANNOTATION_WIDTH 5

void GTUtilsSequenceView::moveMouseToAnnotationInDetView(
    const QString& annotationName,
    int annotationRegionStartPos,
    int sequenceWidgetIndex) {
    ADVSingleSequenceWidget* sequenceView = getSeqWidgetByNumber(sequenceWidgetIndex);
    DetView* detView = sequenceView->getDetView();
    GT_CHECK(detView != nullptr, "detView not found");
    auto renderArea = qobject_cast<DetViewRenderArea*>(detView->getRenderArea());
    GT_CHECK(renderArea != nullptr, "detView render area not found");

    QList<Annotation*> selectedAnnotationList;
    const QSet<AnnotationTableObject*> annotationObjectSet = sequenceView->getSequenceContext()->getAnnotationObjects(true);
    for (const AnnotationTableObject* ao : qAsConst(annotationObjectSet)) {
        for (Annotation* a : ao->getAnnotations()) {
            QVector<U2Region> regions = a->getLocation()->regions;
            for (const U2Region& r : qAsConst(regions)) {
                if (a->getName() == annotationName && r.startPos == annotationRegionStartPos - 1) {
                    selectedAnnotationList << a;
                }
            }
        }
    }
    GT_CHECK(!selectedAnnotationList.empty(), QString("Annotation with annotationName %1 and startPos %2").arg(annotationName).arg(annotationRegionStartPos));
    GT_CHECK(selectedAnnotationList.size() == 1, QString("Several annotation with annotationName %1 and startPos %2. Number is: %3").arg(annotationName).arg(annotationRegionStartPos).arg(selectedAnnotationList.size()));

    Annotation* annotation = selectedAnnotationList.first();

    const SharedAnnotationData& aData = annotation->getData();
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = asr->getAnnotationSettings(aData);

    U2Region annotationRegion;
    int annotationRegionIndex = 0;
    const QVector<U2Region> regionList = annotation->getRegions();
    for (const U2Region& reg : qAsConst(regionList)) {
        if (reg.startPos == annotationRegionStartPos - 1) {
            annotationRegion = reg;
            break;
        }
        annotationRegionIndex++;
    }
    GT_CHECK(!annotationRegion.isEmpty(), "Region not found");

    if (!annotationRegion.intersects(detView->getVisibleRange())) {
        int center = annotationRegion.center();
        goToPosition(center);
    }

    QList<U2Region> yRegionList = renderArea->getAnnotationYRegions(annotation, annotationRegionIndex, as);
    GT_CHECK(!yRegionList.isEmpty(), "yRegionList is empty!");

    U2Region yRegion = yRegionList.first();
    U2Region visibleRegion = detView->getVisibleRange();
    U2Region annotationVisibleRegion = annotationRegion.intersect(visibleRegion);
    int x1 = renderArea->posToCoord(annotationVisibleRegion.startPos, true);
    int x2 = renderArea->posToCoord(annotationVisibleRegion.endPos() - 1, true) + renderArea->getCharWidth();
    if (x2 <= x1) {  // In the wrap mode x2 may be on a different line. In this case use [x1...line-end] as the click region.
        x2 = renderArea->width();
    }

    const QRect clickRect(x1, yRegion.startPos, x2 - x1, yRegion.length);
    GTMouseDriver::moveTo(renderArea->mapToGlobal(clickRect.center()));
}

void GTUtilsSequenceView::clickAnnotationDet(
    const QString& annotationName,
    int annotationRegionStartPos,
    int sequenceWidgetIndex,
    const bool isDoubleClick,
    Qt::MouseButton button) {
    moveMouseToAnnotationInDetView(annotationName, annotationRegionStartPos, sequenceWidgetIndex);
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click(button);
    }
}

void GTUtilsSequenceView::clickAnnotationPan(const QString& name, int startPos, int number, bool isDoubleClick, Qt::MouseButton button, int clickOffsetX) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(number);
    GSequenceLineViewRenderArea* area = seq->getPanView()->getRenderArea();
    auto pan = dynamic_cast<PanViewRenderArea*>(area);
    GT_CHECK(pan != nullptr, "pan view render area not found");

    ADVSequenceObjectContext* context = seq->getSequenceContext();
    context->getAnnotationObjects(true);

    QList<Annotation*> anns;
    foreach (const AnnotationTableObject* ao, context->getAnnotationObjects(true)) {
        const QList<Annotation*>& annotations = ao->getAnnotations();
        for (Annotation* a : qAsConst(annotations)) {
            const int sp = a->getLocation().data()->regions.first().startPos;
            const QString annName = a->getName();
            if (sp == startPos - 1 && annName == name) {
                anns << a;
            }
        }
    }
    GT_CHECK(!anns.empty(), QString("Annotation with name %1 and startPos %2").arg(name).arg(startPos));
    GT_CHECK(anns.size() == 1, QString("Several annotation with name %1 and startPos %2. Number is: %3").arg(name).arg(startPos).arg(anns.size()));

    Annotation* a = anns.first();

    const SharedAnnotationData& aData = a->getData();
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = asr->getAnnotationSettings(aData);

    const U2Region& vr = seq->getPanView()->getVisibleRange();
    QVector<U2Region> regions = a->getLocation().data()->regions;
    const U2Region& r = regions.first();

    if (!r.intersects(vr)) {
        int center = r.center();
        goToPosition(center);
        GTGlobals::sleep();
    }

    U2Region visibleLocation = r.intersect(vr);
    U2Region y = pan->getAnnotationYRange(a, 0, as);

    float start = visibleLocation.startPos;
    float end = visibleLocation.endPos();
    float x1f = (float)(start - vr.startPos) * pan->getCurrentScale();
    float x2f = (float)(end - vr.startPos) * pan->getCurrentScale();

    int rw = qMax(MIN_ANNOTATION_WIDTH, qRound(x2f - x1f));
    int x1 = qRound(x1f);

    QRect annotationRect(x1, y.startPos, rw, y.length);
    auto move2Point = pan->mapToGlobal(annotationRect.center());
    if (clickOffsetX != 0) {
        move2Point = QPoint(move2Point.x() + clickOffsetX, move2Point.y());
    }
    GTMouseDriver::moveTo(move2Point);
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click(button);
    }
}

GSequenceGraphView* GTUtilsSequenceView::getGraphView() {
    auto graph = getSeqWidgetByNumber()->findChild<GSequenceGraphView*>();
    GT_CHECK_RESULT(graph != nullptr, "Graph view is NULL", nullptr);
    return graph;
}

QList<QVariant> GTUtilsSequenceView::getLabelPositions(GSequenceGraphView* graph) {
    QList<QVariant> list;
    graph->getSavedLabelsState(list);
    return list;
}

QList<GraphLabelTextBox*> GTUtilsSequenceView::getGraphLabels(GSequenceGraphView* graph) {
    QList<GraphLabelTextBox*> result = graph->findChildren<GraphLabelTextBox*>();
    return result;
}

QColor GTUtilsSequenceView::getGraphColor(GSequenceGraphView* graph) {
    ColorMap map = graph->getGraphDrawer()->getColors();
    QColor result = map.value("Default color");
    return result;
}

void GTUtilsSequenceView::toggleGraphByName(const QString& graphName, int sequenceViewIndex) {
    QWidget* sequenceWidget = getSeqWidgetByNumber(sequenceViewIndex);
    auto graphAction = GTWidget::findWidget("GraphMenuAction", sequenceWidget, false);
    GTUtilsDialog::waitForDialog(new PopupChooser({graphName}));
    GTWidget::click(graphAction);
}

void GTUtilsSequenceView::zoomIn(int sequenceViewIndex) {
    QWidget* sequenceWidget = getSeqWidgetByNumber(sequenceViewIndex);
    QAction* zoomInAction = GTAction::findActionByText("Zoom In", sequenceWidget);
    GTWidget::click(GTAction::button(zoomInAction));
}

void GTUtilsSequenceView::enableEditingMode(bool enable, int sequenceNumber) {
    DetView* detView = getDetViewByNumber(sequenceNumber);

    auto toolbar = GTWidget::findToolBar("WidgetWithLocalToolbar_toolbar", detView);
    auto editButton = qobject_cast<QToolButton*>(GTToolbar::getWidgetForActionObjectName(toolbar, "edit_sequence_action"));
    CHECK_SET_ERR(editButton != nullptr, "'edit_sequence_action' button is NULL");
    if (editButton->isChecked() != enable) {
        if (editButton->isVisible()) {
            GTWidget::click(editButton);
        } else {
            QPoint gp = detView->mapToGlobal(QPoint(10, detView->rect().height() - 5));
            GTMouseDriver::moveTo(gp);
            GTMouseDriver::click();
            GTGlobals::sleep(500);
            GTKeyboardDriver::keyClick(Qt::Key_Up);
            GTGlobals::sleep(200);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTGlobals::sleep(200);
        }
    }
}

void GTUtilsSequenceView::insertSubsequence(qint64 offset, const QString& subsequence, bool isDirectStrand) {
    makeDetViewVisible();
    enableEditingMode(true);
    setCursor(offset, isDirectStrand);
    GTKeyboardDriver::keySequence(subsequence);
    enableEditingMode(false);
}

void GTUtilsSequenceView::setCursor(qint64 position, bool clickOnDirectLine, bool doubleClick) {
    // Multiline view is not supported correctly.

    DetView* detView = getDetViewByNumber(0);

    DetViewRenderArea* renderArea = detView->getDetViewRenderArea();
    CHECK_SET_ERR(renderArea != nullptr, "DetViewRenderArea is NULL");

    DetViewRenderer* renderer = renderArea->getRenderer();
    CHECK_SET_ERR(renderer != nullptr, "DetViewRenderer is NULL");

    U2Region visibleRange = detView->getVisibleRange();
    if (!visibleRange.contains(position)) {
        GTUtilsSequenceView::goToPosition(position);
        GTGlobals::sleep();
        visibleRange = detView->getVisibleRange();
    }
    CHECK_SET_ERR(visibleRange.contains(position), "Position is out of visible range");

    const double scale = renderer->getCurrentScale();
    const int coord = renderer->posToXCoord(position, renderArea->size(), visibleRange) + (int)(scale / 2);

    bool isWrapMode = detView->isWrapMode();
    if (!isWrapMode) {
        GTMouseDriver::moveTo(renderArea->mapToGlobal(QPoint(coord, 40)));  // TODO: replace the hardcoded value with method in renderer
    } else {
        GTUtilsSequenceView::goToPosition(position);
        GTGlobals::sleep();

        const int symbolsPerLine = renderArea->getSymbolsPerLine();
        const int linesCount = renderArea->getLinesCount();
        visibleRange = GTUtilsSequenceView::getVisibleRange();
        int linesBeforePos = -1;
        for (int i = 0; i < linesCount; i++) {
            const U2Region line(visibleRange.startPos + i * symbolsPerLine, symbolsPerLine);
            if (line.contains(position)) {
                linesBeforePos = i;
                break;
            }
        }
        CHECK_SET_ERR(linesBeforePos != -1, "Position not found");

        const int shiftsCount = renderArea->getShiftsCount();
        int middleShift = (int)(shiftsCount / 2) + 1;  // TODO: this calculation might consider the case then complementary is turned off or translations are drawn
        if (clickOnDirectLine) {
            middleShift--;
        }

        const int shiftHeight = renderArea->getShiftHeight();
        const int lineToClick = linesBeforePos * shiftsCount + middleShift;

        const int yPos = (lineToClick * shiftHeight) - (shiftHeight / 2);

        GTMouseDriver::moveTo(renderArea->mapToGlobal(QPoint(coord, yPos)));
    }
    if (doubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click();
    }
}

qint64 GTUtilsSequenceView::getCursor() {
    DetView* detView = getDetViewByNumber(0);

    DetViewSequenceEditor* dwSequenceEditor = detView->getEditor();
    GT_CHECK_RESULT(dwSequenceEditor != nullptr, "DetViewSequenceEditor is NULL", -1);

    const bool isEditMode = detView->isEditMode();
    GT_CHECK_RESULT(isEditMode, "Edit mode is disabled", -1);

    return dwSequenceEditor->getCursorPosition();
}

QString GTUtilsSequenceView::getRegionAsString(const U2Region& region) {
    GTUtilsSequenceView::selectSequenceRegion(region.startPos, region.endPos() - 1);
    GTGlobals::sleep();

    GTKeyboardUtils::copy();

    return GTClipboard::text();
}

void GTUtilsSequenceView::clickOnDetView() {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow == NULL");

    MWMDIWindow* mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != nullptr, "MDI window == NULL");

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    GTGlobals::sleep(500);
}

void GTUtilsSequenceView::makeDetViewVisible() {
    auto toggleDetViewButton = GTWidget::findToolButton("show_hide_details_view");
    if (!toggleDetViewButton->isChecked()) {
        GTWidget::click(toggleDetViewButton);
    }
}

void GTUtilsSequenceView::makePanViewVisible(bool enable) {
    auto toggleZoomViewButton = GTWidget::findToolButton("show_hide_zoom_view");
    if (toggleZoomViewButton->isChecked() != enable) {
        GTWidget::click(toggleZoomViewButton);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
