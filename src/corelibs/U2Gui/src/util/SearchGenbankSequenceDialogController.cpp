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

#include "SearchGenbankSequenceDialogController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include "DownloadRemoteFileDialog.h"
#include "ui_SearchGenbankSequenceDialog.h"

namespace U2 {

SearchGenbankSequenceDialogController::SearchGenbankSequenceDialogController(QWidget* p)
    : QDialog(p) {
    ui = new Ui_SearchGenbankSequenceDialog();
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929336");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Download"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));

    ui->databaseBox->addItem(EntrezUtils::NCBI_DB_NUCLEOTIDE);
    ui->databaseBox->addItem(EntrezUtils::NCBI_DB_PROTEIN);

    queryBlockController = new QueryBuilderController(this);

    downloadButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    downloadButton->setEnabled(false);

    connect(ui->searchButton, &QPushButton::clicked, this, &SearchGenbankSequenceDialogController::sl_searchButtonClicked);
    connect(downloadButton, &QPushButton::clicked, this, &SearchGenbankSequenceDialogController::sl_downloadButtonClicked);
    connect(ui->treeWidget, &QTreeWidget::itemSelectionChanged, this, &SearchGenbankSequenceDialogController::sl_itemSelectionChanged);
    connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &SearchGenbankSequenceDialogController::sl_downloadButtonClicked);
    connect(AppContext::getTaskScheduler(), &TaskScheduler::si_stateChanged, this, &SearchGenbankSequenceDialogController::sl_taskStateChanged);

    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
}

SearchGenbankSequenceDialogController::~SearchGenbankSequenceDialogController() {
    // if dialog was closed during query execution
    if (summaryTask != nullptr && !summaryTask->isFinished()) {
        summaryTask->cancel();
    }
    if (searchTask != nullptr && !searchTask->isFinished()) {
        searchTask->cancel();
    }
}

void SearchGenbankSequenceDialogController::addQueryBlockWidget(QWidget* w) {
    ui->queryBuilderBox->layout()->addWidget(w);
    w->setObjectName("query_block_widget_" + QString::number(ui->queryBuilderBox->findChildren<QWidget*>(QRegExp("query_block_widget_\\d+")).size()));
}

void SearchGenbankSequenceDialogController::removeQueryBlockWidget(QWidget* w) {
    ui->queryBuilderBox->layout()->removeWidget(w);
}

void SearchGenbankSequenceDialogController::setQueryText(const QString& queryText) {
    ui->queryEdit->setText(queryText);
}

void SearchGenbankSequenceDialogController::prepareSummaryRequestTask(const QStringList& results) {
    summaryTask = nullptr;
    SAFE_POINT(!results.isEmpty(), "There are no search results to process", );
    if (results.size() <= MAX_IDS_PER_QUERY) {
        QString ids = results.join(",");
        QString query(EntrezUtils::NCBI_ESUMMARY_URL.arg(ui->databaseBox->currentText()).arg(ids));
        summaryResultHandler.reset(new ESummaryResultHandler());
        summaryTask = new EntrezQueryTask(summaryResultHandler.data(), query);
    } else {
        QStringList queries = splitIds(results);
        QList<Task*> tasks;
        for (const QString& query : qAsConst(queries)) {
            tasks << new EntrezQueryTask(new ESummaryResultHandler, query);
        }
        summaryTask = new MultiTask("EntrezQueryTask", tasks, false);
    }
}

QStringList SearchGenbankSequenceDialogController::splitIds(const QStringList& idsList) {
    int fullQueryCount = idsList.size() / MAX_IDS_PER_QUERY;
    int tailIdsCount = idsList.size() % MAX_IDS_PER_QUERY;
    QStringList queries;
    for (int i = 0; i < fullQueryCount; i++) {
        QString ids = getIdsString(idsList, i * MAX_IDS_PER_QUERY, MAX_IDS_PER_QUERY);
        queries << EntrezUtils::NCBI_ESUMMARY_URL.arg(ui->databaseBox->currentText()).arg(ids);
    }
    if (tailIdsCount > 0) {
        QString tailIds = getIdsString(idsList, fullQueryCount * MAX_IDS_PER_QUERY, tailIdsCount);
        queries << EntrezUtils::NCBI_ESUMMARY_URL.arg(ui->databaseBox->currentText()).arg(tailIds);
    }
    return queries;
}

