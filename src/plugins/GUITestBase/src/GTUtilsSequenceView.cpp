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

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>

#include <GTGlobals.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Gui/MainWindow.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/DetView.h>
#include <U2View/DetViewRenderer.h>
#include <U2View/DetViewSequenceEditor.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/GSequenceLineViewAnnotated.h>
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
    GTSequenceReader(HI::GUITestOpStatus &_os, QString *_str):Filler(_os, "EditSequenceDialog"), str(_str){}
    void commonScenario() {
        QWidget *widget = QApplication::activeModalWidget();
        GT_CHECK(widget != NULL, "active widget not found");

        QPlainTextEdit *textEdit = widget->findChild<QPlainTextEdit*>();
        GT_CHECK(textEdit != NULL, "PlainTextEdit not found");

        *str = textEdit->toPlainText();

        QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", widget));
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton* button = box->button(QDialogButtonBox::Cancel);
        GT_CHECK(button !=NULL, "cancel button is NULL");
        GTWidget::click(os, button);
    }

private:
    QString *str;
};
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsSequenceView"

#define GT_METHOD_NAME "getSequenceAsString"
void GTUtilsSequenceView::getSequenceAsString(HI::GUITestOpStatus &os, QString &sequence)
{
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);
    GT_CHECK(mdiWindow != NULL, "MDI window == NULL");

    QWidget *mdiSequenceWidget = mdiWindow->findChild<ADVSingleSequenceWidget*>();
    GTWidget::click(os, mdiSequenceWidget);

    Runnable *filler = new SelectSequenceRegionDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, filler);

    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);

    Runnable *chooser = new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REPLACE_SUBSEQUENCE, GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, chooser);
    Runnable *reader = new GTSequenceReader(os, &sequence);
    GTUtilsDialog::waitForDialog(os, reader);

    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceAsString"
QString GTUtilsSequenceView::getSequenceAsString(HI::GUITestOpStatus &os, int number) {
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);
    GT_CHECK_RESULT(mdiWindow != NULL, "MDI window == NULL", "");

    GTWidget::click(os, getSeqWidgetByNumber(os, number));

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(500);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY << "Copy sequence"));
    GTWidget::click(os, getSeqWidgetByNumber(os, number), Qt::RightButton);
    QString result = GTClipboard::text(os);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getBeginOfSequenceAsString"

QString GTUtilsSequenceView::getBeginOfSequenceAsString(HI::GUITestOpStatus &os, int length)
{
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);
    GT_CHECK_RESULT(mdiWindow != NULL, "MDI window == NULL", NULL);

   // GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center())); commented for test 6232_4
   // GTMouseDriver::click();

    Runnable *filler = new SelectSequenceRegionDialogFiller(os, length);
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);

    GTGlobals::sleep(1000); // don't touch
    QString sequence;
    Runnable *chooser = new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REPLACE_SUBSEQUENCE, GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, chooser);
    Runnable *reader = new GTSequenceReader(os, &sequence);
    GTUtilsDialog::waitForDialog(os, reader);

    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);

    return sequence;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEndOfSequenceAsString"
QString GTUtilsSequenceView::getEndOfSequenceAsString(HI::GUITestOpStatus &os, int length)
{
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);
    GT_CHECK_RESULT(mdiWindow != NULL, "MDI window == NULL", NULL);

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    Runnable *filler = new SelectSequenceRegionDialogFiller(os, length, false);
    GTUtilsDialog::waitForDialog(os, filler);

    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000); // don't touch

    QString sequence;
    Runnable *chooser = new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REPLACE_SUBSEQUENCE, GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, chooser);
    Runnable *reader = new GTSequenceReader(os, &sequence);
    GTUtilsDialog::waitForDialog(os, reader);

    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);

    return sequence;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLengthOfSequence"
