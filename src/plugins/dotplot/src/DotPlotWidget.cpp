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

#include <math.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolTip>

#include <U2Algorithm/RepeatFinderSettings.h>
#include <U2Algorithm/RepeatFinderTaskFactory.h>
#include <U2Algorithm/RepeatFinderTaskFactoryRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/AppResources.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/GraphUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/PanView.h>

#include "DotPlotDialog.h"
#include "DotPlotFilterDialog.h"
#include "DotPlotImageExportTask.h"
#include "DotPlotTasks.h"
#include "DotPlotWidget.h"

namespace U2 {

DotPlotWidget::DotPlotWidget(AnnotatedDNAView* dnaView)
    :ADVSplitWidget(dnaView),
    selecting(false),shifting(false),miniMapLooking(false), selActive(true), nearestSelecting(false),
    selectionX(NULL),selectionY(NULL),sequenceX(NULL),sequenceY(NULL), direct(true), inverted(false), nearestInverted(false), ignorePanView(false), keepAspectRatio(false),
    zoom(1.0f, 1.0f), shiftX(0), shiftY(0),
    minLen(100), identity(100),
    pixMapUpdateNeeded(true), deleteDotPlotFlag(false), filtration(false), dotPlotIsCalculating(false), dotPlotTask(NULL), pixMap(NULL), miniMap(NULL),
    nearestRepeat(NULL),
    clearedByRepitSel(false)
{
    textSpace = w = h = 0; // values are used and will be initialized in drawAll.

    dpDirectResultListener = new DotPlotResultsListener();
    dpRevComplResultsListener = new DotPlotRevComplResultsListener();
    dpFilteredResults = QSharedPointer<QList<DotPlotResults> >( new QList<DotPlotResults>() );
    dpFilteredResultsRevCompl = QSharedPointer<QList<DotPlotResults> >( new QList<DotPlotResults>() );
    foreach(DotPlotResults dpR, *dpDirectResultListener->dotPlotList){
        dpFilteredResults->append(dpR);
    }

    foreach(DotPlotResults dpR, *dpRevComplResultsListener->dotPlotList){
        dpFilteredResultsRevCompl->append(dpR);
    }


    SAFE_POINT(dnaView, "dnaView is NULL", );
    this->dnaView = dnaView;

    initActionsAndSignals();

    dotPlotBGColor = QColor(240, 240, 255);
    dotPlotNearestRepeatColor = QColor(240, 0, 0);

    setFocusPolicy(Qt::WheelFocus);

    timer = new QTimer(this);
    timer->setInterval(2000);
    connect(timer, SIGNAL(timeout()), this, SLOT(sl_timer()));

    exitButton = new QToolButton(this);
    connect(exitButton, SIGNAL(clicked()), SLOT(sl_showDeleteDialog()));
    exitButton->setToolTip("Close");
    QIcon exitIcon = QIcon(QString(":dotplot/images/exit.png"));
    exitButton->setIcon(exitIcon);

    exitButton->setAutoFillBackground(true);
    exitButton->setAutoRaise(true);
    exitButton->setObjectName("exitButton");

    this->setObjectName("dotplot widget");
}

// init menu items, actions and connect signals
void DotPlotWidget::initActionsAndSignals() {

    showSettingsDialogAction = new QAction(tr("Parameters"), this);
    connect(showSettingsDialogAction, SIGNAL(triggered()), SLOT(sl_showSettingsDialog()));

    saveImageAction = new QAction(tr("Save as image"), this);
    saveImageAction->setObjectName("Save as image");
    connect(saveImageAction, SIGNAL(triggered()), SLOT(sl_showSaveImageDialog()));

    saveDotPlotAction = new QAction(tr("Save"), this);
    saveDotPlotAction->setObjectName("Save");
    connect(saveDotPlotAction, SIGNAL(triggered()), SLOT(sl_showSaveFileDialog()));

    loadDotPlotAction = new QAction(tr("Load"), this);
    connect(loadDotPlotAction, SIGNAL(triggered()), SLOT(sl_showLoadFileDialog()));

    deleteDotPlotAction = new QAction(tr("Remove"), this);
    deleteDotPlotAction->setObjectName("Remove");
    connect(deleteDotPlotAction, SIGNAL(triggered()), SLOT(sl_showDeleteDialog()));

    filterDotPlotAction = new QAction(tr("Filter Results"), this);
    connect(filterDotPlotAction, SIGNAL(triggered()), SLOT(sl_filter()));

    foreach (ADVSequenceWidget *advSeqWidget, dnaView->getSequenceWidgets()) {
        ADVSingleSequenceWidget *ssw = qobject_cast<ADVSingleSequenceWidget*>(advSeqWidget);
        if (ssw != NULL) {
            connect (ssw->getPanView(), SIGNAL(si_visibleRangeChanged()), SLOT(sl_panViewChanged()));
        }
    }

    setMouseTracking(true);
}

// connect signals to know if user clicks on dotplot sequences
void DotPlotWidget::connectSequenceSelectionSignals() {

    if (!(sequenceX && sequenceY)) {
        return;
    }

    SAFE_POINT(dnaView, "dnaView is NULL", );
    connect (dnaView, SIGNAL(si_sequenceWidgetRemoved(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetRemoved(ADVSequenceWidget*)));

    foreach (ADVSequenceObjectContext* ctx, dnaView->getSequenceContexts()) {

        SAFE_POINT(ctx, "ctx is NULL", );

        connect(ctx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>& , const QVector<U2Region>&)));
    }
}

DotPlotWidget::~DotPlotWidget() {

    if (dotPlotTask) {
        cancelRepeatFinderTask();
    }

    delete timer;
    delete showSettingsDialogAction;
    delete saveImageAction;
    delete saveDotPlotAction;
    delete loadDotPlotAction;
    delete deleteDotPlotAction;
    delete filterDotPlotAction;
    delete pixMap;

    delete dpDirectResultListener;
    delete dpRevComplResultsListener;
    dpFilteredResults.clear();
    dpFilteredResultsRevCompl.clear();
}

bool DotPlotWidget::onCloseEvent() {

    if (dotPlotTask && !deleteDotPlotFlag) {
        cancelRepeatFinderTask();
        return false;
    }

    return true;
}

// called by DotPlotSplitter
void DotPlotWidget::buildPopupMenu(QMenu *m) const {
    QPoint mapped(mapFromGlobal(QCursor::pos()));

    // build menu only if the mouse cursor is over this dotplot
    if (sequenceX && sequenceY && QRect(0, 0, width(), height()).contains(mapped)) {
        assert(!m->actions().isEmpty());

        QMenu* dotPlotMenu = new QMenu(tr("Dotplot"), m);
        dotPlotMenu->menuAction()->setObjectName("Dotplot");
        QMenu* saveMenu = new QMenu(tr("Save/Load"), dotPlotMenu);
        saveMenu->menuAction()->setObjectName("Save/Load");

        saveMenu->addAction(saveImageAction);
        saveMenu->addAction(saveDotPlotAction);
        saveMenu->addAction(loadDotPlotAction);

        dotPlotMenu->setIcon(QIcon(":dotplot/images/dotplot.png"));
        dotPlotMenu->addAction(filterDotPlotAction);
        dotPlotMenu->addAction(showSettingsDialogAction);
        dotPlotMenu->addMenu(saveMenu);
        dotPlotMenu->addAction(deleteDotPlotAction);

        QAction *b = *(m->actions().begin());
        m->insertMenu(b, dotPlotMenu);
        m->setObjectName("dotplot context menu");
    }
}

QPointF DotPlotWidget::zoomTo(Qt::Axis axis, const U2Region &lr, bool emitSignal) {

    if(lr.length == 0){
        return zoom;
    }

    int seqLen = 0;
    switch (axis) {
        case Qt::XAxis :
            seqLen = sequenceX->getSequenceLength();

            zoom.setX(seqLen/(float)lr.length);
            if (zoom.x() > seqLen/2) {
                zoom.setX(seqLen/2);
            }

            shiftX = -lr.startPos*w/(float)seqLen;
            shiftX*=zoom.x();
            break;

        case Qt::YAxis :
            seqLen = sequenceY->getSequenceLength();

            zoom.setY(seqLen/(float)lr.length);
            if (zoom.y() > seqLen/2) {
                zoom.setY(seqLen/2);
            }

            shiftY = -lr.startPos*h/(float)seqLen;
            shiftY*=zoom.y();
            break;

        default:
            return zoom;
    }

    checkShift(emitSignal);
    pixMapUpdateNeeded = true;
    update();

    return zoom;
}

