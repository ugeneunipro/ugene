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

#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectTreeControllerModeSettings.h>
#include <U2Core/U2ObjectTypeUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DatasetsController.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/SharedDbUrlUtils.h>

#include "DatasetWidget.h"
#include "ui_DatasetWidget.h"

namespace U2 {

URLListWidget::URLListWidget(URLListController *_ctrl)
    : QWidget(),
      ui(new Ui_DatasetWidget),
      ctrl(_ctrl),
      connectToDbDialog(new SharedConnectionsDialog(this)),
      waitingForDbToConnect(false)
{
    ui->setupUi(this);
    popup = new OptionsPopup(this);

    reset();
    QIcon fileIcon = QIcon(QString(":U2Designer/images/add_file.png"));
    QIcon dirIcon = QIcon(QString(":U2Designer/images/add_directory.png"));
    QIcon dbIcon = QIcon(QString(":U2Designer/images/database_add.png"));
    QIcon deleteIcon = QIcon(QString(":U2Designer/images/exit.png"));
    QIcon upIcon = QIcon(QString(":U2Designer/images/up.png"));
    QIcon downIcon = QIcon(QString(":U2Designer/images/down.png"));

    ui->addFileButton->setIcon(fileIcon);
    ui->addDirButton->setIcon(dirIcon);
    ui->addFromDbButton->setIcon(dbIcon);
    ui->deleteButton->setIcon(deleteIcon);
    ui->upButton->setIcon(upIcon);
    ui->downButton->setIcon(downIcon);

    connect(ui->addFileButton, SIGNAL(clicked()), SLOT(sl_addFileButton()));
    connect(ui->addDirButton, SIGNAL(clicked()), SLOT(sl_addDirButton()));
    connect(ui->addFromDbButton, SIGNAL(clicked()), SLOT(sl_addFromDbButton()));
    connect(ui->downButton, SIGNAL(clicked()), SLOT(sl_downButton()));
    connect(ui->upButton, SIGNAL(clicked()), SLOT(sl_upButton()));
    connect(ui->deleteButton, SIGNAL(clicked()), SLOT(sl_deleteButton()));
    connect(connectToDbDialog.data(), SIGNAL(si_connectionCompleted()), SLOT(sl_sharedDbConnected()));

    connect(ui->itemsArea, SIGNAL(itemSelectionChanged()), SLOT(sl_itemChecked()));

    if (!readingFromDbIsSupported()) {
        ui->addFromDbButton->hide();
    }

    QAction *deleteAction = new QAction(ui->itemsArea);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setShortcutContext(Qt::WidgetShortcut);
    connect(deleteAction, SIGNAL(triggered()), SLOT(sl_deleteButton()));
    ui->itemsArea->addAction(deleteAction);

    QAction *selectAction = new QAction(ui->itemsArea);
    selectAction->setShortcut(QKeySequence::SelectAll);
    selectAction->setShortcutContext(Qt::WidgetShortcut);
    connect(selectAction, SIGNAL(triggered()), SLOT(sl_selectAll()));
    ui->itemsArea->addAction(selectAction);

    ui->itemsArea->installEventFilter(this);
}

URLListWidget::~URLListWidget() {
    delete ui;
}

void URLListWidget::addUrlItem(UrlItem *urlItem) {
    urlItem->setParent(ui->itemsArea);
    ui->itemsArea->addItem(urlItem);
    connect(urlItem, SIGNAL(si_dataChanged()), SLOT(sl_dataChanged()));
}

void URLListWidget::sl_addFileButton() {
    LastUsedDirHelper lod;
    QStringList files;
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        files = U2FileDialog::getOpenFileNames(NULL, tr("Select file"), lod.dir, "", 0, QFileDialog::DontUseNativeDialog);
    } else {
        files = U2FileDialog::getOpenFileNames(NULL, tr("Select file"), lod.dir);
    }
    foreach (const QString &file, files) {
        lod.url = file;
        addUrl(file);
    }
}

void URLListWidget::sl_addDirButton() {
    LastUsedDirHelper lod;
    QString dir = U2FileDialog::getExistingDirectory(NULL, tr("Select a folder"), lod.dir);
    if (!dir.isEmpty()) {
        lod.dir = dir;
        addUrl(dir);
    }
}

namespace {

ProjectTreeControllerModeSettings createProjectTreeSettings(const QSet<GObjectType> &compatibleObjTypes) {
    ProjectTreeControllerModeSettings settings;
    settings.objectTypesToShow += compatibleObjTypes;

    Project *proj = AppContext::getProject();
    SAFE_POINT(NULL != proj, "Invalid project", settings);

    foreach (Document *doc, proj->getDocuments()) {
        if (!doc->isDatabaseConnection()) {
            settings.excludeDocList << doc;
        }
    }

    return settings;
}

}

void URLListWidget::sl_sharedDbConnected() {
    SAFE_POINT(waitingForDbToConnect, "Unexpected database state", );
    waitingForDbToConnect = false;
    sl_addFromDbButton();
}