int GTUtilsSequenceView::getLengthOfSequence(HI::GUITestOpStatus &os)
{
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != NULL, "MainWindow == NULL", 0);

    MWMDIWindow *mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK_RESULT(mdiWindow != NULL, "MDI window == NULL", 0);

    GTGlobals::sleep();

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    int length = -1;
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, &length));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);

    return length;
}
#undef GT_METHOD_NAME

int GTUtilsSequenceView::getVisiableStart(HI::GUITestOpStatus &os, int widgetNumber){
    return getSeqWidgetByNumber(os, widgetNumber)->getDetView()->getVisibleRange().startPos;
}

#define GT_METHOD_NAME "getVisibleRange"
U2Region GTUtilsSequenceView::getVisibleRange(HI::GUITestOpStatus &os, int widgetNumber) {
    ADVSingleSequenceWidget* seqWgt = getSeqWidgetByNumber(os, widgetNumber);
    GT_CHECK_RESULT(seqWgt != NULL, "Cannot find sequence view", U2Region());
    return seqWgt->getDetView()->getVisibleRange();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSequence"
void GTUtilsSequenceView::checkSequence(HI::GUITestOpStatus &os, const QString &expectedSequence)
{
    QString actualSequence;
    getSequenceAsString(os, actualSequence);

    GT_CHECK(expectedSequence == actualSequence, "Actual sequence does not match with expected sequence");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSequenceRegion"
void GTUtilsSequenceView::selectSequenceRegion(HI::GUITestOpStatus &os, int from, int to)
{
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    MWMDIWindow *mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != NULL, "MDI window == NULL");

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, from, to));

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSeveralRegionsByDialog"
void GTUtilsSequenceView::selectSeveralRegionsByDialog(HI::GUITestOpStatus &os, const QString multipleRangeString) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    MWMDIWindow *mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != NULL, "MDI window == NULL");

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, multipleRangeString));

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSequenceView"
void GTUtilsSequenceView::openSequenceView(HI::GUITestOpStatus &os, const QString &sequenceName){
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Open View" << "action_open_view", GTGlobals::UseMouse));

    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addSequenceView"
void GTUtilsSequenceView::addSequenceView(HI::GUITestOpStatus &os, const QString &sequenceName){
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "submenu_add_view" << "action_add_view", GTGlobals::UseMouse));

    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, sequenceName);
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "goToPosition"
void GTUtilsSequenceView::goToPosition(HI::GUITestOpStatus &os, int position) {
    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(NULL != toolbar, "Can't find the toolbar");

    QLineEdit* positionLineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "go_to_pos_line_edit", toolbar);
    GT_CHECK(NULL != positionLineEdit, "Can't find the position line edit");

    GTLineEdit::setText(os, positionLineEdit, QString::number(position));
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqWidgetByNumber"
ADVSingleSequenceWidget* GTUtilsSequenceView::getSeqWidgetByNumber(HI::GUITestOpStatus &os, int number, const GTGlobals::FindOptions &options){
    QWidget *widget = GTWidget::findWidget(os,
        QString("ADV_single_sequence_widget_%1").arg(number),
        GTUtilsMdi::activeWindow(os), options);

    ADVSingleSequenceWidget *seqWidget = qobject_cast<ADVSingleSequenceWidget*>(widget);

    if(options.failIfNotFound){
        GT_CHECK_RESULT(NULL != widget, QString("Sequence widget %1 was not found!").arg(number), NULL);
    }

    return seqWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDetViewByNumber"
DetView* GTUtilsSequenceView::getDetViewByNumber(HI::GUITestOpStatus &os, int number, const GTGlobals::FindOptions &options) {
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if (options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("sequence view with num %1 not found").arg(number), NULL);
    } else {
        return NULL;
    }

    DetView* result = seq->findChild<DetView*>();
    if (options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("det view with number %1 not found").arg(number), NULL)
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPanViewByNumber"
PanView* GTUtilsSequenceView::getPanViewByNumber(HI::GUITestOpStatus &os, int number, const GTGlobals::FindOptions &options){
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if(options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("sequence view with num %1 not found").arg(number), NULL);
    }else {
        return NULL;
    }

    PanView* result = seq->findChild<PanView*>();
    if(options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("pan view with number %1 not found").arg(number), NULL)
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOverViewByNumber"
Overview* GTUtilsSequenceView::getOverviewByNumber(HI::GUITestOpStatus &os, int number, const GTGlobals::FindOptions &options){
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number, options);
    if(options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("sequence view with num %1 not found").arg(number), NULL);
    }else {
        return NULL;
    }

    Overview* result = seq->findChild<Overview*>();
    if(options.failIfNotFound){
        GT_CHECK_RESULT(seq != NULL, QString("pan view with number %1 not found").arg(number), NULL)
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqWidgetsNumber"
int GTUtilsSequenceView::getSeqWidgetsNumber(HI::GUITestOpStatus &os) {
    QList<ADVSingleSequenceWidget*> seqWidgets = GTUtilsMdi::activeWindow(os)->findChildren<ADVSingleSequenceWidget*>();
    return seqWidgets.size();
}
#undef GT_METHOD_NAME

QVector<U2Region> GTUtilsSequenceView::getSelection(HI::GUITestOpStatus &os, int number){
    PanView* panView = getPanViewByNumber(os, number);
    QVector<U2Region> result = panView->getSequenceContext()->getSequenceSelection()->getSelectedRegions();
    return result;
}

#define GT_METHOD_NAME "getSeqName"
QString GTUtilsSequenceView::getSeqName(HI::GUITestOpStatus &os, int number) {
    return getSeqName(os, getSeqWidgetByNumber(os, number));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqName"
QString GTUtilsSequenceView::getSeqName(HI::GUITestOpStatus &os, ADVSingleSequenceWidget* seqWidget){
    GT_CHECK_RESULT(NULL != seqWidget, "Sequence widget is NULL!", "");
    QLabel *nameLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "nameLabel", seqWidget));
    GT_CHECK_RESULT(NULL != nameLabel, "Name label is NULL!", "");

    QString labelText = nameLabel->text();
    QString result = labelText.left(labelText.indexOf("[")-1);//detachment of name from label text
    return result;
}
#undef GT_METHOD_NAME