U2Region DotPlotWidget::getVisibleRange(Qt::Axis axis) {

    QPointF zeroPoint(0, 0);
    QPointF rightPoint(w, 0);
    QPointF topPoint(0, h);

    int startPos = 0;
    int len = 0;

    switch (axis) {
        case Qt::XAxis:
            startPos = sequenceCoords(unshiftedUnzoomed(zeroPoint)).x();
            len = sequenceCoords(unshiftedUnzoomed(rightPoint)).x();
            break;

        case Qt::YAxis:
            startPos = sequenceCoords(unshiftedUnzoomed(zeroPoint)).y();
            len = sequenceCoords(unshiftedUnzoomed(topPoint)).y();
            break;

        default:
            return U2Region();
    }

    len-=startPos;
    if (0 == len) { // to avoid division by zero
        len = 1;
    }

    return U2Region(startPos, len);
}

int DotPlotWidget::getLrDifference(const U2Region &a, const U2Region &b) {

    return qAbs(a.startPos - b.startPos) + qAbs(a.length - b.length);
}

void DotPlotWidget::sl_panViewChanged() {

    GSequenceLineView *lw = qobject_cast<GSequenceLineView*>(sender());
    PanView *panView = qobject_cast<PanView*>(sender());

    if (selecting || shifting || !(lw && panView) || nearestSelecting) {
        return;
    }

    U2Region lr = panView->getVisibleRange();
    SequenceObjectContext* ctx = lw->getSequenceContext();

    if (!ctx || ignorePanView) {
        return;
    }

    U2Region xAxisVisibleRange = getVisibleRange(Qt::XAxis);
    if ((ctx == sequenceX) && (lr != xAxisVisibleRange)) {
        zoomTo(Qt::XAxis, lr);
    }

    if (!shifting) {
        U2Region yAxisVisibleRange = getVisibleRange(Qt::YAxis);
        if ((ctx == sequenceY) && (lr != yAxisVisibleRange)) {
            if(sequenceX == sequenceY){
                zoomTo(Qt::XAxis, lr, sequenceX != sequenceY);
            }else{
                zoomTo(Qt::YAxis, lr, sequenceX != sequenceY);
            }
        }
    }
}

void DotPlotWidget::sl_timer(){
    if (hasFocus() && selActive) {
        QPoint pos = clickedSecond.toPoint();
        pos = sequenceCoords(unshiftedUnzoomed(pos));
        const DotPlotResults *res = findNearestRepeat(pos);
        if(res == nearestRepeat){
            QString text = makeToolTipText();
            QFont dFont, f("Monospace");
            f.setPointSize(dFont.pointSize());
            f.setStyleHint(QFont::Courier);
            QToolTip::setFont(f);
            QToolTip::showText(QCursor::pos(), text);
        }
    }
    timer->stop();
}

void DotPlotWidget::sl_taskStateChanged() {
    if (!dotPlotTask || dotPlotTask->getState() != Task::State_Finished) {
        return;
    }
    dotPlotTask = NULL;

    if (deleteDotPlotFlag) {
        emit si_removeDotPlot();
    }
}

void DotPlotWidget::sl_filteringTaskStateChanged() {
    if (!dotPlotTask || dotPlotTask->getState() != Task::State_Finished) {
        return;
    }
    dotPlotTask = NULL;

    // build dotplot task finished
    pixMapUpdateNeeded = true;
    update();
}

void DotPlotWidget::sl_buildDotplotTaskStateChanged() {

    if (!dotPlotTask || dotPlotTask->getState() != Task::State_Finished) {
        return;
    }

    GCOUNTER(c, t, "Create dotplot");
    dpFilteredResults->clear();
    dpFilteredResultsRevCompl->clear();

    if (!dpDirectResultListener->stateOk || !dpRevComplResultsListener->stateOk) {
        QMessageBox::critical(this, tr("Too many results"), tr("Too many results. Try to increase minimum repeat length"));
    }
    foreach(DotPlotResults dpR, *dpDirectResultListener->dotPlotList){
        dpFilteredResults->append(dpR);
    }

    //inverted
    if (inverted){
        foreach(DotPlotResults dpR, *dpRevComplResultsListener->dotPlotList){
                dpFilteredResultsRevCompl->append(dpR);
        }
    }

    dotPlotTask = NULL;
    dpDirectResultListener->setTask(NULL);
    dpRevComplResultsListener->setTask(NULL);

    seqXCache.clear();
    seqYCache.clear();

    dotPlotIsCalculating = false;
    // build dotplot task finished
    pixMapUpdateNeeded = true;
    update();
}

// tell repeat finder that dotPlotResultsListener will be deleted
// if dotPlotTask is not RF task, nothing happened
void DotPlotWidget::cancelRepeatFinderTask() {

    RepeatFinderTaskFactoryRegistry *tfr = AppContext::getRepeatFinderTaskFactoryRegistry();
    RepeatFinderTaskFactory *factory = tfr->getFactory("");
    SAFE_POINT(factory != NULL, "Repeats factory is NULL!", );

    MultiTask *mTask = qobject_cast<MultiTask*>(dotPlotTask);
    if (mTask) {
        mTask->cancel();
        foreach(const QPointer<Task> &t, mTask->getSubtasks()) {
            factory->setRFResultsListener(t.data(), NULL);
        }
    }
}

bool DotPlotWidget::event(QEvent *event){
    if (event->type() == QEvent::ToolTip && hasFocus() && selActive) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QPoint pos = toInnerCoords(helpEvent->pos().x(), helpEvent->pos().y());
        pos = sequenceCoords(unshiftedUnzoomed(pos));
        const DotPlotResults *res = findNearestRepeat(pos);
        if(res == nearestRepeat){
            QString text = makeToolTipText();
            QFont dFont, f("Monospace");
            f.setPointSize(dFont.pointSize());
            f.setStyleHint(QFont::Courier);
            QToolTip::setFont(f);
            QToolTip::showText(helpEvent->globalPos(), text);
        }
    }
    return QWidget::event(event);
}

void DotPlotWidget::sl_sequenceWidgetRemoved(ADVSequenceWidget* w) {

    bool needed = false;
    foreach (ADVSequenceObjectContext *deleted, w->getSequenceContexts()) {
        if (deleted == sequenceX) {
            sequenceX = NULL;
            needed = true;
        }
        if (deleted == sequenceY) {
            sequenceY = NULL;
            needed = true;
        }
    }

    if (needed) {
        deleteDotPlotFlag = true;
        if (dotPlotTask) {
            cancelRepeatFinderTask();
        } else {
            emit si_removeDotPlot();
        }
    }
}


// click on the sequence view
void DotPlotWidget::sl_onSequenceSelectionChanged(LRegionsSelection* s, const QVector<U2Region>&, const QVector<U2Region>&) {

    QObject *sen = sender();

    if((sequenceX == NULL)||(sequenceY == NULL)){
        return;
    }
    DNASequenceSelection *dnaSelection = qobject_cast<DNASequenceSelection*>(sen);
    if (dnaSelection) {

        const U2SequenceObject *selectedSequence = dnaSelection->getSequenceObject();
            if (selectedSequence == sequenceX->getSequenceGObject()) {
                if(!nearestSelecting)
                    clearedByRepitSel = false;
                selectionX = s;
            }

            if (selectedSequence == sequenceY->getSequenceObject()) {
                if(!nearestSelecting)
                    clearedByRepitSel = false;
                selectionY = s;
            }


        update();
    }
    emit si_dotPlotSelecting();
}

// save dotplot as image
void DotPlotWidget::sl_showSaveImageDialog() {
    exitButton->hide();

    QString s1 = GUrlUtils::fixFileName(sequenceX->getSequenceGObject()->getGObjectName());
    QString s2 = GUrlUtils::fixFileName(sequenceY->getSequenceGObject()->getGObjectName());
    QString fileName = s1 == s2 ? s1 : s1 + "_" + s2;

    DotPlotImageExportController factory(this);
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(&factory,
                                                                           ExportImageDialog::DotPlot,
                                                                           fileName,
                                                                           ExportImageDialog::SupportScaling,
                                                                           this);
    dialog->exec();
    CHECK(!dialog.isNull(), );

    exitButton->show();
}

