/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <QPushButton>

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
#define GT_METHOD_NAME "commonScenario"
class GTSequenceReader : public Filler {
public:
    GTSequenceReader(HI::GUITestOpStatus& _os, QString* _str)
        : Filler(_os, "EditSequenceDialog"), str(_str) {
    }
    void commonScenario() {
        QWidget* widget = GTWidget::getActiveModalWidget(os);

        QPlainTextEdit* textEdit = widget->findChild<QPlainTextEdit*>();
        GT_CHECK(textEdit != nullptr, "PlainTextEdit not found");

        *str = textEdit->toPlainText();

        GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
    }

private:
    QString* str;
};
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsSequenceView"

#define GT_METHOD_NAME "getActiveSequenceViewWindow"
QWidget* GTUtilsSequenceView::getActiveSequenceViewWindow(GUITestOpStatus& os) {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(os, AnnotatedDNAViewFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSequenceViewWindowIsActive"
void GTUtilsSequenceView::checkSequenceViewWindowIsActive(GUITestOpStatus& os) {
    getActiveSequenceViewWindow(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoSequenceViewWindowIsOpened"
void GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(GUITestOpStatus& os) {
    GTUtilsMdi::checkNoObjectViewWindowIsOpened(os, AnnotatedDNAViewFactory::ID);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceAsString"
void GTUtilsSequenceView::getSequenceAsString(HI::GUITestOpStatus& os, QString& sequence) {
    QWidget* sequenceWidget = getPanOrDetView(os);
    GTWidget::click(os, sequenceWidget);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os));
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsDialog::waitForDialog(os, new GTSequenceReader(os, &sequence));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, sequenceWidget);
    GTUtilsDialog::checkNoActiveWaiters(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceAsString"
QString GTUtilsSequenceView::getSequenceAsString(HI::GUITestOpStatus& os, int number) {
    getActiveSequenceViewWindow(os);
    GTWidget::click(os, getSeqWidgetByNumber(os, number));

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os));
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(500);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "Copy sequence"}));
    // Use PanView or DetView but not the sequence widget itself: there are internal scrollbars in the SequenceWidget that may affect popup menu content.
    QWidget* panOrDetView = getDetViewByNumber(os, number, {false});
    if (panOrDetView == nullptr) {
        panOrDetView = getPanViewByNumber(os, number);
    }
    GTWidget::click(os, panOrDetView, Qt::RightButton);
    QString result = GTClipboard::text(os);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getBeginOfSequenceAsString"

QString GTUtilsSequenceView::getBeginOfSequenceAsString(HI::GUITestOpStatus& os, int length) {
    checkSequenceViewWindowIsActive(os);
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, length));
    GTKeyboardUtils::selectAll();
    GTThread::waitForMainThread();

    QString sequence;
    GTUtilsDialog::waitForDialog(os, new GTSequenceReader(os, &sequence));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));
    openPopupMenuOnSequenceViewArea(os);
    GTUtilsDialog::checkNoActiveWaiters(os);

    return sequence;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEndOfSequenceAsString"
QString GTUtilsSequenceView::getEndOfSequenceAsString(HI::GUITestOpStatus& os, int length) {
    QWidget* mdiWindow = getActiveSequenceViewWindow(os);
    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    Runnable* filler = new SelectSequenceRegionDialogFiller(os, length, false);
    GTUtilsDialog::waitForDialog(os, filler);

    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);  // don't touch

    QString sequence;
    GTUtilsDialog::waitForDialog(os, new GTSequenceReader(os, &sequence));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseKey));

    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);

    return sequence;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLengthOfSequence"
int GTUtilsSequenceView::getLengthOfSequence(HI::GUITestOpStatus& os) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != nullptr, "MainWindow == NULL", 0);

    MWMDIWindow* mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK_RESULT(mdiWindow != nullptr, "MDI window == NULL", 0);

    GTGlobals::sleep();

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    int length = -1;
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, &length));
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(1000);

    return length;
}
#undef GT_METHOD_NAME

