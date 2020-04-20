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
#include <QHeaderView>
#include <QMenu>
#include <QTabWidget>
#include <QTableView>
#include <QtMath>

#include "InvestigationDataModel.h"
#include "WorkflowInvestigationWidgetsController.h"

const qint16 HEADER_TEXT_MARGIN = 40;
const int DEFAULT_SELECTED_COLUMN = -1;
const char *CONVERT_TO_DOC_ACTION_NAME = "Convert to document";
const char *COPY_TO_CLIPBOARD_ACTION_NAME = "Copy to clipboard";
const char *HIDE_SELECTED_COLUMN_ACTION_NAME = "Hide this column";
const char *HIDE_ALL_COLUMNS_BUT_SELECTED_ACTION_NAME = "Hide all columns but this";
const char *SHOW_ALL_COLUMNS_ACTION_NAME = "Show all columns";

namespace U2 {

WorkflowInvestigationWidgetsController::WorkflowInvestigationWidgetsController(QWidget *parent)
    : QObject(qobject_cast<QObject *>(parent)),
      investigationView(NULL),
      investigationModel(NULL),
      investigatedLink(NULL),
      investigatorName(),
      wasDisplayed(false),
      exportInvestigationAction(NULL),
      copyToClipboardAction(NULL),
      hideThisColumnAction(NULL),
      hideAllColumnsButThisAction(NULL),
      showAllColumnsAction(NULL),
      selectedColumn(DEFAULT_SELECTED_COLUMN),
      columnWidths()
{
    QTabWidget *container = dynamic_cast<QTabWidget *>(parent);
    Q_ASSERT(NULL != container);
    Q_UNUSED(container);

    exportInvestigationAction = new QAction(
        QIcon(":workflow_designer/images/document_convert.png"),
        tr(CONVERT_TO_DOC_ACTION_NAME), this);
    connect(exportInvestigationAction, SIGNAL(triggered()), SLOT(sl_exportInvestigation()));

    copyToClipboardAction = new QAction(QIcon(":workflow_designer/images/clipboard.png"),
        tr(COPY_TO_CLIPBOARD_ACTION_NAME), this);
    connect(copyToClipboardAction, SIGNAL(triggered()), SLOT(sl_copyToClipboard()));

    hideThisColumnAction = new QAction(tr(HIDE_SELECTED_COLUMN_ACTION_NAME), this);
    connect(hideThisColumnAction, SIGNAL(triggered()), SLOT(sl_hideSelectedColumn()));

    hideAllColumnsButThisAction = new QAction(tr(HIDE_ALL_COLUMNS_BUT_SELECTED_ACTION_NAME), this);
    connect(hideAllColumnsButThisAction, SIGNAL(triggered()), SLOT(sl_hideAllColumnsButSelected()));

    showAllColumnsAction = new QAction(tr(SHOW_ALL_COLUMNS_ACTION_NAME), this);
    connect(showAllColumnsAction, SIGNAL(triggered()), SLOT(sl_showAllColumns()));
}

WorkflowInvestigationWidgetsController::~WorkflowInvestigationWidgetsController() {
    deleteBusInvestigations();
}

bool WorkflowInvestigationWidgetsController::eventFilter(QObject *watched, QEvent *event) {
    if(QEvent::Paint == event->type()
        && NULL != investigationView
        && watched == dynamic_cast<QObject *>(investigationView->viewport()))
    {
        if(NULL == investigationView->model() && NULL != investigatedLink) {
            createInvestigationModel();
            investigationView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
            adjustInvestigationColumnWidth(investigationView);
        }
    }
    return QObject::eventFilter(watched, event);
}

void WorkflowInvestigationWidgetsController::setCurrentInvestigation(const Workflow::Link *bus) {
    QTabWidget *container = dynamic_cast<QTabWidget *>(parent());
    Q_ASSERT(NULL != container);
    const int tabNumberForInvestigation = container->indexOf(investigationView);
    if(-1 != tabNumberForInvestigation) {
        deleteBusInvestigations();
        container->removeTab(tabNumberForInvestigation);
    }
    investigatedLink = bus;
    createNewInvestigation();
    investigationView->setParent(container);
    investigatorName = tr("Messages from '") + bus->source()->owner()->getLabel()
        + tr("' to '") + bus->destination()->owner()->getLabel() + tr("'");
    container->addTab(investigationView, investigatorName);
    container->setCurrentWidget(investigationView);
    if(!container->isVisible()) {
        container->show();
    }
}

void WorkflowInvestigationWidgetsController::deleteBusInvestigations() {
    if(NULL != investigationView && NULL != investigationModel) {
        const QBitArray hiddenColumns = investigationModel->getColumnsVisibility();
        for(int column = 0; investigationModel->columnCount() > column; ++column) {
            const int absoluteColumnNumber = investigationModel->getAbsoluteNumberOfVisibleColumn(column);
            columnWidths[investigatedLink][absoluteColumnNumber]
            = investigationView->columnWidth(column)
                * static_cast<int>(qPow(-1, hiddenColumns.testBit(absoluteColumnNumber)));
        }
        delete investigationModel;
        investigationModel = NULL;
        investigationView->viewport()->removeEventFilter(this);
        delete investigationView;
        investigationView = NULL;
    }
}

void WorkflowInvestigationWidgetsController::resetInvestigations() {
    investigatedLink = NULL;
    investigatorName.clear();
    columnWidths.clear();
}

void WorkflowInvestigationWidgetsController::createNewInvestigation() {
    investigationView = new QTableView();
    investigationView->viewport()->installEventFilter(this);
    investigationView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(investigationView, SIGNAL(customContextMenuRequested(const QPoint &)),
        SLOT(sl_contextMenuRequested(const QPoint &)));
    investigationView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(investigationView->horizontalHeader(),
        SIGNAL(customContextMenuRequested(const QPoint &)),
        SLOT(sl_hotizontalHeaderContextMenuRequested(const QPoint &)));
}

void WorkflowInvestigationWidgetsController::createInvestigationModel() {
    Q_ASSERT(NULL != investigatedLink && NULL != investigationView);
    investigationModel = new InvestigationDataModel(investigatedLink, this);

    connect(investigationModel, SIGNAL(si_investigationRequested(const Workflow::Link *, int)),
        SIGNAL(si_updateCurrentInvestigation(const Workflow::Link *, int)));
    connect(investigationModel, SIGNAL(si_countOfMessagesRequested(const Workflow::Link *)),
        SIGNAL(si_countOfMessagesRequested(const Workflow::Link *)));
    connect(investigationModel, SIGNAL(si_columnsVisibilityRequested()),
        SLOT(sl_columnsVisibilityResponse()));

    investigationView->setModel(investigationModel);
}

void WorkflowInvestigationWidgetsController::adjustInvestigationColumnWidth(
    WorkflowInvestigationWidget *investigator)
{
    for(int currentColumn = 0; investigationModel->columnCount() > currentColumn; ++currentColumn)
    {
        const int absoluteColumnNumber
            = investigationModel->getAbsoluteNumberOfVisibleColumn(currentColumn);
        const int width = (columnWidths[investigatedLink].size() <= absoluteColumnNumber
            || 0 == columnWidths[investigatedLink][absoluteColumnNumber])
            ? investigator->fontMetrics().width(investigationModel->headerData(
            currentColumn, Qt::Horizontal).toString()) + HEADER_TEXT_MARGIN
            : columnWidths[investigatedLink][absoluteColumnNumber];
       Q_ASSERT(0 < width);
       investigator->setColumnWidth(currentColumn, width);
    }
}

void WorkflowInvestigationWidgetsController::setInvestigationWidgetsVisible(bool visible) {
    QTabWidget *container = dynamic_cast<QTabWidget *>(parent());
    Q_ASSERT(NULL != container);
    if(!visible && NULL != investigationView) {
        WorkflowInvestigationWidget *currentWidget
            = dynamic_cast<WorkflowInvestigationWidget *>(container->currentWidget());
        wasDisplayed = (investigationView == currentWidget);
        container->removeTab(container->indexOf(static_cast<QWidget *>(investigationView)));
        deleteBusInvestigations();
        if(wasDisplayed) {
            container->hide();
        }
    } else if(visible && NULL != investigatedLink) {
        createNewInvestigation();
        investigationView->setParent(container);
        container->addTab(investigationView, investigatorName);
        if(wasDisplayed) {
            container->show();
            container->setCurrentWidget(investigationView);
        }
    }
}

void WorkflowInvestigationWidgetsController::sl_currentInvestigationUpdateResponse(
    const WorkflowInvestigationData &investigationInfo, const Workflow::Link *bus)
{
    Q_ASSERT(bus == investigatedLink);
    Q_UNUSED(bus);
    if(!investigationInfo.isEmpty()) {
        const int rowInsertionStartPosition = investigationModel->loadedRowCount();
        if(!investigationModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).isValid()) {
            const QList<QString> headerTitles = investigationInfo.keys();
            for(int i = 0; i < headerTitles.size(); ++i) {
                investigationModel->setHeaderData(i, Qt::Horizontal, headerTitles[i]);
            }
            if(columnWidths[investigatedLink].isEmpty()) {
                columnWidths[investigatedLink].resize(headerTitles.size());
                columnWidths[investigatedLink].fill(0);
            }
        }
        for(int columnNumber = 0; columnNumber < investigationInfo.keys().size(); ++columnNumber) {
            const QString key = investigationInfo.keys()[columnNumber];
            for(int rowNumber = rowInsertionStartPosition;
                rowNumber < investigationInfo[key].size() + rowInsertionStartPosition; ++rowNumber)
            {
                investigationModel->setData(investigationModel->index(rowNumber, columnNumber),
                    investigationInfo[key][rowNumber - rowInsertionStartPosition]);
            }
        }
    } else if ( investigationModel->getColumnsVisibility( ).isNull( ) ) {
        investigationModel->setColumnsVisibility( QBitArray( 0 ) );
    }
}