// save dotplot into a dotplot file, return true if not canceled
bool DotPlotWidget::sl_showSaveFileDialog() {
    LastUsedDirHelper lod("Dotplot");
    if (dpDirectResultListener->dotPlotList->isEmpty() && dpRevComplResultsListener->dotPlotList->isEmpty()) {
        QMessageBox::critical(this, tr("Error Saving Dotplot"), tr("The dotplot can't be saved as it is empty."));
        return true;
    }

    lod.url = U2FileDialog::getSaveFileName(NULL, tr("Save Dotplot"), lod.dir, tr("Dotplot files (*.dpt)"));

    if (lod.url.length() <= 0) {
        return false; // Cancel button pressed
    }

    // check if file opens
    DotPlotErrors err = SaveDotPlotTask::checkFile(lod.url);

    switch (err) {
        case ErrorOpen:
            QMessageBox::critical(this, tr("File opening error"), tr("Error opening file %1").arg(lod.url));
            return false;
        default:
            break;
    }

    TaskScheduler* ts = AppContext::getTaskScheduler();
    if (dotPlotTask) { // Check if there is already some dotPlotTask
        QMessageBox::critical(this, tr("Task is already running"), tr("Build or Load DotPlot task is already running"));
        return false;
    }
    SAFE_POINT(dpDirectResultListener, "dpDirectResultListener is NULL", false);
    SAFE_POINT(sequenceX, "sequenceX is NULL", false);
    SAFE_POINT(sequenceY, "sequenceY is NULL", false);

    dotPlotTask = new SaveDotPlotTask(
            lod.url,
            dpDirectResultListener->dotPlotList,
            dpRevComplResultsListener->dotPlotList,
            sequenceX->getSequenceObject(),
            sequenceY->getSequenceObject(),
            minLen,
            identity
    );
    ts->registerTopLevelTask(dotPlotTask);
    connect(dotPlotTask, SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()));

    return true;
}

// load dotplot from the dotplot file, return true if successful
bool DotPlotWidget::sl_showLoadFileDialog() {

    LastUsedDirHelper lod("Dotplot");
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Load Dotplot"), lod.dir, tr("Dotplot files (*.dpt)"));

    if (lod.url.length() <= 0) {
        return false; // Cancel button pressed
    }

    if (dotPlotTask) { // Check if there is already some dotPlotTask
        QMessageBox::critical(this, tr("Task is already running"), tr("Build or Load DotPlot task is already running"));
        return false;
    }

    SAFE_POINT(sequenceX, "sequenceX is NULL", false);
    SAFE_POINT(sequenceY, "sequenceY is NULL", false);

    SAFE_POINT(sequenceX->getSequenceObject(), "sequenceX->getSequenceObject() object is NULL", false);
    SAFE_POINT(sequenceY->getSequenceObject(), "sequenceY->getSequenceObject() object is NULL", false);

    // check if the file opens and names of the loading sequences same as names of current sequences
    DotPlotErrors err = LoadDotPlotTask::checkFile(
            lod.url,
            sequenceX->getSequenceObject()->getGObjectName(),
            sequenceY->getSequenceObject()->getGObjectName()
    );

    switch (err) {
        case ErrorOpen:
            QMessageBox::critical(this, tr("File opening error"), tr("Error opening file %1").arg(lod.url));
            return false;

        case ErrorNames:
            if (QMessageBox::critical(this, tr("Sequences are different"), tr("Current and loading sequences are different. Continue loading dot-plot anyway?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                break; // load dotplot anyway
            }
            else {
                return false;
            }

        default:
            break;
    }

    SAFE_POINT(dpDirectResultListener, "dpDirectResultListener is NULL", false);
    SAFE_POINT(dpDirectResultListener->dotPlotList, "dpDirectResultListener->dotPlotList is NULL", false);

    SAFE_POINT(dpRevComplResultsListener, "dpRevComplResultsListener is NULL", false);
    SAFE_POINT(dpRevComplResultsListener->dotPlotList, "dpRevComplResultsListener->dotPlotList is NULL", false);

    dotPlotTask = new LoadDotPlotTask(
            lod.url,
            dpDirectResultListener->dotPlotList,
            dpRevComplResultsListener->dotPlotList,
            sequenceX->getSequenceObject(),
            sequenceY->getSequenceObject(),
            &minLen,
            &identity,
            &direct,
            &inverted
    );
    dotPlotIsCalculating = true;

    TaskScheduler* ts = AppContext::getTaskScheduler();
    ts->registerTopLevelTask(dotPlotTask);
    connect(dotPlotTask, SIGNAL(si_stateChanged()), SLOT(sl_buildDotplotTaskStateChanged()));

    pixMapUpdateNeeded = true;
    update();

    return true;
}

// This is maximum sequence size (X+Y) we allow to use with dot plots
// W is 100% match algorithm, it uses less memory that WK which is a <100% algorithm and requires more memory.
#define MAX_DOT_PLOT_W_SUM_SEQUENCE_LENGTH_32_BIT_OS (600 * 1000 * 1000)
#define MAX_DOT_PLOT_WK_SUM_SEQUENCE_LENGTH_32_BIT_OS (200 * 1000 * 1000)

// creating new dotplot or changing settings
bool DotPlotWidget::sl_showSettingsDialog(bool disableLoad) {

    if (dotPlotTask) { // Check if there is already some dotPlotTask
        QMessageBox::critical(this, tr("Task is already running"), tr("Build or Load DotPlot task is already running"));
        return false;
    }

    SAFE_POINT(dnaView, "dnaView is NULL", false);

    QObjectScopedPointer<DotPlotDialog> d = new DotPlotDialog(QApplication::activeWindow(), dnaView, minLen, identity, sequenceX, sequenceY, direct, inverted, dotPlotDirectColor, dotPlotInvertedColor, disableLoad);
    d->exec();
    CHECK(!d.isNull(), false);

    if (d->result() != QDialog::Accepted) {
        return false;
    }
    setMinimumHeight(200);

    nearestRepeat = NULL;
    nearestInverted = false;

    bool res = false;
    if ((sequenceX != d->getXSeq()) || (sequenceY != d->getYSeq())) {
        res = true;
    }

    sequenceX = d->getXSeq();
    sequenceY = d->getYSeq();

    if (AppResourcePool::is32BitBuild()) {
        quint64 sumSeqLen = sequenceX->getSequenceLength() + sequenceY->getSequenceLength();
        bool wkMode = identity < 100;
        if ((wkMode && sumSeqLen > MAX_DOT_PLOT_WK_SUM_SEQUENCE_LENGTH_32_BIT_OS)||
                (!wkMode && sumSeqLen > MAX_DOT_PLOT_W_SUM_SEQUENCE_LENGTH_32_BIT_OS)) {
            QMessageBox::warning(this, L10N::warningTitle(),  tr("Sequence size is too large!"));
            return false;
        }
    }

    if (res){
        resetZooming();
    }

    direct = d->isDirect();
    inverted = d->isInverted();

    SAFE_POINT(direct || inverted, "direct || inverted is false", false);

    dotPlotDirectColor = d->getDirectColor();
    dotPlotInvertedColor = d->getInvertedColor();

    SAFE_POINT(d->minLenBox, "d.minLenBox is NULL", false);
    SAFE_POINT(d->identityBox, "d.identityBox is NULL", false);

    minLen = d->minLenBox->value();
    identity = d->identityBox->value();

    connectSequenceSelectionSignals();

    SAFE_POINT(dpDirectResultListener, "dpDirectResultListener is NULL", false);
    SAFE_POINT(dpDirectResultListener->dotPlotList, "dpDirectResultListener->dotPlotList is NULL", false);
    dpDirectResultListener->dotPlotList->clear();

    SAFE_POINT(dpRevComplResultsListener, "dpRevComplResultsListener is NULL", false);
    SAFE_POINT(dpRevComplResultsListener->dotPlotList, "dpRevComplResultsListener->dotPlotList is NULL", false);
    dpRevComplResultsListener->dotPlotList->clear();

    if(!sequenceX){
        QMessageBox::critical(this, tr("Invalid sequence"), tr("First selected sequence is invalid"));
        return false;
    }

    if(!sequenceY){
        QMessageBox::critical(this, tr("Invalid sequence"), tr("Second selected sequence is invalid"));
        return false;
    }


    if ((sequenceX->getAlphabet()->getType() != sequenceY->getAlphabet()->getType())){

        sequenceX = NULL;
        sequenceY = NULL;

        QMessageBox::critical(this, tr("Wrong alphabet types"), tr("Both sequence must have the same alphabet"));
        return false;
    }

    SAFE_POINT(sequenceX->getSequenceObject(), "sequenceX->getSequenceObject() is NULL", false);
    SAFE_POINT(sequenceY->getSequenceObject(), "sequenceY->getSequenceObject() is NULL", false);


    const DNAAlphabet *al = sequenceX->getAlphabet();
    RFAlgorithm alg = d->getAlgo();
    if ((al->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()) || (al->getId() == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT())) {
        al = sequenceY->getAlphabet();
    }else{
        alg = RFAlgorithm_Diagonal; // only this algorithm works with protein sequences
    }

    RepeatFinderTaskFactoryRegistry *tfr = AppContext::getRepeatFinderTaskFactoryRegistry();
    RepeatFinderTaskFactory *factory = tfr->getFactory("");
    SAFE_POINT(factory != NULL, "Repeats factory is NULL!", false);

    QList<Task*> tasks;

    U2OpStatusImpl os;
    seqXCache = sequenceX->getSequenceObject()->getWholeSequenceData(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), false);
    seqYCache = sequenceY->getSequenceObject()->getWholeSequenceData(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), false);
    if (d->isDirect()) {
        RepeatFinderSettings cDirect(
            dpDirectResultListener,
            seqXCache.constData(),
            seqXCache.length(),
            false,                        // direct
            seqYCache.constData(),
            seqYCache.length(),
            al,
            d->getMinLen(), d->getMismatches(),
            alg
            );

        Task *dotPlotDirectTask = factory->getTaskInstance(cDirect);
        dpDirectResultListener->setTask(dotPlotDirectTask);

        tasks << dotPlotDirectTask;
    }

    if (d->isInverted()) {
        RepeatFinderSettings cInverse(
            dpRevComplResultsListener,
            seqXCache.constData(),
            seqXCache.length(),
            true,                        // inverted
            seqYCache.constData(),
            seqYCache.length(),
            al,
            d->getMinLen(), d->getMismatches(),
            alg
            );

        Task *dotPlotInversedTask = factory->getTaskInstance(cInverse);
        dpRevComplResultsListener->setTask(dotPlotInversedTask);
        dpRevComplResultsListener->xLen = sequenceX->getSequenceLength();

        tasks << dotPlotInversedTask;
    }

    dotPlotTask = new MultiTask("Searching repeats", tasks, true);
    dotPlotIsCalculating = true;

    TaskScheduler* ts = AppContext::getTaskScheduler();
    ts->registerTopLevelTask(dotPlotTask);
    connect(dotPlotTask, SIGNAL(si_stateChanged()), SLOT(sl_buildDotplotTaskStateChanged()));

    return true;
}