int GTUtilsSequenceView::getVisibleStart(HI::GUITestOpStatus& os, int widgetNumber) {
    return getSeqWidgetByNumber(os, widgetNumber)->getDetView()->getVisibleRange().startPos;
}

#define GT_METHOD_NAME "getVisibleRange"
U2Region GTUtilsSequenceView::getVisibleRange(HI::GUITestOpStatus& os, int widgetNumber) {
    ADVSingleSequenceWidget* seqWgt = getSeqWidgetByNumber(os, widgetNumber);
    GT_CHECK_RESULT(seqWgt != nullptr, "Cannot find sequence view", U2Region());
    return seqWgt->getDetView()->getVisibleRange();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSequence"
void GTUtilsSequenceView::checkSequence(HI::GUITestOpStatus& os, const QString& expectedSequence) {
    QString actualSequence;
    getSequenceAsString(os, actualSequence);

    GT_CHECK(expectedSequence == actualSequence, "Actual sequence does not match with expected sequence");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSequenceRegion"
void GTUtilsSequenceView::selectSequenceRegion(HI::GUITestOpStatus& os, int from, int to) {
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, from, to));
    clickMouseOnTheSafeSequenceViewArea(os);
    GTKeyboardUtils::selectAll();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSeveralRegionsByDialog"
void GTUtilsSequenceView::selectSeveralRegionsByDialog(HI::GUITestOpStatus& os, const QString& multipleRangeString) {
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, multipleRangeString));
    clickMouseOnTheSafeSequenceViewArea(os);
    GTKeyboardUtils::selectAll();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSequenceView"
