/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/Counter.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DatasetsController.h>

#include "DatasetsListWidget.h"

namespace U2 {

DatasetsListWidget::DatasetsListWidget(DatasetsController *_ctrl)
: QWidget(), ctrl(_ctrl)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    tabs = new DatasetsTabWidget(this);
    l->addWidget(tabs);

    QToolButton *newTabButton = new QToolButton(this);
    tabs->setCornerWidget(newTabButton, Qt::TopRightCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setText("+");
    newTabButton->setObjectName("+");
    newTabButton->setToolTip(tr("Add dataset"));
    QIcon addIcon = QIcon(QString(":U2Designer/images/add.png"));
    newTabButton->setIcon(addIcon);
    connect(newTabButton, SIGNAL(clicked()), SLOT(sl_newDataset()));
    connect(tabs, SIGNAL(tabCloseRequested(int)), SLOT(sl_deleteDataset(int)));
    connect(tabs, SIGNAL(si_contextMenu(const QPoint &, int)), SLOT(sl_contextMenu(const QPoint &, int)));
}

void DatasetsListWidget::appendPage(const QString &name, QWidget *page) {
    int lastPos = tabs->count();
    tabs->insertTab(lastPos, page, name);
}

void DatasetsListWidget::sl_deleteDataset(int idx) {
    GCOUNTER(cvar, tvar, "WD::Dataset::Delete Dataset");
    QWidget *w = tabs->widget(idx);
    tabs->removeTab(idx);
    ctrl->deleteDataset(idx);
    delete w;
}

QString DatasetsListWidget::getTip() const {
    QStringList names;
    for (int i=0; i<tabs->count(); i++) {
        names << tabs->tabText(i);
    }
    int idx = names.count();
    QString result;
    do {
        idx++;
        result = QString("Dataset %1").arg(idx);
    } while (names.contains(result));

    return result;
}

void DatasetsListWidget::sl_newDataset() {
    GCOUNTER(cvar, tvar, "WD::Dataset::New Dataset");
    QString error;
    QString text = getTip();
    do {
        bool ok = false;
        text = QInputDialog::getText(this,
            tr("Enter Dataset Name"),
            tr("New dataset name:"),
            QLineEdit::Normal,
            text, &ok);
        if (!ok) {
            return;
        }
        U2OpStatusImpl os;
        ctrl->addDataset(text, os);
        error = os.getError();
        if (!error.isEmpty()) {
            QMessageBox::critical(this, tr("Error"), error);
        }
    } while (!error.isEmpty());

    // activate the tab after adding
    tabs->setCurrentIndex(tabs->count() - 1);
}

void DatasetsListWidget::sl_renameDataset() {
    GCOUNTER(cvar, tvar, "WD::Dataset::Rename Dataset");
    QAction *a = dynamic_cast<QAction*>(sender());
    CHECK(NULL != a, );

    int idx = a->property("idx").toInt();
    CHECK(idx < tabs->count(), );

    bool error = false;
    QString text = tabs->tabText(idx);
    do {
        bool ok = false;
        text = QInputDialog::getText(this,
            tr("Rename Dataset"),
            tr("New dataset name:"),
            QLineEdit::Normal,
            text, &ok);
        if (!ok) {
            return;
        }
        U2OpStatusImpl os;
        ctrl->renameDataset(idx, text, os);
        if (os.hasError()) {
            QMessageBox::critical(this, tr("Error"), os.getError());
        }
        error = os.hasError();
    } while (error);

    tabs->setTabText(idx, text);
}

void DatasetsListWidget::sl_contextMenu(const QPoint &p, int idx) {
    QMenu menu;
    QAction *renameAction = new QAction(tr("Rename dataset"), &menu);
    renameAction->setProperty("idx", idx);
    connect(renameAction, SIGNAL(triggered()), SLOT(sl_renameDataset()));
    menu.addAction(renameAction);
    menu.exec(p);
}

/************************************************************************/
/* DatasetsTabWidget */
/************************************************************************/
DatasetsTabWidget::DatasetsTabWidget(QWidget *parent)
: QTabWidget(parent)
{
    setUsesScrollButtons(true);
    setTabsClosable(true);
    tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar(), SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_contextMenu(const QPoint &)));
#ifdef Q_OS_MACX
    QString style = "QTabWidget::pane {"
                    "border-top: 1px solid #9B9B9B;"
                    "}"

                    "QTabWidget::tab-bar {"
                    "left: 0px;"
                    "}"

                    "QTabBar::tab {"
                    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                    "stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                    "border: 1px solid #9B9B9B;"
                    "border-bottom: 0px;"
                    "border-top-left-radius: 0px;"
                    "border-top-right-radius: 0px;"
                    "min-width: 10ex;"
                    "padding: 2px;"
                    "}"

                    "QTabBar::tab:selected, QTabBar::tab:hover {"
                    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
                    "stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                    "}"

                    "QTabBar::tab:selected {"
                    "border-color: #9B9B9B;"
                    "}";
    setStyleSheet(style);
#else
    setDocumentMode(true);
#endif
}

void DatasetsTabWidget::sl_contextMenu(const QPoint &p) {
    int idx = tabBar()->tabAt(p);
    if (-1 != idx) {
        emit si_contextMenu(tabBar()->mapToGlobal(p), idx);
    }
}

} // U2