// ask user if he wants to save dotplot first
void DotPlotWidget::sl_showDeleteDialog() {

    int answer = QMessageBox::information(this, tr("Save dot-plot"), tr("Save dot-plot data before closing?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    bool saveDotPlot;

    switch (answer) {
        case QMessageBox::Cancel:
            return;

        case QMessageBox::Yes:
            saveDotPlot = sl_showSaveFileDialog();
            if (!saveDotPlot) { // cancel button pressed
                return;
            }
            break;

        default:
            break;
    }

    emit si_removeDotPlot();
}

// dotplot results updated, need to update picture
void DotPlotWidget::pixMapUpdate() {

    if (!pixMapUpdateNeeded || !sequenceX || !sequenceY || dotPlotTask) {
        return;
    }
    qint64 seqXLen = sequenceX->getSequenceLength();
    qint64 seqYLen = sequenceY->getSequenceLength();
    if (seqXLen <= 0 || seqYLen <= 0) {
        return;
    }

    float ratioX = w/(float)seqXLen;
    float ratioY = h/(float)seqYLen;
    //float ratioY = ratioX;

    delete pixMap;
    pixMap = new QPixmap(w, h);

    QPainter pixp(pixMap);
    pixp.setPen( Qt::NoPen ); // do not draw outline
    pixp.setBrush(dotPlotBGColor);
    pixp.drawRect(0, 0, w, h);

    QLine line;

    SAFE_POINT(dpDirectResultListener, "dpDirectResultListener is NULL", );
    SAFE_POINT(dpDirectResultListener->dotPlotList, "dpDirectResultListener->dotPlotList is NULL", );

    // draw to the dotplot direct results
    if (direct) {
        pixp.setPen(dotPlotDirectColor);
        foreach(const DotPlotResults &r, *dpFilteredResults) {

            if (!getLineToDraw(r, &line, ratioX, ratioY)) {
                continue;
            }

            pixp.drawLine(line);
        }
    }

    // draw to the dotplot inverted results
    if (inverted) {
        pixp.setPen(dotPlotInvertedColor);
        foreach(const DotPlotResults &r, *dpFilteredResultsRevCompl) {

            if (!getLineToDraw(r, &line, ratioX, ratioY, true)) {
                continue;
            }

            pixp.drawLine(line);
        }
    }

    pixMapUpdateNeeded = false;
}

// return true if the line intersects with the area to draw
bool DotPlotWidget::getLineToDraw(const DotPlotResults &r, QLine *line, float ratioX, float ratioY, bool invert) const {

    float x1 = r.x*ratioX * zoom.x() + shiftX;
    float y1 = r.y*ratioY * zoom.y() + shiftY;
    float x2 = x1 + r.len*ratioX * zoom.x();
    float y2 = y1 + r.len*ratioY * zoom.y();

    if ((x2 < 0) || (y2 < 0) || (x1 > w) || (y1 > h)) {
        return false;
    }

    if (x1<0) {
        float y_0 = y1 - x1*(y2-y1)/(x2-x1);
        if ((y_0 >= 0) && (y_0 <= h)) {
            x1 = 0;
            y1 = y_0;
        }

    }

    if (x2>w) {
        float y_w = y1 + (w-x1)*(y2-y1)/(x2-x1);
        if ((y_w >= 0) && (y_w <= h)) {
            x2 = w;
            y2 = y_w;
        }

    }

    if (y1<0) {
        float x_0 = x1 - y1*(x2-x1)/(y2-y1);
        if ((x_0 >= 0) && (x_0 <= w)) {
            y1 = 0;
            x1 = x_0;
        }

    }

    if (y2>h) {
        float x_h = x1 + (h-y1)*(x2-x1)/(y2-y1);
        if ((x_h >= 0) && (x_h <= w)) {
            y2 = h;
            x2 = x_h;
        }

    }

    if ((x1 < 0) || (x2 < 0) || (y1 < 0) || (y2 < 0) || (x1 > w) || (y1 > h) || (x2 > w) || (y2 > h)) {
        return false;
    }

    SAFE_POINT(line, "line is NULL", false);

    if (invert) {
        float tmpX = x1;
        x1 = x2;
        x2 = tmpX;
    }
    line->setLine(x1, y1, x2, y2);
    return true;
}

// draw everything to provided size
void DotPlotWidget::drawAll(QPainter &p, QSize &size, qreal fontScale, DotPlotImageExportSettings& exportSettings) {
    p.save();

    SAFE_POINT(w != 0 && h != 0, tr("Invalid weight and height parameters!"), );
    qreal scaleCoeff = fontScale * qMin( (qreal)size.width() / w, (qreal) size.height() / h );
    QFont f = p.font();
    f.setPointSize( (int)(f.pointSizeF()*scaleCoeff + 0.5) );
    p.setFont(f);

    // save widget parameters
    int wSaved = w;
    int hSaved = h;
    int textSpaceSaved = textSpace;
    float shiftX_saved = shiftX;
    float shiftY_saved = shiftY;

    // adapt sizes to provided w and h
    w = size.width() - 2 * textSpace;
    h = size.height() - 2 * textSpace;
    miniMap->updatePosition(w, h);
    pixMapUpdateNeeded = true;

    shiftX = w * shiftX_saved / wSaved;
    shiftY = h * shiftY_saved / hSaved;

    // draw all
    drawAll(p, scaleCoeff, false, exportSettings.includeAreaSelection, exportSettings.includeRepeatSelection);

    // restore widget parameters
    w = wSaved;
    h = hSaved;
    miniMap->updatePosition(w, h);
    textSpace = textSpaceSaved;
    shiftX = shiftX_saved;
    shiftY = shiftY_saved;

    p.restore();
    pixMapUpdateNeeded = true;
}

// draw everything
void DotPlotWidget::drawAll(QPainter& p, qreal rulerFontScale, bool _drawFocus,
                            bool drawAreaSelection, bool drawRepeatSelection) {
    if (sequenceX == NULL || sequenceY == NULL) {
        return;
    }

    QFontMetrics fm = p.fontMetrics();
    // min textSpace is 4 characters: this is important for sequence name labels
    textSpace = fm.width("0") * qMax(4, qRound(1 + log10(sequenceX->getSequenceLength())));
    // border around view
    w = width() - 2 * textSpace;
    h = height() - 2 * textSpace;

    if (dotPlotIsCalculating) {
        GUIUtils::showMessage(this, p, tr("Dotplot is calculating..."));
    } else {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);

        p.setBrush(QBrush(palette().window().color()));

        drawNames(p);
        p.translate(textSpace, textSpace);

        drawAxises(p);
        drawDots(p);
        if (drawAreaSelection) {
            drawSelection(p);
        }
        drawMiniMap(p);
        if (drawRepeatSelection) {
            drawNearestRepeat(p);
        }

        p.translate(-textSpace, -textSpace);
        drawRulers(p, rulerFontScale);

        p.restore();

        if (hasFocus() && _drawFocus) {
            drawFocus(p);
        }
    }
#define DP_MARGIN 2
#define DP_EXIT_BUTTON_SIZE 20
    exitButton->setGeometry(width()- DP_MARGIN - DP_EXIT_BUTTON_SIZE, DP_MARGIN, DP_EXIT_BUTTON_SIZE, DP_EXIT_BUTTON_SIZE);
}

void DotPlotWidget::drawFocus(QPainter& p) const{
    p.setPen(QPen(Qt::black, 1, Qt::DotLine));
    p.drawRect(0, 0, width()-1, height()-1);
}

void DotPlotWidget::drawNames(QPainter &p) const {

    SAFE_POINT(sequenceX->getSequenceObject(), "sequenceX->getSequenceObject() is NULL", );
    SAFE_POINT(sequenceX->getSequenceObject(), "sequenceY->getSequenceObject() is NULL", );
    QString nameX = sequenceX->getSequenceObject()->getGObjectName();
    QString nameY = sequenceY->getSequenceObject()->getGObjectName();

    const QFontMetrics &fm = this->fontMetrics();

    nameX += tr(" (min length %1, identity %2%)").arg(minLen).arg(identity);
    int nameXWidth = fm.width(nameX);

    // If nameX doesn't fit, it should be aligned left instead of center
    int flags = (nameXWidth < w) ? Qt::AlignCenter : Qt::AlignVCenter | Qt::AlignLeft;
    p.drawText(textSpace, h + textSpace, w, textSpace, flags, nameX);

    p.save();

        p.rotate(90);
        p.translate(DP_MARGIN + textSpace, -(w +textSpace*2));

        int nameYWidth = fm.width(nameY);

        // If nameY doesn't fit, it should be aligned left instead of center
        flags = (nameYWidth < h) ? Qt::AlignCenter : Qt::AlignVCenter | Qt::AlignLeft;
        p.drawText(0, 0, h, textSpace, flags, nameY);

    p.restore();
}

void DotPlotWidget::drawNearestRepeat(QPainter& p) const {

    if (!nearestRepeat) {
        return;
    }
    p.save();
    p.setPen(dotPlotNearestRepeatColor);

    float ratioX = w/(float)sequenceX->getSequenceLength();
    float ratioY = h/(float)sequenceY->getSequenceLength();

    QLine line;
    if (getLineToDraw(*nearestRepeat, &line, ratioX, ratioY, nearestInverted)) {
        p.drawLine(line);
    }

    p.restore();
}

void DotPlotWidget::drawMiniMap(QPainter& p) const {

    if (miniMap && ((zoom.x() > 1.0f) || (zoom.y() > 1.0f))) {
        miniMap->draw(p, shiftX, shiftY, zoom);
    }
}

// update dotplot picture if needed and draw it
void DotPlotWidget::drawDots(QPainter& p) {

    pixMapUpdate();

    if (pixMap) {
        p.drawPixmap(0, 0, w, h, *pixMap);
    }
}

void DotPlotWidget::drawAxises(QPainter& p) const {

    QPoint zeroPoint(0, 0);
    QPoint lowLeftPoint(0, h);
    QPoint topRightPoint(w, 0);

    p.drawLine(zeroPoint, lowLeftPoint);
    p.drawLine(zeroPoint, topRightPoint);
}

// get cutted text to draw rulers
QString DotPlotWidget::getRoundedText(QPainter &p, int num, int size) const {

    QRectF rect;
    QString curStr = QString::number(num);

    rect = p.boundingRect(0, 0, size, 100, Qt::AlignLeft | Qt::AlignTop, curStr);
    if (rect.width() < size)
        return curStr;

    curStr = QString::number(num/(float)1000, 'f', 1) + QString("K");
    rect = p.boundingRect(0, 0, size, 100, Qt::AlignLeft | Qt::AlignTop, curStr);
    if (rect.width() < size)
        return curStr;

    curStr = QString::number(num/(float)1000000, 'f', 1) + QString("M");
    rect = p.boundingRect(0, 0, size, 100, Qt::AlignLeft | Qt::AlignTop, curStr);
    if (rect.width() < size)
        return curStr;

    return "";
}

void DotPlotWidget::drawRulers(QPainter &p, qreal fontScale) const{
    GraphUtils::RulerConfig rConf;

    rConf.notchSize = rulerNotchSize;
    rConf.textPosition = GraphUtils::LEFT;
    QFont rulerFont;

    rulerFont.setFamily("Arial");
    rulerFont.setPointSize(8 * fontScale);

    int startX = sequenceCoords(unshiftedUnzoomed(QPointF(0,0))).x(),
        endX = sequenceCoords(unshiftedUnzoomed(QPointF(w,0))).x(),
        startY = sequenceCoords(unshiftedUnzoomed(QPointF(0,0))).y(),
        endY = sequenceCoords(unshiftedUnzoomed(QPointF(0,h))).y();

    QPoint extraLen(0, 0);
    int xSeqLen = sequenceX->getSequenceLength();
    int ySeqLen = sequenceY->getSequenceLength();

    if (xSeqLen && ySeqLen) {
        float ratioX = w/(float)xSeqLen;
        float ratioY = h/(float)ySeqLen;

        extraLen = QPoint(0.5*ratioX, 0.5*ratioY);
    }

    GraphUtils::drawRuler(p, QPoint(textSpace + extraLen.x(), textSpace), w - 2*extraLen.x(), startX+1, endX, rulerFont, rConf);

    rConf.direction = GraphUtils::TopToBottom;
    rConf.textBorderEnd = 10;
    GraphUtils::drawRuler(p, QPoint(textSpace, textSpace + extraLen.y()), h - 2*extraLen.y(), startY+1, endY, rulerFont, rConf);
}

// need to draw inside area not containing borders
void DotPlotWidget::drawRectCorrect(QPainter &p, QRectF r) const {

    if ((r.right() < 0) || (r.left() > w) || (r.bottom() < 0) || (r.top() > h)) {
        return;
    }

    if (r.left() < 0) {
        r.setLeft(0);
    }

    if (r.top() < 0) {
        r.setTop(0);
    }

    if (r.right() > w) {
        r.setRight(w);
    }

    if (r.bottom() > h) {
        r.setBottom(h);
    }

    p.drawRect(r);
}

// part of the sequence is selected, show it
void DotPlotWidget::drawSelection(QPainter &p) const{

    if (!sequenceX || !sequenceY) {
        return;
    }

    if (!(selectionX || selectionY)) {
        return;
    }
    if(clearedByRepitSel){
        return;
    }

    p.save();

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setColor(QColor(0, 125, 227, 200));

    p.setPen(pen);
    p.setBrush(QBrush(QColor(200, 200, 200, 100)));

    float xLeft, xLen, yBottom, yLen;
    int xSeqLen = sequenceX->getSequenceLength();
    int ySeqLen = sequenceY->getSequenceLength();

    SAFE_POINT(xSeqLen, "xSeqLen is zero", );
    SAFE_POINT(ySeqLen, "ySeqLen is zero", );

    // for each selected part on the sequence X, highlight selected part on the sequence Y
    if((selectionX && selectionX->getSelectedRegions().size() > 1)){
        const QVector<U2Region>& sel = selectionX->getSelectedRegions();
        const U2Region &rx = sel[0];
        xLeft = rx.startPos/(float)xSeqLen * w*zoom.x();
        xLen = rx.length/(float)xSeqLen * w*zoom.x();
        const U2Region &ry = sel[1];
        yBottom = ry.startPos/(float)ySeqLen * h*zoom.y();
        yLen = ry.length/(float)ySeqLen * h*zoom.y();

        drawRectCorrect(p, QRectF(xLeft + shiftX, yBottom + shiftY, xLen, yLen));
    }else{
        if (selectionX) {
            foreach(const U2Region &rx, selectionX->getSelectedRegions()) {

                xLeft = rx.startPos/(float)xSeqLen * w*zoom.x();
                xLen = rx.length/(float)xSeqLen * w*zoom.x();

                if (!selectionY || selectionY->getSelectedRegions().size() == 0) {
                    yBottom = 0;
                    yLen = 1.0f * h*zoom.y();

                    drawRectCorrect(p, QRectF(xLeft + shiftX, yBottom + shiftY, xLen, yLen));
                }
                else {
                    foreach(const U2Region &ry, selectionY->getSelectedRegions()) {
                        yBottom = ry.startPos/(float)ySeqLen * h*zoom.y();
                        yLen = ry.length/(float)ySeqLen * h*zoom.y();

                        drawRectCorrect(p, QRectF(xLeft + shiftX, yBottom + shiftY, xLen, yLen));

                    }
                }
            }
        }

        // user selected only part of the Y sequence
        if ((!selectionX || selectionX->getSelectedRegions().size() == 0) && (selectionY && selectionY->getSelectedRegions().size() != 0)) {
            xLeft = 0;
            xLen = 1.0f * w*zoom.x();

            foreach(const U2Region &ry, selectionY->getSelectedRegions()) {
                yBottom = ry.startPos/(float)ySeqLen * h*zoom.y();
                yLen = ry.length/(float)ySeqLen * h*zoom.y();

                drawRectCorrect(p, QRectF(xLeft + shiftX, yBottom + shiftY, xLen, yLen));
            }
        }
    }

    p.restore();
}

// shifted camera or changed zoom
void DotPlotWidget::checkShift(bool emitSignal) {

    if (shiftX > 0) {
        shiftX = 0;
    }

    if (shiftY > 0) {
        shiftY = 0;
    }

    if (shiftX < w*(1-zoom.x())) {
        shiftX = w*(1-zoom.x());
    }

    if (shiftY < h*(1-zoom.y())) {
        shiftY = h*(1-zoom.y());
    }

    if (emitSignal) {
        emit si_dotPlotChanged(sequenceX, sequenceY, shiftX/w, shiftY/h, zoom);
    }
    U2Region visRangeX;
    U2Region visRangeY;

    foreach (ADVSequenceWidget *advSeqWidget, dnaView->getSequenceWidgets()) {
        ADVSingleSequenceWidget *advSingleSeqWidget = qobject_cast<ADVSingleSequenceWidget*>(advSeqWidget);
        visRangeX = getVisibleRange(Qt::XAxis);
        visRangeY = getVisibleRange(Qt::YAxis);

        if (advSingleSeqWidget->getSequenceContext() == sequenceX) {
            if (advSingleSeqWidget->getVisibleRange() != getVisibleRange(Qt::XAxis)) {
                advSingleSeqWidget->setVisibleRange(getVisibleRange(Qt::XAxis));
            }
        }
        if ((sequenceX != sequenceY) && (advSingleSeqWidget->getSequenceContext() == sequenceY)) {
            if (advSingleSeqWidget->getVisibleRange() != getVisibleRange(Qt::YAxis)) {
                advSingleSeqWidget->setVisibleRange(getVisibleRange(Qt::YAxis));
            }
        }
    }
}

void DotPlotWidget::calcZooming(const QPointF &oldzoom, const QPointF &nZoom, const QPoint &inner, bool emitSignal) {

    if (dotPlotTask || (w<=0) || (h<=0)) {
        return;
    }

    if (!(sequenceX && sequenceY)) {
        return;
    }

    float seqLenX = sequenceX->getSequenceLength();
    float seqLenY = sequenceY->getSequenceLength();

    QPointF newzoom(nZoom);
    // limit maximum zoom
    if (newzoom.x() > seqLenX/2) {
        newzoom.setX(seqLenX/2);
    }
    if (newzoom.y() > seqLenY/2) {
        newzoom.setY(seqLenY/2);
    }
    // dotplot has no zooming and the user tries to zoom out
    if (newzoom.x() < 1.0f) {
        newzoom.setX(1.0f);
    }
    if (newzoom.y() < 1.0f) {
        newzoom.setY(1.0f);
    }

    float xi = (inner.x() - shiftX)/oldzoom.x();
    float yi = (inner.y() - shiftY)/oldzoom.y();

    shiftX = inner.x() - xi*newzoom.x();
    shiftY = inner.y() - yi*newzoom.y();

    if (zoom != newzoom) {
        pixMapUpdateNeeded = true;
        update();
    }
    zoom = newzoom;
    checkShift(emitSignal);
}

void DotPlotWidget::multZooming(float multzoom) {

    if (multzoom <=0 ) {
        return;
    }

    calcZooming(zoom, zoom*multzoom, QPoint(w/2, h/2));
}

void DotPlotWidget::setShiftZoom(ADVSequenceObjectContext* seqX, ADVSequenceObjectContext* seqY, float shX, float shY, const QPointF &z) {

    shX*=w;
    shY*=h;

    if ((sequenceX == seqX) && (sequenceY == seqY)) {
        if (shiftX!=(int)shX || shiftY!=(int)shY || zoom!=z) {
            pixMapUpdateNeeded = true;
        }

        shiftX = shX;
        shiftY = shY;
        zoom = z;

        update();
    }
}

void DotPlotWidget::setIgnorePanView(bool ignore) {

    ignorePanView = ignore;
}

void DotPlotWidget::setKeepAspectRatio(bool keepAr) {

    keepAspectRatio = keepAr;
}

void DotPlotWidget::zoomIn() {
    if (hasSelection()) {

        if (selectionX && !selectionX->getSelectedRegions().isEmpty()) {
            zoomTo(Qt::XAxis, selectionX->getSelectedRegions().first());
        }

        if (selectionY && !selectionY->getSelectedRegions().isEmpty()) {
            if(sequenceX != sequenceY || selectionY->getSelectedRegions().count() == 1){
                zoomTo(Qt::YAxis, selectionY->getSelectedRegions().first());
            }else{
                zoomTo(Qt::YAxis, selectionY->getSelectedRegions().at(1));
            }
        }
    }else{
        multZooming(2.0f);
    }
}

void DotPlotWidget::zoomOut() {

    multZooming(0.5f);
}

void DotPlotWidget::zoomReset() {

    resetZooming();
}


// translate visible coords to the sequence coords (starts from 0)
QPoint DotPlotWidget::sequenceCoords(const QPointF &c) const {

    SAFE_POINT(sequenceX, "sequenceX is NULL", QPoint());
    SAFE_POINT(sequenceY, "sequenceY is NULL", QPoint());

    int xLen = sequenceX->getSequenceLength();
    int yLen = sequenceY->getSequenceLength();

    SAFE_POINT(w>0, "w is lesser or equal zero", QPoint());
    SAFE_POINT(h>0, "h is lesser or equal zero", QPoint());

    int innerX = (c.x() * xLen)/w;
    int innerY = (c.y() * yLen)/h;

    return QPoint(innerX, innerY);
}

// select sequences using sequence coords
void DotPlotWidget::sequencesCoordsSelection(const QPointF &start, const QPointF &end) {

    float startX = start.x();
    float startY = start.y();
    float endX = end.x();
    float endY = end.y();

    if (endX < startX) {
        float tmp = endX;
        endX = startX;
        startX = tmp;
    }
    if (endY < startY) {
        float tmp = endY;
        endY = startY;
        startY = tmp;
    }

    SAFE_POINT(dnaView, "dnaView is NULL", );
    foreach (ADVSequenceWidget *w, dnaView->getSequenceWidgets()) {

        SAFE_POINT(w, "w is NULL", );
        foreach (ADVSequenceObjectContext *s, w->getSequenceContexts()) {
            SAFE_POINT(s, "s is NULL", );

            if ( ((int)(endX-startX) > 0) && (s == sequenceX)) {
                s->getSequenceSelection()->setRegion(U2Region(startX, endX-startX));

                w->centerPosition(startX);
            }

            if ( ((int)(endY-startY) > 0) && (s == sequenceY)) {
                if(sequenceX != sequenceY){
                    s->getSequenceSelection()->clear();
                }
                s->getSequenceSelection()->addRegion(U2Region(startY, endY-startY));

                w->centerPosition(startY);
            }
        }
    }

    update();
}

// select sequences with mouse
void DotPlotWidget::sequencesMouseSelection(const QPointF &zoomedA, const QPointF &zoomedB) {

    if (!(sequenceX || sequenceY)) {
        return;
    }

    if (zoomedA == zoomedB) {
        selectionX = NULL;
        selectionY = NULL;

        return;
    }

    QPointF a(unshiftedUnzoomed(zoomedA));
    QPointF b(unshiftedUnzoomed(zoomedB));

    QPointF start = sequenceCoords(a);
    QPointF end = sequenceCoords(b);

    sequencesCoordsSelection(start, end);
}

// get mouse coords, select the nearest found repeat
void DotPlotWidget::selectNearestRepeat(const QPointF &p) {

    QPoint seqCoords = sequenceCoords(unshiftedUnzoomed(p));

    nearestRepeat = findNearestRepeat(seqCoords);
    if (!nearestRepeat) {
        return;
    }

    nearestSelecting = true;
    sequencesCoordsSelection(
        QPoint(nearestRepeat->x, nearestRepeat->y),
        QPoint(nearestRepeat->x + nearestRepeat->len, nearestRepeat->y + nearestRepeat->len)
    );


    foreach (ADVSequenceWidget *w, dnaView->getSequenceWidgets()) {
        foreach (ADVSequenceObjectContext *s, w->getSequenceContexts()) {
            if (s == sequenceX) {
                w->centerPosition(nearestRepeat->x);
            }
        }
    }
    nearestSelecting = false;
}

// get sequence coords, return sequence coords of the nearest repeat
const DotPlotResults* DotPlotWidget::findNearestRepeat(const QPoint &p) {

    const DotPlotResults* need = NULL;
    float minD = 0;

    float x = p.x();
    float y = p.y();

    SAFE_POINT(sequenceX, "sequenceX is NULL", NULL);
    SAFE_POINT(sequenceY, "sequenceY is NULL", NULL);

    if ((sequenceX->getSequenceLength() <= 0) || (sequenceY->getSequenceLength() <= 0)) {
        return NULL;
    }

    float ratioX = w/(float)sequenceX->getSequenceLength();
    float ratioY = h/(float)sequenceY->getSequenceLength();

    ratioX*=ratioX;
    ratioY*=ratioY;

    bool first = true;

    SAFE_POINT (dpDirectResultListener, "dpDirectResultListener is NULL", NULL);
    //foreach (const DotPlotResults &r, *dpDirectResultListener->dotPlotList) {
    foreach (const DotPlotResults &r, *dpFilteredResults) {

        float halfLen = r.len/(float)2;
        float midX = r.x + halfLen;
        float midY = r.y + halfLen;

        // square of the distance between two points. ratioX and ratioY are squared
        float d = (x-midX)*(x-midX)*ratioX + (y-midY)*(y-midY)*ratioY;

        if (d < minD || first) {
            minD = d;
            need = &r;

            nearestInverted = false;
        }

        first = false;
    }

    SAFE_POINT (dpRevComplResultsListener, "dpRevComplResultsListener is NULL", NULL);
    //foreach (const DotPlotResults &r, *dpRevComplResultsListener->dotPlotList) {
    foreach (const DotPlotResults &r, *dpFilteredResultsRevCompl) {

        float halfLen = r.len/(float)2;
        float midX = r.x + halfLen;
        float midY = r.y + halfLen;

        // square of the distance between two points. ratioX and ratioY are squared
        float d = (x-midX)*(x-midX)*ratioX + (y-midY)*(y-midY)*ratioY;

        if (d < minD || first) {
            minD = d;
            need = &r;

            nearestInverted = true;
        }

        first = false;
    }
    return need;
}

// get mouse coords, return coords in the area without border
QPoint DotPlotWidget::toInnerCoords(int x, int y) const {

    x = x - textSpace;
    y = y - textSpace;

    if (x > w) {
        x = w;
    }
    if (y > h) {
        y = h;
    }

    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }

    return QPoint(x, y);
}