void WorkflowInvestigationWidgetsController::sl_countOfMessagesResponse(const Workflow::Link * /*bus*/,
    int countOfMessages)
{
    investigationModel->insertRows(0, countOfMessages, QModelIndex());
}

void WorkflowInvestigationWidgetsController::sl_contextMenuRequested(const QPoint &cursorPosition)
{
    const QItemSelectionModel *selectionModel = investigationView->selectionModel();
    const QModelIndexList selection = selectionModel->selectedIndexes();
    if(1 == selection.size()) {
        QMenu contextMenu;
        contextMenu.addAction(exportInvestigationAction);
        contextMenu.addSeparator();
        contextMenu.addAction(copyToClipboardAction);
        contextMenu.exec(investigationView->viewport()->mapToGlobal(cursorPosition));
    }
}

void WorkflowInvestigationWidgetsController::sl_hotizontalHeaderContextMenuRequested(
    const QPoint &cursorPosition)
{
    QMenu contextMenu;
    selectedColumn = investigationView->columnAt(cursorPosition.x());
    if(DEFAULT_SELECTED_COLUMN != selectedColumn) {
        if(1 < investigationModel->columnCount()) {
            contextMenu.addAction(hideThisColumnAction);
            contextMenu.addAction(hideAllColumnsButThisAction);
        }
        if(investigationModel->isAnyColumnHidden()) {
            contextMenu.addAction(showAllColumnsAction);
        }
        contextMenu.exec(investigationView->viewport()->mapToGlobal(QPoint(cursorPosition.x(),
            cursorPosition.y() - investigationView->horizontalHeader()->height())));
        selectedColumn = DEFAULT_SELECTED_COLUMN;
    }
}