void GTUtilsSequenceView::openSequenceView(HI::GUITestOpStatus& os, const QString& sequenceName) {
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Open View", "action_open_view"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addSequenceView"
void GTUtilsSequenceView::addSequenceView(HI::GUITestOpStatus& os, const QString& sequenceName) {
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"submenu_add_view", "action_add_view"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "goToPosition"
void GTUtilsSequenceView::goToPosition(HI::GUITestOpStatus& os, qint64 position) {
    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(nullptr != toolbar, "Can't find the toolbar");

    GTLineEdit::setText(os, "go_to_pos_line_edit", QString::number(position), toolbar);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMouseOnTheSafeSequenceViewArea"
void GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea(HI::GUITestOpStatus& os) {
    QWidget* panOrDetView = getPanOrDetView(os);
    GT_CHECK(panOrDetView != nullptr, "No pan or det-view found!");
    GTMouseDriver::moveTo(panOrDetView->mapToGlobal(panOrDetView->rect().center()));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMouseOnTheSafeSequenceViewArea"
void GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(HI::GUITestOpStatus& os, int number) {
    QWidget* panOrDetView = getPanOrDetView(os, number);
    GT_CHECK(panOrDetView != nullptr, "No pan or det-view found!");
    GTWidget::click(os, panOrDetView, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPanOrDetView"
QWidget* GTUtilsSequenceView::getPanOrDetView(HI::GUITestOpStatus& os, int number) {
    QWidget* panOrDetView = getDetViewByNumber(os, number, {false});
    if (panOrDetView == nullptr) {
        panOrDetView = getPanViewByNumber(os, number);
    }
    return panOrDetView;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqWidgetByNumber"
ADVSingleSequenceWidget* GTUtilsSequenceView::getSeqWidgetByNumber(HI::GUITestOpStatus& os, int number, const GTGlobals::FindOptions& options) {
    auto widget = GTWidget::findWidget(os,
                                       QString("ADV_single_sequence_widget_%1").arg(number),
                                       getActiveSequenceViewWindow(os),
                                       options);

    ADVSingleSequenceWidget* seqWidget = qobject_cast<ADVSingleSequenceWidget*>(widget);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(widget != nullptr, QString("Sequence widget %1 was not found!").arg(number), nullptr);
    }

    return seqWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDetViewByNumber"
DetView* GTUtilsSequenceView::getDetViewByNumber(HI::GUITestOpStatus& os, int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    DetView* result = seq->findChild<DetView*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(result != nullptr, QString("det view with number %1 not found").arg(number), nullptr);
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPanViewByNumber"
PanView* GTUtilsSequenceView::getPanViewByNumber(HI::GUITestOpStatus& os, int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    PanView* result = seq->findChild<PanView*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("pan view with number %1 not found").arg(number), nullptr)
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOverViewByNumber"
Overview* GTUtilsSequenceView::getOverviewByNumber(HI::GUITestOpStatus& os, int number, const GTGlobals::FindOptions& options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("sequence view with num %1 not found").arg(number), nullptr);
    } else if (seq == nullptr) {
        return nullptr;
    }

    Overview* result = seq->findChild<Overview*>();
    CHECK(result->isVisible(), nullptr);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(seq != nullptr, QString("pan view with number %1 not found").arg(number), nullptr)
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqWidgetsNumber"
int GTUtilsSequenceView::getSeqWidgetsNumber(HI::GUITestOpStatus& os) {
    QList<ADVSingleSequenceWidget*> seqWidgets = getActiveSequenceViewWindow(os)->findChildren<ADVSingleSequenceWidget*>();
    return seqWidgets.size();
}
#undef GT_METHOD_NAME

QVector<U2Region> GTUtilsSequenceView::getSelection(HI::GUITestOpStatus& os, int number) {
    PanView* panView = getPanViewByNumber(os, number);
    QVector<U2Region> result = panView->getSequenceContext()->getSequenceSelection()->getSelectedRegions();
    return result;
}

#define GT_METHOD_NAME "getSeqName"
QString GTUtilsSequenceView::getSeqName(HI::GUITestOpStatus& os, int number) {
    return getSeqName(os, getSeqWidgetByNumber(os, number));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqName"
QString GTUtilsSequenceView::getSeqName(HI::GUITestOpStatus& os, ADVSingleSequenceWidget* seqWidget) {
    GT_CHECK_RESULT(nullptr != seqWidget, "Sequence widget is NULL!", "");
    auto nameLabel = GTWidget::findLabel(os, "nameLabel", seqWidget);

    QString labelText = nameLabel->text();
    QString result = labelText.left(labelText.indexOf("[") - 1);  // detachment of name from label text
    return result;
}
#undef GT_METHOD_NAME

#define MIN_ANNOTATION_WIDTH 5

#define GT_METHOD_NAME "moveMouseToAnnotationInDetView"
void GTUtilsSequenceView::moveMouseToAnnotationInDetView(HI::GUITestOpStatus& os,
                                                         const QString& annotationName,
                                                         int annotationRegionStartPos,
                                                         int sequenceWidgetIndex) {
    ADVSingleSequenceWidget* sequenceView = getSeqWidgetByNumber(os, sequenceWidgetIndex);
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
    GT_CHECK(selectedAnnotationList.size() != 0, QString("Annotation with annotationName %1 and startPos %2").arg(annotationName).arg(annotationRegionStartPos));
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
        goToPosition(os, center);
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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickAnnotationDet"
void GTUtilsSequenceView::clickAnnotationDet(HI::GUITestOpStatus& os,
                                             const QString& annotationName,
                                             int annotationRegionStartPos,
                                             int sequenceWidgetIndex,
                                             const bool isDoubleClick,
                                             Qt::MouseButton button) {
    moveMouseToAnnotationInDetView(os, annotationName, annotationRegionStartPos, sequenceWidgetIndex);
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click(button);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickAnnotationPan"
void GTUtilsSequenceView::clickAnnotationPan(HI::GUITestOpStatus& os, QString name, int startPos, int number, const bool isDoubleClick, Qt::MouseButton button) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number);
    GSequenceLineViewRenderArea* area = seq->getPanView()->getRenderArea();
    PanViewRenderArea* pan = dynamic_cast<PanViewRenderArea*>(area);
    GT_CHECK(pan != nullptr, "pan view render area not found");

    ADVSequenceObjectContext* context = seq->getSequenceContext();
    context->getAnnotationObjects(true);

    QList<Annotation*> anns;
    foreach (const AnnotationTableObject* ao, context->getAnnotationObjects(true)) {
        foreach (Annotation* a, ao->getAnnotations()) {
            const int sp = a->getLocation().data()->regions.first().startPos;
            const QString annName = a->getName();
            if (sp == startPos - 1 && annName == name) {
                anns << a;
            }
        }
    }
    GT_CHECK(anns.size() != 0, QString("Annotation with name %1 and startPos %2").arg(name).arg(startPos));
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
        goToPosition(os, center);
        GTGlobals::sleep();
    }

    const U2Region visibleLocation = r.intersect(vr);

    U2Region y = pan->getAnnotationYRange(a, 0, as);

    float start = visibleLocation.startPos;
    float end = visibleLocation.endPos();
    float x1f = (float)(start - vr.startPos) * pan->getCurrentScale();
    float x2f = (float)(end - vr.startPos) * pan->getCurrentScale();

    int rw = qMax(MIN_ANNOTATION_WIDTH, qRound(x2f - x1f));
    int x1 = qRound(x1f);

    const QRect annotationRect(x1, y.startPos, rw, y.length);
    GTMouseDriver::moveTo(pan->mapToGlobal(annotationRect.center()));
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click(button);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphView"
GSequenceGraphView* GTUtilsSequenceView::getGraphView(HI::GUITestOpStatus& os) {
    GSequenceGraphView* graph = getSeqWidgetByNumber(os)->findChild<GSequenceGraphView*>();
    GT_CHECK_RESULT(graph != nullptr, "Graph view is NULL", nullptr);
    return graph;
}
#undef GT_METHOD_NAME

QList<QVariant> GTUtilsSequenceView::getLabelPositions(HI::GUITestOpStatus& os, GSequenceGraphView* graph) {
    Q_UNUSED(os);
    QList<QVariant> list;
    graph->getSavedLabelsState(list);
    return list;
}

QList<GraphLabelTextBox*> GTUtilsSequenceView::getGraphLabels(HI::GUITestOpStatus& os, GSequenceGraphView* graph) {
    Q_UNUSED(os);
    QList<GraphLabelTextBox*> result = graph->findChildren<GraphLabelTextBox*>();
    return result;
}

QColor GTUtilsSequenceView::getGraphColor(HI::GUITestOpStatus& /*os*/, GSequenceGraphView* graph) {
    ColorMap map = graph->getGraphDrawer()->getColors();
    QColor result = map.value("Default color");
    return result;
}

#define GT_METHOD_NAME "toggleGraphByName"
void GTUtilsSequenceView::toggleGraphByName(HI::GUITestOpStatus& os, const QString& graphName, int sequenceViewIndex) {
    QWidget* sequenceWidget = getSeqWidgetByNumber(os, sequenceViewIndex);
    auto graphAction = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget, false);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {graphName}));
    GTWidget::click(os, graphAction);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsSequenceView::zoomIn(HI::GUITestOpStatus& os, int sequenceViewIndex) {
    QWidget* sequenceWidget = getSeqWidgetByNumber(os, sequenceViewIndex);
    QAction* zoomInAction = GTAction::findActionByText(os, "Zoom In", sequenceWidget);
    GTWidget::click(os, GTAction::button(os, zoomInAction));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enableEditingMode"
void GTUtilsSequenceView::enableEditingMode(GUITestOpStatus& os, bool enable, int sequenceNumber) {
    DetView* detView = getDetViewByNumber(os, sequenceNumber);

    auto toolbar = GTWidget::findToolBar(os, "WidgetWithLocalToolbar_toolbar", detView);
    auto editButton = qobject_cast<QToolButton*>(GTToolbar::getWidgetForActionObjectName(os, toolbar, "edit_sequence_action"));
    CHECK_SET_ERR(editButton != nullptr, "'edit_sequence_action' button is NULL");
    if (editButton->isChecked() != enable) {
        if (editButton->isVisible()) {
            GTWidget::click(os, editButton);
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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "insertSubsequence"
void GTUtilsSequenceView::insertSubsequence(HI::GUITestOpStatus& os, qint64 offset, const QString& subsequence, bool isDirectStrand) {
    makeDetViewVisible(os);
    enableEditingMode(os, true);
    setCursor(os, offset, isDirectStrand);
    GTKeyboardDriver::keySequence(subsequence);
    enableEditingMode(os, false);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCursor"
void GTUtilsSequenceView::setCursor(GUITestOpStatus& os, qint64 position, bool clickOnDirectLine, bool doubleClick) {
    // Multiline view is not supported correctly.

    DetView* detView = getDetViewByNumber(os, 0);

    DetViewRenderArea* renderArea = detView->getDetViewRenderArea();
    CHECK_SET_ERR(nullptr != renderArea, "DetViewRenderArea is NULL");

    DetViewRenderer* renderer = renderArea->getRenderer();
    CHECK_SET_ERR(nullptr != renderer, "DetViewRenderer is NULL");

    U2Region visibleRange = detView->getVisibleRange();
    if (!visibleRange.contains(position)) {
        GTUtilsSequenceView::goToPosition(os, position);
        GTGlobals::sleep();
        visibleRange = detView->getVisibleRange();
    }
    SAFE_POINT_EXT(visibleRange.contains(position), os.setError("Position is out of visible range"), );

    const double scale = renderer->getCurrentScale();
    const int coord = renderer->posToXCoord(position, renderArea->size(), visibleRange) + (int)(scale / 2);

    bool isWrapMode = detView->isWrapMode();
    if (!isWrapMode) {
        GTMouseDriver::moveTo(renderArea->mapToGlobal(QPoint(coord, 40)));  // TODO: replace the hardcoded value with method in renderer
    } else {
        GTUtilsSequenceView::goToPosition(os, position);
        GTGlobals::sleep();

        const int symbolsPerLine = renderArea->getSymbolsPerLine();
        const int linesCount = renderArea->getLinesCount();
        visibleRange = GTUtilsSequenceView::getVisibleRange(os);
        int linesBeforePos = -1;
        for (int i = 0; i < linesCount; i++) {
            const U2Region line(visibleRange.startPos + i * symbolsPerLine, symbolsPerLine);
            if (line.contains(position)) {
                linesBeforePos = i;
                break;
            }
        }
        SAFE_POINT_EXT(linesBeforePos != -1, os.setError("Position not found"), );

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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCursor"
qint64 GTUtilsSequenceView::getCursor(HI::GUITestOpStatus& os) {
    DetView* detView = getDetViewByNumber(os, 0);

    DetViewSequenceEditor* dwSequenceEditor = detView->getEditor();
    GT_CHECK_RESULT(dwSequenceEditor != nullptr, "DetViewSequenceEditor is NULL", -1);

    const bool isEditMode = detView->isEditMode();
    GT_CHECK_RESULT(isEditMode, "Edit mode is disabled", -1);

    return dwSequenceEditor->getCursorPosition();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRegionAsString"
QString GTUtilsSequenceView::getRegionAsString(HI::GUITestOpStatus& os, const U2Region& region) {
    GTUtilsSequenceView::selectSequenceRegion(os, region.startPos, region.endPos() - 1);
    GTGlobals::sleep();

    GTKeyboardUtils::copy();

    return GTClipboard::text(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOnDetView"
void GTUtilsSequenceView::clickOnDetView(HI::GUITestOpStatus& os) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow == NULL");

    MWMDIWindow* mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != nullptr, "MDI window == NULL");

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "makeDetViewVisible"
void GTUtilsSequenceView::makeDetViewVisible(HI::GUITestOpStatus& os) {
    auto toggleDetViewButton = GTWidget::findToolButton(os, "show_hide_details_view");
    if (!toggleDetViewButton->isChecked()) {
        GTWidget::click(os, toggleDetViewButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "makePanViewVisible"
void GTUtilsSequenceView::makePanViewVisible(HI::GUITestOpStatus& os, bool enable) {
    auto toggleZoomViewButton = GTWidget::findToolButton(os, "show_hide_zoom_view");
    if (toggleZoomViewButton->isChecked() != enable) {
        GTWidget::click(os, toggleZoomViewButton);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