QPoint DotPlotWidget::toInnerCoords(const QPoint &p) const {

    return toInnerCoords(p.x(), p.y());
}

void DotPlotWidget::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    drawAll(p);
    QWidget::paintEvent(e);
}

void DotPlotWidget::resizeEvent(QResizeEvent *e) {

    SAFE_POINT(e, "e is NULL", );

    if (e->oldSize() == e->size()) {
        return;
    }

    int oldw = w;
    int oldh = h;

    w = e->size().width() - 2*textSpace;
    h = e->size().height() - 2*textSpace;

    // update shift when resizing
    if (pixMap && (oldw > 0) && (oldh > 0)) {
        shiftX *= w/(float)(oldw);
        shiftY *= h/(float)(oldh);
    }

    delete miniMap;
    miniMap = new DotPlotMiniMap(w, h, 10);

    pixMapUpdateNeeded = true;
}

// zoom in/zoom out
void DotPlotWidget::wheelEvent(QWheelEvent *e) {

    SAFE_POINT(e, "e is NULL", );
    setFocus();
    if (dotPlotTask) {
        return;
    }

    QPointF oldzoom = zoom;
    QPointF newzoom = zoom*(1 + e->delta()/(float)1000);

    // cursor coords excluding dotplot border
    calcZooming(oldzoom, newzoom, toInnerCoords(e->pos()));
    update();
}