QString SearchGenbankSequenceDialogController::getIdsString(const QStringList& idsList, int startIndex, int count) {
    return idsList.mid(startIndex, count).join(",");
}

QList<EntrezSummary> SearchGenbankSequenceDialogController::getSummaryResults() const {
    QList<EntrezSummary> results;
    auto singleTask = qobject_cast<EntrezQueryTask*>(summaryTask);
    auto multiTask = qobject_cast<MultiTask*>(summaryTask);
    if (singleTask != nullptr) {
        SAFE_POINT(summaryResultHandler != nullptr, L10N::nullPointerError("summary results handler"), results);
        results << summaryResultHandler->getResults();
    } else if (multiTask != nullptr) {
        QList<QPointer<Task>> multiTaskSubtasks = multiTask->getSubtasks();
        for (const QPointer<Task>& subtask : qAsConst(multiTaskSubtasks)) {
            auto summarySubtask = qobject_cast<EntrezQueryTask*>(subtask.data());
            SAFE_POINT(summarySubtask != nullptr, L10N::internalError(tr("an unexpected subtask")), results);
            auto resultHandler = dynamic_cast<const ESummaryResultHandler*>(summarySubtask->getResultHandler());
            SAFE_POINT(resultHandler != nullptr, L10N::nullPointerError("ESummaryResultHandler"), results);
            results << resultHandler->getResults();
            delete resultHandler;
        }
    }
    return results;
}

void SearchGenbankSequenceDialogController::sl_searchButtonClicked() {
    CHECK(ui->searchButton->isEnabled(), );
    QString query = ui->queryEdit->toPlainText();
    CHECK(!query.isEmpty(), );

    int maxRet = ui->resultLimitBox->value();
    QString qUrl(EntrezUtils::NCBI_ESEARCH_URL.arg(ui->databaseBox->currentText()).arg(query).arg(maxRet));
    searchResultHandler.reset(new ESearchResultHandler());
    searchTask = new EntrezQueryTask(searchResultHandler.data(), qUrl);

    AppContext::getTaskScheduler()->registerTopLevelTask(searchTask);
    ui->searchButton->setDisabled(true);
}

void SearchGenbankSequenceDialogController::sl_taskStateChanged(Task* task) {
    CHECK(task->getState() == Task::State_Finished, );
    if (task == searchTask) {
        ui->treeWidget->clear();
        const QStringList& results = searchResultHandler->getIdList();
        if (results.size() == 0) {
            QMessageBox::information(this, windowTitle(), tr("No results found corresponding to the query"));
            ui->searchButton->setEnabled(true);
        } else {
            prepareSummaryRequestTask(results);
            if (summaryTask != nullptr) {
                AppContext::getTaskScheduler()->registerTopLevelTask(summaryTask);
            }
        }
        searchTask = nullptr;
    } else if (task == summaryTask) {
        QList<EntrezSummary> results = getSummaryResults();

        for (const EntrezSummary& desc : qAsConst(results)) {
            ui->treeWidget->addTopLevelItem(new SearchResultRowItem(ui->treeWidget, desc));
        }
        summaryTask = nullptr;
        ui->searchButton->setEnabled(true);
    }
}

void SearchGenbankSequenceDialogController::sl_downloadButtonClicked() {
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    QStringList ids;
    for (QTreeWidgetItem* item : qAsConst(selectedItems)) {
        ids.append(item->text(0));
    }

    QObjectScopedPointer<DownloadRemoteFileDialog> dlg = new DownloadRemoteFileDialog(ids.join(";"), ui->databaseBox->currentText(), this);
    dlg->exec();
}