void URLListWidget::sl_addFromDbButton() {
    CHECK(!waitingForDbToConnect, );
    if (!ProjectUtils::areSharedDatabasesAvailable()) {
        const int dialogResult = connectToDbDialog->exec();
        CHECK(!connectToDbDialog.isNull(), );
        if (QDialog::Accepted == dialogResult) {
            waitingForDbToConnect = true;
        }
        return;
     } else {
        waitingForDbToConnect = false;
     }

    const QSet<GObjectType> compatTypes = ctrl->getCompatibleObjTypes();
    SAFE_POINT(!compatTypes.isEmpty(), "Invalid object types", );
    const ProjectTreeControllerModeSettings settings = createProjectTreeSettings(compatTypes);

    QList<Folder> folders;
    QList<GObject *> objects;
    ProjectTreeItemSelectorDialog::selectObjectsAndFolders(settings, this, folders, objects);

    foreach (const Folder &f, folders) {
        // FIXME when readers for different data types appear
        addUrl(SharedDbUrlUtils::createDbFolderUrl(f, U2ObjectTypeUtils::toDataType(*compatTypes.begin())));
    }

    foreach (GObject *obj, objects) {
        addUrl(SharedDbUrlUtils::createDbObjectUrl(obj));
    }
}

void URLListWidget::addUrl(const QString &url) {
    U2OpStatusImpl os;
    ctrl->addUrl(url, os);
    if (os.hasError()) {
        QMessageBox::critical(this, tr("Error"), os.getError());
    }
}

void URLListWidget::sl_itemChecked() {
    reset();
    if (ui->itemsArea->selectedItems().size() > 0) {
        ui->deleteButton->setEnabled(true);
        bool firstSelected = ui->itemsArea->item(0)->isSelected();
        bool lastSelected = ui->itemsArea->item(ui->itemsArea->count() - 1)->isSelected();
        ui->upButton->setEnabled(!firstSelected);
        ui->downButton->setEnabled(!lastSelected);
    }
}

void URLListWidget::reset() {
    ui->deleteButton->setEnabled(false);
    ui->upButton->setEnabled(false);
    ui->downButton->setEnabled(false);
    popup->hideOptions();
}

bool URLListWidget::readingFromDbIsSupported() const {
    return !ctrl->getCompatibleObjTypes().isEmpty();
}

void URLListWidget::sl_downButton() {
    CHECK(ui->itemsArea->selectedItems().size() > 0, );

    for (int pos = ui->itemsArea->count() - 2; pos >= 0; pos--) { // without last item
        if (ui->itemsArea->item(pos)->isSelected()) {
            QListWidgetItem *item = ui->itemsArea->takeItem(pos);
            ui->itemsArea->insertItem(pos + 1, item);
            item->setSelected(true);
            ctrl->replaceUrl(pos, pos + 1);
        }
    }
}

void URLListWidget::sl_upButton() {
    CHECK(ui->itemsArea->selectedItems().size() > 0, );

    for (int pos = 1; pos < ui->itemsArea->count(); pos++) { // without first item
        if (ui->itemsArea->item(pos)->isSelected()) {
            QListWidgetItem *item = ui->itemsArea->takeItem(pos);
            ui->itemsArea->insertItem(pos - 1, item);
            item->setSelected(true);
            ctrl->replaceUrl(pos, pos - 1);
        }
    }
}

void URLListWidget::sl_deleteButton() {
    foreach (QListWidgetItem *item, ui->itemsArea->selectedItems()) {
        int pos = ui->itemsArea->row(item);
        ctrl->deleteUrl(pos);
        delete ui->itemsArea->takeItem(pos);
    }
}

void URLListWidget::sl_selectAll() {
    for (int i = 0; i < ui->itemsArea->count(); i++) {
        ui->itemsArea->item(i)->setSelected(true);
    }
}

void URLListWidget::sl_dataChanged() {
    ctrl->updateUrl(dynamic_cast<UrlItem*>(sender()));
}

bool URLListWidget::eventFilter(QObject *obj, QEvent *event) {
    CHECK(ui->itemsArea == obj, false);
    if (event->type() == QEvent::ContextMenu) {
        CHECK(1 == ui->itemsArea->selectedItems().size(), false);

        QContextMenuEvent *e = static_cast<QContextMenuEvent *>(event);
        QListWidgetItem *item = ui->itemsArea->itemAt(e->pos());
        CHECK(NULL != item, false);
        CHECK(item->isSelected(), false);

        UrlItem *urlItem = static_cast<UrlItem*>(item);
        CHECK(NULL != urlItem, false);

        QWidget *options = urlItem->getOptionsWidget();
        if (NULL != options) {
            popup->showOptions(options, ui->itemsArea->mapToGlobal(e->pos()));
        }
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

/************************************************************************/
/* OptionsPopup */
/************************************************************************/
OptionsPopup::OptionsPopup(QWidget *parent)
: QFrame(parent)
{
    l = new QVBoxLayout(this);
    l->setMargin(0);
    setWindowFlags(Qt::Popup);
    setFrameShape(QFrame::StyledPanel);
}

void OptionsPopup::closeEvent(QCloseEvent *event) {
    removeOptions();
    QWidget::closeEvent(event);
}

void OptionsPopup::showOptions(QWidget *options, const QPoint &p) {
    l->insertWidget(0, options);
    move(p);
    show();

    // if the popup is not fit in the screen, then move it
    int maxWidth = QApplication::desktop()->width();
    int maxHeight = QApplication::desktop()->height();
    QPoint rightBottom = pos() + QPoint(width(), height());
    if (rightBottom.x() > maxWidth) {
        move(x() - (rightBottom.x() - maxWidth), y());
    }
    if (rightBottom.y() > maxHeight) {
        move(x(), y() - (rightBottom.y() - maxHeight));
    }
}

void OptionsPopup::removeOptions() {
    QLayoutItem *child;
    while (NULL != (child = l->takeAt(0))) {
        child->widget()->setParent(NULL);
        delete child;
    }
}

void OptionsPopup::hideOptions() {
    removeOptions();
    hide();
}

} // U2