QString DotPlotWidget::makeToolTipText() const{
    if (!nearestRepeat){
        return "";
    }

    if (!sequenceX || !sequenceY){
        return "";
    }

    int maxRepeatSequenceShowLen = 32;
    QString text ="HIT:  len: %1, match: %2, %: %3\n";
    QString coord = "Coordinates(beg.): x: %1 y: %2\n";
    QString upLineSeq ="";
    QString middleLineSeq = "";
    QString downLineSeq = "";
    QByteArray repX = sequenceX->getSequenceObject()->getSequenceData(U2Region(nearestRepeat->x, nearestRepeat->len));
    QByteArray repY = sequenceY->getSequenceObject()->getSequenceData(U2Region(nearestRepeat->y, nearestRepeat->len));
    if (nearestInverted) {
        DNATranslation* complT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(sequenceX->getAlphabet());
        assert(complT != NULL);
        TextUtils::reverse(repX.data(), repX.length());
        complT->translate(repX.data(), repX.length());
    }
    int match = 0;

    for (int i = 0; i < nearestRepeat->len; i++){
        if (repX[i] == repY[i]) {
            match++;
        }
    }

    text = text.arg(nearestRepeat->len).arg(match).arg((float)(match/(float)nearestRepeat->len)*100);


    coord = coord.arg(nearestRepeat->x + 1).arg(nearestRepeat->y + 1);
    text.append(coord);

    for(int i = 0; i < nearestRepeat->len && i < maxRepeatSequenceShowLen; i++) {
        char rx = repX[i];
        char ry = repY[i];
        if (rx == ry) {
            middleLineSeq.append(rx);
        } else {
            middleLineSeq.append("*");
        }
        upLineSeq.append(rx);
        downLineSeq.append(ry);
    }

    QString ttt = nearestRepeat->len <= maxRepeatSequenceShowLen ? "\n" : "...\n";
    text.append("X: ");
    text.append(upLineSeq);
    text.append(ttt);
    text.append(">: ");

    text.append(middleLineSeq);
    text.append(ttt);
    text.append("Y: ");
    text.append(downLineSeq);
    ttt.chop(1);
    text.append(ttt);

    return text;
}

