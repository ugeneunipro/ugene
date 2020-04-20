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

#include <QHBoxLayout>
#include <QTreeWidget>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/HelpButton.h>

#include "AppSettingsDialogController.h"
#include "AppSettingsDialogTree.h"

namespace U2 {

AppSettingsDialogController::AppSettingsDialogController(const QString& pageId, QWidget *p) :QDialog(p) {
    setupUi(this);

    currentPage = NULL;
    helpButton = new HelpButton(this, buttonBox, QString());
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));


    QHBoxLayout *pageLayout = new QHBoxLayout();
    settingsBox->setLayout(pageLayout);

    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(sl_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    QList<AppSettingsGUIPageController*> pages = AppContext::getAppSettingsGUI()->getRegisteredPages();
    foreach(AppSettingsGUIPageController* page, pages) {
        registerPage(page);
    }

    if (tree->topLevelItemCount() > 0) {
        if (!pageId.isEmpty()) {
            AppSettingsTreeItem* item = findPageItem(pageId);
            if (item != NULL) {
                tree->setCurrentItem(item);
            }
        }
        if (tree->currentItem() == NULL) {
            tree->setCurrentItem(tree->topLevelItem(0));
        }
    }

}

bool AppSettingsDialogController::checkCurrentState(bool saveStateInItem, bool showError) {
    if (currentPage == NULL) {
        return true;
    }
    QString err;
    currentPage->pageState = currentPage->pageWidget->getState(err);
    if (currentPage->pageState == NULL) {
        if (showError) {
            if (err.isEmpty()) {
                err = tr("Error");
            }
            QMessageBox::critical(this, tr("Error"), err);
        }
        return false;
    }
    if (!saveStateInItem) {
        delete currentPage->pageState;
        currentPage->pageState = NULL;
    } else {
        currentPage->pageState->setParent(this);
    }
    return true;
}

bool AppSettingsDialogController::turnPage(AppSettingsTreeItem* page) {
    assert(page == NULL || page->pageWidget == NULL);

    if (currentPage != NULL) {
        assert(currentPage->pageWidget != NULL);
        assert(currentPage->pageState == NULL);
        if (!checkCurrentState(true, false)) {
            return false;
        }
        assert(currentPage->pageState != NULL);
        settingsBox->setTitle("");
        delete currentPage->pageWidget;
        currentPage->pageWidget = NULL;
        currentPage = NULL;
    }
    if (page != NULL) {
        assert(currentPage == NULL);
        settingsBox->setTitle(page->pageController->getPageName());
        page->pageState = page->pageState == NULL ? page->pageController->getSavedState() : page->pageState;
        page->pageState->setParent(this);
        page->pageWidget = page->pageController->createWidget(page->pageState);
        settingsBox->layout()->addWidget(page->pageWidget);
        delete page->pageState;
        page->pageState = NULL;

        currentPage = page;
        helpButton->updatePageId(currentPage->pageController->getHelpPageId());
        connect(currentPage->pageWidget, SIGNAL(si_setLockState(bool)), SLOT(sl_setLockState(bool)));
    }
    return true;
}

void AppSettingsDialogController::registerPage(AppSettingsGUIPageController* page) {
    assert(findPageItem(page->getPageId()) == NULL);
    tree->addTopLevelItem(new AppSettingsTreeItem(page));
}

AppSettingsTreeItem* AppSettingsDialogController::findPageItem(const QString& id) const {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        AppSettingsTreeItem* item = static_cast<AppSettingsTreeItem*>(tree->topLevelItem(i));
        if (item->pageController->getPageId() == id) {
            return item;
        }
    }
    return NULL;
}

void AppSettingsDialogController::accept() {
    if (!checkCurrentState(false, true)) {
        return;
    }

    turnPage(NULL);//make current state saved in item
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        AppSettingsTreeItem* item = static_cast<AppSettingsTreeItem*>(tree->topLevelItem(i));
        if (item->pageState != NULL) {
            item->pageController->saveState(item->pageState);
        }
    }

    QDialog::accept();
}

void AppSettingsDialogController::reject() {
    QDialog::reject();
}

void AppSettingsDialogController::timerEvent(QTimerEvent* te) {
    killTimer(te->timerId());
    tree->setCurrentItem(currentPage);
}

void AppSettingsDialogController::sl_setLockState(bool lockState) {
    buttonBox->button(QDialogButtonBox::Ok)->setDisabled(lockState);
    tree->setDisabled(lockState);
}

void AppSettingsDialogController::sl_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    Q_UNUSED(previous);
    AppSettingsTreeItem* page = static_cast<AppSettingsTreeItem*>(current);
    if (page == currentPage) {
        return;
    }
    if (page == NULL || !checkCurrentState(false, true)) {
        startTimer(0);
        return;
    }
    turnPage(page);
}

AppSettingsTreeItem::AppSettingsTreeItem(AppSettingsGUIPageController* p) :pageController(p), pageState(NULL), pageWidget(NULL) {
    setText(0, "  " + p->getPageName());
}

}//namespace