void WorkflowInvestigationWidgetsController::sl_exportInvestigation() {
    const QItemSelectionModel *selectionModel = investigationView->selectionModel();
    const QModelIndexList selected = selectionModel->selectedIndexes();
    Q_ASSERT(1 == selected.size());
    const QModelIndex item = selected.first();
    const QString messageType = investigationModel->headerData(item.column(), Qt::Horizontal)
        .toString();
    emit si_convertionMessages2DocumentsIsRequested(investigatedLink,
        messageType, item.row());
}

void WorkflowInvestigationWidgetsController::sl_copyToClipboard() const {
    const QModelIndexList selected = investigationView->selectionModel()->selectedIndexes();
    Q_ASSERT(1 == selected.size());
    QApplication::clipboard()->setText(selected.first().data().toString());
}

void WorkflowInvestigationWidgetsController::sl_hideSelectedColumn() {
    const int absoluteColumnNumber
        = investigationModel->getAbsoluteNumberOfVisibleColumn(selectedColumn);
    columnWidths[investigatedLink][absoluteColumnNumber] = -investigationView->columnWidth(selectedColumn);
    investigationModel->removeColumn(selectedColumn);
}

void WorkflowInvestigationWidgetsController::sl_hideAllColumnsButSelected() {
    for(int column = 0; investigationModel->columnCount() > column; ++column) {
        if(selectedColumn != column) {
            columnWidths[investigatedLink][investigationModel->getAbsoluteNumberOfVisibleColumn(column)]
            = -investigationView->columnWidth(column);
        }
    }
    investigationModel->removeColumns(0, selectedColumn);
    investigationModel->removeColumns(1, investigationModel->columnCount() - 1);
}

void WorkflowInvestigationWidgetsController::sl_showAllColumns() {
    const int absoluteSelectedColumnNumber
        = investigationModel->getAbsoluteNumberOfVisibleColumn(selectedColumn);
    investigationModel->showAllHiddenColumns();
    for(int column = 0; investigationModel->columnCount() > column; ++column) {
        const int width = columnWidths[investigatedLink][column];
        if(absoluteSelectedColumnNumber != column && 0 > width) {
            columnWidths[investigatedLink][column] = -width;
            investigationView->setColumnWidth(column, -width);
        }
    }
}

void WorkflowInvestigationWidgetsController::sl_columnsVisibilityResponse() {
    QBitArray hiddenColumns( 0 );
    const QVector<int> widths = columnWidths[investigatedLink];
    if ( !widths.isEmpty( ) ) {
        const int columnCount = widths.size();
        hiddenColumns.resize(columnCount);
        for(int column = 0; columnCount > column; ++column) {
            if(0 > widths[column]) {
                hiddenColumns.setBit(column, true);
            }
        }
    }
    investigationModel->setColumnsVisibility( hiddenColumns );
}

} // namespace U2