void DotPlotWidget::resetZooming() {

    calcZooming(zoom, QPointF(1.0f, 1.0f), QPoint(w/2, h/2));
}

// user clicked on the minimap
void DotPlotWidget::miniMapShift() {

    SAFE_POINT(miniMap, "miniMap is NULL", );

    QPointF fromMiniMap = miniMap->fromMiniMap(clickedSecond, zoom);
    shiftX = -fromMiniMap.x();
    shiftY = -fromMiniMap.y();
    checkShift();
    pixMapUpdateNeeded = true;
    update();
}

void DotPlotWidget::mousePressEvent(QMouseEvent *e) {
    setFocus();

    SAFE_POINT(e, "e is NULL", );

    QWidget::mousePressEvent(e);

    if (dotPlotTask) {
        return;
    }

    clickedFirst = toInnerCoords(e->pos().x(), e->pos().y());
    clickedSecond = clickedFirst;

    // selecting sequences or using minimap
    if (e->button() == Qt::LeftButton) {

        if (miniMap && miniMap->getBoundary().contains(clickedFirst)) { // click on the miniMap
            miniMapLooking = true;
            miniMapShift();

            return;
        }

        if(e->modifiers() & Qt::ControlModifier){
            clearRepeatSelection();
        }else if(e->modifiers() & Qt::ShiftModifier){
            shifting = true;
            cursor.setShape(Qt::OpenHandCursor);
            setCursor(cursor);
        }else if(selActive){
            selecting = true;
        }else{
            shifting = true;
        }
    }
    // shifting dotplot view
    if (e->button() == Qt::MidButton) {
        shifting = true;
    }
    if(timer->isActive()){
        timer->stop();
    }
}