#define MIN_ANNOTATION_WIDTH 5

#define GT_METHOD_NAME "clickAnnotationDet"
void GTUtilsSequenceView::clickAnnotationDet(HI::GUITestOpStatus &os, QString name, int startpos, int number, const bool isDoubleClick, Qt::MouseButton button){
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number);
    GSequenceLineViewRenderArea* area = seq->getDetView()->getRenderArea();
    DetViewRenderArea* det = dynamic_cast<DetViewRenderArea*>(area);
    GT_CHECK(det != NULL, "det view render area not found");

    ADVSequenceObjectContext* context = seq->getSequenceContext();
    context->getAnnotationObjects(true);

    QList<Annotation*> anns;
    foreach (const AnnotationTableObject *ao, context->getAnnotationObjects(true)) {
        foreach (Annotation *a, ao->getAnnotations()) {
            foreach (const U2Region& r, a->getLocation().data()->regions) {
                if (a->getName() == name && r.startPos == startpos - 1) {
                    anns << a;
                }
            }
        }
    }
    GT_CHECK(anns.size() != 0, QString("Annotation with name %1 and startPos %2").arg(name).arg(startpos));
    GT_CHECK(anns.size() == 1, QString("Several annotation with name %1 and startPos %2. Number is: %3").arg(name).arg(startpos).arg(anns.size()));

    Annotation* a = anns.first();

    const SharedAnnotationData &aData = a->getData();
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = asr->getAnnotationSettings(aData);


    const U2Region &visibleRange = seq->getDetView()->getVisibleRange();
    QVector <U2Region> regions = a->getLocation().data()->regions;
    U2Region annotationRegion;
    int regionId = 0;
    foreach (const U2Region& reg, regions) {
        if (reg.startPos == startpos - 1) {
            annotationRegion = reg;
            break;
        }
        regionId++;
    }
    GT_CHECK(!annotationRegion.isEmpty(), "Region not found");

    if (!annotationRegion.intersects(visibleRange)) {
        int center = annotationRegion.center();
        goToPosition(os, center);
        GTGlobals::sleep();
    }

    const U2Region visibleRegionPart = annotationRegion.intersect(visibleRange);

    U2Region y;
    y = det->getAnnotationYRange(a, regionId, as);

    float visibleRegionPartStart = visibleRegionPart.startPos;
    float visibleRegionPartEnd = visibleRegionPart.endPos();
    if (seq->getDetView()->isWrapMode() && annotationRegion.endPos() > visibleRange.endPos()) {
        visibleRegionPartEnd = visibleRegionPart.startPos + seq->getDetView()->getSymbolsPerLine();
    }
    float x1f = (float)(visibleRegionPartStart - visibleRange.startPos) * det->getCharWidth();
    float x2f = (float)(visibleRegionPartEnd - visibleRange.startPos) * det->getCharWidth();

    int rw = qMax(MIN_ANNOTATION_WIDTH, qRound(x2f - x1f));
    int x1 = qRound(x1f);

    const QRect annotationRect(x1, y.startPos, rw, y.length);
    GTMouseDriver::moveTo(det->mapToGlobal(annotationRect.center()));
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click(button);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickAnnotationPan"
void GTUtilsSequenceView::clickAnnotationPan(HI::GUITestOpStatus &os, QString name, int startpos, int number, const bool isDoubleClick, Qt::MouseButton button){
    ADVSingleSequenceWidget* seq = getSeqWidgetByNumber(os, number);
    GSequenceLineViewRenderArea* area = seq->getPanView()->getRenderArea();
    PanViewRenderArea* pan = dynamic_cast<PanViewRenderArea*>(area);
    GT_CHECK(pan != NULL, "pan view render area not found");

    ADVSequenceObjectContext* context = seq->getSequenceContext();
    context->getAnnotationObjects(true);

    QList<Annotation*> anns;
    foreach(const AnnotationTableObject *ao, context->getAnnotationObjects(true)) {
        foreach(Annotation *a, ao->getAnnotations()) {
            const int sp = a->getLocation().data()->regions.first().startPos;
            const QString annName = a->getName();
            if (sp == startpos - 1 && annName == name){
                anns << a;
            }
        }
    }
    GT_CHECK(anns.size() != 0, QString("Annotation with name %1 and startPos %2").arg(name).arg(startpos));
    GT_CHECK(anns.size() == 1, QString("Several annotation with name %1 and startPos %2. Number is: %3").arg(name).arg(startpos).arg(anns.size()));

    Annotation* a = anns.first();

    const SharedAnnotationData &aData = a->getData();
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = asr->getAnnotationSettings(aData);


    const U2Region &vr = seq->getPanView()->getVisibleRange();
    QVector <U2Region> regions = a->getLocation().data()->regions;
    const U2Region &r = regions.first();

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
GSequenceGraphView *GTUtilsSequenceView::getGraphView(HI::GUITestOpStatus &os){
    GSequenceGraphView* graph = getSeqWidgetByNumber(os)->findChild<GSequenceGraphView*>();
    GT_CHECK_RESULT(graph != NULL, "Graph view is NULL", NULL);
    return graph;
}
#undef GT_METHOD_NAME

QList<QVariant> GTUtilsSequenceView::getLabelPositions(HI::GUITestOpStatus &os, GSequenceGraphView *graph){
    Q_UNUSED(os);
    QList<QVariant> list;
    graph->getLabelPositions(list);
    return list;
}

QList<TextLabel *> GTUtilsSequenceView::getGraphLabels(HI::GUITestOpStatus &os, GSequenceGraphView *graph){
    Q_UNUSED(os);
    QList<TextLabel*> result = graph->findChildren<TextLabel*>();
    return result;
}

QColor GTUtilsSequenceView::getGraphColor(HI::GUITestOpStatus & /*os*/, GSequenceGraphView *graph){
    ColorMap map = graph->getGSequenceGraphDrawer()->getColors();
    QColor result = map.value("Default color");
    return result;
}

#define GT_METHOD_NAME "enableEditingMode"
void GTUtilsSequenceView::enableEditingMode(GUITestOpStatus &os, bool enable, int sequenceNumber) {
    DetView *detView = getDetViewByNumber(os, sequenceNumber);
    CHECK_SET_ERR(NULL != detView, "DetView is NULL");

    QToolButton *editButton = qobject_cast<QToolButton *>(GTToolbar::getWidgetForActionTooltip(os, GTWidget::findExactWidget<QToolBar *>(os, "", detView), "Edit sequence"));
    CHECK_SET_ERR(NULL != editButton, "'Edit sequence' button is NULL");
    if (editButton->isChecked() != enable) {
        if (editButton->isVisible()) {
            GTWidget::click(os, editButton);
        } else {
            const QPoint gp = detView->mapToGlobal(QPoint(10, detView->rect().height() - 5));
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

#define GT_METHOD_NAME "setCursor"
void GTUtilsSequenceView::setCursor(GUITestOpStatus &os, qint64 position, bool clickOnDirectLine, bool doubleClick) {
    // Multiline view is no supported correctly

    DetView *detView = getDetViewByNumber(os, 0);
    CHECK_SET_ERR(NULL != detView, "DetView is NULL");

    DetViewRenderArea* renderArea = detView->getDetViewRenderArea();
    CHECK_SET_ERR(NULL != renderArea, "DetViewRenderArea is NULL");

    DetViewRenderer* renderer = renderArea->getRenderer();
    CHECK_SET_ERR(NULL != renderer, "DetViewRenderer is NULL");

    U2Region visibleRange = detView->getVisibleRange();
    if (!visibleRange.contains(position)) {
        GTUtilsSequenceView::goToPosition(os, position);
        GTGlobals::sleep();
        visibleRange = detView->getVisibleRange();
    }
    SAFE_POINT_EXT(visibleRange.contains(position), os.setError("Position is out of visible range"), );

    const double scale = renderer->getCurrentScale();
    const int coord = renderer->posToXCoord(position, renderArea->size(), visibleRange) + (int)(scale / 2);

    const bool wrapMode = detView->isWrapMode();
    if (!wrapMode) {
        GTMouseDriver::moveTo(renderArea->mapToGlobal(QPoint(coord, 40)));    // TODO: replace the hardcoded value with method in renderer
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
        int middleShift = (int)(shiftsCount / 2) + 1;     //TODO: this calculation might consider the case then complementary is turned off or translations are drawn
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
qint64 GTUtilsSequenceView::getCursor(HI::GUITestOpStatus &os) {
    DetView *detView = getDetViewByNumber(os, 0);
    GT_CHECK_RESULT(NULL != detView, "DetView is NULL", -1);

    DetViewSequenceEditor* dwSequenceEditor = detView->getEditor();
    GT_CHECK_RESULT(dwSequenceEditor != NULL, "DetViewSequenceEditor is NULL", -1);

    const bool isEditMode = detView->isEditMode();
    GT_CHECK_RESULT(isEditMode, "Edit mode is disabled", -1);

    const qint64 result = dwSequenceEditor->getCursorPosition();

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRegionAsString"
QString GTUtilsSequenceView::getRegionAsString(HI::GUITestOpStatus &os, const U2Region& region) {
    GTUtilsSequenceView::selectSequenceRegion(os, region.startPos, region.endPos() - 1);
    GTGlobals::sleep();

    GTKeyboardUtils::copy(os);

    const QString result = GTClipboard::text(os);

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOnDetView"
void GTUtilsSequenceView::clickOnDetView(HI::GUITestOpStatus &os) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    MWMDIWindow *mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != NULL, "MDI window == NULL");

    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click();

    GTGlobals::sleep(500);
}
#undef MIN_ANNOTATION_WIDTH

#undef GT_CLASS_NAME

} // namespace U2