void SearchGenbankSequenceDialogController::sl_itemSelectionChanged() {
    downloadButton->setEnabled(ui->treeWidget->selectedItems().size() > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryBlockWidget::QueryBlockWidget(QueryBuilderController* controller, bool first)
    : conditionBox(nullptr), termBox(nullptr), queryEdit(nullptr) {
    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);

    if (first) {
        auto label = new QLabel(tr("Term:"));
        layout->addWidget(label);
    } else {
        conditionBox = new QComboBox(this);
        conditionBox->addItems(ctx.rules);
        connect(conditionBox, SIGNAL(currentIndexChanged(int)), controller, SLOT(sl_updateQuery()));
        layout->addWidget(conditionBox);
        conditionBox->setObjectName("condition_box");
    }

    termBox = new QComboBox(this);
    termBox->addItem(tr("All fields"));
    termBox->addItems(ctx.fields);
    connect(termBox, SIGNAL(currentIndexChanged(int)), controller, SLOT(sl_updateQuery()));
    termBox->setObjectName("term_box");

    queryEdit = new QLineEdit(this);
    queryEdit->setObjectName("queryEditLineEdit");
    connect(queryEdit, SIGNAL(textEdited(const QString&)), controller, SLOT(sl_updateQuery()));
    connect(queryEdit, SIGNAL(returnPressed()), controller, SLOT(sl_queryReturnPressed()));

    layout->addWidget(termBox);
    layout->addWidget(queryEdit);

    if (first) {
        auto addBlockButton = new QToolButton();
        addBlockButton->setText("+");
        layout->addWidget(addBlockButton);
        connect(addBlockButton, SIGNAL(clicked()), controller, SLOT(sl_addQueryBlockWidget()));
        addBlockButton->setObjectName("add_block_button");
    } else {
        auto removeBlockButton = new QToolButton();
        removeBlockButton->setText("-");
        layout->addWidget(removeBlockButton);
        connect(removeBlockButton, SIGNAL(clicked()), controller, SLOT(sl_removeQueryBlockWidget()));
        removeBlockButton->setObjectName("remove_block_button");
    }

    setLayout(layout);
}

void QueryBlockWidget::setInputFocus() {
    queryEdit->setFocus();
}

QString QueryBlockWidget::getQuery() const {
    QString query = queryEdit->text();
    CHECK(!query.isEmpty(), "");

    if (termBox->currentIndex() != 0) {
        query += "[" + termBox->currentText() + "]";
    }
    if (conditionBox) {
        query.prepend(" " + conditionBox->currentText() + " ");
    }
    return query;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryBuilderController::QueryBuilderController(SearchGenbankSequenceDialogController* p)
    : QObject(p), parentController(p) {
    auto widget = new QueryBlockWidget(this, true);
    parentController->addQueryBlockWidget(widget);
    queryBlockWidgets.append(widget);
    widget->setInputFocus();
}

void QueryBuilderController::sl_addQueryBlockWidget() {
    auto widget = new QueryBlockWidget(this, false);
    parentController->addQueryBlockWidget(widget);
    queryBlockWidgets.append(widget);
}

void QueryBuilderController::sl_removeQueryBlockWidget() {
    auto callbackButton = qobject_cast<QToolButton*>(sender());
    SAFE_POINT(callbackButton, "Not a callbackButton", );

    auto queryBlockWidget = qobject_cast<QueryBlockWidget*>(callbackButton->parentWidget());
    SAFE_POINT(callbackButton, "Not a queryBlockWidget", );

    parentController->removeQueryBlockWidget(queryBlockWidget);
    queryBlockWidgets.removeAll(queryBlockWidget);

    queryBlockWidget->deleteLater();

    sl_updateQuery();
}

void QueryBuilderController::sl_updateQuery() {
    QString query;
    for (QueryBlockWidget* w : qAsConst(queryBlockWidgets)) {
        query += w->getQuery();
    }

    parentController->setQueryText(query);
}

void QueryBuilderController::sl_queryReturnPressed() {
    parentController->sl_searchButtonClicked();
}

SearchResultRowItem::SearchResultRowItem(QTreeWidget* treeWidget, const EntrezSummary& desc)
    : QTreeWidgetItem(treeWidget) {
    setText(0, desc.name);
    setText(1, desc.title);
    setText(2, QString("%1").arg(desc.size));
    setData(2, Qt::UserRole, desc.size);
    setTextAlignment(2, Qt::AlignRight);
}

bool SearchResultRowItem::operator<(const QTreeWidgetItem& other) const {
    int column = treeWidget()->sortColumn();
    if (column == 2) {
        int myValue = data(2, Qt::UserRole).toInt();
        int otherValue = other.data(2, Qt::UserRole).toInt();
        return myValue < otherValue;
    }
    return QTreeWidgetItem::operator<(other);
}

}  // namespace U2