// return real coords on the dotplot
QPointF DotPlotWidget::unshiftedUnzoomed(const QPointF &p) const {

    return QPointF((p.x() - shiftX)/zoom.x(), (p.y() - shiftY)/zoom.y());
}

void DotPlotWidget::mouseMoveEvent(QMouseEvent *e) {

    SAFE_POINT(e, "e is NULL", );

    QWidget::mouseMoveEvent(e);


    if (dotPlotTask) {
        return;
    }

    clickedSecond = toInnerCoords(e->pos().x(), e->pos().y());

    if (miniMapLooking) {
        miniMapShift();

        return;
    }

    QToolTip::showText(e->globalPos(), "");

    if (selecting) {
        if ((clickedFirst.x() != clickedSecond.x()) && (clickedFirst.y() != clickedSecond.y())) {
            clearedByRepitSel = false;
            sequencesMouseSelection(clickedFirst, clickedSecond);
        }
    }

    if (shifting) {
        shiftX += (clickedSecond.x() - clickedFirst.x());
        shiftY += (clickedSecond.y() - clickedFirst.y());

        clickedFirst = toInnerCoords(e->pos().x(), e->pos().y());
        checkShift();
        pixMapUpdateNeeded = true;
        update();
    }
    if(timer->isActive()){
        timer->stop();
    }

}
void DotPlotWidget::sequenceClearSelection(){
    SAFE_POINT(dnaView, "dnaView is NULL", );
    foreach (ADVSequenceWidget *w, dnaView->getSequenceWidgets()) {
        SAFE_POINT(w, "w is NULL", );
        foreach (ADVSequenceObjectContext *s, w->getSequenceContexts()) {
            SAFE_POINT(s, "s is NULL", );
            s->getSequenceSelection()->clear();
        }
    }
}

QString DotPlotWidget::getXSequenceName() {

    if (!sequenceX) {
        return "";
    }

    return sequenceX->getSequenceObject()->getGObjectName();
}

QString DotPlotWidget::getYSequenceName() {

    if (!sequenceY) {
        return "";
    }

    return sequenceY->getSequenceObject()->getGObjectName();
}

void DotPlotWidget::setSequences(U2SequenceObject* seqX, U2SequenceObject* seqY)
{
    CHECK(dnaView != NULL,);
    if(seqX != NULL) {
        sequenceX = dnaView->getSequenceContext(seqX);
    }
    if(seqY != NULL) {
        sequenceY = dnaView->getSequenceContext(seqY);
    }
}

void DotPlotWidget::sl_filter(){
    QObjectScopedPointer<DotPlotFilterDialog> d = new DotPlotFilterDialog(QApplication::activeWindow(), sequenceX, sequenceY);
    d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted == d->result()) {
        SAFE_POINT(dpDirectResultListener, "dpDirectResultListener is NULL", );
        SAFE_POINT(sequenceX, "sequenceX is NULL", );
        SAFE_POINT(sequenceY, "sequenceY is NULL", );

        QList<Task*> tasks;

        Task* directT = new DotPlotFilterTask(sequenceX,
            sequenceY,
            d->getFeatureNames(),
            dpDirectResultListener->dotPlotList,
            dpFilteredResults,
            d->getFilterType());
        tasks << directT;

        //inverted
        if(inverted){
            Task* recComplT = new DotPlotFilterTask(sequenceX,
                sequenceY,
                d->getFeatureNames(),
                dpRevComplResultsListener->dotPlotList,
                dpFilteredResultsRevCompl,
                d->getFilterType());
            tasks << recComplT;
        }

        dotPlotTask = new MultiTask("Filtration", tasks);
        connect(dotPlotTask, SIGNAL(si_stateChanged()), SLOT(sl_filteringTaskStateChanged()));

        filtration = true;

        TaskScheduler* ts = AppContext::getTaskScheduler();
        ts->registerTopLevelTask(dotPlotTask);
    }
}

void DotPlotWidget::mouseReleaseEvent(QMouseEvent *e) {
    setFocus();
    SAFE_POINT(e, "e is NULL", );

    if (dotPlotTask) {
        return;
    }

    QWidget::mouseReleaseEvent(e);

    if (e->button() == Qt::LeftButton) {
        if(!shifting){
            selecting = false;
            miniMapLooking = false;
            if (clickedFirst == clickedSecond && !(e->modifiers() & Qt::ControlModifier)) {
                if(!timer->isActive()){
                     timer->start();
                }
                sequenceClearSelection();
                clearedByRepitSel = true;
                selectNearestRepeat(clickedFirst);

            }
        }
        shifting = false;
        updateCursor();
    }

    if (e->button() == Qt::MidButton) {
        shifting = false;
    }

    update();
}


bool DotPlotWidget::hasSelection() const {

    if (selectionX) {
        foreach (const U2Region &lr, selectionX->getSelectedRegions()) {
            if (lr.length>0) {
                return true;
            }
        }
    }

    if (selectionY) {
        foreach (const U2Region &lr, selectionY->getSelectedRegions()) {
            if (lr.length>0) {
                return true;
            }
        }
    }

    return false;
}

bool DotPlotWidget::hasSelectedArea() const {
    bool selected = true;
    if (!sequenceX || !sequenceY) {
        selected = false;
    }

    if (!(selectionX || selectionY)) {
        selected = false;
    }
    if(clearedByRepitSel){
        selected = false;
    }
    return selected;
}

bool DotPlotWidget::canZoomIn(){
    float seqLenX = sequenceX->getSequenceLength();
    float seqLenY = sequenceY->getSequenceLength();

    return ((zoom.x() < seqLenX) && (zoom.y() < seqLenY));
}
bool DotPlotWidget::canZoomOut(){
    return ((zoom.x() > 1.0f) || (zoom.y() > 1.0f));
}

void DotPlotWidget::setSelActive(bool state){
    selActive = state;
    updateCursor();
}

void DotPlotWidget::focusInEvent(QFocusEvent* fe) {
    QWidget::focusInEvent(fe);

    emit si_dotPlotChanged(sequenceX, sequenceY, shiftX/w, shiftY/h, zoom);
}
void DotPlotWidget::focusOutEvent(QFocusEvent* fe){
    QWidget::focusOutEvent(fe);

    emit si_dotPlotChanged(sequenceX, sequenceY, shiftX/w, shiftY/h, zoom);
}
void DotPlotWidget::updateCursor(){
    if(selActive){
        cursor.setShape(Qt::ArrowCursor);
    }else{
        cursor.setShape(Qt::OpenHandCursor);
    }
    setCursor(cursor);
}

void DotPlotWidget::clearRepeatSelection(){
    nearestRepeat = NULL;
    clearedByRepitSel = true;
    update();
}

} // namespace
